#include "libs/inja.hpp"
#include "libs/maddy/parser.h"
#include <glob.h>
#include <experimental/filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <streambuf>

using namespace inja;
using json = nlohmann::json;

std::string read_file(std::string file_name)
{
    std::ifstream t(file_name);
    std::string file_contents((std::istreambuf_iterator<char>(t)),
                              std::istreambuf_iterator<char>());
    return file_contents;
}

std::string parse_md(std::string md_string)
{
    std::stringstream markdownInput(md_string);
    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>();
    return parser->Parse(markdownInput);
}

json parse_json_string(std::string raw_string)
{
    return json::parse(raw_string);
}

json cpp_ssg_config_data = parse_json_string(read_file("cpp_ssg_config.json"));

void save_html_file(std::string content, std::string output_path)
{
    std::ofstream out(output_path);
    out << content;
    out.close();
    std::cout << "FILE GENERATED: " << output_path << std::endl;
}

void render_pages()
{
    const std::string layout = "pages.html";
    glob_t glob_result;

    glob("pages/*", GLOB_TILDE, NULL, &glob_result);

    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
    {
        std::string full_input_path = glob_result.gl_pathv[i];
        std::string file_name = full_input_path.substr(6, full_input_path.length() - 9);
        std::string full_output_path = file_name + ".html";

        std::string html_with_full_data = render(read_file(full_input_path), cpp_ssg_config_data);
        html_with_full_data = parse_md(html_with_full_data);
        cpp_ssg_config_data["content"] = html_with_full_data;

        std::string temp_layout = read_file("layouts/" + layout);

        std::string temp_full_file = render(temp_layout, cpp_ssg_config_data);
        save_html_file(temp_full_file, "site/" + full_output_path);

        cpp_ssg_config_data["page_list"][i] = {{"title", file_name}, {"url", full_output_path}};
    }
}

void render_collections()
{
    const int collections_count = cpp_ssg_config_data["collections"].size();
    json collections = cpp_ssg_config_data["collections"];

    glob_t glob_result;

    for (size_t index = 0; index < collections_count; index++)
    {
        std::experimental::filesystem::create_directory("site/" + collections[index].get<std::string>());
        std::string temp_path = "collections/" + collections[index].get<std::string>() + "/*";
        glob(temp_path.c_str(), GLOB_TILDE, NULL, &glob_result);
        for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
        {
            std::string full_input_path = glob_result.gl_pathv[i];

            char char_array[full_input_path.length() + 1];
            strcpy(char_array, full_input_path.c_str());

            std::string full_output_path = strtok(char_array, ".");
            std::string file_name = full_output_path.substr(13 + collections[index].get<std::string>().length(), full_output_path.length());
            full_output_path = full_output_path + ".html";

            cpp_ssg_config_data["content"] = parse_md(read_file(full_input_path));

            std::string temp_layout = read_file("layouts/" + collections[index].get<std::string>() + ".html");

            std::string temp_full_file = render(temp_layout, cpp_ssg_config_data);
            save_html_file(temp_full_file, "site/" + full_output_path.substr(12, temp_full_file.length()));

            // making a list for storing links for items inside each collection
            int len = full_output_path.length();
            cpp_ssg_config_data["collection_items_list"][collections[index].get<std::string>()][i] = {{"title", file_name}, {"url", full_output_path.substr(12, len)}};
            cpp_ssg_config_data["content"] = "";
        }
    }
}

int main()
{
    std::string html = read_file("layouts/pages.html");
    std::experimental::filesystem::create_directory("site");
    render_collections();
    render_pages();
    // std::cout << cpp_ssg_config_data.dump(4);
}
# BLOG PAGE

{% for i in collection_items_list.blog %}
<a href="{{ i.url }}">
{{i.title}}
</a>
{% endfor %}
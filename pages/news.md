# NEWS PAGE

{% for i in collection_items_list.news %}
<a href="{{i.url}}">{{i.title}}</a>
{% endfor %}

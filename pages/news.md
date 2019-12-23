# NEWS

{% for i in collection_items_list.news %}
<article>
<h2>{{i.title}}</h2>
<div>
<a class="more" href="{{i.url}}">Read More</a>
</div>
</article>
{% endfor %}
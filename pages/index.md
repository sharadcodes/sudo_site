# BLOG

{% for i in collection_items_list.blog %}
<article>
<h2>{{i.title}}</h2>
<div>
<a class="more" href="{{i.url}}">Read More</a>
</div>
</article>
{% endfor %}
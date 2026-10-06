---
# Feel free to add content and custom Front Matter to this file.
# To modify the layout, see https://jekyllrb.com/docs/themes/#overriding-theme-defaults

layout: home
---
Blog Home.
{% for vk_note in site.vk %}
    <h2>
        <a href = "{{ vk_note.url }}">
            {{ vk_note.title}}
        </a>
    </h2>
{% endfor %}

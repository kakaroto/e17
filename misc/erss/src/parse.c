#include "erss.h"
#include "parse.h"
#include "parse_config.h"

Ewd_List *list = NULL;
Article *item = NULL;


void erss_story_new ()
{
	item = malloc (sizeof (Article));
  item->description = NULL;
  item->url = NULL;
  memset(item, 0, sizeof (Article));
}

void erss_story_end ()
{
	ewd_list_append (list, item);
}

void erss_parse_story (xmlDocPtr doc, xmlNodePtr cur)
{
	char *text;
	xmlChar *str;
	int i;
	
	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if (ewd_list_nodes (list) >= cfg->num_stories)
			return;

		if (!strcmp(cur->name, cfg->item_start)) {
			erss_story_new ();
			erss_parse_story (doc, cur);
			erss_story_end ();
		} 
		
		if ((!strcmp(cur->name, cfg->item_title)) && item) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			
			i = strlen(str) + 3 + strlen(cfg->prefix);
			text = malloc (i);
		
			snprintf (text, i, " %s %s", cfg->prefix, str);
			
			item->obj = edje_object_add (evas);
			edje_object_file_set (item->obj, cfg->theme, "erss_item");
			evas_object_show (item->obj);

			evas_object_event_callback_add (item->obj,
					EVAS_CALLBACK_MOUSE_IN, cb_mouse_in, NULL);
			evas_object_event_callback_add (item->obj,
					EVAS_CALLBACK_MOUSE_OUT, cb_mouse_out, NULL);
		
			e_container_element_append(cont, item->obj);
			edje_object_part_text_set (item->obj, "article", text);

			free (text);
			xmlFree (str);
		}
	
		if (cfg->item_url) {
			if (!strcmp(cur->name, cfg->item_url) && item) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				item->url = strdup (str);

				edje_object_signal_callback_add (item->obj, "exec*", "*",
						cb_mouse_out_item, item);
				edje_object_signal_emit (item->obj, "mouse,in", "article");
				edje_object_signal_emit (item->obj, "mouse,out", "article");

				xmlFree (str);
			}
		}
		
		if (cfg->item_description) {
			if (!strcmp(cur->name, cfg->item_description) && item) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				item->description = strdup (str);
				xmlFree (str);
			}
		}

		cur = cur->next;
	}
}

void erss_parse (xmlDocPtr doc)
{
	xmlNodePtr cur;
	
	if (doc == NULL ) {
		fprintf(stderr, "%s warn: buffer not parsed successfully.\n", PACKAGE);
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr,"%s error: empty buffer\n", PACKAGE);
		xmlFreeDoc(doc);
		exit (-1);
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
    if (ewd_list_nodes (list) >= cfg->num_stories)
			return;

		if (cfg->item_root) {
			if (!strcmp(cur->name, cfg->item_root)) {
				erss_parse_story (doc, cur);
			}
		} else {
			if (!strcmp(cur->name, cfg->item_start)) {
				erss_story_new ();
				erss_parse_story (doc, cur);
				erss_story_end ();
			}
		}
		
		cur = cur->next;
	}
	
	xmlFreeDoc(doc);
}


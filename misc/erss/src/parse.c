#include "erss.h"
#include "parse.h"
#include "parse_config.h"
#include "tooltip.h"
#include "gui.h"



static Erss_Article *erss_story_new (erss_feed *f)
{
	f->item=malloc(sizeof(Erss_Article));
	memset(f->item,0,sizeof(Erss_Article));
	f->item->description=NULL;
	f->item->url=NULL;
	f->item->ts=time(NULL);
	return f->item;
}

static void erss_story_end (erss_feed *f)
{
	ewd_list_append (f->list, f->item);
	f->item = NULL;
}

static void erss_parse_story (erss_feed *f, xmlNodePtr cur)
{
	Erss_Tooltip *tt = NULL;
	char *text;
	xmlChar *str;
	int i;

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if (ewd_list_nodes (f->list) >= cfg->num_stories)
			return;

		if (!strcmp(cur->name, cfg->item_start)) {
			erss_story_new (f);
			erss_parse_story (f, cur);
			erss_story_end (f);
		}

		if ((!strcmp(cur->name, cfg->item_title)) && f->item) {
			str = xmlNodeListGetString(f->doc, cur->xmlChildrenNode, 1);

			i = strlen(str) + 3 + strlen(cfg->prefix);
			text = malloc (i);

			snprintf (text, i, " %s %s", cfg->prefix, str);

			f->item->obj = edje_object_add (evas);
			edje_object_file_set (f->item->obj, cfg->theme, "erss_item");
			evas_object_show (f->item->obj);

			evas_object_event_callback_add (f->item->obj,
					EVAS_CALLBACK_MOUSE_IN, erss_mouse_in_cursor_change, NULL);
			evas_object_event_callback_add (f->item->obj,
					EVAS_CALLBACK_MOUSE_OUT, erss_mouse_out_cursor_change, NULL);

			e_container_element_append(cont, f->item->obj);
			edje_object_part_text_set (f->item->obj, "article", text);

			free (text);
			xmlFree (str);
		}
	
		if (cfg->item_url) {
			if (!strcmp(cur->name, cfg->item_url) && f->item) {
				str = xmlNodeListGetString(f->doc, cur->xmlChildrenNode, 1);
				f->item->url = strdup (str);

				edje_object_signal_callback_add (f->item->obj, "exec*", "*",
						erss_mouse_click_item, f->item->url);
				edje_object_signal_emit (f->item->obj, "mouse,in", "article");
				edje_object_signal_emit (f->item->obj, "mouse,out", "article");

				xmlFree (str);
			}
		}
		
		if (cfg->item_description) {
			if (!strcmp(cur->name, cfg->item_description) && f->item) {
				str = xmlNodeListGetString(f->doc, cur->xmlChildrenNode, 1);

				tt = erss_tooltip_new (str);

				if (f->item->obj) {
					evas_object_event_callback_add (f->item->obj,
							EVAS_CALLBACK_MOUSE_IN, erss_tooltip_mouse_in, tt);
					evas_object_event_callback_add (f->item->obj,
							EVAS_CALLBACK_MOUSE_OUT, erss_tooltip_mouse_out, tt);
				}

				xmlFree (str);
			}
		}

		cur = cur->next;
	}
}

void erss_parse (erss_feed *f)
{
	xmlNodePtr cur;

	if (f->doc == NULL ) {
		fprintf(stderr, "%s warn: buffer not parsed successfully.\n", PACKAGE);
	}

	cur = xmlDocGetRootElement(f->doc);

	if (cur == NULL) {
		fprintf(stderr,"%s error: empty buffer\n", PACKAGE);
		xmlFreeDoc(f->doc);
		exit (-1);
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if (ewd_list_nodes (f->list) >= cfg->num_stories)
			return;

		if (cfg->item_root) {
			if (!strcmp(cur->name, cfg->item_root)) {
				erss_parse_story (f, cur);
			}
		} else {
			if (!strcmp(cur->name, cfg->item_start)) {
				erss_story_new (f);
				erss_parse_story (f, cur);
				erss_story_end (f);
			}
		}

		cur = cur->next;
	}

	xmlFreeDoc(f->doc);
}

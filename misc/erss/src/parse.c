#include "erss.h"
#include "parse.h"

Ewd_List *list = NULL;
Article *item = NULL;
Config *cfg = NULL;
Rc_Config *rc = NULL;


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

int parse_rc_file ()
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *str = NULL;
	char file[PATH_MAX];

	snprintf (file, PATH_MAX, "%s/.erssrc", getenv ("HOME"));

	rc = malloc (sizeof (Rc_Config));
	memset(rc, 0, sizeof(Rc_Config));
	
	if (!rc)
		return FALSE;
	
	doc = xmlParseFile (file);

	if (doc) {
		cur = xmlDocGetRootElement(doc);

		cur = cur->xmlChildrenNode;
		while (cur != NULL) {
			if (!strcmp(cur->name, "config")) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (str)
					rc->config = strdup (str);
			} else if (!strcmp(cur->name, "theme")) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (str)
					rc->theme = strdup (str);
			} else if (!strcmp(cur->name, "browser")) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (str)
					rc->browser = strdup (str);
			} else if (!strcmp(cur->name, "proxy")) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (str)
					rc->proxy = strdup (str);
			} else if (!strcmp(cur->name, "proxy_port")) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (str)
					rc->proxy_port = atoi (str);
			}

			if (str) {
				xmlFree (str);
				str = NULL;
			}

			cur = cur->next;
		}
	}

	/*
	 * Set sane defaults for unspecified config options.
	 */
	if (!rc->theme)
		rc->theme = strdup(PACKAGE_DATA_DIR "/default.eet");
	if (!rc->config)
		rc->config = strdup(PACKAGE_DATA_DIR "/config/slashdot.cfg");

	if (!rc->browser && getenv("BROWSER"))
		rc->browser = strdup(getenv("BROWSER"));
	if (!rc->browser)
		rc->browser = strdup("mozilla");

	/* 
	 * If there is no rc file return false for us to know
	 */
	if (!doc)
		return FALSE;
	
	xmlFreeDoc(doc);

	return TRUE;
}

void parse_config_file (char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *str = NULL;
	char *tmp;
	int match = FALSE;

	/*
	 * Look in the list of config files and try to
	 * match our argument.
	 */

	doc = xmlParseFile (file);
	
	if (doc == NULL) {
		list_config_files (FALSE);

		tmp = ewd_list_goto_first (config_files);
		while ((tmp = ewd_list_current (config_files))) {
			if (strstr (tmp, file))  {
				doc = xmlParseFile (tmp);
				
				if (doc == NULL) {
					fprintf (stderr, "%s error: Can't open config file %s\n",
							PACKAGE, tmp);
					exit (-1);
				}

				match = TRUE;
				break;
			} 
			
			ewd_list_next (config_files);
		}

		if (!match) {
			fprintf (stderr, "%s error: No match for %s\n", PACKAGE, file);
			exit (-1);
		} else {
			printf ("%s info: your string '%s' matches %s\n", PACKAGE, file, tmp);
			printf ("%s info: using %s as config file\n", PACKAGE, tmp);
		}
			
	}


	/*
	 * Now allocate and fill the config struct
	 */
	cfg = malloc (sizeof (Config));
	memset(cfg, 0, sizeof (Config));

	if (!cfg) {
		fprintf (stderr, "%s error: out of memory\n", PACKAGE);
		exit (-1);
	}
	
	cur = xmlDocGetRootElement(doc);
	cur = cur->xmlChildrenNode;
	
	while (cur != NULL) {
		if (!strcmp(cur->name, "header")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->header = strdup (str);
		} else if (!strcmp(cur->name, "hostname")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->hostname = strdup (str);
		} else if (!strcmp(cur->name, "url")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->url = strdup (str);
		} else if (!strcmp(cur->name, "item_root")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->item_root = strdup (str);
		} else if (!strcmp(cur->name, "item_start")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->item_start = strdup (str);
		} else if (!strcmp(cur->name, "item_title")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->item_title = strdup (str);
		} else if (!strcmp(cur->name, "item_url")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->item_url = strdup (str);
		} else if (!strcmp(cur->name, "item_description")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->item_description = strdup (str);
		} else if (!strcmp(cur->name, "prefix")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->prefix = strdup (str);
		} else if (!strcmp(cur->name, "update_rate")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->update_rate = atoi (str);
		} else if (!strcmp(cur->name, "clock")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->clock = atoi (str);
			if (cfg->clock != 1 && cfg->clock != 0) {
				fprintf (stderr,
						"%s error: Clock option has wrong value - check your config file!\n",
						PACKAGE);
				exit (-1);
			}
		} else if (!strcmp(cur->name, "stories")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->num_stories = atoi (str);

			if (cfg->num_stories > 10) {
				fprintf (stderr,
						"%s error: Max stories to show is 10 - check your config file!\n",
						PACKAGE);
				exit (-1);
			}
		} else if (!strcmp(cur->name, "borderless")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->borderless = atoi (str);

			if (cfg->borderless != 1 && cfg->borderless != 0) {
				fprintf (stderr,
						"%s error:: Borderless option has wrong value - check your config file!\n"
						, PACKAGE);
				exit (-1);
			}
		} else if (!strcmp(cur->name, "x")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->x = atoi (str);
		} else if (!strcmp(cur->name, "y")) {
			str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (str)
				cfg->y = atoi (str);
		}

		if (str) {
			xmlFree (str);
			str = NULL;
		}
		
		cur = cur->next;
	}

	xmlFreeDoc(doc);

	/*
	 * Now start checking for sane values!
	 */
	if (!cfg->hostname) {
		fprintf (stderr, "%s error: No hostname defined!\n", PACKAGE);
		exit (-1);
	}

	if (!cfg->url) {
		fprintf (stderr, "%s error: No url defined!\n", PACKAGE);
		exit (-1);
	}

	if (!cfg->num_stories) {
		fprintf (stderr,
				"%s error: you need to define item_start in your config file\n",
				PACKAGE);
		exit (-1);
	}

	if (!cfg->item_title) {
		fprintf (stderr, "%s error: no <item_title> tag defined\n", PACKAGE);
		exit (-1);
	}

	if (!cfg->update_rate) {
		fprintf (stderr, "%s error: you need to define update_rate in your config file\n", PACKAGE);
		exit (-1);
	}
		
	if (!cfg->prefix)
		cfg->prefix = strdup(" . ");

}

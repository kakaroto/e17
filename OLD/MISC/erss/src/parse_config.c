#include "erss.h"              /* Erss_Config */
#include "parse_config.h"      /* Erss_Rc_Config  */
#include "ls.h"                /* erss_list_config_files() */

Erss_Rc_Config *rc = NULL;


int erss_parse_rc_file ()
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *str = NULL;
	char file[PATH_MAX];

	snprintf (file, PATH_MAX, "%s/.erssrc", getenv ("HOME"));

	rc = malloc (sizeof (Erss_Rc_Config));
	if (!rc)
		return FALSE;
	
	memset(rc, 0, sizeof (Erss_Rc_Config));

	rc->clock=-1;
	
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
			} else if (!strcmp(cur->name, "clock")) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (str)
					rc->clock = atoi (str);
			} else if (!strcmp(cur->name, "tooltip_delay")) {
				str = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (str)
					rc->tooltip_delay = atof (str);
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
		rc->theme = strdup(PACKAGE_DATA_DIR "/default.edj");
	if (!rc->config)
		rc->config = strdup(PACKAGE_DATA_DIR "/config/slashdot.cfg");

	if (!rc->browser && getenv("BROWSER"))
		rc->browser = strdup(getenv("BROWSER"));
	if (!rc->browser)
		rc->browser = strdup("mozilla");

	if (!rc->tooltip_delay)
		rc->tooltip_delay = 1.5;

	/* 
	 * If there is no rc file return false for us to know
	 */
	if (!doc)
		return FALSE;
	
	xmlFreeDoc(doc);

	return TRUE;
}






Erss_Config *erss_cfg_dst (Erss_Config **c) {
	Erss_Config *cfg;
	if((c != NULL) && ((cfg=*c) != NULL)) {
		if(cfg->header)           free(cfg->header);
		if(cfg->hostname)         free(cfg->hostname);
		if(cfg->url)              free(cfg->url);
		if(cfg->item_root)        free(cfg->item_root);
		if(cfg->item_start)       free(cfg->item_start);
		if(cfg->item_title)       free(cfg->item_title);
		if(cfg->item_url)         free(cfg->item_url);
		if(cfg->item_description) free(cfg->item_description);
		if(cfg->prefix)           free(cfg->prefix);
		if(cfg->theme)            free(cfg->theme);
		if(cfg->config)           free(cfg->config);

		memset(cfg,0,sizeof(Erss_Config));
		*c = NULL;
	}
	return NULL;
}



Erss_Config *erss_parse_config_file (char *file)
{
	xmlDocPtr    doc;
	xmlNodePtr   cur;
	xmlChar     *str = NULL;
	char        *tmp;
	int          match = FALSE;
	Erss_Config *cfg = NULL;

	/*
	 * Look in the list of config files and try to
	 * match our argument.
	 */

	doc = xmlParseFile (file);
	
	if (doc == NULL) {
		erss_list_config_files (FALSE);

		tmp = ecore_list_first_goto (config_files);
		while ((tmp = ecore_list_current (config_files))) {
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
			
			ecore_list_next (config_files);
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
	cfg = malloc (sizeof (Erss_Config));

	if (!cfg) {
		fprintf (stderr, "%s error: out of memory\n", PACKAGE);
		exit (-1);
	}

	memset(cfg, 0, sizeof (Erss_Config));
	
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

	return cfg;
}

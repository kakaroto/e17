typedef struct _article {
	Evas_Object *obj;

	char *url;
	char *description;
} Article;

extern Article *item;

void erss_story_new ();
void erss_story_end ();
void erss_parse_story (xmlDocPtr doc, xmlNodePtr cur);
void erss_parse (xmlDocPtr doc);

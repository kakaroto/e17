
typedef struct _erss_article Erss_Article;

struct _erss_article {
	Evas_Object *obj;

	char *url;
	char *description;
};

extern Erss_Article *item;

void erss_story_new ();
void erss_story_end ();
void erss_parse_story (xmlDocPtr doc, xmlNodePtr cur);
void erss_parse (xmlDocPtr doc);

#include "erss.h"
#include "parse.h"          /* ERSS_PARSE_* */



static Erss_Article *erss_parse_story_new (Erss_Feed *f)
{
	f->item=malloc(sizeof(Erss_Article));
	memset(f->item,0,sizeof(Erss_Article));
	f->item->title=NULL;
	f->item->description=NULL;
	f->item->url=NULL;
	f->item->ts=0L;
	return f->item;
}

static void erss_parse_story_end (Erss_Feed *f)
{
	ewd_list_append (f->list, f->item);
	f->item = NULL;
}

static char *erss_parse_desc_clean (char *description) {
	/* remove potential tags. not using libXML here, contents may not
	   be well-formed...  */

	char *p;

	if((description==NULL)||(*description=='\0')||((description=strdup(description))==NULL))
		return NULL;

	p=description;
	while(*p) {
	  if(*p=='<') {
	    char *p2=p;
	    do {
	      p2++;
	    } while(*p2&&(*p2!='>'));
	    if(*p2)
	      memmove(p,p2+1,strlen(p2));
	    else
	      *p='\0'; }
	  else
	    p++; }

	return description;
}

static void erss_parse_story (Erss_Feed *f, xmlNodePtr cur)
{
	char        *text;
	xmlChar     *str;
	int          i;
	Erss_Config *cfg=f->cfg;

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if (ewd_list_nodes (f->list) >= cfg->num_stories)
			return;

		if (!strcmp(cur->name, cfg->item_start)) {
			erss_parse_story_new (f);
			erss_parse_story (f, cur);
			erss_parse_story_end (f);
		}

		if ((!strcmp(cur->name, cfg->item_title)) && f->item) {
			str = xmlNodeListGetString(f->doc, cur->xmlChildrenNode, 1);

			i = strlen(str) + 3 + strlen(cfg->prefix);
			text = malloc (i);

			snprintf (text, i, " %s %s", cfg->prefix, str);

			f->item->title = text;

			xmlFree (str);
		}

		if (cfg->item_url) {
			if (!strcmp(cur->name, cfg->item_url) && f->item) {
				str = xmlNodeListGetString(f->doc, cur->xmlChildrenNode, 1);
				f->item->url = strdup (str);
				xmlFree (str);
			}
		}

		if (cfg->item_description) {
			if (!strcmp(cur->name, cfg->item_description) && f->item) {
				char *desc;

				if((str = xmlNodeListGetString(f->doc, cur->xmlChildrenNode, 1))) {
					if((desc = erss_parse_desc_clean(str)))
						f->item->description=desc;
					xmlFree (str);
				}
			}
		}

		cur = cur->next;
	}
}



int erss_parse_free (Erss_Feed *f) {
	if(f->item)
		erss_parse_story_end(f);
	if(f->doc) {
		xmlFreeDoc(f->doc);
		f->doc = NULL;
	}
	return TRUE;
}



int erss_parse (Erss_Feed *f)
{
	xmlNodePtr   cur;
	int          ret;
	Erss_Config *cfg = f->cfg;

	if (f->doc == NULL ) {
		fprintf(stderr, "%s warn: buffer not parsed successfully.\n", PACKAGE);
		return ERSS_PARSE_FAIL;
	}

	cur = xmlDocGetRootElement(f->doc);

	if (cur == NULL) {
		fprintf(stderr,"%s error: empty buffer\n", PACKAGE);
		xmlFreeDoc(f->doc);
		return ERSS_PARSE_EMPTY;
	}

	ret=ewd_list_nodes (f->list);
	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		if (ewd_list_nodes (f->list) >= cfg->num_stories) {
			ret=cfg->num_stories;
			break;
		}

		if (cfg->item_root) {
			if (!strcmp(cur->name, cfg->item_root)) {
				erss_parse_story (f, cur);
			}
		} else if (!strcmp(cur->name, cfg->item_start)) {
			erss_parse_story_new (f);
			erss_parse_story (f, cur);
			erss_parse_story_end (f);
		}

		cur = cur->next;
	}

	erss_parse_free(f);

	return ret;
}

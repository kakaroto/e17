#include "dEvian.h"

#ifdef HAVE_RSS

/*
 * Parse functions
 *
 * Must be used in a proper order to work !
 * Its not libxml like =)
 *
 * Each function returns the result taken in the given buf
 * or NULL if it fails
 * -
 * It can also be more specific functions which
 * fills some args values to return more informations
 * and 1 on success, 0 on fail
 */

#define PARSE_ITEM_TITLE 1
#define PARSE_ITEM_URL 2
#define PARSE_ITEM_DESC 3
#define PARSE_ITEM_DATE_10 4
#define PARSE_ITEM_DATE_20 5

#define PARSE_ARTICLE_FAILS() \
DEVIANF(data_rss_article_free) (article); \
return NULL;

#define PARSE_INFOS_FAILS() \
if (name) \
  evas_stringshare_del(name); \
if (link) \
  evas_stringshare_del(link); \
if (description) \
  evas_stringshare_del(description); \
return 0;

static char *_parse_item(char *buf, const char **text, int type);
static char *_parse_item_clean(char *buf, int size, int type);
static char *_parse_item_date_10(char *buf);
static char *_parse_item_date_20(char *buf);
static char *_parse_go_begin_meta(char *buf);
static char *_parse_go_end_meta(char *buf);


/* PUBLIC FUNCTIONS */

/**
 * Parse a feed
 *
 * @param feed Feed object
 * @param old_list Old feed items list (NULL if old list was empty)
 * @param list New feed item list
 * @return 1 on success, 0 on fail, and -1 of the new list is identical to the old one from old_list
 */
int DEVIANF(data_rss_parse_feed) (Rss_Feed *feed, Ecore_List *old_list, Ecore_List *list)
{
   char *buf;
   float i;

   buf = feed->buffer;

   if (!(feed->source->devian->conf->rss_doc->version = DEVIANF(data_rss_parse_rss_version) (buf)))
      return 0;

   i = feed->source->devian->conf->rss_doc->version;
   if (i == 1.0)
     {
        if (!feed->item_meta)
          {
             if (strstr(feed->buffer, "<item rdf"))
                feed->item_meta = strdup("<item rdf");
             else
                feed->item_meta = strdup("<item>");
          }
        if (!feed->channel_meta)
          {
             if (strstr(feed->buffer, "<channel rdf"))
                feed->channel_meta = strdup("<channel rdf");
             else
                feed->channel_meta = strdup("<channel>");
          }
     }
   else
     {
        if (i == 2.0)
          {
             if (!feed->item_meta)
                feed->item_meta = strdup("<item>");
             if (!feed->channel_meta)
                feed->channel_meta = strdup("<channel>");
          }
        else
          {
             if ((i > 0.0) && (i < 10.0))
               {
                  if (!feed->item_meta)
                     feed->item_meta = strdup("<item>");
                  if (!feed->channel_meta)
                     feed->channel_meta = strdup("<channel>");
               }
             else
                return 0;
          }
     }

   /* Parse infos if its allowed */
   if (feed->source->devian->conf->rss_doc->w_name || feed->source->devian->conf->rss_doc->w_description)
     {
        if (!DEVIANF(data_rss_parse_doc_infos) (feed, buf))
           return 0;
     }

   DDATARSSP(("Going to parse items"));

   if (!(buf = strstr(buf, feed->item_meta)))
      return 0;
   buf++;

   DDATARSSP(("Going to parse items"));

   /* Parse */
   i = 0;
   while ((buf = DEVIANF(data_rss_parse_article) (feed, buf, list)))
     {
        if (!(buf = strstr(buf, feed->item_meta)))
           break;
        i++;
        if (i >= feed->source->devian->conf->rss_nb_items)
           break;
        buf++;
     }

   E_FREE(feed->buffer);
   feed->buffer = NULL;
   feed->buffer_size = 0;

   /* Something has changed ? */
   if (old_list)
     {
        Rss_Article *old, *new;

        if (ecore_list_nodes(list) == ecore_list_nodes(old_list))
          {
             new = ecore_list_goto_first(list);
             old = ecore_list_goto_first(old_list);
             if (!strcmp(old->title, new->title))
               {
                  new = ecore_list_goto_last(list);
                  old = ecore_list_goto_last(old_list);
                  if (!strcmp(old->title, new->title))
                     return -1; //No
               }
          }
     }
   return 1;                    /* Yes */
}

/**
 * Parse the current element in buf to an item
 *
 * @param feed Teed object
 * @param buf Buffer
 * @param list New list of items, where to add the new item
 * @return 1 on success, 0 on fail
 */
char *DEVIANF(data_rss_parse_article) (Rss_Feed *feed, char *buf, Ecore_List *list)
{
   Rss_Article *article;
   char *tmp;
   int ver;

   if (!buf)
      return NULL;

   DDATARSSP(("Start parsing article (%8.8s)", buf));

   ver = feed->source->devian->conf->rss_doc->version;

   /* Go to first meta */
   while (*buf && (*buf != '<'))
      buf++;

   if (!*buf)
      return NULL;

   DDATARSSP(("Recaled (%8.8s)", buf));

   article = E_NEW(Rss_Article, 1);
   article->feed = feed;
   article->obj = NULL;
   article->title = NULL;
   article->url = NULL;
   article->description = NULL;
   article->date = NULL;
   article->date_simple = NULL;
   article->new = 1;
   article->reused = 0;

   do
     {
        tmp = NULL;
        /* Compare for * rss versions */
        if (!strncmp(buf, "<title", 6))
          {
             DDATARSSP(("detected title"));
             if (!(tmp = _parse_item(buf, &article->title, PARSE_ITEM_TITLE)))
		{
		   PARSE_ARTICLE_FAILS();
		}
          }
        else
          {
             if (!strncmp(buf, "<link", 5))
               {
                  DDATARSSP(("detected link"));
                  if (!(tmp = _parse_item(buf, &article->url, PARSE_ITEM_URL)))
		     {
			PARSE_ARTICLE_FAILS();
		     }
               }
             else
               {
                  if (!strncmp(buf, "<description", 12))
                    {
                       DDATARSSP(("detected description"));
                       if (!(tmp = _parse_item(buf, &article->description, PARSE_ITEM_DESC)))
			  {
			     PARSE_ARTICLE_FAILS();
			  }
                    }
                  else
                    {
                       /* Compare for specific rss versions */
                       switch ((int)ver)
                         {
                         case 1:
                            if (!strncmp(buf, "<dc:date", 8))
                              {
                                 char *buf_sav = buf;

                                 /* Parse it one time ->date and one time ->date_simple */
                                 DDATARSSP(("detected pubdate"));
                                 if (!(tmp = _parse_item(buf, &article->date, PARSE_ITEM_TITLE)))
				    {
				       PARSE_ARTICLE_FAILS();
				    }
                                 else
                                   {
                                      buf = buf_sav;
                                      if (!(tmp = _parse_item(buf, &article->date_simple, PARSE_ITEM_DATE_10)))
					 {
					    PARSE_ARTICLE_FAILS();
					 }
                                   }
                              }
                            break;
                         case 2:
                            if (!strncmp(buf, "<pubDate", 8))
                              {
                                 char *buf_sav = buf;

                                 /* Parse it one time ->date and one time ->date_simple */
                                 DDATARSSP(("detected pubdate"));
                                 if (!(tmp = _parse_item(buf, &article->date, PARSE_ITEM_TITLE)))
				    {
				       PARSE_ARTICLE_FAILS();
				    }
                                 else
                                   {
                                      buf = buf_sav;
                                      if (!(tmp = _parse_item(buf, &article->date_simple, PARSE_ITEM_DATE_20)))
					 {
					    PARSE_ARTICLE_FAILS();
					 }
                                   }
                              }
                            break;
                         }

                       if (!tmp)
                         {
                            /* We didnt found what is the meta ? The we jump over it :)
                             * Before, test if where not going to crash after jump =) */
                            DDATARSSP(("unknow meta (%8.8s), skip", buf));
                            if (!(tmp = _parse_go_end_meta(buf)))
			       {
				  PARSE_ARTICLE_FAILS();
			       }
                         }
                    }
               }
          }

        /* We have all data ? */
        if (article->title && article->url && article->description && article->date)
           break;
        /* Prepare next (+1 not to take the same) */
        buf = tmp + 1;
        /* Go to begining of next meta */
        while (*buf && (*buf != '<'))
           buf++;
        /* Check prematurate end of article, missing data
         * keep it anyway */
        if (!*(buf + 1))
	   {
	      PARSE_ARTICLE_FAILS();
	   }
        if (*(buf + 1) == '/')
           break;
     }
   while (buf);

   DDATARSSP(("Parsed article:"));
   DDATARSSP(("title: %s\nurl: %s\ndescription: %s\ndate: %s", article->title, article->url, article->description, article->date));

   ecore_list_append(list, article);

   return buf;
}

float DEVIANF(data_rss_parse_rss_version) (char *buf)
{
   float ver;
   char *tmp;

   tmp = strstr(buf, "<rss");
   if (tmp)
     {
        tmp = strstr(tmp, "version");
        if (!tmp)
           return 0.0;
        sscanf(tmp + 9, "%f", &ver);
     }
   else
     {
        tmp = strstr(buf, "<rdf:RDF");
        if (!tmp)
           return 0.0;
        ver = 1.0;
     }
   DDATARSSP(("Parsed rss version: %f", ver));
   return ver;
}

int DEVIANF(data_rss_parse_doc_infos) (Rss_Feed *feed, char *buf)
{
   char *tmp;
   const char *name, *description, *link;

   name = NULL;
   description = NULL;
   link = NULL;
   buf = strstr(buf, feed->channel_meta);

   if (!buf)
      return 0;

   buf = buf + 9;
   while (*buf && (*buf != '<'))
      buf++;

   do
     {
        if (!strncmp(buf, "<title", 6))
          {
             DDATARSSP(("Infos: detected title"));
             if (!(tmp = _parse_item(buf, &name, PARSE_ITEM_TITLE)))
		{
		   PARSE_INFOS_FAILS();
		}
          }
        else
          {
             if (!strncmp(buf, "<link", 5))
               {
                  DDATARSSP(("Infos: detected link"));
                  if (!(tmp = _parse_item(buf, &link, PARSE_ITEM_URL)))
		     {
			PARSE_INFOS_FAILS();
		     }
               }
             else
               {
                  if (!strncmp(buf, "<description", 12))
                    {
                       DDATARSSP(("Infos: detected description"));
                       if (!(tmp = _parse_item(buf, &description, PARSE_ITEM_DESC)))
			  {
			     PARSE_INFOS_FAILS();
			  }
                    }
                  else
                    {
                       DDATARSSP(("Infos: unknow meta (%8.8s), skip", buf));
                       if (!(tmp = _parse_go_end_meta(buf)))
			  {
			     PARSE_INFOS_FAILS();
			  }
                    }
               }
          }
        /* We have all data ? */
        if (name && description && link)
           break;
        /* Prepare next (+1 not to take the same */
        buf = tmp + 1;
        /* Go to begining of next meta */
        while (*buf && (*buf != '<'))
           buf++;
     }
   while (*buf);

   if (feed->source->devian->conf->rss_doc->w_name)
     {
        if (!name)
           name = evas_stringshare_add("No title");
        if (feed->source->devian->conf->rss_doc->name)
           evas_stringshare_del(feed->source->devian->conf->rss_doc->name);
        feed->source->devian->conf->rss_doc->name = name;
     }

   if (!feed->source->devian->conf->rss_doc->link)
     {
        if (link)
           feed->source->devian->conf->rss_doc->link = link;
     }
   else
     {
        if (link)
           evas_stringshare_del(link);
     }

   if (feed->source->devian->conf->rss_doc->w_description)
     {
        if (!description)
           description = evas_stringshare_add("No description");
        if (feed->source->devian->conf->rss_doc->description)
           evas_stringshare_del(feed->source->devian->conf->rss_doc->description);
        feed->source->devian->conf->rss_doc->description = description;
     }

   DDATARSSP(("Parsed infos:"));
   DDATARSSP(("name: %s\ndescription: %s", name, description));

   return 1;
}

/* PRIVATE FUNCTION */

static char *
_parse_item(char *buf, const char **text, int type)
{
   char *p;
   char *tmp;
   int tmp_l;

   /* Get content in tmp */

   /* Get the begining position of the content */
   if (!(tmp = _parse_go_begin_meta(buf)))
      return NULL;

   /* Get the end position of the content */
   if (!(p = _parse_go_end_meta(buf)))
      return NULL;

   /* Get the content */
   buf = tmp;
   tmp_l = p - buf;
   DDATARSSP(("Item size: %d", tmp_l));
   if (!tmp_l)                  /* Nothing in item, skip it */
     {
        *text = evas_stringshare_add("No");
        return p;
     }
   tmp = E_NEW(char, tmp_l + 1);
   if (!tmp)
      return NULL;
   memcpy(tmp, buf, tmp_l);
   *(tmp + tmp_l) = '\0';

   /* Clean the content of the item */
   switch (type)
     {
     case PARSE_ITEM_DATE_10:
        if (!(tmp = _parse_item_date_10(tmp)))
          {
             E_FREE(tmp);
             return NULL;
          }
        break;
     case PARSE_ITEM_DATE_20:
        if (!(tmp = _parse_item_date_20(tmp)))
          {
             E_FREE(tmp);
             return NULL;
          }
        break;

     default:
        if (!(tmp = _parse_item_clean(tmp, tmp_l + 1, type)))
          {
             E_FREE(tmp);
             return NULL;
          }
     }

   /*
    * {
    * char buf[8182];
    * 
    * strncpy(buf, tmp, sizeof(buf));
    * if (type == PARSE_ITEM_TITLE)
    * *text = DEVIANF(ss_utf8_add)(buf, 0);
    * else */
   *text = evas_stringshare_add(tmp);
   //}

   E_FREE(tmp);

   return p;
}

static char *
_parse_item_clean(char *buf, int size, int type)
{
   char *p;
   int new_size;                /* Future size, to avoid realloc on each memmove */

   if ((!*buf))
      return NULL;
   new_size = size;
   p = buf;

   while (*p)
     {
        DDATARSSP(("%d %d -------------------------------\n%30.30s", size, new_size, p));
        /* Skip metas inside text */
        if (*p == '<')
          {
             char *p2;

             /* Skip cfdata */
             if (*(p + 1) == '!')
               {
                  if (!strncmp(p, "<![CDATA[", 9))
                    {
                       memmove(p, p + 9, strlen(p + 9) + 1);
                       new_size -= 9;
                    }
                  else
                     p++;       /* Next char */
               }
             else
               {
                  /* Skip normal meta */
                  p2 = p;
                  do
                    {
                       p2++;
                       /* Check &gt (>) */
                       if (*p2 == '&')
                         {
                            if (!strncmp(p2 + 1, "gt;", 3))
                              {
                                 p2 += 3;
                                 break;
                              }
                         }
                    }
                  while (*p2 && (*p2 != '>'));
                  if (*p2)
                    {
                       memmove(p, p2 + 1, strlen(p2 + 1) + 1);
                       new_size -= ((p2 + 1) - p);
                    }
                  else
                    {
                       *p = '\0';
                       new_size -= (p2 - p);
                    }
               }
          }
        else
          {
             /* Skip end cfdata */
             if (*p == ']')
               {
                  if (!strncmp(p + 1, "]>", 2))
                    {
                       memmove(p, p + 3, strlen(p + 3) + 1);
                       new_size -= 3;
                    }
                  else
                     p++;       /* Next char */
               }
             else
               {
                  /* \n */
                  if (*p == 0xa)
                    {
                       if (type == PARSE_ITEM_TITLE)
                         {
                            memmove(p, p + 1, strlen(p + 1) + 1);
                            new_size -= 1;
                         }
                       else
                         {
                            new_size += 3;
                            if (new_size > size)
                              {
                                 int diff;

                                 diff = p - buf;
                                 size = new_size;
                                 buf = realloc(buf, size);
                                 p = buf + diff;
                              }
                            memmove(p + 3, p, strlen(p) + 1);
                            *p = '<';
                            *(p + 1) = 'b';
                            *(p + 2) = 'r';
                            *(p + 3) = '>';
                            p = p + 4;
                         }
                    }
                  else
                    {
                       /* \t */
                       if (*p == 0x9)
                         {
                            if (type == PARSE_ITEM_TITLE)
                              {
                                 memmove(p, p + 1, strlen(p + 1) + 1);
                                 new_size -= 1;
                              }
                            else
                              {
                                 new_size += 4;
                                 if (new_size > size)
                                   {
                                      int diff;

                                      diff = p - buf;
                                      size = new_size;
                                      buf = realloc(buf, size);
                                      p = buf + diff;
                                   }
                                 memmove(p + 4, p, strlen(p) + 1);
                                 *p = '<';
                                 *(p + 1) = 't';
                                 *(p + 2) = 'a';
                                 *(p + 3) = 'b';
                                 *(p + 4) = '>';
                                 p = p + 5;
                              }
                         }
                       else
                         {
                            /* \??? (newline) to nothing */
                            if (*p == 0xd)
                              {
                                 memmove(p, p + 1, strlen(p + 1) + 1);
                                 new_size -= 1;
                              }
                            else
                              {
                                 /* html codes */
                                 if (*p == '&')
                                   {
                                      /* &lt convertion to <
                                       * &gt is checked when skipping meta */
                                      if (!strncmp(p + 1, "lt;", 3))
                                        {
                                           memmove(p + 1, p + 4, strlen(p + 4) + 1);
                                           new_size -= 3;
                                           *p = '<';
                                        }
                                      else
                                        {
                                           /* Add html chars convertion, dec->hex */
                                           if (*(p + 1) == '#')
                                             {
                                                char b[4];
                                                unsigned int i;
                                                int len;
                                                char *p2 = p + 2;

                                                while (*p2 != ';')
                                                   p2++;
                                                p2++;
                                                if (((p2 - p) == 3) || (p2 - p) > 6)
                                                   p = p2;      /* Next char */
                                                else
                                                  {
                                                     if (!sscanf(p + 2, "%d", &i))
                                                        p = p2 + 1;     /* Next char */
                                                     else
                                                       {
                                                          snprintf(b, sizeof(b), "%c", i);
                                                          len = strlen(b);
                                                          memmove(p + 1, p2, strlen(p2) + 1);
                                                          new_size -= (p2 - p - 1);
                                                          memcpy(p, b, len);
                                                       }
                                                  }
                                             }
                                           else
                                              p++;      /* Next char */
                                        }
                                   }
                                 else
                                    p++;        /* Next char */
                              }
                         }
                    }
               }
          }

     }

   DDATARSSP(("Item clean: size: %d new_size: %d", size, new_size));
   buf = realloc(buf, new_size);

   return buf;
}

static char *
_parse_item_date_10(char *buf)
{
   char *p;

   if (!*buf)
      return NULL;
   p = buf;

   while (*p && (*p != '-'))
      p++;
   p++;
   if (!*p)
      return NULL;
   while (*p && (*p != '-'))
      p++;
   p++;
   if (!*p)
      return NULL;

   memmove(buf, p, strlen(p) + 1);
   p = buf;

   if (*(buf + 2) == ' ')
      p = buf + 3;
   else
      p = buf + 2;

   *p = ' ';
   p = p + 6;
   *p = '\0';

   return buf;
}

static char *
_parse_item_date_20(char *buf)
{
   int i;
   char *p, *p2;

   if (!*buf)
      return NULL;
   p = buf;

   for (i = 0; (i < 5 && *p); i++)
      p++;
   if (!*p)
      return NULL;
   memmove(buf, p, strlen(p) + 1);

   if (*(buf + 2) == ' ')
      p = buf + 3;
   else
      p = buf + 2;
   p2 = p;
   do
     {
        p2++;
     }
   while (*p2 && (*p2 != ':'));
   if (!*p2)
      return NULL;
   p2 = p2 - 2;
   memmove(p, p2, strlen(p2) + 1);
   p = p + 5;

   *p = '\0';

   return buf;
}

/**
 * Go at the first char of content in the meta pointed by buf
 *
 * @param buf text buffer
 * @return pointer at on the first char of the text
 */
static char *
_parse_go_begin_meta(char *buf)
{
   char *p;

   p = buf;

   /* Looking for autoclosing meta */
   while (*p && (*p != '>'))
      p++;
   if (!(*p))
      return NULL;
   if (*(p-1) == '/')
      {
	 DDATARSSP(("Searching end of an autoclosing meta !"));
	 return buf;
      }
   return p+1;
}

/**
 * Go at the last char of the content of the meta pointed by buf
 * (Find a meta of the same layer, or autoclosing meta, like in xhtml, <br/>)
 *
 * @param buf text buffer
 * @return pointer at the last char of the meta
 */
static char *
_parse_go_end_meta(char *buf)
{
   char *p;
   int layer;

   p = buf;
   layer = 0;

   DDATARSSP(("Looking for end meta (%20.20s)", p));

   /* Looking for autoclosing meta */
   while (*p && (*p != '>'))
      p++;
   if (*(p-1) == '/')
      {
	 DDATARSSP(("Searching end of an autoclosing meta !"));
	 return p+1;
      }

   while (*p)
     {
        /* Look for meta */
        while (*p && (*p != '<'))
           p++;
        if (*(p + 1))
          {
             /* Closing meta */
             if (*(p + 1) == '/')
               {
                  if (!layer)
		     goto done;
                  layer--;
               }
             else
               {
                  /* CDATA meta, direct skip
                   * because it cant include another one */
                  if (*(p + 1) == '!')
                    {
                       if (!strncmp(p + 1, "![CDATA", 7))
                         {
                            p = strstr(p + 1, "]>");
                            if (!p)
                               return NULL;
                            p++;
                         }
                    }
                  else
		     {
			/* Looking for autoclosing meta */
			while (*p && (*p != '>'))
			   p++;
			if (*(p-1) == '/')
			   ;   /* Skip meta */
			else
			   layer++;   /* Opening meta */
		     }
               }
          }
        /* Next char */
        p++;
     }
   DDATARSSP(("End meta NOT found, prematurate end of doc !"));
   return NULL;

 done:
   DDATARSSP(("End meta found here %15.15s", p));
   return p;
}
#endif

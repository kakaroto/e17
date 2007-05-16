#include "News.h"
#include "news_parse_private.h"

static Evas_List *_parsers;

static int  _parse_type(News_Feed_Document *doc);
static void _update_doc(News_Parse *parser);

/*
 * Public functions
 */

int
news_parse_init(void)
{
   _parsers = NULL;

   return 1;
}

void
news_parse_shutdown(void)
{
   News_Parse *p;

   while (_parsers)
     {
        p = _parsers->data;
        news_parse_stop(p->doc);
        _parsers = evas_list_remove_list(_parsers, _parsers);
     }
}

void
news_parse_go(News_Feed_Document *doc,
              void (cb_func) (News_Feed_Document *doc, News_Parse_Error error, int changes))
{
   News_Parse *parser;

   if (doc->parse.parser) return;

   if (!_parse_type(doc))
     {
        if (doc->articles && evas_list_count(doc->articles))
          cb_func(doc, NEWS_PARSE_ERROR_TYPE_UNKNOWN, 0);
        else
          cb_func(doc, NEWS_PARSE_ERROR_TYPE_UNKNOWN, 1);
        return;
     }

   parser = E_NEW(News_Parse, 1);
   parser->doc = doc;
   parser->cb_func = cb_func;
   parser->buffer_pos = parser->doc->server.buffer;

   doc->parse.parser = parser;
   _parsers = evas_list_append(_parsers, parser);

   switch (doc->parse.type)
     {
     case NEWS_FEED_TYPE_RSS:
        news_parse_rss_go(parser);
        break;
     case NEWS_FEED_TYPE_ATOM:
        news_parse_atom_go(parser);
        break;
     case NEWS_FEED_TYPE_UNKNOWN:
        break;
     }
}

void
news_parse_stop(News_Feed_Document *doc)
{
   if (!doc->parse.parser) return;

   DPARSE(("Parse STOP"));

   news_parse_free(doc->parse.parser);
   _parsers = evas_list_remove(_parsers,
                               doc->parse.parser);
   doc->parse.parser = NULL;
}


/*
 * Functions avalaible to parse files only
 */

char *
news_parse_meta_block_find(char **buf)
{
   char *p1, *p2;

   if (!buf || !(*buf) || !(**buf)) return NULL;

   p1 = *buf;
   while (p1 && (*p1 != '>')) p1++;
   if ( !p1 || !(*p1) || (*(p1-1) == '/') || !(*(p1+1)) ) return NULL;
   p1++;
   *buf = p1;

   p2 = news_parse_meta_block_end(*buf, 0, 1);
   if (!p2) return NULL;

   return p2;
}

char *
news_parse_meta_block_end(char *buf, int beg_before, int end_before)
{
   char *p;
   int layer;

   if (!buf || !(*buf)) return NULL;

   p = buf;
   layer = 0;

   DPARSE(("Looking for end meta block (%20.20s)", p));

   if (beg_before)
     {
        /* Looking for autoclosing meta */
        while (*p && (*p != '>')) p++;
        if (*(p-1) == '/')
          {
             DPARSE(("Searching end of an autoclosing meta !"));
             return buf;
          }
	p++;
     }

   while (*p)
     {
        /* look for meta */
        while (*p && (*p != '<'))
          p++;
        if (*(p + 1))
          {
             /* closing meta */
             if (*(p + 1) == '/')
               {
                  if (!layer) goto done;
                  layer--;
               }
             else
               {
                  /* cdata meta, direct skip
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
                       /* looking for autoclosing meta */
                       while (*p && (*p != '>'))
                         p++;
                       if (*(p-1) == '/')
                         ;   /* skip meta */
                       else
                         layer++;   /* opening meta */
                    }
               }
          }
        /* next char */
        p++;
     }

   DPARSE(("End meta NOT found, prematurate end of doc !"));
   return NULL;

 done:

   DPARSE(("End meta found here %15.15s", p));

   if (end_before)
     {
        while ( (*p != '<') ) p--;
     }
   else
     {
        while ( *p && (*p != '>') ) p++;
        if ( !(*p) || !(*(p+1)) ) return NULL;
        p++;
     }

   return p;
}

void
news_parse_finished(News_Parse *parser)
{
   News_Feed_Document *doc;

   doc = parser->doc;

   DPARSE(("Parse finished ! %d articles", evas_list_count(parser->articles)));
  
   if ( (parser->error != NEWS_PARSE_ERROR_TYPE_UNKNOWN) &&
        (parser->error != NEWS_PARSE_ERROR_NOT_IMPLEMENTED))
     {
        _update_doc(parser);
     }

   parser->cb_func(doc, parser->error, parser->changes);
   parser->idler = NULL;
   parser->doc->parse.parser = NULL;

   _parsers = evas_list_remove(_parsers, parser);
   news_parse_free(parser);
}

void
news_parse_free(News_Parse *parser)
{
   if (parser->doc) parser->doc->parse.parser = NULL;
  
   while (parser->articles)
     {
        News_Parse_Article *a;
      
        a = parser->articles->data;
        news_parse_article_free(a);
        parser->articles = evas_list_remove_list(parser->articles,
                                                 parser->articles);
     }
   if (parser->idler) ecore_idler_del(parser->idler);
   if (parser->art) news_parse_article_free(parser->art);

   free(parser);
}

void
news_parse_article_free(News_Parse_Article *a)
{
   E_FREE(a->title);
   E_FREE(a->url);
   //E_FREE(a->image); //TODO
   E_FREE(a->description);

   free(a);
}


/*
 * Private functions
 */

static int
_parse_type(News_Feed_Document *doc)
{
   char *buf, *p;

   buf = doc->server.buffer;

   p = strstr(buf, "<rss"); // FIXME: valgrind says "Invalid read of size 1" ... ??? maybe valgrind is lost
   if (p)
     {
        p = strstr(p, "version");
        if (!p) return 0;
        doc->parse.type = NEWS_FEED_TYPE_RSS;
        sscanf(p + 9, "%f", &doc->parse.version);
        return 1;
     }

   p = strstr(buf, "<rdf:RDF");
   if (p)
     {
        doc->parse.type = NEWS_FEED_TYPE_RSS;
        doc->parse.version = 1.0;
        return 1;
     }

   p = strstr(buf, "<feed xmlns");
   if (p)
     {
        doc->parse.type = NEWS_FEED_TYPE_ATOM;
        doc->parse.version = 1.0;
     }

   doc->parse.type = NEWS_FEED_TYPE_UNKNOWN;
   doc->parse.version = 0.0;

   return 0;
}

static void
_update_doc(News_Parse *parser)
{
   News_Parse_Article *pa;
   News_Feed_Article *fa;
   Evas_List *list, *l;
   int pos;

   list = NULL;

   /* create a list of new articles */

   for (l=parser->articles; l; l=evas_list_next(l))
     {
        pa = evas_list_data(l);
        if (!pa->article)
          {
             /* create a brand new article */
             parser->changes = 1;
             DPARSE(("** New articles list, create fa %s", pa->title));
             fa = E_NEW(News_Feed_Article, 1);
             fa->doc = parser->doc;
             fa->title = pa->title;
             pa->title = NULL;
             fa->url = pa->url;
             pa->url = NULL;
             fa->description = pa->description;
             pa->description = NULL;

             //TODO : call news_util_image_get (get image)
             
             memcpy(&fa->date, &pa->date, sizeof(fa->date));

             /* set as unread if its not first parse */
             if (parser->doc->parse.last_time)
               news_feed_article_unread_set(fa, 1);
          }
        else
          {
             /* remove the article reused from old list */
             DPARSE(("** New articles list, reused fa %s", pa->article->title));
             fa = pa->article;
             fa->reused = 0;
	     parser->doc->articles = evas_list_remove(parser->doc->articles, fa);
          }
        news_parse_article_free(pa);

	list = evas_list_append(list, fa);
     }
   evas_list_free(parser->articles);
   parser->articles = NULL;

   /* remove old articles,
    * except unread articles, we keep them */
   pos = evas_list_count(parser->doc->articles) - 1;
   while ((fa = evas_list_nth(parser->doc->articles, pos)))
     {
        if (!fa->unread ||
            (parser->doc->unread_count > NEWS_FEED_UNREAD_COUNT_MAX))
          {
             news_feed_article_del(fa);
             parser->changes = 1;
          }
        pos--;
     }

   /* append the old articles list to the new one */
   for (l=parser->doc->articles; l; l=evas_list_next(l))
     {
        fa = l->data;
        list = evas_list_append(list, fa);
     }
   /* and replace the old list by the new one */
   evas_list_free(parser->doc->articles);
   parser->doc->articles = list;

   /* DEBUG : list the articles */
   DPARSE(("-- New articles list for feed %s", parser->doc->feed->name));
   for (l=parser->doc->articles; l; l=evas_list_next(l))
     {
        fa = evas_list_data(l);
        DPARSE(("- %s", fa->title));
     }
   DPARSE(("---------"));

   /*
     FIXME:
     if sorting, no need to KEEP THE ORDER upper, so can optimize
   */
}

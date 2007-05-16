#include "News.h"
#include "news_parse_private.h"

static int          _idler(void *data);
static const char * _parse_infos_item(News_Parse *parser, char *meta, const char *prev, int *edit);

static News_Parse_Error _parse_detect_metas(News_Parse *parser);
static News_Parse_Error _parse_infos(News_Parse *parser);
static News_Parse_Error _parse_article_init(News_Parse *parser);
static News_Parse_Error _parse_article_exists(News_Parse *parser);
static News_Parse_Error _parse_article_end(News_Parse *parser);
static News_Parse_Error _parse_item_title(News_Parse *parser);
static News_Parse_Error _parse_item_date(News_Parse *parser);
static News_Parse_Error _parse_item_image(News_Parse *parser);
static News_Parse_Error _parse_item_link(News_Parse *parser);
static News_Parse_Error _parse_item_description(News_Parse *parser);
static News_Parse_Error _parse_item_clean(News_Parse *parser);

/*
 * Public functions
 */

void
news_parse_rss_go(News_Parse *parser)
{
   parser->articles = NULL;
   parser->oc.action = NEWS_PARSE_OC_DETECT_METAS;
   parser->idler = ecore_idler_add(_idler, parser);
}


/*
 * Private functions
 *
 */

static int
_idler(void *data)
{
   News_Parse *parser;
   News_Parse_Error err;

   parser = data;

   switch(parser->oc.action)
     {
     case NEWS_PARSE_OC_DETECT_METAS:
        err = _parse_detect_metas(parser);
        parser->oc.action = NEWS_PARSE_OC_INFOS;
        break;
     case NEWS_PARSE_OC_INFOS:
        err = _parse_infos(parser);
        parser->oc.action = NEWS_PARSE_OC_ARTICLE_BEG;
        break;
     case NEWS_PARSE_OC_ARTICLE_BEG:
        err = _parse_article_init(parser);
        if (parser->oc.action == NEWS_PARSE_OC_END) break;
        parser->oc.action = NEWS_PARSE_OC_ITEM_TITLE;
        break;
     case NEWS_PARSE_OC_ARTICLE_END:
        err = _parse_article_end(parser);
        parser->oc.action = NEWS_PARSE_OC_ARTICLE_BEG;
        break;
     case NEWS_PARSE_OC_ITEM_TITLE:
        err = _parse_item_title(parser);
        parser->oc.action = NEWS_PARSE_OC_ITEM_TITLE_CLEAN;
        parser->oc.after = NEWS_PARSE_OC_ITEM_DATE;
        break;
     case NEWS_PARSE_OC_ITEM_DATE:
        err = _parse_item_date(parser);
        parser->oc.action = NEWS_PARSE_OC_ITEM_LINK;
        break;
     case NEWS_PARSE_OC_ITEM_LINK:
        err = _parse_item_link(parser);
        parser->oc.action = NEWS_PARSE_OC_ITEM_LINK_CLEAN;
        parser->oc.after = NEWS_PARSE_OC_ARTICLE_EXISTS;
        break;
     case NEWS_PARSE_OC_ARTICLE_EXISTS:
        err = _parse_article_exists(parser);
        if (parser->oc.action == NEWS_PARSE_OC_ARTICLE_END) break;
        parser->oc.action = NEWS_PARSE_OC_ITEM_IMAGE;
        break;
     case NEWS_PARSE_OC_ITEM_IMAGE:
        err = _parse_item_image(parser);
        parser->oc.action = NEWS_PARSE_OC_ITEM_DESCRIPTION;
        break;
     case NEWS_PARSE_OC_ITEM_DESCRIPTION:
        err = _parse_item_description(parser);
        parser->oc.action = NEWS_PARSE_OC_ITEM_DESCRIPTION_CLEAN;
        parser->oc.after = NEWS_PARSE_OC_ARTICLE_END;
        break;
     case NEWS_PARSE_OC_ITEM_TITLE_CLEAN:
     case NEWS_PARSE_OC_ITEM_LINK_CLEAN:
     case NEWS_PARSE_OC_ITEM_DESCRIPTION_CLEAN:
        err = _parse_item_clean(parser);
        parser->oc.action = parser->oc.after;
        break;
     case NEWS_PARSE_OC_END:
        news_parse_finished(parser);
        return 0;
     }

   /* error returned */
   if (err)
     {
        parser->error = err;
        parser->oc.action = NEWS_PARSE_OC_END;
     }

   return 1;
}

static const char *
_parse_infos_item(News_Parse *parser, char *meta, const char *prev, int *edit)
{
   const char *item;
   char *ibeg, *iend;
   int   ilen;
   char  buf[8192];

   item = prev;
   ibeg = strstr(parser->buffer_pos, meta);
   if ( ibeg &&
        (ibeg < (parser->doc->server.buffer + parser->doc->server.buffer_size)) &&
        (iend = news_parse_meta_block_find(&ibeg)) )
     {
        ilen = iend - ibeg;
        if ( !prev || (prev && strncmp(ibeg, prev, ilen)) )
          {
             memcpy(buf, ibeg, ilen);
             buf[ilen] = '\0';
             item = evas_stringshare_add(buf);
             *edit = 1;
          }
     }

   return item;
}

static News_Parse_Error
_parse_detect_metas(News_Parse *parser)
{
   News_Feed *feed;
   News_Feed_Document *doc;
   int ver;

   doc = parser->doc;
   feed = doc->feed;
   ver = doc->parse.version;

   /* already done in a previous update ? */
   if (doc->parse.meta_channel) return NEWS_PARSE_ERROR_NO;
   
   if (ver == 1.0)
     {
        if (strstr(doc->server.buffer, "<item rdf"))
          doc->parse.meta_article = evas_stringshare_add("<item rdf");
        else
          doc->parse.meta_article = evas_stringshare_add("<item>");

        if (strstr(doc->server.buffer, "<channel rdf"))
          doc->parse.meta_channel = evas_stringshare_add("<channel rdf");
        else
          doc->parse.meta_channel = evas_stringshare_add("<channel>");

	doc->parse.meta_date = evas_stringshare_add("<dc:date");
     }
   else if ((ver > 0.0) && (ver <= 2.0))
     {
        doc->parse.meta_article = evas_stringshare_add("<item>");
        doc->parse.meta_channel = evas_stringshare_add("<channel>");
	doc->parse.meta_date = evas_stringshare_add("<pubDate");
     }

   if (!doc->parse.meta_channel)
     return NEWS_PARSE_ERROR_TYPE_UNKNOWN;

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_infos(News_Parse *parser)
{
   News_Feed *feed;
   const char *name, *language, *description, *url_home, *icon;
   int   edit = 0;

   /* already done in a previous update ? */
   if (parser->doc->parse.got_infos) return NEWS_PARSE_ERROR_NO;

   feed = parser->doc->feed;
   name = feed->name;
   language = feed->language;
   description = feed->description;
   url_home = feed->url_home;
   icon = feed->icon;
  
   if (feed->name_ovrw)
     name = _parse_infos_item(parser, "<title", feed->name, &edit);
   if (feed->language_ovrw)
     language = _parse_infos_item(parser, "<language", feed->language, &edit);
   if (feed->description_ovrw)
     description = _parse_infos_item(parser, "<description", feed->description, &edit);
   if (feed->url_home_ovrw)
     url_home = _parse_infos_item(parser, "<link", feed->url_home, &edit);
   //TODO handle feed image from server
   /*if (feed->icon_ovrw)
     icon = _parse_infos_item(parser, "<image", feed->icon, &edit); */

   if (edit)
     {
        news_feed_edit(feed,
                       (char *)name, feed->name_ovrw,
                       (char *)language, feed->language_ovrw,
                       (char *)description, feed->description_ovrw,
                       (char *)url_home, feed->url_home_ovrw,
                       (char *)feed->url_feed,
                       (char *)icon, feed->icon_ovrw,
                       feed->urgent,
                       feed->category,
                       0);
        if (feed->item && feed->item->viewer)
          news_viewer_refresh(feed->item->viewer);
        news_feed_lists_refresh(1);
     }

   parser->doc->parse.got_infos = 1;

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_article_init(News_Parse *parser)
{
   News_Parse_Article *art;
   char *pos;
   char *p1, *p2;

   pos = parser->buffer_pos;

   if (pos >= (parser->doc->server.buffer + parser->doc->server.buffer_size))
     {
        parser->oc.action = NEWS_PARSE_OC_END;
        return NEWS_PARSE_ERROR_NO;
     }
     
   p1 = strstr(pos, parser->doc->parse.meta_article); // FIXME: valgrind says "Invalid read of size 1" ... ??? maybe valgrind is lost
   if (!p1)
     {
        if (evas_list_count(parser->articles))
          {
             parser->oc.action = NEWS_PARSE_OC_END;
             return NEWS_PARSE_ERROR_NO;
          }
        return NEWS_PARSE_ERROR_BROKEN_FEED;
     }
   p2 = news_parse_meta_block_end(p1, 1, 1);
   if (!p2) return NEWS_PARSE_ERROR_BROKEN_FEED;

   art = E_NEW(News_Parse_Article, 1);
   art->pos_end = p2;

   parser->art = art;
   parser->buffer_pos = p1;

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_article_exists(News_Parse *parser)
{
   News_Parse_Article *art;
   News_Feed_Article *a;
   Evas_List *l;

   art = parser->art;

   if (!parser->doc->articles ||
       !evas_list_count(parser->doc->articles))
     return NEWS_PARSE_ERROR_NO;

   DPARSE(("ALREADY EXISTS : Go %s %s",
           art->url, art->title));
   for (l=parser->doc->articles; l; l=evas_list_next(l))
     {
        a = l->data;

        if (
            ( !a->reused )
            &&
            ( !news_util_datecmp(&art->date, &a->date) )
            &&
            ( ( (art->title && a->title) &&
                !strcmp(art->title, a->title) ) ||
              (!art->title && !a->title) )
            )
          {
             art->article = a;
             a->reused = 1;
             parser->oc.action = NEWS_PARSE_OC_ARTICLE_END;
             DPARSE(("ALREADY EXISTS : MATCH !!!"));
             return NEWS_PARSE_ERROR_NO;
          }
        DD(("ALREADY EXISTS : NO Match %s %s",
            a->url, a->title));        
     }

   DPARSE(("ALREADY_EXISTS : DOES NOT exists"));

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_article_end(News_Parse *parser)
{
   parser->articles = evas_list_append(parser->articles,
                                       parser->art);
   parser->buffer_pos = parser->art->pos_end;
   DPARSE(("Parse article %s end", parser->art->title));
   parser->art = NULL;

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_item_title(News_Parse *parser)
{
   News_Parse_Article *art;
   char *pos;
   char *p1, *p2;
   int len;

   art = parser->art;
   pos = parser->buffer_pos;

   p1 = strstr(pos, "<title");
   p2 = news_parse_meta_block_find(&p1);
   if (!p2) return NEWS_PARSE_ERROR_BROKEN_FEED;

   if (p2 < art->pos_end)
     {
        len = p2 - p1;
        art->title = E_NEW(char, len+1);
        memcpy(art->title, p1, len);
        art->title[len] = '\0';
     }
   else
     {
        art->title = strdup("No title");
     }

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_item_date(News_Parse *parser)
{
   News_Parse_Article *art;
   char *pos, *date;
   char *p1, *p2;
   int len, i;
   struct tm *tm;

   art = parser->art;
   pos = parser->buffer_pos;

   p1 = strstr(pos, parser->doc->parse.meta_date);
   p2 = news_parse_meta_block_find(&p1);
   if (!p2) return NEWS_PARSE_ERROR_BROKEN_FEED;
   if (p2 >= art->pos_end) return NEWS_PARSE_ERROR_BROKEN_FEED;

   len = p2 - p1;
   date = p1;
   tm = &art->date;
   DD(("DATE LEN %d", len));

   if ((parser->doc->parse.version == 1.0) &&
       (len >= 22))
     {
        sscanf(date, "%4d", &i);
        tm->tm_year = i - 1900;
        sscanf(date+5, "%2d", &tm->tm_mon);
        tm->tm_mon--;   /* tm works with 0-11 */
        sscanf(date+8, "%2d", &tm->tm_mday);
        sscanf(date+11, "%2d", &tm->tm_hour);
        sscanf(date+14, "%2d", &tm->tm_min);
        if (date[16] == ':') /* seconds are precised ? */
          sscanf(date+17, "%2d", &tm->tm_sec);
        else
          tm->tm_sec = 0;
        DD(("DATE seconds %d", tm->tm_sec));
     }
   else if (len >= 25)
     {
        sscanf(date+5, "%2d", &tm->tm_mday);
        if (!strncmp(date+8, "Jan", 3)) tm->tm_mon = 0;
        else if (!strncmp(date+8, "Feb", 3)) tm->tm_mon = 1;
        else if (!strncmp(date+8, "Mar", 3)) tm->tm_mon = 2;
        else if (!strncmp(date+8, "Apr", 3)) tm->tm_mon = 3;
        else if (!strncmp(date+8, "May", 3)) tm->tm_mon = 4;
        else if (!strncmp(date+8, "Jun", 3)) tm->tm_mon = 5;
        else if (!strncmp(date+8, "Jul", 3)) tm->tm_mon = 6;
        else if (!strncmp(date+8, "Aug", 3)) tm->tm_mon = 7;
        else if (!strncmp(date+8, "Sep", 3)) tm->tm_mon = 8;
        else if (!strncmp(date+8, "Oct", 3)) tm->tm_mon = 9;
        else if (!strncmp(date+8, "Nov", 3)) tm->tm_mon = 10;
        else if (!strncmp(date+8, "Dec", 3)) tm->tm_mon = 11;
        else tm->tm_mon = 0;
        sscanf(date+12, "%4d", &i);
        tm->tm_year = i - 1900;
        sscanf(date+17, "%2d", &tm->tm_hour);
        sscanf(date+20, "%2d", &tm->tm_min);
        sscanf(date+23, "%2d", &tm->tm_sec);
     }

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_item_image(News_Parse *parser)
{
   // TODO parse and ecore_download

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_item_link(News_Parse *parser)
{
   News_Parse_Article *art;
   char *pos;
   char *p1, *p2;
   int len;

   art = parser->art;
   pos = parser->buffer_pos;

   p1 = strstr(pos, "<link");
   p2 = news_parse_meta_block_find(&p1);
   if (!p2) return NEWS_PARSE_ERROR_BROKEN_FEED;

   if (p2 < art->pos_end)
     {
        len = p2 - p1;
        art->url = E_NEW(char, len+1);
        memcpy(art->url, p1, len);
        art->url[len] = '\0';
     }
   else
     {
        art->url = strdup("No link");
     }

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_item_description(News_Parse *parser)
{
   News_Parse_Article *art;
   char *pos;
   char *p1, *p2;
   int len;

   art = parser->art;
   pos = parser->buffer_pos;

   p1 = strstr(pos, "<description");
   p2 = news_parse_meta_block_find(&p1);
   if (!p2) return NEWS_PARSE_ERROR_BROKEN_FEED;

   if (p2 < art->pos_end)
     {
        len = p2 - p1;
        art->description = E_NEW(char, len+1);
        memcpy(art->description, p1, len);
        art->description[len] = '\0';
     }
   else
     {
        art->description = strdup("No content");
     }

   return NEWS_PARSE_ERROR_NO;
}

static News_Parse_Error
_parse_item_clean(News_Parse *parser)
{
   char *buf, *p;
   char **buf_org;
   int size;
   int new_size; /* future size, to avoid realloc on each memmove */

   switch (parser->oc.action)
     {
     case NEWS_PARSE_OC_ITEM_TITLE_CLEAN:
        buf_org = &parser->art->title;
        break;
     case NEWS_PARSE_OC_ITEM_LINK_CLEAN:
        buf_org = &parser->art->url;
        break;
     case NEWS_PARSE_OC_ITEM_DESCRIPTION_CLEAN:
        buf_org = &parser->art->description;
        break;
     default:
        return NEWS_PARSE_ERROR_NO;
     }

   buf = *buf_org;
   if (!buf) return NEWS_PARSE_ERROR_NO; /* nothing to clean, ok */
   p = buf;
   size = strlen(buf) + 1;
   new_size = size;

   while (*p)
     {
        //DPARSE(("%d %d ------\n%30.30s", size, new_size, p));
        /* skip metas inside text */
        if (*p == '<')
          {
             char *p2;

             /* skip cfdata */
             if (*(p + 1) == '!')
               {
                  if (!strncmp(p, "<![CDATA[", 9))
                    {
                       memmove(p, p + 9, strlen(p + 9) + 1);
                       new_size -= 9;
                    }
                  else
                    p++;       /* next char */
               }
             else
               {
                  /* skip normal meta */
                  p2 = p;
                  do
                    {
                       p2++;
                       /* check &gt (>) */
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
        else if (*p == ']')                       /* skip end cfdata */
          {
             if (!strncmp(p + 1, "]>", 2))
               {
                  memmove(p, p + 3, strlen(p + 3) + 1);
                  new_size -= 3;
               }
             else
               p++; /* next char */
          }
        else if (*p == 0xa)                        /* \n */
          {
             if (parser->oc.action == NEWS_PARSE_OC_ITEM_TITLE)
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
                  DPARSE(("buf %p p %p new_size %d strlen(buf) %d strlen(p) %d\np :\n%30.30s\n", buf, p, new_size, (int)strlen(buf), (int)strlen(p), p ));
                  memmove(p + 3, p, strlen(p) + 1);
                  *p = '<';
                  *(p + 1) = 'b';
                  *(p + 2) = 'r';
                  *(p + 3) = '>';
                  p = p + 4;
               }
          }
        else if (*p == 0x9)                        /* \t */
          {
             if (parser->oc.action == NEWS_PARSE_OC_ITEM_TITLE)
               {
                  memmove(p, p + 1, strlen(p + 1) + 1); //FIXME : strlen(p) ? EVERYWHERE ?
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
        else if (*p == 0xd)                     /* \??? (newline) to nothing */
          {
             memmove(p, p + 1, strlen(p + 1) + 1);
             new_size -= 1;
          }
        else if (*p == '&')                    /* html codes */
          {
             /* &lt convertion to <
              * &gt is checked when skipping meta */
             if (!strncmp(p + 1, "lt;", 3))
               {
                  memmove(p + 1, p + 4, strlen(p + 4) + 1);
                  new_size -= 3;
                  *p = '<';
               }
             else if (0) /* TODO: add html chars convertion */
               {
                  
               }
             else if (*(p + 1) == '#') /* ascii hexadecimal char */
               {
                  char b[4];
                  unsigned int i;
                  int len;
                  char *p2 = p + 2;
                  
                  /* dec->hex */
                  //TODO: FIX !
                  while (*p2 != ';')
                    p2++;
                  p2++;
                  if (((p2 - p) == 3) || (p2 - p) > 6)
                    p = p2; /* next char */
                  else //TODO: STOPPED HERE
                    {
                       if (!sscanf(p + 2, "%d", &i))
                         p = p2 + 1; /* next char */
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
               p++; /* next char */
          }
        else
          p++;                                /* nothing interesting :) next char */
     }


   DPARSE(("Item clean: size: %d new_size: %d", size, new_size));

   buf = realloc(buf, new_size);
   *buf_org = buf;

   return NEWS_PARSE_ERROR_NO;
}

#ifndef NEWS_PARSE_PRIVATE_H_INCLUDED
#define NEWS_PARSE_PRIVATE_H_INCLUDED

typedef enum News_Parse_Oc
   {
      NEWS_PARSE_OC_DETECT_METAS,
      NEWS_PARSE_OC_INFOS,
      NEWS_PARSE_OC_ARTICLE_BEG,
      NEWS_PARSE_OC_ITEM_TITLE,
      NEWS_PARSE_OC_ITEM_TITLE_CLEAN,
      NEWS_PARSE_OC_ITEM_DATE,
      NEWS_PARSE_OC_ITEM_IMAGE,
      NEWS_PARSE_OC_ITEM_LINK,
      NEWS_PARSE_OC_ITEM_LINK_CLEAN,
      NEWS_PARSE_OC_ARTICLE_EXISTS,
      NEWS_PARSE_OC_ITEM_DESCRIPTION,
      NEWS_PARSE_OC_ITEM_DESCRIPTION_CLEAN,
      NEWS_PARSE_OC_ARTICLE_END,
      NEWS_PARSE_OC_END
   } News_Parse_Oc;

typedef struct _News_Parse_Oc_Actions News_Parse_Oc_Actions;
typedef struct _News_Parse_Article News_Parse_Article;

struct _News_Parse_Oc_Actions
{
   News_Parse_Oc action;
   News_Parse_Oc after;
};

struct _News_Parse
{
   News_Feed_Document *doc;
   void (*cb_func) (News_Feed_Document *doc, News_Parse_Error error, int changes);
   char *buffer_pos;

   News_Parse_Oc_Actions oc;
   Evas_List            *articles;
   Ecore_Idler          *idler;
   News_Parse_Article   *art;
   int error;
   int changes : 1;
};

struct _News_Parse_Article
{
   News_Feed_Article *article;

   char      *pos_end;

   char      *title;
   char      *url;
   char      *image;
   char      *description;
   struct tm  date;
};

char *news_parse_meta_block_find(char **buf);
char *news_parse_meta_block_end(char *buf, int beg_before, int end_before);

void  news_parse_finished(News_Parse *parser);
void  news_parse_free(News_Parse *parser);
void  news_parse_article_free(News_Parse_Article *a);

#endif

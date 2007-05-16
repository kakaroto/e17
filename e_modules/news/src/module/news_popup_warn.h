#ifdef E_MOD_NEWS_TYPEDEFS

typedef struct _News_Popup_Warn News_Popup_Warn;

typedef enum _News_Popup_Warn_Active
   {
      NEWS_POPUP_WARN_ACTIVE_NO,
      NEWS_POPUP_WARN_ACTIVE_SUM,
      NEWS_POPUP_WARN_ACTIVE_VERBOSE
   } News_Popup_Warn_Active;

typedef enum _News_Popup_Warn_Type
   {
      NEWS_POPUP_WARN_TYPE_NEWS,
      NEWS_POPUP_WARN_TYPE_INFO,
      NEWS_POPUP_WARN_TYPE_ERROR
   } News_Popup_Warn_Type;

#else

#ifndef NEWS_POPUP_WARN_H_INCLUDED
#define NEWS_POPUP_WARN_H_INCLUDED

#define NEWS_POPUP_WARN_TIMER_S_MIN 2
#define NEWS_POPUP_WARN_TIMER_S_MAX 60

#define NEWS_POPUP_WARN_EDJE_MESSAGE_SHOW_DESACTIVATE 0
#define NEWS_POPUP_WARN_EDJE_MESSAGE_TYPE 1

#define NEWS_POPUP_WARN_OVERLAP_BORDER 15
#define NEWS_POPUP_WARN_OVERLAP_CHECK_MAX 50

struct _News_Popup_Warn
{
   News_Popup_Warn_Type type;
   E_Popup *pop;
   Evas_Object *face;
  Evas_Object *tb;
  Evas_List *log; /* 2 consecutive strings for each popup */

   Ecore_Timer *timer;
   int (*func_close) (News_Popup_Warn *popw, void *data);
   void (*func_desactivate) (News_Popup_Warn *popw, void *data);
  void *data;

   int timer_org;
   int x, y, w, h;
};

int              news_popup_warn_init(void);
void             news_popup_warn_shutdown(void);

News_Popup_Warn *news_popup_warn_add(int type, const char *title, const char *text, int timer, int (*func_close) (News_Popup_Warn *popw, void *data), void (func_desactivate) (News_Popup_Warn *popw, void *data), void *data);
void             news_popup_warn_del(News_Popup_Warn *popw);

#endif
#endif

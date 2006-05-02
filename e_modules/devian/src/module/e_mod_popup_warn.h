#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Popup_Warn Popup_Warn;

#else

#ifndef E_MOD_POPUP_H_INCLUDED
#define E_MOD_POPUP_H_INCLUDED

#define POPUP_WARN_DEVIAN_ACTIVE_DEFAULT 1

#define POPUP_WARN_TYPE_INFO 0
#define POPUP_WARN_TYPE_NEWS 1
#define POPUP_WARN_TYPE_ERROR 2

#define POPUP_WARN_EDJE_MESSAGE_SHOW_DESACTIVATE 0
#define POPUP_WARN_EDJE_MESSAGE_TYPE 1

#define POPUP_WARN_TIMER_DEFAULT 8
#define POPUP_WARN_TIMER_MAX 60

#define POPUP_WARN_OVERLAP_BORDER 15
#define POPUP_WARN_OVERLAP_CHECK_MAX 50

struct _Popup_Warn
{
   const char *name;
   int type;
   E_Popup *pop;
   Evas_Object *face;
   Evas_List *log;

   Ecore_Timer *timer;
   DEVIANN *devian;
   int (*func_close) (Popup_Warn *popw, void *data);
   void (*func_desactivate) (Popup_Warn *popw, void *data);

   int timer_org;
   int x, y, w, h;
};

int         DEVIANF(popup_warn_init) (void);
void        DEVIANF(popup_warn_shutdown) (void);
Popup_Warn *DEVIANF(popup_warn_add) (int type, const char *text, Popup_Warn *popw_old, int keep_old, int timer, DEVIANN *devian, int (*func_close) (Popup_Warn *popw, void *data), void (*func_desactivate) (Popup_Warn *popw, void *data));
void        DEVIANF(popup_warn_del) (Popup_Warn *popw);
void        DEVIANF(popup_warn_theme_change) (void);

#endif
#endif

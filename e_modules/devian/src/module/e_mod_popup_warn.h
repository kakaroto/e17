#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Popup_Warn Popup_Warn;

#else

#ifndef E_MOD_POPUP_H_INCLUDED
#define E_MOD_POPUP_H_INCLUDED

#define POPUP_WARN_DEVIAN_ACTIVE_DEFAULT 1

#define POPUP_WARN_TYPE_DEVIAN 0
#define POPUP_WARN_TYPE_INFO 1
#define POPUP_WARN_TYPE_INFO_TIMER 2

#define POPUP_WARN_EDJE_MESSAGE_SHOW_DESACTIVATE 0

#define POPUP_WARN_TIMER_DEFAULT 8
#define POPUP_WARN_TIMER_MAX 60

#define POPUP_WARN_OVERLAP_BORDER 15
#define POPUP_WARN_OVERLAP_CHECK_MAX 50

struct _Popup_Warn
{
  int type;

  const char *name;
  E_Popup *pop;
  Evas_List *log;
  Evas_Object *face;
  Ecore_Timer *timer;
  int timer_org;
  int x, y, w, h;
  void *data;
};


int  DEVIANF(popup_warn_add)(Popup_Warn **popup_warn, int type,
			   const char *text, void *data);
void DEVIANF(popup_warn_del)(Popup_Warn *popw);
void DEVIANF(popup_warn_del_all)(void);
void DEVIANF(popup_warn_devian_desactivate)(void);
void DEVIANF(popup_warn_theme_change)(void);

#endif
#endif

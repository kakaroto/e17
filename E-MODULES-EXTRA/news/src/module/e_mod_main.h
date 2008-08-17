#define D_(str) dgettext(PACKAGE, str)

#ifdef E_MOD_NEWS_TYPEDEFS

typedef struct _News News;

#else

#ifndef E_MOD_MAIN_H_INCLUDED
#define E_MOD_MAIN_H_INCLUDED

struct _News
{
   E_Module *module;

   News_Config *config;
   E_Config_Dialog *config_dialog;
   E_Config_Dialog *config_dialog_feeds;
   E_Config_Dialog *config_dialog_feed_new;
   E_Config_Dialog *config_dialog_category_new;
   E_Config_Dialog *config_dialog_langs;

   Evas_List *items;
   Evas_List *langs;
   Ecore_Timer *feeds_timer;
};

News *news;

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int   e_modapi_shutdown(E_Module *m);
EAPI int   e_modapi_save(E_Module *m);

const char *_gc_name(void);

#endif
#endif


#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <e.h>

typedef struct _Config	    Config;
typedef struct _Config_Face Config_Face;
typedef struct _Lang	    Lang;
typedef struct _Lang_Face   Lang_Face;

typedef enum { LS_GLOBAL_POLICY,
	       LS_WINDOW_POLICY,
	       LS_UNKNOWN_POLICY }lang_switch_policy_t;

struct _Config
{
   lang_switch_policy_t	lang_policy;
   int			lang_show_indicator;

   E_Config_Binding_Key *bk_next;
   E_Config_Binding_Key	*bk_prev;

   Evas_List		*languages;
};

struct _Config_Face
{
   unsigned char enabled;
};

struct _Lang
{
   E_Menu      *config_menu;
   Lang_Face   *face;
   Config      *conf;
   E_Module    *module;

   int	       current_lang_selector;

   E_Config_DD	    *conf_edd;
   E_Config_DD	    *conf_bk_next_edd;
   E_Config_DD	    *conf_bk_prev_edd;
   E_Config_Dialog  *cfd;
};

struct _Lang_Face
{
   Evas		*evas;
   E_Container	*con;
   E_Menu	*menu;

   Lang		*lang;

   Config_Face	*conf;
   E_Config_DD	*conf_face_edd;

   Evas_Object	*lang_obj;
   Evas_Object	*text_obj;
   Evas_Object	*event_obj;

   Ecore_Timer	*monitor;

   E_Gadman_Client  *gmc;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);
EAPI int e_modapi_info(E_Module *m);
EAPI int e_modapi_about(E_Module *m);
EAPI int e_modapi_config(E_Module *m);

void lang_face_language_indicator_set(Lang *l);

#endif

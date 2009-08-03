#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <e.h>

/******************* Shelf Code ****************************/
typedef enum { LS_GLOBAL_POLICY,
	       LS_WINDOW_POLICY,
	       LS_APPLICATION_POLICY,
	       LS_UNKNOWN_POLICY }lang_switch_policy_t;

typedef struct _Config	Config;

struct _Config
{
   /* saved * loaded config values */
   lang_switch_policy_t	lang_policy;
   int			lang_show_indicator;

   E_Config_Binding_Key	switch_next_lang_key;
   E_Config_Binding_Key	switch_prev_lang_key;

   Eina_List		*languages; // Language

   /* config state */
   E_Module	     *module;
   E_Config_Dialog   *config_dialog;
   Eina_List	     *instances; // Instance
   E_Menu	     *menu;

   Eina_List	     *handlers;

   /* lang related stuff */
   unsigned int	  language_selector;
   Eina_List	  *language_predef_list; // Language_Predef
   Eina_List	  *language_kbd_model_list; // Language_Kbd_Model

   struct
     { 
	Eina_List *border_lang_setup; // Border_Language_Settings 
	E_Border  *current;
     } l;
};

/********** module api *********************/

EAPI extern E_Module_Api   e_modapi;

EAPI void   *e_modapi_init     (E_Module *m);
EAPI int     e_modapi_shutdown (E_Module *m);
EAPI int     e_modapi_save     (E_Module *m);

/*******************************************/

/************ Just publics *****************/
void language_face_language_indicator_update();

void language_register_callback_handlers();
void language_unregister_callback_handlers();

void language_clear_border_language_setup_list();
/*******************************************/

extern Config *language_config;

#endif


#include "e_mod_lang.h"
#include "e_mod_main.h"

#include <EXML.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

#define EXML_RETURN_ON_ERROR(xml) \
   { \
      exml_destroy(xml); \
      return; \
   }

/******************************************************/

//#define DFLT_XKB_RULES_FILE   "/usr/share/X11/xkb/rules/xorg"
#define DFLT_XKB_RULES_FILE   "/etc/X11/xkb/rules/xfree86"
#define DFLT_XKB_LAYOUT	      "us"
#define DFLT_XKB_MODEL	      "pc101"

#define RULES_INDX	0
#define CONFIG_INDX	1
#define DISPLAY_INDX	2
#define LOCALE_INDX	3
#define MODEL_INDX	4
#define LAYOUT_INDX	5
#define VARIANT_INDX	6
#define KEYCODES_INDX	7
#define TYPES_INDX	8
#define COMPAT_INDX	9
#define SYMBOLS_INDX	10
#define GEOMETRY_INDX	11
#define KEYMAP_INDX	12
#define NUM_OF_INDX	13

#define LXC_FREE(__p) \
   { \
      int i; \
      for (i = 0; i < NUM_OF_INDX; i++) \
	 if (__p->sv[i]) evas_stringshare_del(__p->sv[i]); \
      if (__p->dpy) XCloseDisplay(__p->dpy); \
      E_FREE(__p); \
   }

#define LANG_SETTING_SET_RETURN_ON_ERROR(__p) \
   { \
      LXC_FREE(__p); \
      e_module_dialog_show("Error", "Error: The module was not able to switch between the<br>" \
				    "selected languages. The main reason could be is that X<br>" \
				    "keyboard rules file cannot be loaded propertly, or it <br>" \
				    "contains incorrect rules information. This can happen if<br>" \
				    "you updated your X server to version 7.x. Please fix the<br>" \
				    "X keyboar rules file located via /etc/X11/xkb/rules/xfree86." \
			   ); \
      return; \
   }

#define LXC_SET_STRING(l, c, s) \
   { \
      if (!(l->sv[c])) \
	 l->sv[c] = (!s ? NULL : (char *)evas_stringshare_add(s)); \
   }

typedef struct _Language_Xkb_Config Language_Xkb_Config;

struct _Language_Xkb_Config
{
   Display	     *dpy;
   XkbRF_VarDefsRec  rdefs;
   char		     *sv[NUM_OF_INDX];
};

#if 0
static int _lang_check_name(char *name, char* string);
#endif

static int _lang_apply_components_names(Language_Xkb_Config *lxc);
static int _lang_apply_rules(Language_Xkb_Config *lxc);
static int _lang_server_values_get(Language_Xkb_Config *lxc);
static Display * _lang_server_display_get();
static void _lang_apply_language_settings(const char *model, const char *layout,
					  const char *variant);

/******************************************************/

/************** private ******************/
static void	     _lang_load_xfree_language_kbd_layouts_load_configItem(EXML *xml,
									   Language_Predef *lp);
static void	     _lang_load_xfree_language_kbd_layouts_load_variantList(EXML *xml,
									    Language_Predef *lp);
static void	     _lang_load_xfree_language_register_language(Config *cfg,
					    const char *lang_name, const char *lang_shortcut,
					    const char *lang_flag, const char *kbd_layout,
					    Evas_List *kbd_layout_variant);
static void	     _lang_free_predef_language(Language_Predef *lp);
int		     _lang_predef_language_sort_cb(void *e1, void *e2);
/****************************************/

void
lang_language_switch_to(Config *cfg, unsigned int n, int ignore_n)
{
#define APPLY_LANGUAGE_SETTINGS(__c) \
   { \
      Language *__l = evas_list_nth(__c->languages, __c->language_selector); \
      if (__l) _lang_apply_language_settings(__l->kbd_model, __l->kbd_layout, __l->kbd_variant); \
   }
   
   
   if (!cfg) return;

   if (cfg->lang_policy == LS_GLOBAL_POLICY)
     { 
	if (n < 0) n = 0;
	else if (n >= evas_list_count(cfg->languages))
	  n = evas_list_count(cfg->languages) - 1;

	cfg->language_selector = n; 
        
	APPLY_LANGUAGE_SETTINGS(cfg);
     }
   else if (cfg->lang_policy == LS_WINDOW_POLICY)
     {
	if (ignore_n)
	  { 
	     Evas_List *l;
	     E_Border *bd = NULL;
	     E_Desk *desk;

	     desk = e_desk_current_get
		     (e_zone_current_get(e_container_current_get(e_manager_current_get())));

	     for (l = e_border_focus_stack_get(); l; l = l->next) 
	       { 
		  bd = l->data; 
		  if ((!bd->iconic) && (bd->visible) && 
		      (((bd->desk == desk) ||
		       ((bd->sticky) && (bd->zone == desk->zone))))) 
		    { 
		       break; 
		    } 
		  bd = NULL;
	       }

	     cfg->language_selector = 0;
	     if (bd)
	       {
		  // here we have to set the language used by the window
		  // if at all we have to do this ???? QUESTION - because,
		  // if we just switch the desktop and we have focused window on it
		  // them most probably !!! the focus_in callback will activate
		  Border_Language_Settings *bls = NULL;

		  for (l = cfg->border_lang_setup; l; l = l->next)
		    {
		       bls = l->data;

		       if (bls->bd == bd)
			 {
			    // so we found a window with border settings set
			    // 
			    Language *lang = evas_list_nth(cfg->languages,
							   bls->language_selector);
			    if (strcmp(bls->language_name, lang->lang_name))
			      {
				 // here we will be smart :) we will try first to
				 // find correct language settings and if we fail
				 // then to fallback to default language

				 bls->language_selector = 0;

				 /*Evas_List *l2;
				 int j = 0;
				 for (l2 = cfg->languages; l2; l2 = l2->next, j++)
				   {
				      lang = l->data;
				      if (!strcmp(bls->language_name, lang->lang_name))
					break;
				   }

				 if (j) 
				   bls->language_selector = j;
				 else
				   {
				      //hhhhmmm the language_settings of the border is incorrect.
				      //remove them. and fallback to default language.
				      if (bls->language_name)
					evas_stringshare_del(bls->language_name);
				      E_FREE(bls);
				      cfg->border_lang_setup = evas_list_remove_list
					 (cfg->border_lang_setup, l);
				   }*/
			      }
			    break;
			 }
		       bls = NULL;
		    }

		  if (bls) 
		    cfg->language_selector = bls->language_selector;
	       } 
	     APPLY_LANGUAGE_SETTINGS(cfg);
	  }
	else
	  { 
	     Evas_List *l;
	     E_Border *bd = NULL;

	     if (n >= evas_list_count(cfg->languages))
	       n = evas_list_count(cfg->languages) - 1;

	     bd = e_border_focused_get();

	     if (bd)
	       {
		  Evas_List *l;
		  Border_Language_Settings *bls; 
		  Language *lang;
		  // we have a border. Let find it in the list of borders that have
		  // language settings.

		  cfg->language_selector = n;
		  lang = evas_list_nth(cfg->languages, n);

		  bls = NULL;
		  for (l = cfg->border_lang_setup; l; l = l->next)
		    {
		       bls = l->data;
		       if (bls && (bls->bd = bd))
			 break;
		       bls = NULL;
		    }

		  if (bls)
		    {
		       // kewl (ooohhh devilhorns :)) we have a border with language 
		       // settings set. Updata them
		       bls->language_selector = n;
		       if (bls->language_name) evas_stringshare_del(bls->language_name);
		       bls->language_name = evas_stringshare_add(lang->lang_name);
		    }
		  else
		    {
		       // there is no window.
		       if (n) 
			 {
			    // we need to save none default lang settings
			    bls = E_NEW(Border_Language_Settings, 1);
			    bls->bd = bd;
			    bls->language_selector = n;
			    bls->language_name = evas_stringshare_add(lang->lang_name);

			    cfg->border_lang_setup = evas_list_append(cfg->border_lang_setup, bls);
			 }
		    }
	       }
	     else
	       cfg->language_selector = n;

	     APPLY_LANGUAGE_SETTINGS(cfg);
		  //e_module_dialog_show("Warning", "Focused window on desktop");
	  }
     }
   else if (cfg->lang_policy == LS_APPLICATION_POLICY)
     {
	//e_module_dialog_show("Warning" , "Warning : This feature is not implemented yet.");
     }

   language_face_language_indicator_update();
}
void
lang_language_switch_to_next(Config *cfg)
{
   int	 size;
   if (!cfg) return;

   size = evas_list_count(cfg->languages);
   if (size <= 1) return;

   if (cfg->language_selector >= size - 1)
     lang_language_switch_to(cfg, 0, 0);
   else
     lang_language_switch_to(cfg, cfg->language_selector + 1, 0);
}
void
lang_language_switch_to_prev(Config *cfg)
{
   int size;
   if (!cfg) return;

   size = evas_list_count(cfg->languages);
   if (size <= 1) return;

   if (cfg->language_selector == 0)
     lang_language_switch_to(cfg, size - 1, 0);
   else
     lang_language_switch_to(cfg, cfg->language_selector - 1, 0);
}
Language *
lang_get_default_language(Config *cfg)
{
   Evas_List   *l;
   Language    *lang = NULL;

   for (l = cfg->language_predef_list; l; l = l->next)
     {
	Language_Predef	*lp = l->data;

	if (!strcmp(lp->kbd_layout, "us"))
	  {
	     lang = E_NEW(Language, 1);
	     if (!lang)
	       break;

	     lang->id = 0; 
	     lang->lang_name = evas_stringshare_add(lp->lang_name); 
	     lang->lang_shortcut = evas_stringshare_add(lp->lang_shortcut); 
	     lang->lang_flag = evas_stringshare_add(lp->lang_flag); 
	     lang->kbd_model = lang_language_current_kbd_model_get();
	     lang->kbd_layout = evas_stringshare_add(lp->kbd_layout); 
	     lang->kbd_variant = evas_stringshare_add("basic"); 
	     break; 
	  }
     }

   return lang;
}
void 
lang_load_xfree_language_kbd_layouts(Config *cfg)
{
   EXML	 *exml;
   int	 found;

   if (!cfg) return;

   exml = exml_new();

   if (!exml) return;
   if (!exml_init(exml)) EXML_RETURN_ON_ERROR(exml);

   if (!exml_file_read(exml, "/usr/lib/X11/xkb/rules/xfree86.xml"))
     EXML_RETURN_ON_ERROR(exml);

   exml_down(exml);

   found = 0;
   while (!found)
     {
	if (!strcasecmp(exml_tag_get(exml), "layoutList"))
	  found = 1;
	else
	  {
	     if (!exml_next_nomove(exml))
	       break;
	  }
     }

   if (!found)
     EXML_RETURN_ON_ERROR(exml);

   exml_down(exml);

   if (strcasecmp(exml_tag_get(exml), "layout"))
     EXML_RETURN_ON_ERROR(exml);

   while (1)
     {
	Language_Predef	*lp;
	EXML_Node	*curren_layout_node = exml_get(exml);

	lp = E_NEW(Language_Predef, 1);
	if (lp)
	  {
	     exml_down(exml);

	     while (1)
	       {
		  _lang_load_xfree_language_kbd_layouts_load_configItem(exml, lp);
		  _lang_load_xfree_language_kbd_layouts_load_variantList(exml, lp);

		  if (!exml_next_nomove(exml))
		    break;
	       }

	     if (lp->lang_name && lp->lang_shortcut && lp->kbd_layout) 
	       _lang_load_xfree_language_register_language(cfg, lp->lang_name, lp->lang_shortcut,
							   lp->lang_flag, lp->kbd_layout,
							   lp->kbd_variant);
	     _lang_free_predef_language(lp);
	  }
	exml_goto_node(exml, curren_layout_node);
	if (!exml_next_nomove(exml)) 
	  break;
     }
   exml_destroy(exml);
}
void 
lang_free_xfree_language_kbd_layouts(Config *cfg)
{
   Language_Predef   *lp;

   if (!cfg) return;

   while (cfg->language_predef_list)
     { 
	_lang_free_predef_language(cfg->language_predef_list->data);
	cfg->language_predef_list = evas_list_remove_list(cfg->language_predef_list,
							  cfg->language_predef_list);
     }
}
void 
lang_load_xfree_kbd_models(Config *cfg)
{
   EXML	 *exml;
   int	 found;

   exml = exml_new();

   if (!exml) return;
   if (!exml_init(exml))
     EXML_RETURN_ON_ERROR(exml);

   if (!exml_file_read(exml, "/usr/lib/X11/xkb/rules/xfree86.xml"))
     EXML_RETURN_ON_ERROR(exml);

   if (strcasecmp(exml_tag_get(exml), "xkbConfigRegistry"))
     EXML_RETURN_ON_ERROR(exml);

   exml_down(exml);

   found = 0;
   while (!found)
     {
	if (!strcasecmp(exml_tag_get(exml), "modelList"))
	  found = 1;
	else
	  {
	     if (!exml_next_nomove(exml))
	       break;
	  }
     }

   if (!found)
     EXML_RETURN_ON_ERROR(exml);

   exml_down(exml);

   if (strcasecmp(exml_tag_get(exml), "model"))
     EXML_RETURN_ON_ERROR(exml);

   while (1)
     {
	Language_Kbd_Model *lkm;
	EXML_Node	   *current_model_node = exml_get(exml);

	exml_down(exml);
	if (!strcasecmp(exml_tag_get(exml), "configItem"))
	  { 
	     lkm = E_NEW(Language_Kbd_Model, 1); 
	     if (lkm) 
	       { 
		  exml_down(exml); 
		  
		  while (1) 
		    { 
		       char *tag = exml_tag_get(exml); 
		       
		       if (!strcasecmp(tag, "name")) 
			 lkm->kbd_model = evas_stringshare_add(exml_value_get(exml)); 
		       
		       if (!strcasecmp(tag, "description")) 
			 { 
			    char *attr = exml_attribute_get(exml, "xml:lang"); 
			    if (!attr) 
			      lkm->kbd_model_desctiption = 
						      evas_stringshare_add(exml_value_get(exml)); 
			 } 
		       
		       if (lkm->kbd_model && lkm->kbd_model_desctiption) 
			 break; 
		       
		       if (!exml_next_nomove(exml)) 
			 break; 
		    } 
		  
		  if (lkm->kbd_model && lkm->kbd_model_desctiption) 
		    cfg->language_kbd_model_list = evas_list_append(cfg->language_kbd_model_list,
								    lkm); 
		  else 
		    { 
		       if (lkm->kbd_model) evas_stringshare_del(lkm->kbd_model); 
		       if (lkm->kbd_model_desctiption) 
			 evas_stringshare_del(lkm->kbd_model_desctiption); 
		       E_FREE(lkm); 
		    } 
	       }
	  }
	exml_goto_node(exml, current_model_node);
	if (!exml_next_nomove(exml))
	  break;
     }
   exml_destroy(exml);
}
void 
lang_free_xfree_kbd_models(Config *cfg)
{
   Language_Kbd_Model	*lkm;

   if (!cfg) return;

   while (cfg->language_kbd_model_list)
     {
	lkm = cfg->language_kbd_model_list->data;

	if (lkm->kbd_model) evas_stringshare_del(lkm->kbd_model);
	if (lkm->kbd_model_desctiption) evas_stringshare_del(lkm->kbd_model_desctiption);
	E_FREE(lkm);
	cfg->language_kbd_model_list = evas_list_remove_list(cfg->language_kbd_model_list,
							     cfg->language_kbd_model_list);
     }
   cfg->language_kbd_model_list = NULL;
}

Language *
lang_language_copy(const Language *l)
{
   Language *lang;

   if (!l) return NULL;

   lang = E_NEW(Language, 1);
   if (!lang) return NULL;

   lang->id = l->id;
   lang->lang_name = l->lang_name ? evas_stringshare_add(l->lang_name) : NULL;
   lang->lang_shortcut = l->lang_shortcut ? evas_stringshare_add(l->lang_shortcut) : NULL;
   lang->lang_flag = l->lang_flag ? evas_stringshare_add(l->lang_flag) : NULL;
   lang->kbd_model = l->kbd_model ? evas_stringshare_add(l->kbd_model) : NULL;
   lang->kbd_layout = l->kbd_layout ? evas_stringshare_add(l->kbd_layout) : NULL;
   lang->kbd_variant = l->kbd_variant ? evas_stringshare_add(l->kbd_variant) : NULL;

   return lang;
}

void
lang_language_free(Language *l)
{
   if (!l) return;

   if (l->lang_name) evas_stringshare_del(l->lang_name);
   if (l->lang_shortcut) evas_stringshare_del(l->lang_shortcut);
   if (l->lang_flag) evas_stringshare_del(l->lang_flag);
   if (l->kbd_model) evas_stringshare_del(l->kbd_model);
   if (l->kbd_layout) evas_stringshare_del(l->kbd_layout);
   if (l->kbd_variant) evas_stringshare_del(l->kbd_variant);
   E_FREE(l);
}
const char *
lang_language_current_kbd_model_get()
{
   //FIXME: make the function return the actual keyboard model
   return evas_stringshare_add("compaqik13");
} 

/************** private ******************/
static void
_lang_load_xfree_language_kbd_layouts_load_configItem(EXML *xml, Language_Predef *lp)
{
   EXML_Node   *current_node;
   if (!xml || !lp) return;

   if (strcasecmp(exml_tag_get(xml), "configItem")) return;

   current_node = exml_get(xml);

   exml_down(xml);
   while (1)
     {
	char *tag = exml_tag_get(xml);

	if (!strcasecmp(tag, "name"))
	  { 
	     if (lp->kbd_layout) evas_stringshare_del(lp->kbd_layout);
	     lp->kbd_layout = evas_stringshare_add(exml_value_get(xml));
	  }

	if (!strcasecmp(tag, "shortDescription"))
	  {
	     char *attr = exml_attribute_get(xml, "xml:lang");
	     if (!attr)
	       {
		  int i;
		  char buf[1024];
		  char *ls = strdup(exml_value_get(xml));

		  if (lp->lang_shortcut) evas_stringshare_del(lp->lang_shortcut);

		  if (ls)
		    {
		       for (i = 0; ls[i]; i++)
			 ls[i] = (char)toupper(ls[i]);
		       lp->lang_shortcut = evas_stringshare_add(ls);
		    }

		  if (lp->lang_flag) evas_stringshare_del(lp->lang_flag);
		  snprintf(buf, sizeof(buf), "%s_flag", ls ? ls : "language");
		  lp->lang_flag = evas_stringshare_add(buf);
	       }
	  }

	if (!strcasecmp(tag, "description"))
	  {
	     char *attr = exml_attribute_get(xml, "xml:lang");
	     if (!attr)
	       {
		  if (lp->lang_name) evas_stringshare_del(lp->lang_name);
		  lp->lang_name = evas_stringshare_add(exml_value_get(xml));
	       }
	  }

	if (lp->lang_name && lp->lang_shortcut && lp->kbd_layout)
	  break;

	if (!exml_next_nomove(xml))
	  break;
     }
   exml_goto_node(xml, current_node);
}
static void 
_lang_load_xfree_language_kbd_layouts_load_variantList(EXML *xml, Language_Predef *lp)
{
   EXML_Node   *current_node;
   if (!xml || !lp) return;

   if (strcasecmp(exml_tag_get(xml), "variantList")) return;

   current_node = exml_get(xml);

   if (!exml_down(xml)) return;

   while (1)
     {
	if (!strcasecmp(exml_tag_get(xml), "variant"))
	  {
	     EXML_Node *current_variant_node = exml_get(xml);

	     exml_down(xml);
	     if (!strcasecmp(exml_tag_get(xml), "configItem"))
	       {
		  exml_down(xml);
		  while (1)
		    {
		       char *tag = exml_tag_get(xml);
		       if (!strcasecmp(tag, "name"))
			 {
			    lp->kbd_variant = evas_list_append(lp->kbd_variant,
							       exml_value_get(xml));
			    break;
			 }
		       if (!exml_next_nomove(xml))
			 break;
		    }
	       }
	     exml_goto_node(xml, current_variant_node);
	  }
	if (!exml_next_nomove(xml))
	  break;
     }
   exml_goto_node(xml, current_node);
}
static void
_lang_load_xfree_language_register_language(Config *cfg,
					    const char *lang_name, const char *lang_shortcut,
					    const char *lang_flag, const char *kbd_layout,
					    Evas_List *kbd_layout_variant)
{
   Language_Predef   *lp;
   Evas_List	     *l;
   int		     found = 0;
   int		     i;

   if (!lang_name || !lang_shortcut || !kbd_layout) return;

   for (l = cfg->language_predef_list; l && !found; l = l->next)
     {
	lp = l->data;

	if (!strcmp(lp->lang_name, lang_name))
	  found = 1;
     }

   if (found)
     {
	if (kbd_layout_variant)
	  {
	     for (l = kbd_layout_variant; l; l = l->next)
	       lp->kbd_variant = evas_list_append(lp->kbd_variant, evas_stringshare_add(l->data));
	  }
	return;
     }
   else
     {
	lp = E_NEW(Language_Predef, 1);
	if (!lp) return;

	lp->lang_name = evas_stringshare_add(lang_name);
	lp->lang_shortcut = evas_stringshare_add(lang_shortcut);
	lp->lang_flag = !lang_flag ? NULL : evas_stringshare_add(lang_flag);
	lp->kbd_layout = evas_stringshare_add(kbd_layout);

	if (kbd_layout_variant)
	  {
	     for (l = kbd_layout_variant; l; l = l->next)
	       lp->kbd_variant = evas_list_append(lp->kbd_variant, evas_stringshare_add(l->data));
	  }
     }

   cfg->language_predef_list = evas_list_append(cfg->language_predef_list, lp);

   cfg->language_predef_list = evas_list_sort(cfg->language_predef_list,
					      evas_list_count(cfg->language_predef_list),
					      _lang_predef_language_sort_cb);
}
static void
_lang_free_predef_language(Language_Predef *lp)
{
   if (!lp) return;

   if (lp->lang_name) evas_stringshare_del(lp->lang_name);
   if (lp->lang_shortcut) evas_stringshare_del(lp->lang_shortcut);
   if (lp->lang_flag) evas_stringshare_del(lp->lang_flag);
   if (lp->kbd_layout) evas_stringshare_del(lp->kbd_layout);
   while (lp->kbd_variant)
     {
	if (lp->kbd_variant->data)
	  evas_stringshare_del(lp->kbd_variant->data);
	lp->kbd_variant = evas_list_remove_list(lp->kbd_variant, lp->kbd_variant);
     }
   E_FREE(lp);
}
int 
_lang_predef_language_sort_cb(void *e1, void *e2)
{
   Language_Predef   *lp1 = e1;
   Language_Predef   *lp2 = e2;

   if (!e1) return 1;
   if (!e2) return -1;

   return strcmp((const char *)lp1->lang_name, (const char *)lp2->lang_name);
}
/****************************************/

static void
_lang_apply_language_settings(const char *model, const char *layout,
			      const char *variant)
{
   Language_Xkb_Config	*lxc;

   if (!layout) return;

   lxc = E_NEW(Language_Xkb_Config, 1);
   if (!lxc) return;

   LXC_SET_STRING(lxc, MODEL_INDX, model);
   LXC_SET_STRING(lxc, LAYOUT_INDX, layout);
   LXC_SET_STRING(lxc, VARIANT_INDX, variant);

   if (!(lxc->dpy = _lang_server_display_get()))
     LANG_SETTING_SET_RETURN_ON_ERROR(lxc);

   // this is a hack of locale.
   lxc->sv[LOCALE_INDX] = (char *)evas_stringshare_add("C");

   if (!_lang_server_values_get(lxc))
     LANG_SETTING_SET_RETURN_ON_ERROR(lxc);

   if (!_lang_apply_rules(lxc))
     LANG_SETTING_SET_RETURN_ON_ERROR(lxc);

   if (!_lang_apply_components_names(lxc))
     LANG_SETTING_SET_RETURN_ON_ERROR(lxc);

   LXC_FREE(lxc);
}
static Display *
_lang_server_display_get()
{
   int	    major, minor, why;
   char	    *display;

   major = XkbMajorVersion;
   minor = XkbMinorVersion;

   display = getenv("DISPLAY");

   return XkbOpenDisplay(display, NULL, NULL, &major, &minor, &why);
}

static int
_lang_server_values_get(Language_Xkb_Config *lxc)
{
   XkbRF_VarDefsRec  vd;
   char		     *tmp = NULL;

   if (!lxc) return 0;

   if (!XkbRF_GetNamesProp(lxc->dpy, &tmp, &vd) || !tmp)
     {
	tmp	     = DFLT_XKB_RULES_FILE;
	vd.model     = DFLT_XKB_MODEL;
	vd.layout    = DFLT_XKB_LAYOUT;
	vd.variant   = NULL;
	vd.options   = NULL;
     }

   if (tmp) LXC_SET_STRING(lxc, RULES_INDX, tmp);
   if (vd.model) LXC_SET_STRING(lxc, MODEL_INDX, vd.model);
   if (vd.layout) LXC_SET_STRING(lxc, LAYOUT_INDX, vd.layout);
   if (vd.variant) LXC_SET_STRING(lxc, VARIANT_INDX, vd.variant);

   if (vd.options) XFree(vd.options);

   return 1;
}

static int
_lang_apply_rules(Language_Xkb_Config *lxc)
{
   //FIXME: rfName is a hack it points to /usr/share/X11/xkb/rules/xorg
   //I'm not sure that this file exist in all the versions of X
   //Mostprobably this path should be /usr/X11R6/lib/X11/xkb/rules/xfree86
   char			*rfName = DFLT_XKB_RULES_FILE;
   XkbComponentNamesRec	rnames;
   XkbRF_RulesPtr	rules = NULL;

   if (!lxc) return 0;

   lxc->rdefs.model	= lxc->sv[MODEL_INDX];
   lxc->rdefs.layout	= lxc->sv[LAYOUT_INDX];
   lxc->rdefs.variant	= lxc->sv[VARIANT_INDX];

   rules = XkbRF_Load(rfName, lxc->sv[LOCALE_INDX], True, True);

   if (!rules)
     return 0;

   XkbRF_GetComponents(rules, &(lxc->rdefs), &rnames);

   if (rnames.keycodes)
     { 
	LXC_SET_STRING(lxc, KEYCODES_INDX, rnames.keycodes);
	rnames.keycodes = NULL;
	//E_FREE(rnames.keycodes);
     }
   if (rnames.symbols)
     {
	LXC_SET_STRING(lxc, SYMBOLS_INDX, rnames.symbols);
	rnames.symbols = NULL;
	//E_FREE(rnames.symbols);
     }
   if(rnames.types)
     {
	LXC_SET_STRING(lxc, TYPES_INDX, rnames.types);
	rnames.types = NULL;
	//E_FREE(rnames.types);
     }
   if (rnames.compat)
     {
	LXC_SET_STRING(lxc, COMPAT_INDX, rnames.compat);
	rnames.compat = NULL;
	//E_FREE(rnames.compat);
     }
   if (rnames.geometry)
     {
	LXC_SET_STRING(lxc, GEOMETRY_INDX, rnames.geometry);
	rnames.geometry = NULL;
	//E_FREE(rnames.geometry);
     }
   if (rnames.keymap)
     {
	LXC_SET_STRING(lxc, KEYMAP_INDX, rnames.keymap);
	rnames.keymap = NULL;
	//E_FREE(rnames.keymap);
     }
   return 1;
}
static int
_lang_apply_components_names(Language_Xkb_Config *lxc)
{
   XkbDescPtr		xkb = NULL;
   XkbComponentNamesRec	cmdNames;

   if (!lxc) return 0;

#if 0
   if (!_lang_check_name(lxc->sv[TYPES_INDX], "types")) { printf("\n[1]\n");return 0;}
   if (!_lang_check_name(lxc->sv[COMPAT_INDX], "compat")) { printf("\n[2]\n");return 0;}
   if (!_lang_check_name(lxc->sv[SYMBOLS_INDX], "symbols")) { printf("\n[3]\n");return 0;}
   if (!_lang_check_name(lxc->sv[KEYCODES_INDX], "keycodes")) { printf("\n[4]\n");return 0;}
   if (!_lang_check_name(lxc->sv[GEOMETRY_INDX], "geometry")) { printf("\n[5]\n");return 0;}
   if (!_lang_check_name(lxc->sv[KEYMAP_INDX], "keymap")) { printf("\n[6]\n");return 0;}
#endif

   memset(&cmdNames, 0, sizeof(XkbComponentNamesRec));

   cmdNames.types    = lxc->sv[TYPES_INDX];
   cmdNames.compat   = lxc->sv[COMPAT_INDX];
   cmdNames.symbols  = lxc->sv[SYMBOLS_INDX];
   cmdNames.keycodes = lxc->sv[KEYCODES_INDX];
   cmdNames.geometry = lxc->sv[GEOMETRY_INDX];
   cmdNames.keymap   = lxc->sv[KEYMAP_INDX];

   xkb = XkbGetKeyboardByName(lxc->dpy, XkbUseCoreKbd, &cmdNames,
			      XkbGBN_AllComponentsMask,
			      XkbGBN_AllComponentsMask & (~XkbGBN_GeometryMask), 1);

   if (!xkb)
     {
	return 0;
     }

   if (lxc->rdefs.model || lxc->rdefs.layout)
     {
	if (!XkbRF_SetNamesProp(lxc->dpy, DFLT_XKB_RULES_FILE, &(lxc->rdefs)))
	  {
	     return 0;
	  }
     }
   return 1;
}

/********************** event callbacks ***************************************/
int
lang_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   Config	     *conf;
   Evas_List	     *l;
   E_Border	     *bd;

   if (!(conf = data)) return 1;

   ev = event;
   // Actually this code should be executed only if WINDOW ir APPLICATION policy
   // is used.
   if (conf->lang_policy == LS_GLOBAL_POLICY)
     { 
	e_module_dialog_show("Warning", "Warning: This is a bug in the code. This message<br>"
				        "should in this context when GLOBAL policy is used.<br>"
					"Please report this behaviour.");
	return 1;
     }

   for (l = e_border_focus_stack_get(); l; l = l->next) 
     { 
	bd = l->data; 
	if ((!bd->iconic) && (bd->visible) && 
	    (((bd->desk == ev->desk) ||
	     ((bd->sticky) && (bd->zone == ev->desk->zone))))) 
	  { 
	     break; 
	  } 
	bd = NULL; 
     }

   if (!bd)
     lang_language_switch_to(conf, 0, 0);

   return 1;
}

int lang_cb_event_border_focus_in(void *data, int type, void *ev)
{
   E_Event_Border_Focus_In *e;

   e = ev;

   lang_language_switch_to(data, 0, 1);
   return 1;
}

int lang_cb_event_border_remove(void *data, int type, void *ev)
{
   E_Event_Border_Remove      *e;
   Border_Language_Settings   *bls;

   Evas_List   *l;
   Config      *cfg;

   e = ev;
   cfg = data;

   for (l = cfg->border_lang_setup; l; l = l->next)
     {
	bls = l->data;

	if (bls->bd == e->border)
	  {
	     cfg->border_lang_setup = evas_list_remove(cfg->border_lang_setup, bls);
	     if (bls->language_name) evas_stringshare_del(bls->language_name);
	     E_FREE(bls);
	     break;
	  }
     }

   if (!e_border_focused_get())
     lang_language_switch_to(cfg, 0, 0);

   return 1;
}

int
lang_cb_event_border_iconify(void *data, int type, void *ev)
{
   E_Event_Border_Iconify *e;

   e = ev;

   if (!e_border_focused_get())
     lang_language_switch_to(data, 0, 0);

   return;
}

#if 0
int
lang_cb_event_border_iconify(void *data, int type, void *event)
{
  // e_module_dialog_show("1", "lang_cb_event_border_iconify");
   return 1;
}
int
lang_cb_event_border_uniconify(void *data, int type, void *event)
{
   //e_module_dialog_show("1", "lang_cb_event_border_uniconify");
   return 1;
}
int
lang_cb_event_border_zone_set(void *data, int type, void *event)
{
   //e_module_dialog_show("1", "lang_cb_event_border_zone_set");
   return 1;
}
int
lang_cb_event_border_desk_set(void *data, int type, void *event)
{
   //e_module_dialog_show("1", "lang_cb_event_border_desk_set");
   return 1;
}
int
lang_cb_event_border_hide(void *data, int type, void *event)
{
   //e_module_dialog_show("1", "lang_cb_event_border_hide");
   return 1;
}
#endif

/******************************************************************************/

#if 0
static int 
_lang_check_name(char *name, char* string)
{
   char *i = name, *opar = NULL;
   int ret = 1; 

   if(!name)
      return 1;

   while (*i){
      if (opar == NULL) {
         if (*i == '(')
         opar = i;
      } else {
         if ((*i == '(') || (*i == '|') || (*i == '+')) {
             ret = 0;
             break;
         }
         if (*i == ')')
             opar = NULL;
      }
      i++;
   }
   if (opar)
      ret = 0;
   if (!ret) {
      char c;
      int n = 1;
      for(i = opar+1; *i && n; i++) {
         if (*i == '(') n++;
         if (*i == ')') n--;
      }
      if (*i) i++;
      c = *i;
      *i = '\0';
      printf("Illegal map name '%s' ", opar);
      *i = c;
      printf("in %s name '%s'\n", string, name);
   }
   return ret;
}
#endif

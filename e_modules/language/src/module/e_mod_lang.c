
#include "e_mod_lang.h"
#include "e_mod_main.h"
#include <EXML.h>

#define EXML_RETURN_ON_ERROR(xml) \
   { \
      exml_destroy(xml); \
      return; \
   }

/******************************************************/

#ifdef __FreeBSD__
   #define DFLT_XKB_RULES_FILE   "/usr/X11R6/lib/X11/xkb/rules/xfree86"
#else
   #define DFLT_XKB_RULES_FILE   "/etc/X11/xkb/rules/xfree86"
#endif // __FreeBSD__
#define DFLT_XKB_LAYOUT	      "us"
#define DFLT_XKB_MODEL	      "pc101"

static int  _lang_apply_language_conponent_names(Language *l);
static void _lang_apply_language_settings(Language *l);

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
lang_language_switch_to(Config *cfg, unsigned int n)
{
#define APPLY_LANGUAGE_SETTINGS(__c) \
     { \
	Language *__l = evas_list_nth(__c->languages, __c->language_selector); \
	if (__l) _lang_apply_language_settings(__l); \
     }
   
   
   if (!cfg) return;
   if (!cfg->languages)
     {
	language_face_language_indicator_update();
	return;
     }

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
	E_Border *bd = NULL;

	if (n >= evas_list_count(cfg->languages)) 
	  n = evas_list_count(cfg->languages) - 1;

        bd = e_border_focused_get();

	if (bd)
	  {
	     Evas_List *l;
	     Border_Language_Settings *bls; 
	     Language *lang;

	     cfg->language_selector = n;
	     lang = evas_list_nth(cfg->languages, n);

	     bls = NULL;
	     for (l = cfg->l.border_lang_setup; l; l = l->next)
	       {
	          bls = l->data;
	          if (bls && (bls->bd = bd))
		    { 
		       if (!n)
			 {
			    if (bls->language_name) evas_stringshare_del(bls->language_name);
			    E_FREE(bls);
			    cfg->l.border_lang_setup = evas_list_remove_list(
								     cfg->l.border_lang_setup, l);
			 }
		       break;
		    }
	          bls = NULL;
	       }

	     if (bls)
	       {
	          bls->language_selector = n;
	          if (bls->language_name) evas_stringshare_del(bls->language_name);
	          bls->language_name = evas_stringshare_add(lang->lang_name);
		  cfg->l.current = bd;
	       }
	     else
	       {
		  cfg->l.current = NULL;
	          if (n) 
	            {
		       bls = E_NEW(Border_Language_Settings, 1); 
		       bls->bd = bd; 
		       bls->language_selector = n; 
		       bls->language_name = evas_stringshare_add(lang->lang_name); 
		       
		       cfg->l.border_lang_setup = evas_list_append(cfg->l.border_lang_setup, bls);
	            }
	       }
	  } 
	else 
	  cfg->language_selector = n; 
	
	APPLY_LANGUAGE_SETTINGS(cfg);
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
     lang_language_switch_to(cfg, 0);
   else
     lang_language_switch_to(cfg, cfg->language_selector + 1);
}
void
lang_language_switch_to_prev(Config *cfg)
{
   int size;
   if (!cfg) return;

   size = evas_list_count(cfg->languages);
   if (size <= 1) return;

   if (cfg->language_selector == 0)
     lang_language_switch_to(cfg, size - 1);
   else
     lang_language_switch_to(cfg, cfg->language_selector - 1);
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

	     lang->id		 = 0; 
	     lang->lang_name	 = evas_stringshare_add(lp->lang_name); 
	     lang->lang_shortcut = evas_stringshare_add(lp->lang_shortcut); 
	     lang->lang_flag	 = evas_stringshare_add(lp->lang_flag); 

	     lang->rdefs.model	 = (char *) lang_language_current_kbd_model_get();
	     lang->rdefs.layout	 = (char *) evas_stringshare_add(lp->kbd_layout); 
	     lang->rdefs.variant = (char *) evas_stringshare_add("basic"); 
	     lang_language_xorg_values_get(lang);
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
   int	 result;

   if (!cfg) return;

   exml = exml_new();

   if (!exml) return;
   if (!exml_init(exml)) EXML_RETURN_ON_ERROR(exml);

   result = exml_file_read(exml, "/etc/X11/xkb/rules/xfree86.xml");
   if (!result || result == -1)
     { 
#ifdef __FreeBSD__
	result = exml_file_read(exml, "/usr/X11R6/lib/X11/xkb/rules/xfree86.xml");
#else
	result = exml_file_read(exml, "/usr/lib/X11/xkb/rules/xfree86.xml");
#endif // __FreeBSD__
	if (!result || result == -1) 
	  EXML_RETURN_ON_ERROR(exml);
     }

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
   int	 result;

   exml = exml_new();

   if (!exml) return;
   if (!exml_init(exml))
     EXML_RETURN_ON_ERROR(exml);

   result = exml_file_read(exml, "/etc/X11/xkb/rules/xfree86.xml");
   if (!result || result == -1)
     { 
#ifdef __FreeBSD__
	result = exml_file_read(exml, "/usr/X11R6/lib/X11/xkb/rules/xfree86.xml");
#else
	result = exml_file_read(exml, "/usr/lib/X11/xkb/rules/xfree86.xml");
#endif // __FreeBSD__
	if (!result || result == -1) 
	  EXML_RETURN_ON_ERROR(exml);
     }

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
			      lkm->kbd_model_description = 
						      evas_stringshare_add(exml_value_get(exml)); 
			 } 
		       
		       if (lkm->kbd_model && lkm->kbd_model_description) 
			 break; 
		       
		       if (!exml_next_nomove(exml)) 
			 break; 
		    } 
		  
		  if (lkm->kbd_model && lkm->kbd_model_description) 
		    cfg->language_kbd_model_list = evas_list_append(cfg->language_kbd_model_list,
								    lkm); 
		  else 
		    { 
		       if (lkm->kbd_model) evas_stringshare_del(lkm->kbd_model); 
		       if (lkm->kbd_model_description) 
			 evas_stringshare_del(lkm->kbd_model_description); 
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
	if (lkm->kbd_model_description) evas_stringshare_del(lkm->kbd_model_description);
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

   lang->id	       = l->id;
   lang->lang_name     = l->lang_name ? evas_stringshare_add(l->lang_name) : NULL;
   lang->lang_shortcut = l->lang_shortcut ? evas_stringshare_add(l->lang_shortcut) : NULL;
   lang->lang_flag     = l->lang_flag ? evas_stringshare_add(l->lang_flag) : NULL;

   lang->rdefs.model   = !l->rdefs.model ? NULL : (char *) evas_stringshare_add(l->rdefs.model);
   lang->rdefs.layout  = !l->rdefs.layout ? NULL : (char *) evas_stringshare_add(l->rdefs.layout);
   lang->rdefs.variant = !l->rdefs.variant ? NULL : (char *) evas_stringshare_add(l->rdefs.variant);

   lang->cNames.keycodes = !l->cNames.keycodes ? NULL :
						(char *) evas_stringshare_add(l->cNames.keycodes);
   lang->cNames.symbols	 = !l->cNames.symbols ? NULL :
						(char *) evas_stringshare_add(l->cNames.symbols);
   lang->cNames.types	 = !l->cNames.types ? NULL :
						(char *) evas_stringshare_add(l->cNames.types);
   lang->cNames.compat	 = !l->cNames.compat ? NULL :
						(char *) evas_stringshare_add(l->cNames.compat);
   lang->cNames.geometry = !l->cNames.geometry ? NULL :
						(char *) evas_stringshare_add(l->cNames.geometry);
   lang->cNames.keymap = !l->cNames.keymap ? NULL :
						(char *) evas_stringshare_add(l->cNames.keymap);

   return lang;
}

void
lang_language_free(Language *l)
{
   if (!l) return;

   if (l->lang_name) evas_stringshare_del(l->lang_name);
   if (l->lang_shortcut) evas_stringshare_del(l->lang_shortcut);
   if (l->lang_flag) evas_stringshare_del(l->lang_flag);

   if (l->rdefs.model) evas_stringshare_del(l->rdefs.model);
   if (l->rdefs.layout) evas_stringshare_del(l->rdefs.layout);
   if (l->rdefs.variant) evas_stringshare_del(l->rdefs.variant);

   if (l->cNames.keycodes) evas_stringshare_del(l->cNames.keycodes);
   if (l->cNames.symbols) evas_stringshare_del(l->cNames.symbols);
   if (l->cNames.types) evas_stringshare_del(l->cNames.types);
   if (l->cNames.compat) evas_stringshare_del(l->cNames.compat);
   if (l->cNames.geometry) evas_stringshare_del(l->cNames.geometry);
   if (l->cNames.keymap) evas_stringshare_del(l->cNames.keymap);

   E_FREE(l);
}
const char *
lang_language_current_kbd_model_get()
{
   XkbRF_VarDefsRec vd;
   char *tmp;

   if (!XkbRF_GetNamesProp((Display *)ecore_x_display_get(), &tmp, &vd))
     return evas_stringshare_add("pc101");
   return evas_stringshare_add(vd.model);
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
		  if (ls) free(ls);
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
						      evas_stringshare_add(exml_value_get(xml)));
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
_lang_apply_language_settings(Language *l)
{
   if (!l) return;


   if (!_lang_apply_language_conponent_names(l))
     return;
}
////////////////// move this to public ////////////////////////
int 
lang_language_xorg_values_get(Language *l)
{
#if 0
   XkbRF_VarDefsRec  vd;
   char		     *tmp = NULL;
#endif
   XkbComponentNamesRec	rnames;
   XkbRF_RulesPtr	rules = NULL;

   if (!l) return 0;

#if 0
   if (!XkbRF_GetNamesProp((Display *)ecore_x_display_get(), &tmp, &vd) || !tmp)
     {
	vd.model     =  DFLT_XKB_MODEL;
	vd.layout    =  DFLT_XKB_LAYOUT;
	vd.variant   = NULL;
	vd.options   = NULL;
     }
   if (vd.model)
     { if (!l->rdefs.model) l->rdefs.model = vd.model; }
   if (vd.layout)
     { if (!l->rdefs.layout) l->rdefs.layout = vd.layout; }
   if (vd.variant)
     { if (!l->rdefs.variant) l->rdefs.variant = vd.variant; }

   if (vd.options) XFree(vd.options);
#endif 

   rules = XkbRF_Load(DFLT_XKB_RULES_FILE, "C", True, True);
   if (!rules) return 0;

   XkbRF_GetComponents(rules, &(l->rdefs), &rnames);

   if (rnames.keycodes)
     { 
	if (l->cNames.keycodes) evas_stringshare_del(l->cNames.keycodes);
	l->cNames.keycodes = (char *) evas_stringshare_add(rnames.keycodes);
     }
   if (rnames.symbols)
     { 
	if (l->cNames.symbols) evas_stringshare_del(l->cNames.symbols);
	l->cNames.symbols = (char *) evas_stringshare_add(rnames.symbols);
     }
   if (rnames.types)
     { 
	if (l->cNames.types) evas_stringshare_del(l->cNames.types);
	l->cNames.types = (char *) evas_stringshare_add(rnames.types);
     }
   if (rnames.compat)
     { 
	if (l->cNames.compat) evas_stringshare_del(l->cNames.compat);
	l->cNames.compat = (char *) evas_stringshare_add(rnames.compat);
     }
   if (rnames.geometry)
     { 
	if (l->cNames.geometry) evas_stringshare_del(l->cNames.geometry);
	l->cNames.geometry = (char *) evas_stringshare_add(rnames.geometry); 
     }
   if (rnames.keymap)
     { 
	if (l->cNames.keymap) evas_stringshare_del(l->cNames.keymap);
	l->cNames.keymap = (char *) evas_stringshare_add(rnames.keymap); 
     }

   return 1;
}
static int
_lang_apply_language_conponent_names(Language *l)
{
   XkbDescPtr  xkb = NULL;
   if (!l) return 0;

   xkb = XkbGetKeyboardByName((Display *)ecore_x_display_get(), XkbUseCoreKbd, &(l->cNames),
			      XkbGBN_AllComponentsMask,
			      XkbGBN_AllComponentsMask & (~XkbGBN_GeometryMask) , True);

   if (!xkb) return 0;

   if (!XkbRF_SetNamesProp((Display *)ecore_x_display_get(),
			   DFLT_XKB_RULES_FILE, &(l->rdefs))) 
     return 0;

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
	e_module_dialog_show(NULL, "Warning", "Warning: This is a bug in the code. This message<br>"
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

   if (!bd && conf->language_selector)
     lang_language_switch_to(conf, 0);

   return 1;
}

int
lang_cb_event_border_focus_in(void *data, int type, void *ev)
{
   Border_Language_Settings *bls;
   Config *conf;
   E_Event_Border_Focus_In *e;
   Evas_List *l;

   e = ev;
   conf = data;

   if (conf->l.current == e->border)
     return 1;
   else
     conf->l.current = e->border;

   bls = NULL;
   for (l = conf->l.border_lang_setup; l; l = l->next)
     {
	bls = l->data;
	if (bls->bd == e->border && (bls->language_selector != conf->language_selector))
	  {
	     Language *lang;

	     conf->language_selector = bls->language_selector;
	     lang = evas_list_nth(conf->languages, conf->language_selector);
	     _lang_apply_language_settings(lang);
	     language_face_language_indicator_update();
	     break;
	  }
	bls = NULL;
     }

   if (!bls && conf->language_selector)
     {
	conf->language_selector = 0;
	_lang_apply_language_settings(conf->languages->data);
	language_face_language_indicator_update();
     }

   return 1;
}
int 
lang_cb_event_border_remove(void *data, int type, void *ev)
{
   E_Event_Border_Remove      *e;
   Border_Language_Settings   *bls;

   Evas_List   *l;
   Config      *conf;

   e = ev;
   conf = data;

   for (l = conf->l.border_lang_setup; l; l = l->next)
     {
	bls = l->data;

	if (bls->bd == e->border)
	  {
	     conf->l.border_lang_setup = evas_list_remove(conf->l.border_lang_setup, bls);
	     if (bls->language_name) evas_stringshare_del(bls->language_name);
	     E_FREE(bls);
	     break;
	  }
     }

   if (!e_border_focused_get() && conf->language_selector)
     lang_language_switch_to(conf, 0);

   return 1;
}

int
lang_cb_event_border_iconify(void *data, int type, void *ev)
{
   E_Event_Border_Iconify *e;
   Config *conf;

   e = ev;
   conf = data;

   if (!e_border_focused_get() && conf->language_selector)
     lang_language_switch_to(conf, 0);

   return 1;
}







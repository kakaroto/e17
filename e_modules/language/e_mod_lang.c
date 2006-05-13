
#include "e_mod_lang.h"
#include "e_mod_main.h"

#include <EXML.h>

#define EXML_RETURN_ON_ERROR(xml) \
   { \
      exml_destroy(xml); \
      return 0; \
   }

Evas_List   *language_def_list = NULL; // Language_Def
Evas_List   *language_kbd_model_list = NULL; // Language_Kbd_Model

/************ extern ********************/
extern Lang *lang;
/****************************************/

/************ private ********************/
int _lang_def_language_sort_cb(void *e1, void *e2);

int  _lang_load_xfree_languages_load_configItem(EXML *xml, Language_Def *ld);
int  _lang_load_xfree_languages_load_variantList(EXML *xml, Language_Def *ld);
static void _lang_register_language(const char *lang_name, const char *lang_shortcut,
				    const char *lang_flag, const char *kbd_layout,
				    Evas_List *kbd_layout_variant);
/****************************************/

void  lang_switch_language_to(Lang *l, unsigned int n)
{
   if (!l || n >= evas_list_count(l->conf->languages)) return;

   l->current_lang_selector = n;


   /* here goes the actuall work that calls X to switch the kbd layout, etc. */

   /* debug */
   {
   char buf[4096];
   Language *ll;

   ll = evas_list_nth(lang->conf->languages, lang->current_lang_selector);

   snprintf(buf, sizeof(buf), 
	    "message 1: current_lang_selector : %d : lang->confg->languages.size() : %d<br>"
	    "ll->lang_name: %s<br>ll->lang_shortcut: %s<br>ll->lang_flag: %s<br>ll->kbd_model:"
	    "%s<br>ll->kbd_layout: %s<br>ll->kbd_variant: %s",
	    lang->current_lang_selector, evas_list_count(lang->conf->languages),
	    ll->lang_name, ll->lang_shortcut, ll->lang_flag, ll->kbd_model,
	    ll->kbd_layout, ll->kbd_variant);
   e_module_dialog_show( _("Enlightenment Language Enhancment Module"), buf);
   }



   lang_face_language_indicator_set(l);
   lang_face_menu_language_indicator_set(l);
   return;
}

void lang_switch_language_next(Lang *l)
{
   if (!l || !l->conf || evas_list_count(l->conf->languages) <= 1) return;

   if (l->current_lang_selector >= evas_list_count(l->conf->languages) - 1)
     lang_switch_language_to(l, 0);
   else
     lang_switch_language_to(l, l->current_lang_selector + 1);
}

void lang_switch_language_prev(Lang *l)
{
   if (!l || !l->conf || evas_list_count(l->conf->languages) <= 1) return;

   if (l->current_lang_selector == 0)
     lang_switch_language_to(l, evas_list_count(lang->conf->languages) - 1);
   else
     lang_switch_language_to(l, l->current_lang_selector - 1);
}

Language *lang_get_default_language()
{
   Evas_List   *l;
   Language    *lang = NULL; 
   
   for (l = language_def_list; l; l = l->next) 
     { 
	Language_Def  *ld = l->data; 
	
	if (!strcmp(ld->kbd_layout, "us")) 
	  { 
	     lang = E_NEW(Language, 1); 
	     if (!lang) 
	       break; 
	     
	     lang->id = 0; 
	     lang->lang_name = evas_stringshare_add(ld->lang_name); 
	     lang->lang_shortcut = evas_stringshare_add(ld->lang_shortcut); 
	     lang->lang_flag = evas_stringshare_add(ld->lang_flag); 
	     //FIXME: get current model from config. 
	     lang->kbd_model = evas_stringshare_add("compaqik13"); 
	     lang->kbd_layout = evas_stringshare_add(ld->kbd_layout); 
	     lang->kbd_variant = evas_stringshare_add("basic"); 
	     break; 
	  } 
     }
   return lang;
}

int
lang_load_kbd_models()
{
   EXML	 *exml;
   int	 found;

   exml = exml_new();

   if (!exml) return 0;
   if (!exml_init(exml))
     EXML_RETURN_ON_ERROR(exml);

   if (!exml_file_read(exml, "/usr/lib/X11/xkb/rules/xfree86.xml")) 
     EXML_RETURN_ON_ERROR(exml)

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

   // here we run through all the kbd_models
   while (1)
     {
	char buf[1024];
	static counter = 0;
	Language_Kbd_Model *lkm;

	EXML_Node *current_model_node = exml_get(exml);

	exml_down(exml);
	//here we should have an configItem tag
	if (!strcasecmp(exml_tag_get(exml), "configItem"))
	  {
	     Language_Kbd_Model	 *lkm;

	     lkm = E_NEW(Language_Kbd_Model, 1);
	     if (lkm)
	       { 
		  // here we step into configItem 
		  exml_down(exml); 
		  
		  // here we run over the all elements in configItem
		  while (1)
		    {
		       char *tag = exml_tag_get(exml);

		       if (!strcasecmp(tag, "name")) 
			 lkm->kbd_model = evas_stringshare_add(exml_value_get(exml));

		       if (!strcasecmp(tag, "description"))
			 {
			    char *attr = exml_attribute_get(exml, "xml:lang");
			    //FIXME: take into account current E localization
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
		     language_kbd_model_list = evas_list_append(language_kbd_model_list, lkm);
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

   return 1;
}
void
lang_free_kbd_models()
{
   Language_Kbd_Model	*lkm;

   while (language_kbd_model_list)
     {
	lkm = language_kbd_model_list->data;

	if (lkm->kbd_model) evas_stringshare_del(lkm->kbd_model);
	if (lkm->kbd_model_desctiption) evas_stringshare_del(lkm->kbd_model_desctiption);
	E_FREE(lkm);
	language_kbd_model_list = evas_list_remove_list(language_kbd_model_list,
						        language_kbd_model_list);
     }
}
int
lang_load_xfree_languages()
{
   EXML	 *exml;
   int found;

   exml = exml_new();

   if (!exml) return 0;
   if (!exml_init(exml))
     EXML_RETURN_ON_ERROR(exml)

   if (!exml_file_read(exml, "/usr/lib/X11/xkb/rules/xfree86.xml"))
     EXML_RETURN_ON_ERROR(exml)

   if (strcasecmp(exml_tag_get(exml), "xkbConfigRegistry"))
     EXML_RETURN_ON_ERROR(exml)

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
     EXML_RETURN_ON_ERROR(exml)

   exml_down(exml);

   if (strcasecmp(exml_tag_get(exml), "layout"))
     EXML_RETURN_ON_ERROR(exml)

   while (1)
     { 
	Language_Def *ld;
	EXML_Node *cur_layout_node = exml_get(exml);

	ld = E_NEW(Language_Def, 1);
	if (ld)
	  { 
	     // here we enter into "layout" tag 
	     exml_down(exml); 
	     // now we loop in "loyout" tag 
	     while (1) 
	       { 
		  _lang_load_xfree_languages_load_configItem(exml, ld);
		  _lang_load_xfree_languages_load_variantList(exml, ld); 

		  if (!exml_next_nomove(exml)) 
		    break; 
	       }

	     if (ld->lang_name && ld->lang_shortcut && ld->kbd_layout)
	       {
		  _lang_register_language(ld->lang_name, ld->lang_shortcut,
					  ld->lang_flag, ld->kbd_layout,
					  ld->kbd_variant);

		  if (ld->lang_name) evas_stringshare_del(ld->lang_name);
		  if (ld->lang_shortcut) evas_stringshare_del(ld->lang_shortcut);
		  if (ld->lang_flag) evas_stringshare_del(ld->lang_flag);
		  if (ld->kbd_layout) evas_stringshare_del(ld->kbd_layout);

		  while (ld->kbd_variant)
		    ld->kbd_variant = evas_list_remove_list(ld->kbd_variant,
							    ld->kbd_variant);
		  E_FREE(ld);
	       }
	  }

	exml_goto_node(exml, cur_layout_node);
	if (!exml_next_nomove(exml))
	  break;
     }

   exml_destroy(exml);
   return 1;
}
void
lang_free_xfree_languages()
{
   Language_Def	  *ld;
   while (language_def_list)
     {
	ld = language_def_list->data;
	
	if (ld->lang_name) evas_stringshare_del(ld->lang_name);
	if (ld->lang_shortcut) evas_stringshare_del(ld->lang_shortcut);
	if (ld->lang_flag) evas_stringshare_del(ld->lang_flag);
	if (ld->kbd_layout) evas_stringshare_del(ld->kbd_layout);
	while (ld->kbd_variant)
	  {
	     evas_stringshare_del((const char *)ld->kbd_variant);
	     ld->kbd_variant = evas_list_remove_list(ld->kbd_variant, ld->kbd_variant);
	  }
	E_FREE(ld);

	language_def_list = evas_list_remove_list(language_def_list, language_def_list);
     }
}

/******************* private *********************/

int
_lang_load_xfree_languages_load_configItem(EXML *xml, Language_Def *ld)
{
   EXML_Node   *current_node;
   if (!xml || !ld) return 0;

   if (strcasecmp(exml_tag_get(xml), "configItem")) return 0;

   current_node = exml_get(xml);

   exml_down(xml);

   while (1)
     {
	char *tag = exml_tag_get(xml);

	if (!strcasecmp(tag, "name")) 
	  ld->kbd_layout = evas_stringshare_add(exml_value_get(xml));

	if (!strcasecmp(tag, "shortDescription"))
	  {
	     char *attr = exml_attribute_get(xml, "xml:lang");
	     //FIXME: take into account current E localization
	     if (!attr)
	       { 
		  int i;
		  char buf[1024];
		  char *ls = strdup(exml_value_get(xml));
		  if (ld->lang_shortcut) evas_stringshare_del(ld->lang_shortcut);

		  if (ls)
		    {
			for (i = 0; ls[i]; i++)
			  ls[i] = (char)toupper(ls[i]);
			ld->lang_shortcut = evas_stringshare_add(ls); 
		    }

		  if (ld->lang_flag) evas_stringshare_del(ld->lang_flag);
		  snprintf(buf, sizeof(buf), "%s_flag", ls);
		  ld->lang_flag = evas_stringshare_add(buf);
	       }
	  }

	if (!strcasecmp(tag, "description"))
	  {
	     char *attr = exml_attribute_get(xml, "xml:lang");
	     //FIXME: take into account current E localization
	     if (!attr)
	       {
		  if (ld->lang_name) evas_stringshare_del(ld->lang_name);
		  ld->lang_name = evas_stringshare_add(exml_value_get(xml));
	       }
	  }

	if (ld->lang_name && ld->lang_shortcut && ld->kbd_layout)
	  break;

	if (!exml_next_nomove(xml))
	  break;
     }
   exml_goto_node(xml, current_node);
   return 1;
}

int
_lang_load_xfree_languages_load_variantList(EXML *xml, Language_Def *ld)
{
   EXML_Node   *current_node;
   if (!xml || !ld) return 0;

   if (strcasecmp(exml_tag_get(xml), "variantList")) return 0;

   current_node = exml_get(xml);

   if (!exml_down(xml)) return 0;

   // now we run through the variantList
   while (1)
     {
	if (!strcasecmp(exml_tag_get(xml), "variant"))
	  {
	     EXML_Node	*current_variant_node = exml_get(xml);

	     exml_down(xml);
	     if (!strcasecmp(exml_tag_get(xml), "configItem"))
	       {
		  exml_down(xml);
		  // running inside the configItem
		  while (1)
		    {
		       char *tag = exml_tag_get(xml);
		       if (!strcasecmp(tag, "name"))
			 {
			    //we found a name of the variant
			    ld->kbd_variant = evas_list_append(ld->kbd_variant,
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

   return 1;
}

static void 
_lang_register_language(const char *lang_name, const char *lang_shortcut,
		        const char *lang_flag, const char *kbd_layout,
		        Evas_List *kbd_layout_variant)
{
   Language_Def	  *ld;
   Evas_List	  *l;
   int found = 0;
   int i;

   if (!lang_name || !lang_shortcut || !kbd_layout) return;

   for (l = language_def_list; l && !found; l = l->next)
     {
	ld = l->data;

	if (!strcmp(ld->lang_name, lang_name))
	  found = 1;
     }
   if (found)
     {
	if (kbd_layout_variant) 
	  { 
	     for (l = kbd_layout_variant; l; l = l->next) 
	       ld->kbd_variant = evas_list_append(ld->kbd_variant, evas_stringshare_add(l->data));
	  }
     }
   else
     {
	ld = E_NEW(Language_Def, 1); 
	if (!ld) return; 
	
	ld->lang_name = evas_stringshare_add(lang_name); 
	ld->lang_shortcut = evas_stringshare_add(lang_shortcut); 
	ld->lang_flag = !lang_flag ? NULL : evas_stringshare_add(lang_flag); 
	ld->kbd_layout = evas_stringshare_add(kbd_layout);
	
	if (kbd_layout_variant) 
	  { 
	     for (l = kbd_layout_variant; l; l = l->next) 
	       ld->kbd_variant = evas_list_append(ld->kbd_variant, evas_stringshare_add(l->data));
	  }
     }

   language_def_list = evas_list_append(language_def_list, ld);

   language_def_list = evas_list_sort(language_def_list, evas_list_count(language_def_list),
				      _lang_def_language_sort_cb);
}

int
_lang_def_language_sort_cb(void *e1, void *e2)
{
   Language_Def	  *ld1 = e1;
   Language_Def	  *ld2 = e2;

   if (!e1) return 1;
   if (!e2) return -1;

   return strcmp((const char *)ld1->lang_name, (const char *)ld2->lang_name);
}

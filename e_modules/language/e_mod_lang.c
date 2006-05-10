
#include "e_mod_lang.h"
#include "e_mod_main.h"

Evas_List  *language_def_list = NULL;

/************ private ********************/
int _lang_def_language_sort_cb(void *e1, void *e2);
/****************************************/

void 
lang_register_language(const char *lang_name, const char *lang_shortcut,
		       const char *lang_flag, const char *kbd_layout,
		       const char *kbd_layout_variant)
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
   if (found) return;

   ld = E_NEW(Language_Def, 1);
   if (!ld) return;

   ld->lang_name = evas_stringshare_add(lang_name);
   ld->lang_shortcut = evas_stringshare_add(lang_shortcut);
   ld->lang_flag = !lang_flag ? NULL : evas_stringshare_add(lang_flag);
   ld->kbd_layout = evas_list_append(ld->kbd_layout, evas_stringshare_add(kbd_layout));
   if (kbd_layout_variant)
     ld->kbd_variant = evas_list_append(ld->kbd_variant, evas_stringshare_add(kbd_layout_variant));

   language_def_list = evas_list_append(language_def_list, ld);

   language_def_list = evas_list_sort(language_def_list, evas_list_count(language_def_list),
				      _lang_def_language_sort_cb);
}

void 
lang_unregister_all_languages()
{
   Language_Def	  *ld;
   while (language_def_list)
     {
	ld = language_def_list->data;
	
	if (ld->lang_name) evas_stringshare_del(ld->lang_name);
	if (ld->lang_shortcut) evas_stringshare_del(ld->lang_shortcut);
	if (ld->lang_flag) evas_stringshare_del(ld->lang_flag);
	while (ld->kbd_layout)
	  {
	     evas_stringshare_del(ld->kbd_layout->data);
	     ld->kbd_layout = evas_list_remove_list(ld->kbd_layout, ld->kbd_layout);
	  }
	while (ld->kbd_variant)
	  {
	     evas_stringshare_del((const char *)ld->kbd_variant);
	     ld->kbd_variant = evas_list_remove_list(ld->kbd_variant, ld->kbd_variant);
	  }
	E_FREE(ld);

	language_def_list = evas_list_remove_list(language_def_list, language_def_list);
     }
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

#include <e.h>
#include "e_mod_main.h"
#include "e_mod_lang.h"
#include "e_mod_config.h"
#include "config.h"

#define ILIST_ICON_WIDTH   32
#define ILIST_ICON_HEIGHT  32

#define ILIST_LANGUAGE_LABEL_FORMAT "%s (%s)"

struct _E_Config_Dialog_Data
{
   lang_switch_policy_t	lang_policy;
   int			lang_show_indicator;

   //Lang	 *lang;
   
   Evas_List   *selected_languages;
   Evas_List   *kbd_models;

   Evas	 *evas;

   struct 
     {
	Evas_Object  *lang_ilist;
	Evas_Object  *btn_add;
	Evas_Object  *btn_del;
	Evas_Object  *selected_lang_ilist;
	Evas_Object  *kbd_model_ilist;
	Evas_Object  *kbd_layout_ilist;
	Evas_Object  *layout_variant_ilist;
     } gui;
};

/************** extern *****************************/
extern Evas_List   *language_def_list;
extern Evas_List   *language_kbd_model_list;
/**************************************************/

static void	    *_create_data(E_Config_Dialog *cfd);
static void	    _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object  *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas,
					   E_Config_Dialog_Data *cfdata);
static int	    _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

static void	    _fill_data(Lang *l, E_Config_Dialog_Data *cfdata);

/************* private functionality ************************/
static void _lang_update_lang_defined_list(E_Config_Dialog_Data *cfdata);
static void _lang_update_selected_lang_list(E_Config_Dialog_Data *cfdata);
static void _lang_update_select_button(E_Config_Dialog_Data *cfdata);
static void _lang_update_unselect_button(E_Config_Dialog_Data *cfdata);

static void _lang_select_language_cb(void *data, void *data2);
static void _lang_unselect_language_cb(void *data, void *data2);
static void _lang_languages_ilist_cb_change(void *data, Evas_Object *obj);
static void _lang_selected_languages_ilist_cb_change(void *data, Evas_Object *obj);

void _lang_update_kbd_model_list(E_Config_Dialog_Data *cfdata);

static void _lang_free_language(Language  *lang);
/************************************************************/

void _lang_configure_lang_module(E_Container *con, Lang *l)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata	    = _create_data;
   v->free_cfdata	    = _free_data;
   v->basic.apply_cfdata    = _basic_apply_data;
   v->basic.create_widgets  = _basic_create_widgets;

   cfd = e_config_dialog_new(con, _("Language Module Configuration"), NULL, 0, v, l);
   l->cfd = cfd;
}

static void
_fill_data(Lang *l, E_Config_Dialog_Data *cfdata)
{
   Evas_List   *ll;
   Language    *lang, *lang2;
   
   cfdata->lang_policy = l->conf->lang_policy;
   cfdata->lang_show_indicator = l->conf->lang_show_indicator;

   for (ll = l->conf->languages; ll; ll = ll->next)
     {
	lang = ll->data;

	lang2 = E_NEW(Language, 1);
	if (!lang2) continue;

	lang2->lang_name = evas_stringshare_add(lang->lang_name);
	lang2->lang_shortcut = evas_stringshare_add(lang->lang_shortcut);
	lang2->lang_flag = !(lang->lang_flag) ? NULL : evas_stringshare_add(lang->lang_flag);
	lang2->kbd_model = !(lang->kbd_model) ? NULL : evas_stringshare_add(lang->kbd_model);
	lang2->kbd_layout = !(lang->kbd_layout) ? NULL : evas_stringshare_add(lang->kbd_layout);
	lang2->kbd_variant = !(lang->kbd_variant) ? NULL : evas_stringshare_add(lang->kbd_variant);

	cfdata->selected_languages = evas_list_append(cfdata->selected_languages, lang2);
     }

}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Lang *l;

   l = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   //cfdata->lang = l;

   _fill_data(l, cfdata);

   lang_load_kbd_models();
   lang_load_xfree_languages();
   return cfdata;
}


static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Lang	*l;

   while (cfdata->selected_languages)
     {
	_lang_free_language(cfdata->selected_languages->data);
	cfdata->selected_languages = evas_list_remove_list(cfdata->selected_languages,
							   cfdata->selected_languages);
     }

   lang_free_kbd_models();
   lang_free_xfree_languages();

   l = cfd->data;
   l->cfd = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *ot, *ot2, *oft;
   Lang *l;

   l = cfd->data;
   cfdata->evas = evas;

   o = e_widget_list_add(evas, 0, 0);

   ot = e_widget_table_add(evas, 0);
   {
      ot2 = e_widget_table_add(evas, 0);
      {
	 of = e_widget_framelist_add(evas, _("Languages"), 0);
	 {
	    /* languages ilist */
	    ob = e_widget_ilist_add(evas, ILIST_ICON_WIDTH, ILIST_ICON_HEIGHT, NULL);
	    e_widget_on_change_hook_set(ob, _lang_languages_ilist_cb_change, cfdata);
	    cfdata->gui.lang_ilist = ob;
	    e_widget_min_size_set(ob, 250, 300);
	    e_widget_ilist_go(ob);
	    e_widget_framelist_object_append(of, ob);
	 }
	 e_widget_table_object_append(ot2, of, 0, 0, 1, 4, 1, 1, 1, 1);

	 ob = e_widget_button_add(evas, ">>", NULL, _lang_select_language_cb, cfdata, NULL);
	 cfdata->gui.btn_add = ob;
	 e_widget_table_object_append(ot2, ob, 1, 1, 1, 1, 1, 1, 1, 1);

	 ob = e_widget_button_add(evas, "<<", NULL, _lang_unselect_language_cb, cfdata, NULL);
	 cfdata->gui.btn_del = ob;
	 e_widget_table_object_append(ot2, ob, 1, 2, 1, 1, 1, 1, 1, 1);

	 of = e_widget_framelist_add(evas, _("Selected Languages"), 0);
	 {
	    /* selected languages ilist */
	    ob = e_widget_ilist_add(evas, ILIST_ICON_WIDTH, ILIST_ICON_HEIGHT, NULL);
	    e_widget_on_change_hook_set(ob, _lang_selected_languages_ilist_cb_change, cfdata);
	    cfdata->gui.selected_lang_ilist = ob;
	    e_widget_min_size_set(ob, 250, 200);
	    e_widget_ilist_go(ob);
	    e_widget_framelist_object_append(of, ob);
	 }
	 e_widget_table_object_append(ot2, of, 2, 0, 1, 4, 1, 1, 1, 1);
      }
      e_widget_table_object_append(ot, ot2, 0, 0, 1, 1, 1, 1, 1, 1);

      oft = e_widget_frametable_add(evas, _("Language Settings"), 0);
      {
	 of = e_widget_framelist_add(evas, _("Keyboard Model"), 0);
	 {
	    ob = e_widget_ilist_add(evas, ILIST_ICON_WIDTH, ILIST_ICON_HEIGHT, NULL);
	    cfdata->gui.kbd_model_ilist = ob;
	    {
	       e_widget_min_size_set(ob, 250, 150);
	       e_widget_ilist_go(ob);
	    }
	    e_widget_framelist_object_append(of, ob);
	 }
	 e_widget_frametable_object_append(oft, of, 0, 0, 1, 1, 1, 1, 1, 1);

	 of = e_widget_framelist_add(evas, _("Keyboard Layout"), 0);
	 {
	    ob = e_widget_ilist_add(evas, ILIST_ICON_WIDTH, ILIST_ICON_HEIGHT, NULL);
	    cfdata->gui.kbd_layout_ilist = ob;
	    {
	       e_widget_min_size_set(ob, 150, 75);
	       e_widget_ilist_go(ob);
	    }
	    e_widget_framelist_object_append(of, ob);
	 }
	 e_widget_frametable_object_append(oft, of, 1, 0, 1, 1, 1, 1, 1, 1);

	 of = e_widget_framelist_add(evas, _("Layout Variant"), 0);
	 {
	    ob = e_widget_ilist_add(evas, ILIST_ICON_WIDTH, ILIST_ICON_HEIGHT, NULL);
	    cfdata->gui.layout_variant_ilist = ob;
	    {
	       e_widget_min_size_set(ob, 150, 75);
	       e_widget_ilist_go(ob);
	    }
	    e_widget_framelist_object_append(of, ob);
	 }
	 e_widget_frametable_object_append(oft, of, 2, 0, 1, 1, 1, 1, 1, 1);
      }
      e_widget_table_object_append(ot, oft, 0, 1, 1, 1, 1, 1, 1, 1);
   }
   e_widget_list_object_append(o, ot, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("Options"), 0);
   ob = e_widget_check_add(evas, _("Show Language Indicator"), (&(cfdata->lang_show_indicator)));
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   _lang_update_lang_defined_list(cfdata);
   _lang_update_selected_lang_list(cfdata);

   _lang_update_select_button(cfdata);
   _lang_update_unselect_button(cfdata);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   char *tmp;
   Lang *l;

   l = cfd->data;
   l->conf->lang_policy = cfdata->lang_policy;
   l->conf->lang_show_indicator = cfdata->lang_show_indicator;
   e_config_save_queue();

   /* here we have to redo context menu of the module */

   /*if (c->face->monitor)
      ecore_timer_interval_set(c->face->monitor, (double)cfdata->check_interval);*/

   return 1;
}

static void 
_lang_update_lang_defined_list(E_Config_Dialog_Data *cfdata)
{
   //FIXME: optimize - too slow
   char buf[1024];
   Evas_List	  *l, *l2;
   Language_Def	  *ld;
   Language	  *lang;
   int indx;
   char *sel_label = NULL;

   if (!cfdata) return;

   if (e_widget_ilist_count(cfdata->gui.lang_ilist))
     { 
	const char *t;
	t = e_widget_ilist_selected_label_get(cfdata->gui.lang_ilist);
	if (t)
	  sel_label = strdup(t);
     }
   else
     sel_label = NULL;

   e_widget_ilist_clear(cfdata->gui.lang_ilist);
   for (l = language_def_list; l; l = l->next)
     {
	Evas_Object  *ic = NULL;
	int found = 0;
	ld = l->data;

	for (l2 = cfdata->selected_languages; l2 && !found; l2 = l2->next)
	  {
	     lang = l2->data;
	     if (!strcmp(lang->lang_name, ld->lang_name) &&
	         !strcmp(lang->lang_shortcut, ld->lang_shortcut))
	       {
		  found = 1;
	       }
	  }
	if (found) continue;

	snprintf(buf, sizeof(buf), ILIST_LANGUAGE_LABEL_FORMAT, ld->lang_name, ld->lang_shortcut);

	ic = edje_object_add(cfdata->evas);
	e_util_edje_icon_set(ic, "enlightenment/e");
	e_widget_ilist_append(cfdata->gui.lang_ilist, ic, buf, NULL, NULL, NULL);
     }

   if (sel_label)
     {
	int i;
	int size = e_widget_ilist_count(cfdata->gui.lang_ilist);
	const char *t;
	for (i = 0; i < size; i++)
	  {
	     t = e_widget_ilist_nth_label_get(cfdata->gui.lang_ilist, i);
	     if (!strcmp(t, sel_label))
	       { 
		  e_widget_ilist_selected_set(cfdata->gui.lang_ilist, i);
		  break;
	       }
	  }
	free(sel_label);
     }
   e_widget_ilist_go(cfdata->gui.lang_ilist);
}

static void 
_lang_update_selected_lang_list(E_Config_Dialog_Data *cfdata)
{
   //FIXME: optimize - too slow
   char buf[1024];
   Evas_List   *l;
   Language    *lang;
   int indx;

   if (!cfdata) return;

   if (e_widget_ilist_count(cfdata->gui.selected_lang_ilist))
     indx = e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist);
   else
     indx = -1;

   e_widget_ilist_clear(cfdata->gui.selected_lang_ilist);
   for (l = cfdata->selected_languages; l; l = l->next)
     {
	Evas_Object *ic;
	lang = l->data;

	snprintf(buf, sizeof(buf), ILIST_LANGUAGE_LABEL_FORMAT, lang->lang_name,
	         lang->lang_shortcut);

	ic = edje_object_add(cfdata->evas);
	e_util_edje_icon_set(ic, "enlightenment/e");
	e_widget_ilist_append(cfdata->gui.selected_lang_ilist, ic, buf, NULL, NULL, NULL);
     }

   if (indx >= 0)
     {
	if (indx >= e_widget_ilist_count(cfdata->gui.selected_lang_ilist))
	  indx = e_widget_ilist_count(cfdata->gui.selected_lang_ilist) - 1;

	e_widget_ilist_selected_set(cfdata->gui.selected_lang_ilist, indx);
     }

   e_widget_ilist_go(cfdata->gui.selected_lang_ilist);
}

static void
_lang_select_language_cb(void *data, void *data2)
{
   char			buf[1024];
   const char		*sel_label;
   Evas_List		*l;
   E_Config_Dialog_Data	*cfdata;
   Language_Def		*ld = NULL;
   Language		*lang;

   if (!(cfdata = data)) return;

   sel_label = e_widget_ilist_selected_label_get(cfdata->gui.lang_ilist);
   if (!sel_label) return;

   for (l = language_def_list; l; l = l->next)
     {
	ld = l->data;

	snprintf(buf, sizeof(buf), ILIST_LANGUAGE_LABEL_FORMAT, ld->lang_name, ld->lang_shortcut);

	if (!strcmp(sel_label, buf))
	  break;
	ld = NULL;
     }

   if (!ld) return;

   lang = E_NEW(Language, 1);
   if (!lang) return;

   lang->lang_name = evas_stringshare_add(ld->lang_name);
   lang->lang_shortcut = evas_stringshare_add(ld->lang_shortcut);
   lang->lang_flag = !(ld->lang_flag) ? NULL : evas_stringshare_add(ld->lang_flag);
   //FIXME: assign current keyboard model
   lang->kbd_model = evas_stringshare_add("compaqik13");
   lang->kbd_layout = evas_stringshare_add(ld->kbd_layout->data);
   lang->kbd_variant = !(ld->kbd_variant) ? NULL : evas_stringshare_add(ld->kbd_variant->data);

   cfdata->selected_languages = evas_list_append(cfdata->selected_languages, lang);
   //DO NOT sort selected_languages list. Here the sequence of the langs is essential!

   _lang_update_lang_defined_list(cfdata);
   _lang_update_selected_lang_list(cfdata);

   _lang_update_select_button(cfdata);
   _lang_update_unselect_button(cfdata);
}

static void
_lang_unselect_language_cb(void *data, void *data2)
{
   char			buf[1024];
   const char		*sel_label;
   Evas_List		*l;
   Language		*lang;
   E_Config_Dialog_Data	*cfdata;

   if (!(cfdata = data)) return;

   sel_label = e_widget_ilist_selected_label_get(cfdata->gui.selected_lang_ilist);
   if (!sel_label) return;

   for (l = cfdata->selected_languages; l; l = l->next)
     {
	lang = l->data;

	snprintf(buf, sizeof(buf), ILIST_LANGUAGE_LABEL_FORMAT, lang->lang_name,
	         lang->lang_shortcut);

	if (!strcmp(sel_label, buf))
	  {
	     cfdata->selected_languages = evas_list_remove_list(cfdata->selected_languages, l);
	     _lang_free_language(lang);
	     break;
	  }
     }

   _lang_update_lang_defined_list(cfdata);
   _lang_update_selected_lang_list(cfdata);

   _lang_update_select_button(cfdata);
   _lang_update_unselect_button(cfdata);
}
static void
_lang_update_select_button(E_Config_Dialog_Data *cfdata)
{
   if (!cfdata) return;

   if (e_widget_ilist_count(cfdata->gui.lang_ilist) &&
       e_widget_ilist_selected_label_get(cfdata->gui.lang_ilist))
     e_widget_disabled_set(cfdata->gui.btn_add, 0);
   else
     e_widget_disabled_set(cfdata->gui.btn_add, 1);
}
static void
_lang_update_unselect_button(E_Config_Dialog_Data *cfdata)
{
   if (!cfdata) return;

   if (e_widget_ilist_count(cfdata->gui.selected_lang_ilist) &&
       e_widget_ilist_selected_label_get(cfdata->gui.selected_lang_ilist))
     e_widget_disabled_set(cfdata->gui.btn_del, 0);
   else
     e_widget_disabled_set(cfdata->gui.btn_del, 1);
}
static void
_lang_languages_ilist_cb_change(void *data, Evas_Object *obj)
{
   E_Config_Dialog_Data	*cfdata;

   if (!(cfdata = data)) return;

   _lang_update_select_button(cfdata);
}
static void
_lang_selected_languages_ilist_cb_change(void *data, Evas_Object *obj)
{
   E_Config_Dialog_Data	*cfdata;

   if (!(cfdata = data)) return;

   _lang_update_unselect_button(cfdata);

   _lang_update_kbd_model_list(cfdata);
}

void
_lang_update_kbd_model_list(E_Config_Dialog_Data *cfdata)
{
   char *sel_lang_label;
   if (!cfdata) return;

   sel_lang_label = e_widget_ilist_selected_label_get(cfdata->gui.selected_lang_ilist);
   if (!sel_lang_label)
     {
	e_widget_ilist_clear(cfdata->gui.kbd_model_ilist);
	return;
     }

   if (!e_widget_ilist_count(cfdata->gui.kbd_model_ilist))
     {
	int indx = 0;
	char buf[4096];
	Language *lang;
	Evas_List *l;
	Language_Kbd_Model *lkm;

	//FIXME: determine the selection index in advance
	/*for (l = cfdata->selected_languages; l; l = l->next)
	  {
	  }*/

	for (l = language_kbd_model_list; l; l = l->next)
	  {
	     lkm = l->data;
	     snprintf(buf, sizeof(buf), "%s (%s)", lkm->kbd_model_desctiption, lkm->kbd_model);
	     e_widget_ilist_append(cfdata->gui.kbd_model_ilist, NULL, buf, NULL, NULL, NULL);
	  }
     }
   e_widget_ilist_go(cfdata->gui.kbd_model_ilist);

   //FIXME: select the appropriate kbd_model
}

static void
_lang_free_language(Language  *lang)
{
   if (!lang) return;

   if (lang->lang_name) evas_stringshare_del(lang->lang_name);
   if (lang->lang_shortcut) evas_stringshare_del(lang->lang_shortcut);
   if (lang->lang_flag) evas_stringshare_del(lang->lang_flag);
   if (lang->kbd_model) evas_stringshare_del(lang->kbd_model);
   if (lang->kbd_layout) evas_stringshare_del(lang->kbd_layout);
   if (lang->kbd_variant) evas_stringshare_del(lang->kbd_variant);

   E_FREE(lang);
}



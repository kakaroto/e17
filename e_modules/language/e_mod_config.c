#include <e.h>
#include "e_mod_main.h"
#include "e_mod_lang.h"
#include "e_mod_config.h"
#include "config.h"

#define ILIST_ICON_LANG_WIDTH   35
#define ILIST_ICON_LANG_HEIGHT  35

#define ILIST_ICON_WIDTH   32
#define ILIST_ICON_HEIGHT  32

#define ILIST_LANGUAGE_LABEL_FORMAT "%s (%s)"

struct _E_Config_Dialog_Data
{
   lang_switch_policy_t	lang_policy;
   int			lang_show_indicator;

   Evas_List   *selected_languages;
   Evas_List   *kbd_models;

   Evas	 *evas;

   struct 
     {
	E_Config_Dialog	*cfd;
	Evas_Object  *lang_ilist;
	Evas_Object  *btn_add;
	Evas_Object  *btn_del;
	Evas_Object  *btn_move_up;
	Evas_Object  *btn_move_down;
	Evas_Object  *selected_lang_ilist;
	Evas_Object  *kbd_model_ilist;
	Evas_Object  *kbd_layout_variant_ilist;
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
static void _lang_update_kbd_model_list(E_Config_Dialog_Data *cfdata);
static void _lang_update_kbd_layout_variant_list(E_Config_Dialog_Data *cfdata);

static void _lang_update_select_button(E_Config_Dialog_Data *cfdata);
static void _lang_update_unselect_button(E_Config_Dialog_Data *cfdata);
static void _lang_update_language_moveup_button(E_Config_Dialog_Data *cfdata);
static void _lang_update_language_movedown_button(E_Config_Dialog_Data *cfdata);

static void _lang_select_language_cb(void *data, void *data2);
static void _lang_unselect_language_cb(void *data, void *data2);
static void _lang_move_language_order_up_cb(void *data, void *data2);
static void _lang_move_language_order_down_cb(void *data, void *data2);

static void _lang_languages_ilist_cb_change(void *data, Evas_Object *obj);
static void _lang_selected_languages_ilist_cb_change(void *data, Evas_Object *obj);
static void _lang_kbd_model_ilist_cb_change(void *data, Evas_Object *obj);
static void _lang_kbd_layout_variant_ilist_cb_change(void *data, Evas_Object *obj);

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

   cfdata->gui.cfd = l->cfd;
   cfdata->lang_policy = l->conf->lang_policy;
   cfdata->lang_show_indicator = l->conf->lang_show_indicator;

   for (ll = l->conf->languages; ll; ll = ll->next)
     {
	lang = ll->data;

	lang2 = E_NEW(Language, 1);
	if (!lang2) continue;

	lang2->id = lang->id;
	lang2->lang_name = evas_stringshare_add(lang->lang_name);
	lang2->lang_shortcut = evas_stringshare_add(lang->lang_shortcut);
	lang2->lang_flag = !(lang->lang_flag) ? NULL : evas_stringshare_add(lang->lang_flag);
	lang2->kbd_model = !(lang->kbd_model) ? NULL : evas_stringshare_add(lang->kbd_model);
	lang2->kbd_layout = !(lang->kbd_layout) ? NULL : evas_stringshare_add(lang->kbd_layout);
	lang2->kbd_variant = !(lang->kbd_variant) ? NULL : evas_stringshare_add(lang->kbd_variant);

	cfdata->selected_languages = evas_list_append(cfdata->selected_languages, lang2);
     }

   /*if (!cfdata->selected_languages)
     {
	for (ll = language_def_list; ll; ll = ll->next)
	  {
	     Language_Def  *ld = ll->data;

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

		  cfdata->selected_languages = evas_list_append(cfdata->selected_languages, lang);

		  break;
	       }
	  }
     }*/

}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Lang *l;

   l = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(l, cfdata);
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
      ot2 = e_widget_table_add(evas, 1);
      {
	 of = e_widget_framelist_add(evas, _("Languages"), 1);
	 {
	    /* languages ilist */
	    ob = e_widget_ilist_add(evas, ILIST_ICON_LANG_WIDTH, ILIST_ICON_LANG_HEIGHT, NULL);
	    e_widget_on_change_hook_set(ob, _lang_languages_ilist_cb_change, cfdata);
	    cfdata->gui.lang_ilist = ob;
	    e_widget_min_size_set(ob, 250, 250);
	    e_widget_ilist_go(ob);
	    e_widget_framelist_object_append(of, ob);
	 }
	 e_widget_table_object_append(ot2, of, 0, 0, 1, 1, 1, 1, 1, 1);

	 oft = e_widget_frametable_add(evas, _("Selected Languages"), 0);
	 {

	    /* selected languages ilist */
	    ob = e_widget_ilist_add(evas, ILIST_ICON_WIDTH, ILIST_ICON_HEIGHT, NULL);
	    e_widget_on_change_hook_set(ob, _lang_selected_languages_ilist_cb_change, cfdata);
	    cfdata->gui.selected_lang_ilist = ob;
	    e_widget_min_size_set(ob, 250, 250);
	    e_widget_ilist_go(ob);
	    e_widget_frametable_object_append(oft, ob, 0, 0, 4, 1, 1, 1, 1, 1);

	    ob = e_widget_button_add(evas, ">>", NULL, _lang_select_language_cb, cfdata, NULL);
	    cfdata->gui.btn_add = ob;
	    e_widget_frametable_object_append(oft, ob, 0, 1, 1, 1, 1, 1, 1, 1);

	    ob = e_widget_button_add(evas, "<<", NULL, _lang_unselect_language_cb, cfdata, NULL);
	    cfdata->gui.btn_del = ob;
	    e_widget_frametable_object_append(oft, ob, 1, 1, 1, 1, 1, 1, 1, 1);

	    ob = e_widget_button_add(evas, "Up", "widget/up_arrow", 
				     _lang_move_language_order_up_cb, cfdata, NULL);
	    cfdata->gui.btn_move_up = ob;
	    e_widget_frametable_object_append(oft, ob, 2, 1, 1, 1, 1, 1, 1, 1);

	    ob = e_widget_button_add(evas, "Down", "widget/down_arrow",
				     _lang_move_language_order_down_cb, cfdata, NULL);
	    cfdata->gui.btn_move_down = ob;
	    e_widget_frametable_object_append(oft, ob, 3, 1, 1, 1, 1, 1, 1, 1);
	 }
	 e_widget_table_object_append(ot2, oft, 1, 0, 1, 1, 1, 1, 1, 1);
      }
      e_widget_table_object_append(ot, ot2, 0, 0, 1, 1, 1, 1, 1, 1);

      oft = e_widget_frametable_add(evas, _("Language Settings"), 0);
      {
	 of = e_widget_framelist_add(evas, _("Keyboard Model"), 1);
	 {
	    ob = e_widget_ilist_add(evas, ILIST_ICON_WIDTH, ILIST_ICON_HEIGHT, NULL);
	    e_widget_on_change_hook_set(ob, _lang_kbd_model_ilist_cb_change, cfdata);
	    cfdata->gui.kbd_model_ilist = ob;
	    {
	       e_widget_min_size_set(ob, 380, 100);
	       e_widget_ilist_go(ob);
	    }
	    e_widget_framelist_object_append(of, ob);
	 }
	 e_widget_frametable_object_append(oft, of, 0, 0, 1, 1, 1, 1, 1, 1);

	 of = e_widget_framelist_add(evas, _("Layout Variant"), 0);
	 {
	    ob = e_widget_ilist_add(evas, ILIST_ICON_WIDTH, ILIST_ICON_HEIGHT, NULL);
	    e_widget_on_change_hook_set(ob, _lang_kbd_layout_variant_ilist_cb_change, cfdata);
	    cfdata->gui.kbd_layout_variant_ilist = ob;
	    {
	       e_widget_min_size_set(ob, 120, 100);
	       e_widget_ilist_go(ob);
	    }
	    e_widget_framelist_object_append(of, ob);
	 }
	 e_widget_frametable_object_append(oft, of, 1, 0, 1, 1, 1, 1, 1, 1);
      }
      e_widget_table_object_append(ot, oft, 0, 1, 1, 1, 1, 1, 1, 1);
   }
   e_widget_list_object_append(o, ot, 1, 1, 0.5);

   /*of = e_widget_framelist_add(evas, _("Options"), 0);
   ob = e_widget_check_add(evas, _("Show Language Indicator"), (&(cfdata->lang_show_indicator)));
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);*/

   _lang_update_lang_defined_list(cfdata);
   _lang_update_selected_lang_list(cfdata);

   _lang_update_select_button(cfdata);
   _lang_update_unselect_button(cfdata);
   _lang_update_language_movedown_button(cfdata);
   _lang_update_language_moveup_button(cfdata);

   //e_widget_ilist_selected_set(cfdata->gui.selected_lang_ilist, 0);

   /*e_dialog_button_disable_num_set(cfdata->gui.cfd->dia, 0, 0);
   e_dialog_button_disable_num_set(cfdata->gui.cfd->dia, 1, 0);*/
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   char *tmp;
   Lang *l;
   Evas_List   *list;
   Language    *lang, *lang2;

   l = cfd->data;
   l->conf->lang_policy = cfdata->lang_policy;
   l->conf->lang_show_indicator = cfdata->lang_show_indicator;

   while (l->conf->languages)
     {
	lang = l->conf->languages->data;
	_lang_free_language(lang);
	l->conf->languages = evas_list_remove_list(l->conf->languages, l->conf->languages);
     }

   for (list = cfdata->selected_languages; list; list = list->next)
     {
	lang = list->data;

	lang2 = E_NEW(Language, 1);
	if (!lang2) continue;

	lang2->lang_name = evas_stringshare_add(lang->lang_name);
	lang2->lang_shortcut = evas_stringshare_add(lang->lang_shortcut);
	lang2->lang_flag = !(lang->lang_flag) ? NULL : evas_stringshare_add(lang->lang_flag);
	lang2->kbd_model = !(lang->kbd_model) ? NULL : evas_stringshare_add(lang->kbd_model);
	lang2->kbd_layout = !(lang->kbd_layout) ? NULL : evas_stringshare_add(lang->kbd_layout);
	lang2->kbd_variant = !(lang->kbd_variant) ? NULL : evas_stringshare_add(lang->kbd_variant);

	l->conf->languages = evas_list_append(l->conf->languages, lang2);
     }
   l->current_lang_selector = 0;
   e_config_save_queue();

   lang_face_menu_regenerate(l->face);
   lang_face_language_indicator_set(l);

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

   e_widget_ilist_go(cfdata->gui.lang_ilist);
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

   e_widget_ilist_go(cfdata->gui.selected_lang_ilist);
   if (indx >= 0)
     {
	if (indx >= e_widget_ilist_count(cfdata->gui.selected_lang_ilist))
	  indx = e_widget_ilist_count(cfdata->gui.selected_lang_ilist) - 1;

	e_widget_ilist_selected_set(cfdata->gui.selected_lang_ilist, indx);
     }
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

   lang->id = evas_list_count(cfdata->selected_languages);
   lang->lang_name = evas_stringshare_add(ld->lang_name);
   lang->lang_shortcut = evas_stringshare_add(ld->lang_shortcut);
   lang->lang_flag = !(ld->lang_flag) ? NULL : evas_stringshare_add(ld->lang_flag);
   //FIXME: assign current keyboard model
   lang->kbd_model = evas_stringshare_add("compaqik13");
   lang->kbd_layout = evas_stringshare_add(ld->kbd_layout);
   lang->kbd_variant = evas_stringshare_add("basic");
   // = !(ld->kbd_variant) ? NULL : evas_stringshare_add(ld->kbd_variant->data);

   cfdata->selected_languages = evas_list_append(cfdata->selected_languages, lang);
   //DO NOT sort selected_languages list. Here the sequence of the langs is essential!

   _lang_update_lang_defined_list(cfdata);
   _lang_update_selected_lang_list(cfdata);
   e_widget_ilist_selected_set(cfdata->gui.selected_lang_ilist,
			       evas_list_count(cfdata->selected_languages) - 1);

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

   if (!evas_list_count(cfdata->selected_languages))
     { 
	_lang_update_kbd_model_list(cfdata);
	_lang_update_kbd_layout_variant_list(cfdata);
     }
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
_lang_update_language_moveup_button(E_Config_Dialog_Data *cfdata)
{
   if (!cfdata) return;

   if (evas_list_count(cfdata->selected_languages) < 1 ||
       !e_widget_ilist_selected_label_get(cfdata->gui.selected_lang_ilist) ||
       !e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist))
     { 
	e_widget_disabled_set(cfdata->gui.btn_move_up, 1);
     }
   else
     e_widget_disabled_set(cfdata->gui.btn_move_up, 0);
}
static void
_lang_update_language_movedown_button(E_Config_Dialog_Data *cfdata)
{
   if (!cfdata) return;

   if ((evas_list_count(cfdata->selected_languages) < 1) ||
       (!e_widget_ilist_selected_label_get(cfdata->gui.selected_lang_ilist)) ||
       (e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist) ==
	 e_widget_ilist_count(cfdata->gui.selected_lang_ilist) - 1))
     { 
	e_widget_disabled_set(cfdata->gui.btn_move_down, 1);
     }
   else
     e_widget_disabled_set(cfdata->gui.btn_move_down, 0);
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
   _lang_update_language_moveup_button(cfdata);
   _lang_update_language_movedown_button(cfdata);

   _lang_update_kbd_model_list(cfdata); 
   _lang_update_kbd_layout_variant_list(cfdata);
}
static void
_lang_update_kbd_model_list(E_Config_Dialog_Data *cfdata)
{
   int	 kbd_model_indx = -1;
   Language *selected_lang;
   Language_Kbd_Model *lkm;
   Evas_List *l;

   if (!cfdata) return;

   selected_lang = evas_list_nth(cfdata->selected_languages,
				 e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist));

   if (!selected_lang)
     {
	e_widget_ilist_clear(cfdata->gui.kbd_model_ilist);
	return;
     }

   if (!e_widget_ilist_count(cfdata->gui.kbd_model_ilist))
     {
	char buf[4096];
	int i;

	for (l = language_kbd_model_list, i = 0; l; l = l->next, i++)
	  {
	     lkm = l->data;

	     if (!strcmp(lkm->kbd_model, selected_lang->kbd_model))
	       kbd_model_indx = i;

	     //snprintf(buf, sizeof(buf), "%s (%s)", lkm->kbd_model_desctiption, lkm->kbd_model);
	     snprintf(buf, sizeof(buf), "%s", lkm->kbd_model_desctiption);
	     e_widget_ilist_append(cfdata->gui.kbd_model_ilist, NULL, buf, NULL, NULL, NULL);
	  }
     }
   else
     {
	for (l = language_kbd_model_list, kbd_model_indx = 0; l; l = l->next, kbd_model_indx++)
	  {
	     lkm = l->data;
	     if (!strcmp(lkm->kbd_model, selected_lang->kbd_model))
	       break;
	  }
	if (!l)
	  kbd_model_indx = -1;
     }

   e_widget_ilist_go(cfdata->gui.kbd_model_ilist);
   e_widget_ilist_selected_set(cfdata->gui.kbd_model_ilist, kbd_model_indx);
}

static void
_lang_update_kbd_layout_variant_list(E_Config_Dialog_Data *cfdata)
{
   Language	  *selected_lang;
   Language_Def	  *ld;
   Evas_List	  *l;
   int		  kbd_variant_indx = 0;

   if (!cfdata) return;

   selected_lang = evas_list_nth(cfdata->selected_languages,
				 e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist));

   e_widget_ilist_clear(cfdata->gui.kbd_layout_variant_ilist);

   if (!selected_lang) return;

   for (l = language_def_list; l; l = l->next)
     {
	ld = l->data;

	if (!strcmp(ld->lang_name, selected_lang->lang_name))
	  {
	     int i;
	     e_widget_ilist_append(cfdata->gui.kbd_layout_variant_ilist,
				   NULL, "basic", NULL, NULL, NULL);

	     for (l = ld->kbd_variant, i = 1; l; l = l->next)
	       {
		  char *variant = l->data;
		  if (!strcmp(variant, "basic")) continue;

		  if (selected_lang->kbd_variant && !strcmp(selected_lang->kbd_variant, variant))
		    kbd_variant_indx = i;

		  e_widget_ilist_append(cfdata->gui.kbd_layout_variant_ilist,
				        NULL, variant, NULL, NULL, NULL);
		  i++;
	       }
	     break;
	  }
     }

   e_widget_ilist_go(cfdata->gui.kbd_layout_variant_ilist);
   e_widget_ilist_selected_set(cfdata->gui.kbd_layout_variant_ilist, kbd_variant_indx);
}

static void 
_lang_move_language_order_up_cb(void *data, void *data2)
{
   E_Config_Dialog_Data	*cfdata;
   Evas_List   *lang, *lang2;
   void *tmp;
   int indx;

   if (!(cfdata = data)) return;

   indx = e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist);

   if (indx <= 0) return;

   lang = evas_list_nth_list(cfdata->selected_languages, indx);
   lang2 = evas_list_nth_list(cfdata->selected_languages, indx - 1);

   ((Language *)(lang->data))->id --;
   ((Language *)(lang2->data))->id ++;
   tmp = lang->data;
   lang->data = lang2->data;
   lang2->data = tmp;

   _lang_update_selected_lang_list(cfdata);
   e_widget_ilist_selected_set(cfdata->gui.selected_lang_ilist, indx - 1);
}
static void 
_lang_move_language_order_down_cb(void *data, void *data2)
{
   E_Config_Dialog_Data	*cfdata;
   Evas_List *lang, *lang2;
   int indx;
   void *tmp;

   if (!(cfdata = data)) return;

   indx = e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist);

   if ((indx < 0) || (indx >= evas_list_count(cfdata->selected_languages))) return;

   lang = evas_list_nth_list(cfdata->selected_languages, indx);
   lang2 = evas_list_nth_list(cfdata->selected_languages, indx + 1);

   ((Language *)(lang->data))->id ++;
   ((Language *)(lang->data))->id --;
   tmp = lang->data;
   lang->data = lang2->data;
   lang2->data = tmp;

   _lang_update_selected_lang_list(cfdata);
   e_widget_ilist_selected_set(cfdata->gui.selected_lang_ilist, indx + 1);
}

static void 
_lang_kbd_layout_variant_ilist_cb_change(void *data, Evas_Object *obj)
{
   E_Config_Dialog_Data	*cfdata;
   Language *lang;
   int	 indx_variant, indx_lang;

   if (!(cfdata = data)) return;

   indx_lang = e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist);
   if (indx_lang < 0) return;

   indx_variant = e_widget_ilist_selected_get(cfdata->gui.kbd_layout_variant_ilist);
   if (indx_variant < 0) return;

   lang = evas_list_nth(cfdata->selected_languages, indx_lang);
   if (indx_variant == 0)
     {
	if (lang->kbd_variant) evas_stringshare_del(lang->kbd_variant);
	lang->kbd_variant = evas_stringshare_add("basic");
	return;
     }
   else
     {
	Evas_List    *l;
	Language_Def *ld;
	char *variant;

	for (l = language_def_list; l; l = l->next)
	  {
	     ld = l->data;
	     if (!strcmp(ld->lang_name, lang->lang_name))
	       {
		  variant = evas_list_nth(ld->kbd_variant, indx_variant - 1);

		  if (lang->kbd_variant) evas_stringshare_del(lang->kbd_variant);
		  lang->kbd_variant = evas_stringshare_add(variant);
		  break;
	       }
	  }
     }
}
static void 
_lang_kbd_model_ilist_cb_change(void *data, Evas_Object *obj)
{
   E_Config_Dialog_Data	*cfdata;
   Language_Kbd_Model	*lkm;
   Language		*lang;
   int	 indx_model;
   int	 indx_lang;

   if (!(cfdata = data)) return;

   indx_lang = e_widget_ilist_selected_get(cfdata->gui.selected_lang_ilist);
   if (indx_lang < 0) return;

   indx_model = e_widget_ilist_selected_get(cfdata->gui.kbd_model_ilist);
   if (indx_model < 0) return;

   lang	 = evas_list_nth(cfdata->selected_languages, indx_lang);
   lkm	 = evas_list_nth(language_kbd_model_list, indx_model);

   if (!lang || !lkm) return;

   if (lang->kbd_model) evas_stringshare_del(lang->kbd_model);
   lang->kbd_model = evas_stringshare_add(lkm->kbd_model);
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

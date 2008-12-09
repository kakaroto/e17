/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0
 */
#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data 
{
   int sources_index, views_index;

   Evas_Object *toolbar, *main_list;
   Evas_Object *sources_comment, *views_comment;
   Eina_List *packed_widgets;

   Eina_List *sources, *views;
   const char *source, *view;

   Config_Item *ci;
   void *data;
};

/* Local Function Prototypes */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _conf_plugin_sel(void *data1, void *data2);
static void _conf_plugin_set(void *data1, void *data2);
static void _sources_change_cb(void *data, Evas_Object *obj, void *event_info);
static void _views_change_cb(void *data, Evas_Object *obj, void *event_info);

/* External Functions */
EAPI E_Config_Dialog *
e_int_config_drawer_module(E_Container *con, Config_Item *ci) 
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   /* is this config dialog already visible ? */
   if (e_config_dialog_find("Drawer", "_e_module_drawer_cfg_dlg")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", drawer_conf->module->dir);

   /* create new config dialog */
   cfd = e_config_dialog_new(con, D_("Drawer Module"), "Drawer", 
                             "_e_module_drawer_cfg_dlg", buf, 0, v, ci);

   e_dialog_resizable_set(cfd->dia, 1);
   drawer_conf->cfd = cfd;
   return cfd;
}

/* Local Functions */
static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   cfdata->ci = cfd->data;
   _fill_data(cfdata);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   drawer_plugins_list_free(cfdata->views);
   drawer_plugins_list_free(cfdata->sources);

   if (cfdata->view)
     eina_stringshare_del(cfdata->view);
   if (cfdata->source)
     eina_stringshare_del(cfdata->source);

   drawer_conf->cfd = NULL;
   E_FREE(cfdata);
}

static void 
_fill_data(E_Config_Dialog_Data *cfdata) 
{
   cfdata->view = eina_stringshare_add(cfdata->ci->view);
   cfdata->source = eina_stringshare_add(cfdata->ci->source);
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o = NULL, *of = NULL, *ow = NULL, *oi = NULL;

   o = e_widget_list_add(evas, 0, 0);

   cfdata->toolbar = of = e_widget_toolbar_add(evas, 32 * e_scale, 32 * e_scale);
   evas_event_freeze(evas);
   edje_freeze();

   oi = edje_object_add(evas);
   e_util_edje_icon_set(oi, "enlightenment/extensions");
   e_widget_toolbar_item_append(of, oi, D_("Plugins selection"), _conf_plugin_sel, cfd, cfdata);
   oi = edje_object_add(evas);
   e_util_edje_icon_set(oi, "enlightenment/configuration");
   e_widget_toolbar_item_append(of, oi, D_("Plugins settings"), _conf_plugin_set, cfd, cfdata);

   edje_thaw();
   evas_event_thaw(evas);

   e_widget_list_object_append(o, of, 1, 0, 0.5);

   cfdata->main_list = of = e_widget_list_add(evas, 0, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   e_widget_focus_set(cfdata->toolbar, 1);
   e_widget_toolbar_item_select(cfdata->toolbar, 0);

   return o;
}

static int 
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   if (cfdata->ci->view) eina_stringshare_del(cfdata->ci->view);
   if (cfdata->ci->source) eina_stringshare_del(cfdata->ci->source);

   if (strcmp(cfdata->ci->view, cfdata->view))
     drawer_plugin_load(cfdata->ci, DRAWER_VIEWS, cfdata->view);
   if (strcmp(cfdata->ci->source, cfdata->source))
     drawer_plugin_load(cfdata->ci, DRAWER_SOURCES, cfdata->source);

   e_config_save_queue();
   return 1;
}

static void
_conf_plugin_sel(void *data1, void *data2)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;
   Evas_Object *of, *ol, *or, *packed, *otx;
   E_Radio_Group *org;
   Evas *evas;
   Eina_List *l;
   const char *comment = NULL;
   int i = 0;

   cfd = data1;
   cfdata = data2;

   of = cfdata->main_list;
   evas = evas_object_evas_get(of);

   evas_event_freeze(evas);
   edje_freeze();

   EINA_LIST_FOREACH(cfdata->packed_widgets, l, packed)
      evas_object_del(packed);

   ol = e_widget_list_add(evas, 1, 1);
   e_widget_list_object_append(of, ol, 1, 1, 0.5);
   cfdata->packed_widgets = eina_list_append(cfdata->packed_widgets, ol);

   /* XXX: replace radio widgets with ilist? */
   of = e_widget_framelist_add(evas, D_("Source plugins"), 0);
   org = e_widget_radio_group_new(&cfdata->sources_index);
   cfdata->sources = drawer_plugins_list(DRAWER_SOURCES);
   for (l = cfdata->sources; l; l = l->next)
     {
	Drawer_Plugin_Type *pi = l->data;

	if (!(strcmp(cfdata->source, pi->name)))
	  {
	     cfdata->sources_index = i;
	     if (pi->comment)
	       comment = pi->comment;
	  }

	or = e_widget_radio_add(evas, pi->title, i++, org);
	e_widget_framelist_object_append(of, or);
	evas_object_data_set(or, "drawer_plugin_type", pi);
	evas_object_smart_callback_add(or, "changed", _sources_change_cb, cfdata);
     }
   cfdata->sources_comment = otx = e_widget_textblock_add(evas);
   if (comment)
     e_widget_textblock_markup_set(otx, comment);
   else
     e_widget_textblock_markup_set(otx, D_("Description: Unavailable"));
   e_widget_framelist_object_append(of, otx);

   e_widget_list_object_append(ol, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("View plugins"), 0);
   org = e_widget_radio_group_new(&cfdata->views_index);
   cfdata->views = drawer_plugins_list(DRAWER_VIEWS);
   for (l = cfdata->views, i = 0, comment = NULL; l; l = l->next)
     {
	Drawer_Plugin_Type *pi = l->data;

	if (!(strcmp(cfdata->view, pi->name)))
	  {
	     cfdata->views_index = i;
	     if (pi->comment)
	       comment = pi->comment;
	  }

	or = e_widget_radio_add(evas, pi->title, i++, org);
	e_widget_framelist_object_append(of, or);
	evas_object_data_set(or, "drawer_plugin_type", pi);
	evas_object_smart_callback_add(or, "changed", _views_change_cb, cfdata);
     }
   cfdata->views_comment = otx = e_widget_textblock_add(evas);
   if (comment)
     e_widget_textblock_markup_set(otx, comment);
   else
     e_widget_textblock_markup_set(otx, D_("Description: Unavailable"));
   e_widget_framelist_object_append(of, otx);

   e_widget_list_object_append(ol, of, 1, 1, 0.5);

   edje_thaw();
   evas_event_thaw(evas);
}

static void
_sources_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   E_Config_Dialog_Data *cfdata;
   Drawer_Plugin_Type *pi;
   
   cfdata = data;
   pi = evas_object_data_get(obj, "drawer_plugin_type");
   cfdata->source = eina_list_nth(cfdata->sources, cfdata->sources_index);

   if (pi->comment)
     e_widget_textblock_markup_set(cfdata->sources_comment, pi->comment);
   else
     e_widget_textblock_markup_set(cfdata->sources_comment,
	   D_("Description: Unavailable"));
}

static void
_views_change_cb(void *data, Evas_Object *obj, void *event_info)
{
   E_Config_Dialog_Data *cfdata;
   Drawer_Plugin_Type *pi;
   const char *name;
   
   cfdata = data;
   pi = evas_object_data_get(obj, "drawer_plugin_type");
   cfdata->view = eina_list_nth(cfdata->views, cfdata->views_index);

   if (pi->comment)
     e_widget_textblock_markup_set(cfdata->views_comment, pi->comment);
   else
     e_widget_textblock_markup_set(cfdata->views_comment,
	   D_("Description: Unavailable"));
}

static void
_conf_plugin_set(void *data1, void *data2)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;
   Evas_Object *of, *ol, *packed;
   Eina_List *l;
   Evas *evas;

   cfd = data1;
   cfdata = data2;

   of = cfdata->main_list;
   evas = evas_object_evas_get(of);

   evas_event_freeze(evas);
   edje_freeze();

   EINA_LIST_FOREACH(cfdata->packed_widgets, l, packed)
      evas_object_del(packed);

   ol = e_widget_list_add(evas, 0, 0);
   e_widget_list_object_append(of, ol, 1, 1, 0.5);
   cfdata->packed_widgets = eina_list_append(cfdata->packed_widgets, ol);

   of = e_widget_framelist_add(evas, D_("Source settings"), 1);
   e_widget_framelist_object_append(of, drawer_plugin_config_button_get(cfdata->ci, evas, DRAWER_SOURCES));
   e_widget_framelist_content_align_set(of, 0.5, 0.5);
   e_widget_list_object_append(ol, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("View settings"), 1);
   e_widget_framelist_object_append(of, drawer_plugin_config_button_get(cfdata->ci, evas, DRAWER_VIEWS));
   e_widget_framelist_content_align_set(of, 0.5, 0.5);
   e_widget_list_object_append(ol, of, 1, 1, 0.5);

   edje_thaw();
   evas_event_thaw(evas);
}


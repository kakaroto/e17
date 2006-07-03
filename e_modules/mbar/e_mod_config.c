#include <e.h>
#include "e_mod_config.h"
#include "config.h"

struct _E_Config_Dialog_Data
{
   int follower;
   int iconsize;
   int allow_overlap;
   double follow_speed;
   E_Container *con;
   E_App *apps;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);

static void _new_point(void *data, void *data2);
static void _edit_point(void *data, void *data2);
static void _delete_point(void *data, void *data2);

void
_config_mbar_module(E_Container *con, MBar *mbar)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];
   
   v = E_NEW(E_Config_Dialog_View, 1);

   /* Dialog Methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   /* Create The Dialog */
   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(mbar->module));
   cfd = e_config_dialog_new(con, _("Mount Bar Configuration"), buf, 0, v, mbar);
   mbar->config_dialog = cfd;
}

static void
_fill_data(MBar *mb, E_Config_Dialog_Data *cfdata)
{
   cfdata->follower = mb->conf->follower;
   cfdata->iconsize = mb->conf->iconsize;
   cfdata->allow_overlap = mb->conf->allow_overlap;
   cfdata->follow_speed = mb->conf->follow_speed;
   cfdata->apps = mb->apps;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   MBar *mb;

   mb = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(mb, cfdata);
   cfdata->con = cfd->con;
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   MBar *mbar;

   mbar = cfd->data;
   mbar->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *ob, *of, *ot, *il;
   Evas_List *l;
   MBar *mb;
   char buf[4096];

   mb = cfd->data;
   
   o = e_widget_list_add(evas, 0, 0);
   ob = e_widget_check_add(evas, _("Show Follower"), &(cfdata->follower));
   e_widget_list_object_append(o, ob, 1, 1, 0.5);

   ob = e_widget_check_add(evas, _("Allow windows to overlap this gadget"), &(cfdata->allow_overlap));
   e_widget_list_object_append(o, ob, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("Mount Points"), 0);
   ot = e_widget_table_add(evas, 0);
   il = e_widget_ilist_add(evas, 32, 32, NULL);
   e_widget_min_size_set(il, 100, 130);
   if (mb->apps->subapps)
     {
        for (l = mb->apps->subapps; l; l = l->next)
          {
             E_App *app;
             Evas_Object *ic;

             app = l->data;
             snprintf(buf, sizeof(buf), "%s", app->path);

             ic = edje_object_add(evas_object_evas_get(ob));
             edje_object_file_set(ic, buf, "icon");
             e_widget_ilist_append(il, ic, strdup(app->name), NULL, app, strdup(app->name));
          }
     }
   e_widget_ilist_go(il);
   e_widget_table_object_append(ot, il, 0, 0, 3, 3, 1, 0, 1, 0);

   ob = e_widget_button_add(evas, _("New Point"), NULL, _new_point, il, cfd);
   e_widget_table_object_append(ot, ob, 0, 3, 1, 1, 1, 0, 1, 0);
   ob = e_widget_button_add(evas, _("Edit Point"), NULL, _edit_point, il, cfd);
   e_widget_table_object_append(ot, ob, 1, 3, 1, 1, 1, 0, 1, 0);
   ob = e_widget_button_add(evas, _("Delete Point"), NULL, _delete_point, il, cfd);
   e_widget_table_object_append(ot, ob, 2, 3, 1, 1, 1, 0, 1, 0);

   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   MBar *mb;

   mb = cfd->data;
   e_border_button_bindings_ungrab_all();
   if ((cfdata->follower) && (!mb->conf->follower))
      mb->conf->follower = 1;
   else if (!(cfdata->follower) && (mb->conf->follower))
      mb->conf->follower = 0;

   if (cfdata->allow_overlap && !mb->conf->allow_overlap)
      mb->conf->allow_overlap = 1;
   else if (!cfdata->allow_overlap && mb->conf->allow_overlap)
      mb->conf->allow_overlap = 0;

   e_border_button_bindings_grab_all();
   e_config_save_queue();

   _mbar_bar_cb_config_updated(mb);
   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *il, *ot;
   Evas_List *l;
   char buf[4096];

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, _("Follower"), 0);
   ob = e_widget_check_add(evas, _("Visible"), &(cfdata->follower));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, _("Follow Speed"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.2f px/s"), 0.0, 1.0, 0.01, 0, &(cfdata->follow_speed), NULL, 200);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("Icon Size"), 0);
   ob = e_widget_slider_add(evas, 1, 0, _("%3.0f pixels"), 8.0, 128.0, 1.0, 0, NULL, &(cfdata->iconsize), 200);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   /* allow overlap checkbox */
   of = e_widget_framelist_add(evas, _("Extras"), 0);
   ob = e_widget_check_add(evas, _("Allow windows to overlap this gadget"), &(cfdata->allow_overlap));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   MBar *mb;

   mb = cfd->data;
   e_border_button_bindings_ungrab_all();
   if ((cfdata->follower) && (!mb->conf->follower))
      mb->conf->follower = 1;
   else if (!(cfdata->follower) && (mb->conf->follower))
      mb->conf->follower = 0;

   /* allow overlap check box */
   if (cfdata->allow_overlap && !mb->conf->allow_overlap)
      mb->conf->allow_overlap = 1;
   else if (!cfdata->allow_overlap && mb->conf->allow_overlap)
      mb->conf->allow_overlap = 0;

   if (cfdata->iconsize != mb->conf->iconsize)
      mb->conf->iconsize = cfdata->iconsize;
   if (cfdata->follow_speed != mb->conf->follow_speed)
      mb->conf->follow_speed = cfdata->follow_speed;

   e_border_button_bindings_grab_all();
   e_config_save_queue();

   _mbar_bar_cb_config_updated(mb);
   return 1;
}

static void
_new_point(void *data, void *data2)
{
   E_App *app;
   Evas_Object *il;
   E_Config_Dialog *cfd;
   MBar *mb;
   
   cfd = data2;
   il = data;
   mb = cfd->data;
   
   app = e_app_raw_new();
   point_edit_show(cfd->cfdata->con, app, mb->apps, il);
}

static void
_edit_point(void *data, void *data2)
{
   Evas_Object *il;
   E_Config_Dialog *cfd;
   Evas_List *l;
   MBar *mb;
   const char *name;

   il = data;
   cfd = data2;
   mb = cfd->data;
   
   name = e_widget_ilist_selected_label_get(il);
   if (!name)
      return;

   for (l = mb->apps->subapps; l; l = l->next)
     {
        E_App *app;

        app = l->data;
        if (!strcmp(app->name, name))
          {
	     point_edit_show(cfd->cfdata->con, app, mb->apps, il);
             break;
          }
     }
}

static void
_delete_point(void *data, void *data2)
{
   Evas_Object *il;
   E_Config_Dialog *cfd;
   Evas_List *l;
   MBar *mb;
   const char *name;

   il = data;
   cfd = data2;
   mb = cfd->data;

   name = e_widget_ilist_selected_label_get(il);
   if (!name)
      return;

   for (l = mb->apps->subapps; l; l = l->next)
     {
        E_App *app;

        app = l->data;
        if (!strcmp(app->name, name))
          {
             e_app_remove(app);
             e_widget_ilist_remove_label(il, (char *)name);
             break;
          }
     }
}

void
_mbar_point_new(Evas_Object *il, E_App *eap)
{
   Evas_Object *ic;
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s", eap->path);
   ic = edje_object_add(evas_object_evas_get(il));
   edje_object_file_set(ic, buf, "icon");
   e_widget_ilist_append(il, ic, strdup(eap->name), NULL, eap, strdup(eap->name));
}

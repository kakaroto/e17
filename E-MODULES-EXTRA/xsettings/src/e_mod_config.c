#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
  int match_e17_theme;
  int match_e17_icon_theme;
  char *gtk_theme;
  char *icon_theme;

  Evas_Object *o_label_icon_theme;
  Evas_Object *o_label_theme;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);


E_Config_Dialog *
e_int_config_xsettings_module(E_Container *con, const char *params)
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   if (e_config_dialog_find("Xsettings", "advanced/xsettings")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   snprintf(buf, sizeof(buf), "%s/e-module-xsettings.edj", xsettings_conf->module->dir);

   cfd = e_config_dialog_new(con, D_("Xsettings Module"), "Xsettings",
			     "advanced/xsettings", buf, 0, v, NULL);

   e_dialog_resizable_set(cfd->dia, 1);
   xsettings_conf->cfd = cfd;
   return cfd;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   xsettings_conf->cfd = NULL;

   if (cfdata->gtk_theme) free(cfdata->gtk_theme);
   if (cfdata->icon_theme) free(cfdata->icon_theme);
   E_FREE(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
   cfdata->match_e17_theme = xsettings_conf->match_e17_theme;
   cfdata->match_e17_icon_theme = xsettings_conf->match_e17_icon_theme;

   if (xsettings_conf->gtk_theme)
     cfdata->gtk_theme = strdup(xsettings_conf->gtk_theme);

   if (xsettings_conf->icon_theme)
     cfdata->icon_theme = strdup(xsettings_conf->icon_theme);
}

static void
_theme_label_set(E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o = cfdata->o_label_theme;

   if (cfdata->match_e17_theme)
     {
	e_widget_disabled_set(o, 1);
	e_widget_label_text_set(o, "");

	E_Config_Theme *ct;

	if ((ct = e_theme_config_get("theme")))
	  {
	     char *theme;

	     if ((theme = edje_file_data_get(ct->file, "gtk-theme")))
	       e_widget_entry_text_set(o, theme);
	  }
     }
   else
     {
	e_widget_disabled_set(o, 0);
	e_widget_entry_text_set(o, xsettings_conf->gtk_theme);
     }
}

static void
_cb_on_change1(void *data, Evas_Object *obj)
{
   _theme_label_set(data);
}

static void
_icon_theme_label_set(E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o = cfdata->o_label_icon_theme;

   if (cfdata->match_e17_icon_theme)
     {
	e_widget_disabled_set(o, 1);
	e_widget_entry_text_set(o, e_config->icon_theme);
     }
   else
     {
	e_widget_disabled_set(o, 0);
	e_widget_entry_text_set(o, xsettings_conf->icon_theme);
     }
}

static void
_cb_on_change2(void *data, Evas_Object *obj)
{
   _icon_theme_label_set(data);
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o = NULL, *of = NULL, *ow = NULL;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, D_("General"), 0);
   e_widget_framelist_content_align_set(of, 0.0, 0.0);
   ow = e_widget_check_add(evas, D_("Use Gtk theme suggested by e17 theme"),
			   &(cfdata->match_e17_theme));
   e_widget_on_change_hook_set(ow, _cb_on_change1, cfdata);
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_label_add(evas, D_("Use Gtk theme:"));
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_entry_add(evas, &cfdata->gtk_theme, NULL, NULL, NULL);
   cfdata->o_label_theme = ow;
   _theme_label_set(cfdata);
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_check_add(evas, D_("Use Gtk icon theme matching e17"),
			   &(cfdata->match_e17_icon_theme));
   e_widget_on_change_hook_set(ow, _cb_on_change2, cfdata);
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_label_add(evas, D_("Use icon theme:"));
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_entry_add(evas, &cfdata->icon_theme, NULL, NULL, NULL);
   cfdata->o_label_icon_theme = ow;
   _icon_theme_label_set(cfdata);
   e_widget_framelist_object_append(of, ow);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   xsettings_conf->match_e17_theme = cfdata->match_e17_theme;
   xsettings_conf->match_e17_icon_theme = cfdata->match_e17_icon_theme;

   if (cfdata->gtk_theme && cfdata->gtk_theme[0])
     eina_stringshare_replace(&xsettings_conf->gtk_theme, cfdata->gtk_theme);
   else
     eina_stringshare_replace(&xsettings_conf->gtk_theme, NULL);

   if (cfdata->icon_theme && cfdata->icon_theme[0])
     eina_stringshare_replace(&xsettings_conf->icon_theme, cfdata->icon_theme);
   else
     eina_stringshare_replace(&xsettings_conf->icon_theme, NULL);

   xsettings_config_update();

   e_config_save_queue();
   return 1;
}

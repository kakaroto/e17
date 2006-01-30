#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"

typedef struct _Cfg_File_Data Cfg_File_Data;

struct _E_Config_Dialog_Data {
   int                 resolution;
};

struct _Cfg_File_Data {
   E_Config_Dialog    *cfd;
   char               *file;
};

/* Protos */
static void        *_create_data(E_Config_Dialog * cfd);
static void         _free_data(E_Config_Dialog * cfd,
                               E_Config_Dialog_Data * cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog * cfd, Evas * evas,
                                          E_Config_Dialog_Data * cfdata);
static int          _basic_apply_data(E_Config_Dialog * cfd,
                                      E_Config_Dialog_Data * cfdata);

void
_config_tclock_module(E_Container * con, TClock_Face * f)
{
   E_Config_Dialog    *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   cfd = e_config_dialog_new(con, _("Tclock Configuration"), NULL, 0, v, f);
}

static void
_fill_data(TClock_Face * f, E_Config_Dialog_Data * cfdata)
{
   cfdata->resolution = f->conf->resolution;
}

static void        *
_create_data(E_Config_Dialog * cfd)
{
   E_Config_Dialog_Data *cfdata;
   TClock_Face        *f;

   f = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(f, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog * cfd, Evas * evas,
                      E_Config_Dialog_Data * cfdata)
{
   Evas_Object        *o, *of, *ob;
   E_Radio_Group      *rg;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("Resolution"), 0);
   rg = e_widget_radio_group_new(&(cfdata->resolution));
   ob = e_widget_radio_add(evas, _("1 Minute"), RESOLUTION_MINUTE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("1 Second"), RESOLUTION_SECOND, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int
_basic_apply_data(E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
   TClock_Face        *f;

   f = cfd->data;
   e_border_button_bindings_ungrab_all();
   f->conf->resolution = cfdata->resolution;
   e_config_save_queue();
   e_border_button_bindings_grab_all();
   return 1;
}

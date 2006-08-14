#include "e_mod_config.h"

typedef struct _Cfg_File_Data Cfg_File_Data;

struct _E_Config_Dialog_Data
{
};

struct _Cfg_File_Data
{
   E_Config_Dialog *cfd;
   char *file;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

void
_config_emu_module(E_Container *con, Emu_Face *emu_face)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   /* Dialog Methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   /* Create The Dialog */
   cfd = e_config_dialog_new(con, D_("Emu Configuration"), "Emu", "_e_modules_emu_config_dialog", NULL, 0, v, emu_face);
   emu_face->config_dialog = cfd;
}

static void
_fill_data(Emu_Face *emu_face, E_Config_Dialog_Data *cfdata)
{
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Emu_Face *emu_face;

   emu_face = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(emu_face, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Emu_Face *emu_face;

   emu_face = cfd->data;
   emu_face->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o;

   o = e_widget_list_add(evas, 0, 0);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Emu_Face *emu_face;

   emu_face = cfd->data;
   e_border_button_bindings_ungrab_all();


   e_border_button_bindings_grab_all();
   e_config_save_queue();

   _emu_cb_config_updated(emu_face);
   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o;

   o = e_widget_list_add(evas, 0, 0);

   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Emu_Face *emu_face;

   emu_face = cfd->data;
   e_border_button_bindings_ungrab_all();


   e_border_button_bindings_grab_all();
   e_config_save_queue();

   _emu_cb_config_updated(emu_face);
   return 1;
}

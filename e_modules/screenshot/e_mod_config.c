#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

#define S_METHOD_IMPORT 0
#define S_METHOD_SCROT 1

typedef struct _Cfg_File_Data Cfg_File_Data;

struct _E_Config_Dialog_Data {
   Screen             *screen;

   /* Basic */
   int                 method;
   int                 use_import;
   int                 use_scrot;
   double              delay_time;
   char               *location;
   char               *filename;
#ifdef HAVE_IMPORT
   struct {
      int                 use_img_border;
      int                 use_dither;
      int                 use_frame;
      int                 use_window;
      int                 use_silent;
      int                 use_trim;
   } import;
#endif
#ifdef HAVE_SCROT
   struct {
      int                 use_img_border;
      int                 use_count;
      int                 use_window;
      int                 use_thumb;
   } scrot;
#endif
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
static Evas_Object *_advanced_create_widgets(E_Config_Dialog * cfd, Evas * evas,
                                             E_Config_Dialog_Data * cfdata);
static int          _advanced_apply_data(E_Config_Dialog * cfd,
                                         E_Config_Dialog_Data * cfdata);

/* Config Calls */
void
_config_screenshot_module(E_Container * con, Screen * s)
{
   E_Config_Dialog    *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   /* methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   /* create config diaolg */
   cfd = e_config_dialog_new(con, _("Screenshot Configuration"), NULL, 0, v, s);
   s->config_dialog = cfd;
}

static void
_fill_data(Screen * sc, E_Config_Dialog_Data * cfdata)
{
   if (sc->conf->use_import == 1)
     {
        cfdata->method = S_METHOD_IMPORT;
     }
   else if (sc->conf->use_scrot == 1)
     {
        cfdata->method = S_METHOD_SCROT;
     }

   cfdata->delay_time = sc->conf->delay_time;

   if (sc->conf->location != NULL)
     {
        cfdata->location = strdup(sc->conf->location);
     }
   else
     {
        cfdata->location = NULL;
     }

   if (sc->conf->filename != NULL)
     {
        cfdata->filename = strdup(sc->conf->filename);
     }
   else
     {
        cfdata->filename = NULL;
     }

#ifdef HAVE_IMPORT
   cfdata->import.use_img_border = sc->conf->import.use_img_border;
   cfdata->import.use_dither = sc->conf->import.use_dither;
   cfdata->import.use_frame = sc->conf->import.use_frame;
   cfdata->import.use_window = sc->conf->import.use_window;
   cfdata->import.use_silent = sc->conf->import.use_silent;
   cfdata->import.use_trim = sc->conf->import.use_trim;
#endif
#ifdef HAVE_SCROT
   cfdata->scrot.use_img_border = sc->conf->scrot.use_img_border;
   //cfdata->scrot->use_count = sc->conf->scrot.use_count;
   //cfdata->scrot->use_window = sc->conf->scrot.use_window;
   cfdata->scrot.use_thumb = sc->conf->scrot.use_thumb;
#endif
}

static void        *
_create_data(E_Config_Dialog * cfd)
{
   E_Config_Dialog_Data *cfdata;
   Screen             *s;

   s = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   return cfdata;
}

static void
_free_data(E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
   Screen             *s;

   s = cfd->data;
   s->config_dialog = NULL;
   /* Free the cfdata */
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog * cfd, Evas * evas,
                      E_Config_Dialog_Data * cfdata)
{
   Evas_Object        *o, *of, *ob, *ot;
   E_Radio_Group      *rg;
   Screen             *s;

   s = cfd->data;
   _fill_data(s, cfdata);

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("General Settings"), 0);

   ot = e_widget_table_add(evas, 0);
#ifdef HAVE_IMPORT
# ifdef HAVE_SCROT
   rg = e_widget_radio_group_new(&(cfdata->method));
   ob = e_widget_radio_add(evas, _("Use Import"), S_METHOD_IMPORT, rg);
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 1, 0, 1, 0);
   ob = e_widget_radio_add(evas, _("Use Scrot"), S_METHOD_SCROT, rg);
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 1, 0, 1, 0);
# endif
#else
   /* Dont need to add an option as we only have one to use */
#endif

   ob = e_widget_label_add(evas, _("Delay Time:"));
   e_widget_table_object_append(ot, ob, 0, 2, 1, 1, 0, 0, 1, 0);

   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 0.0, 60.0, 1.0, 0,
                            &(cfdata->delay_time), NULL, 200);
   e_widget_table_object_append(ot, ob, 1, 2, 1, 1, 1, 0, 1, 0);

   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("File Settings"), 0);
   ot = e_widget_table_add(evas, 1);

   ob = e_widget_label_add(evas, _("Save Directory:"));
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 0, 0, 1, 0);

   ob = e_widget_entry_add(evas, &cfdata->location);
   e_widget_table_object_append(ot, ob, 1, 0, 1, 1, 1, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Filename (minus extension):"));
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 0, 0, 1, 0);

   ob = e_widget_entry_add(evas, &cfdata->filename);
   e_widget_table_object_append(ot, ob, 1, 1, 1, 1, 1, 0, 1, 0);

   ob = e_widget_label_add(evas,
                           _
                           ("Example: screenshot%d = screenshot1, screenshot2, etc..."));
   e_widget_table_object_append(ot, ob, 0, 2, 2, 1, 0, 0, 1, 0);

   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
   char                tmp[4096];
   int                 length;
   Screen             *sc;

   sc = cfd->data;

   /* Actually take our cfdata settings and apply them in real life */
   e_border_button_bindings_ungrab_all();
#ifdef HAVE_IMPORT
# ifdef HAVE_SCROT
   if (cfdata->method == S_METHOD_IMPORT)
     {
        sc->conf->use_import = 1;
        sc->conf->use_scrot = 0;
     }
   else
     {
        sc->conf->use_scrot = 1;
        sc->conf->use_import = 0;
     }
# else
   sc->conf->use_import = 1;
# endif
#else
   sc->conf->use_import = 0;
# ifdef HAVE_SCROT
   sc->conf->use_scrot = 1;
# else
   sc->conf->use_scrot = 0;
# endif
#endif

   if (cfdata->location == NULL)
     {
        sc->conf->location = strdup(e_user_homedir_get());
     }
   else
     {
        snprintf(tmp, sizeof(tmp), "%s", strdup(cfdata->location));
        if (tmp[(length = strlen(tmp) - 1)] == '/')
           tmp[length] = '\0';
        sc->conf->location = strdup(tmp);
     }
   if (cfdata->filename != NULL)
     {
        sc->conf->filename = strdup(cfdata->filename);
     }
   else
     {
        sc->conf->filename = NULL;
     }

   sc->conf->delay_time = cfdata->delay_time;
   e_config_save_queue();
   e_border_button_bindings_grab_all();

   return 1;                    /* Apply was OK */
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog * cfd, Evas * evas,
                         E_Config_Dialog_Data * cfdata)
{
   Evas_Object        *o, *ob, *of, *ot;
   E_Radio_Group      *rg;
   Screen             *s;

   s = cfd->data;
   _fill_data(s, cfdata);

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("General Settings"), 0);

   ot = e_widget_table_add(evas, 0);
#ifdef HAVE_IMPORT
# ifdef HAVE_SCROT
   rg = e_widget_radio_group_new(&(cfdata->method));
   ob = e_widget_radio_add(evas, _("Use Import"), S_METHOD_IMPORT, rg);
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 1, 0, 1, 0);
   ob = e_widget_radio_add(evas, _("Use Scrot"), S_METHOD_SCROT, rg);
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 1, 0, 1, 0);

# endif
#else
   /* Dont need to add an option as we only have one to use */
#endif

   ob = e_widget_label_add(evas, _("Delay Time:"));
   e_widget_table_object_append(ot, ob, 0, 2, 1, 1, 0, 0, 1, 0);

   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 0.0, 60.0, 1.0, 0,
                            &(cfdata->delay_time), NULL, 200);
   e_widget_table_object_append(ot, ob, 1, 2, 1, 1, 1, 0, 1, 0);

   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("File Settings"), 0);
   ot = e_widget_table_add(evas, 1);

   ob = e_widget_label_add(evas, _("Save Directory:"));
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 0, 0, 1, 0);

   ob = e_widget_entry_add(evas, &cfdata->location);
   e_widget_table_object_append(ot, ob, 1, 0, 1, 1, 1, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Filename (minus extension):"));
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 0, 0, 1, 0);

   ob = e_widget_entry_add(evas, &cfdata->filename);
   e_widget_table_object_append(ot, ob, 1, 1, 1, 1, 1, 0, 1, 0);

   ob = e_widget_label_add(evas,
                           _
                           ("Example: screenshot%d = screenshot1, screenshot2, etc..."));
   e_widget_table_object_append(ot, ob, 0, 2, 2, 1, 0, 0, 1, 0);

   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

#ifdef HAVE_IMPORT
   of = e_widget_framelist_add(evas, _("Import Options"), 0);
   ob = e_widget_check_add(evas, _("Include Image Border"),
                           &(cfdata->import.use_img_border));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, _("Use Image Dithering"),
                           &(cfdata->import.use_dither));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, _("Include Window Manager Frame"),
                           &(cfdata->import.use_frame));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, _("Choose Window To Grab"),
                           &(cfdata->import.use_window));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, _("Silent"), &(cfdata->import.use_silent));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, _("Trim Edges"), &(cfdata->import.use_trim));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
#endif
#ifdef HAVE_SCROT
   of = e_widget_framelist_add(evas, _("Scrot Options"), 0);
   ob = e_widget_check_add(evas, _("Include Image Border"),
                           &(cfdata->scrot.use_img_border));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, _("Generate Thumbnail"),
                           &(cfdata->scrot.use_thumb));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
#endif

   if (!o)
      return NULL;
   return o;
}

static int
_advanced_apply_data(E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
   Screen             *sc;

   sc = cfd->data;
   _basic_apply_data(cfd, cfdata);

   e_border_button_bindings_ungrab_all();
#ifdef HAVE_IMPORT
   sc->conf->import.use_img_border = cfdata->import.use_img_border;
   sc->conf->import.use_dither = cfdata->import.use_dither;
   sc->conf->import.use_frame = cfdata->import.use_frame;
   sc->conf->import.use_window = cfdata->import.use_window;
   sc->conf->import.use_silent = cfdata->import.use_silent;
   sc->conf->import.use_trim = cfdata->import.use_trim;
#endif
#ifdef HAVE_SCROT
   sc->conf->scrot.use_img_border = cfdata->scrot.use_img_border;
   //sc->conf->scrot.use_count = cfdata->scrot.use_count;
   //sc->conf->scrot.use_window = cfdata->scrot.use_window;
   sc->conf->scrot.use_thumb = cfdata->scrot.use_thumb;
#endif

   e_config_save_queue();
   e_border_button_bindings_grab_all();

   return 1;                    /* Apply was OK */
}

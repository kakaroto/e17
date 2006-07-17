
#include <e_mod_volume.h>
#include <e_mod_main.h>
#include <e_mod_face.h>
#include <e_mod_cdialog.h>
#include <e_mod_cmdialog.h>
#include <e.h>

typedef struct _mixerdata Mixer_CFData;

struct _E_Config_Dialog_Data
{
   Evas_List *mixers;
};

struct _mixerdata
{
   Config_Mixer *mixer;
   int active;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

void
e_volume_config_module(E_Container *con, Volume_Face *face)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];
   
   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = NULL;
   v->advanced.create_widgets = NULL;

   snprintf(buf, sizeof(buf), "%s/module.eap", module_root);
   cfd = e_config_dialog_new(con, _("Mixer Face Configuration"), buf, 0, v, face);
}

static void
_fill_data(Volume_Face *face, E_Config_Dialog_Data *cfdata)
{
   Evas_List *l, *l1;
   Config_Mixer_System *system_conf;

   cfdata->mixers = NULL;
   for (l = face->conf->systems; l; l = evas_list_next(l))
     {
        system_conf = evas_list_data(l);
        for (l1 = system_conf->mixers; l1; l1 = evas_list_next(l1))
          {
             Mixer_CFData *mcfd;

             mcfd = calloc(1, sizeof(Mixer_CFData));
             mcfd->mixer = evas_list_data(l1);
             mcfd->active = mcfd->mixer->active;
             cfdata->mixers = evas_list_append(cfdata->mixers, mcfd);
          }
     }
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Evas_List *l;

   for (l = cfdata->mixers; l; l = evas_list_next(l))
     {
        Mixer_CFData *d;

        d = evas_list_data(l);
        free(d);
     }
   evas_list_free(cfdata->mixers);

   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of;
   int i;
   Evas_List *l;
   Volume_Face *face;

   face = cfd->data;
   _fill_data(face, cfdata);
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Mixers"), 0);
   for (l = cfdata->mixers, i = 0; l; l = evas_list_next(l), i++)
     {
        Evas_Object *ob, *obut, *ot;
        Mixer_CFData *mcfd;

        mcfd = evas_list_data(l);
        ot = e_widget_table_add(evas, 0);

        ob = e_widget_check_add(evas, mcfd->mixer->real, &(mcfd->active));
        obut = e_widget_button_add(evas, D_("Configure"), NULL, e_volume_config_mixer, face, mcfd->mixer);

        e_widget_table_object_append(ot, ob, 1, i, 1, 1, 1, 1, 1, 1);
        e_widget_table_object_append(ot, obut, 2, i, 1, 1, 1, 1, 1, 1);

        e_widget_list_object_append(o, ot, 1, 1, 0.5);
     }

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Evas_List *l;
   Volume_Face *face;

   face = cfd->data;

   for (l = cfdata->mixers; l; l = evas_list_next(l))
     {
        Mixer_CFData *mcfd;

        mcfd = evas_list_data(l);
        if (mcfd->active != mcfd->mixer->active)
          {
             mcfd->mixer->active = mcfd->active;
             e_volume_face_mixer_update(face, mcfd->mixer);
          }
     }

   return 1;
}

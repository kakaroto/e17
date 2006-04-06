
#include <Evas.h>
#include <e.h>
#include "e_mod_main.h"
#include "e_mod_volume.h"
#include "e_mod_face.h"
#include "e_mod_slider.h"
#include "e_mod_util.h"
#include "e_mod_menu.h"
#include "e_mod_mixer.h"
#include "e_mod_pack.h"

//int e_volume_update_mixers(Volume* volume);
static int _volume_get_entry_height(Volume_Face *, Evas *evas);

static void
_volume_config_menu_new(Volume *v)
{
   v->config_menu = e_menu_new();
}

struct _Face_Add_Data
{
   E_Container *con;
   Config_Container *con_config;
   E_Menu *con_menu;
   Volume *volume;
};

static void
_face_add_cb(void *data, E_Menu *mn __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   struct _Face_Add_Data *ldata;
   Volume_Face *face;

   ldata = data;

   face = e_volume_face_new(ldata->con, ldata->volume);
   face->con_config = ldata->con_config;
   if (face)
     {
        ldata->volume->faces = evas_list_append(ldata->volume->faces, face);

        /* Config */
        face->conf = calloc(1, sizeof(Config_Face));
        face->conf->orient = ORIENT_HORIZ;
        ldata->con_config->faces = evas_list_append(ldata->con_config->faces, face->conf);
        /* Use automatic size */
        face->gmc->use_autoh = 1;
        face->gmc->use_autow = 1;

        /* Create mixer only after configuration */
        e_volume_face_mixers_create(face);
        /* And update it */
        e_volume_face_mixers_update(face);

        /* Menu */
        /* This menu must be initialized after conf */
        e_volume_face_menu_new(face);

        /*
         * mi = e_menu_item_new(ldata->con_menu);
         * {
         * char buf[128];
         * snprintf(buf, 128, "Face %d", evas_list_count(ldata->con_config->faces));
         * e_menu_item_label_set(mi, buf);
         * }
         * 
         * e_menu_item_submenu_set(mi, face->menu);
         */
     }
}

Evas_List *
_get_active_mixers(Mixer_System_Name *msn, Config *conf)
{
   Evas_List *l, *l1, *l3;
   Evas_List *ret;

   ret = NULL;
   Config_Mixer *mixer_conf;
   Evas_List *mixers;

   mixer_conf = evas_list_data(l3);

   if (!conf->containers)
     {
        Evas_List *mixers;

        for (mixers = msn->mixer_names; mixers; mixers = evas_list_next(mixers))
          {
             Mixer_Name *mixer_name;

             mixer_name = evas_list_data(mixers);
             ret = evas_list_append(ret, mixer_name);
          }
        return ret;
     }
   /* For each of container... */
   else
      for (l = conf->containers; l; l = evas_list_next(l))
        {
           Config_Container *con_conf;

           con_conf = evas_list_data(l);

           /* ...and for each of faces in it... */
           for (l1 = con_conf->faces; l1; l1 = evas_list_next(l1))
             {
                Config_Face *face_conf;

                face_conf = evas_list_data(l1);

                /* ... we should check for mixers names */
                for (mixers = msn->mixer_names; mixers; mixers = evas_list_next(mixers))
                  {
                     Mixer_Name *mixer_name;

                     mixer_name = evas_list_data(mixers);
                     /* If these names are equal (mixer_name->real
                      * is printable form of name
                      * mixer_conf->name is same
                      *
                      * mixer_name->card is card name (lowlevel),
                      * passed to mixer_open
                      */
                     mixer_conf = e_volume_config_mixer_get(mixer_name, face_conf);

                     if (mixer_conf->active)
                       {
                          DBG(stderr, "%s: adding %s\n", __FUNCTION__, mixer_name->card);
                          if (!evas_list_find(ret, mixer_name))
                             ret = evas_list_append(ret, mixer_name);
                       }
                  }
             }
        }
#if 0
   for (l = conf->containers; l; l = evas_list_next(l))
     {
        Config_Container *con_conf;

        con_conf = evas_list_data(l);

        /* ...and for each of faces in it... */
        for (l1 = con_conf->faces; l1; l1 = evas_list_next(l1))
          {
             Config_Face *face_conf;

             face_conf = evas_list_data(l1);

             /* ...we have list of systems... */
             for (l2 = face_conf->systems; l2; l2 = evas_list_next(l2))
               {
                  Config_Mixer_System *system_conf;

                  system_conf = evas_list_data(l2);

                  for (l3 = system_conf->mixers; l3; l3 = evas_list_next(l3))
                    {
                       Config_Mixer *mixer_conf;
                       Evas_List *mixers;

                       mixer_conf = evas_list_data(l3);

                       /* ... we should check for mixers names */
                       for (mixers = msn->mixer_names; mixers; mixers = evas_list_next(mixers))
                         {
                            Mixer_Name *mixer_name;

                            mixer_name = evas_list_data(mixers);
                            /* If these names are equal (mixer_name->real
                             * is printable form of name
                             * mixer_conf->name is same
                             *
                             * mixer_name->card is card name (lowlevel),
                             * passed to mixer_open
                             */
                            if (!strcmp(mixer_name->real, mixer_conf->real) && mixer_conf->active)
                              {
                                 DBG(stderr, "%s: adding %s\n", __FUNCTION__, mixer_name->card);
                                 mixer_name->active = 1;
                                 ret = evas_list_append(ret, mixer_name->card);
                              }
                         }

                    }

               }
          }
     }
#endif

   return ret;
}

/* MAIN INIT FUNCTION */
Volume *
e_volume_init(void *data __UNUSED__)
{
   Volume *volume;
   Evas_List *managers, *l, *l2, *cl;
   E_Menu_Item *mi;

   volume = E_NEW(Volume, 1);

   _volume_config_menu_new(volume);

   e_volume_config_init();
   volume->conf = e_volume_config_load();

   volume->mixer_system_names = mixer_system_get_list();
   for (l = volume->mixer_system_names; l; l = evas_list_next(l))
     {
        Mixer_System_Name *msn;
        Mixer *mixer;
        Evas_List *active;

        msn = evas_list_data(l);

        DBG(stderr, "msn = %p\n", msn);

        DBG(stderr, "msn = { \"%s\", \"%s\", %p }\n", msn->system, msn->name, msn->mixer_names);

        active = _get_active_mixers(msn, volume->conf);

        if (active == NULL)
           continue;

        for (; active; active = evas_list_next(active))
          {
             Mixer_Name *mixer_name;

             mixer_name = evas_list_data(active);
             e_volume_mixer_open(volume, mixer_name);
          }
        evas_list_free(active);
     }

   managers = e_manager_list();
   cl = volume->conf->containers;
   for (l = managers; l; l = l->next)
     {
        E_Manager *man;

        man = l->data;
        for (l2 = man->containers; l2; l2 = l2->next)
          {
             E_Menu *con_menu;
             E_Container *con;
             Evas_List *face_config_list;
             Config_Container *con_config;
             Volume_Face *face;
             int i, faces_count;

             i = 0;

             con = l2->data;

             /* Check for Container configuration */
             if (!cl)
               {
                  con_config = calloc(1, sizeof(Config_Container));
                  volume->conf->containers = evas_list_append(volume->conf->containers, con_config);
               }
             else
               {
                  con_config = cl->data;
                  cl = evas_list_next(cl);
               }

             face_config_list = con_config->faces;

             mi = e_menu_item_new(volume->config_menu);
             e_menu_item_label_set(mi, con->name);

             con_menu = e_menu_new();
             e_menu_item_submenu_set(mi, con_menu);

             mi = e_menu_item_new(con_menu);
             e_menu_item_label_set(mi, "Add new face");

             {
                struct _Face_Add_Data *data;

                data = calloc(1, sizeof(struct _Face_Add_Data));
                data->con = con;
                data->con_config = con_config;
                data->con_menu = con_menu;
                data->volume = volume;
                e_menu_item_callback_set(mi, _face_add_cb, data);
             }

             faces_count = evas_list_count(face_config_list);
             if (faces_count == 0)
                faces_count = 1;

             while (i++ < faces_count)
               {
                  face = e_volume_face_new(con, volume);
                  face->con_config = con_config;
                  if (face)
                    {
                       volume->faces = evas_list_append(volume->faces, face);

                       /* Config */
                       if (!face_config_list)
                         {
                            face->conf = calloc(1, sizeof(Config_Face));
                            face->conf->orient = ORIENT_HORIZ;
                            con_config->faces = evas_list_append(con_config->faces, face->conf);
                            /* Use automatic size */
                            face->gmc->use_autoh = 1;
                            face->gmc->use_autow = 1;
                         }
                       else
                         {
                            face->conf = evas_list_data(face_config_list);
                            face_config_list = evas_list_next(face_config_list);
                         }

                       /* Create mixer only after configuration */
                       e_volume_face_mixers_create(face);

                       /* Menu */
                       /* This menu must be initialized after conf */
                       e_volume_face_menu_new(face);

                       /*
                        * mi = e_menu_item_new(con_menu);
                        * {
                        * char buf[128];
                        * snprintf(buf, 128, "Face %d", i);
                        * e_menu_item_label_set(mi, buf);
                        * }
                        * 
                        * e_menu_item_submenu_set(mi, face->menu);
                        */

#if 0
                       /* Setup */
                       if (!face->conf->enabled)
                          e_volume_face_disable(face);
#endif
                    }
               }
          }
     }

   /* update mixers */
   e_volume_mixers_update(volume);

   return volume;
}

/* Volume is high level functions, mixer - low level (work with library) */
static int
_volume_mixer_create(Mixer *mixer, Evas *ev, Volume_Face *face, int ref)
{
   Evas_List *l;
   Config_Mixer *mixer_conf;
   Evas_List *sliders = NULL;

   mixer_conf = e_volume_config_mixer_get(mixer->name, face->conf);
   if (!mixer_conf->active)
     {
        if (mixer->ref == 0)
           e_volume_mixer_close(face->volume, mixer);
        return 0;
     }

   for (l = mixer->elems; l; l = evas_list_next(l))
     {
        Mixer_Slider *slider;
        Mixer_Elem *melem;
        Config_Mixer_Elem *conf;

        melem = evas_list_data(l);

        conf = e_volume_config_melem_get(melem, face->conf);
        if (!conf->active)
           continue;

        slider = e_volume_slider_create(melem, ev, face->conf->orient, conf->balance, face, ref);

        slider->conf = conf;

        sliders = evas_list_append(sliders, slider);
     }

   sliders = e_util_sliders_sort(sliders);

   for (l = sliders; l; l = evas_list_next(l))
     {
        Mixer_Slider *slider;

        slider = evas_list_data(l);
        e_volume_pack_pack_nth(face, slider, -1);
     }

   return 0;
}

int
e_volume_mixers_create(Volume_Face *face, Mixer *mixer, Evas *evas, int ref)
{
   _volume_get_entry_height(face, evas);
   _volume_mixer_create(mixer, evas, face, ref);
   return 1;
}

int
e_volume_mixers_free(Volume_Face *face, Mixer *mixer, int unref)
{
   while (face->sliders)
     {
        Mixer_Slider *slider;

        slider = evas_list_data(face->sliders);

        if (slider->melem->mixer != mixer && mixer != NULL)
           continue;

        DBG(stderr, "slider->melem->mixer = %p", slider->melem->mixer);

        e_volume_pack_unpack(face, slider);
        e_volume_slider_free(slider, unref);

     }
   return 1;
}

int
e_volume_mixers_update(Volume *volume)
{
   Evas_List *l;

   //DBG(stderr,"volume = %p\n", volume);
   for (l = volume->faces; l; l = evas_list_next(l))
     {
        Volume_Face *face;

        face = evas_list_data(l);
        if (!face)
           continue;

        e_volume_face_mixers_update(face);
     }

   return 1;
}

static int
_volume_get_entry_height(Volume_Face *face, Evas *evas)
{
   Evas_Object *obj;

   obj = edje_object_add(evas);

   edje_object_file_set(obj, module_theme, "volume/mixer/name");

   edje_object_size_min_get(obj, NULL, &face->entry_height);

   evas_object_del(obj);

   return 1;
}

int
e_volume_shutdown(Volume *volume)
{
   /* Shut down module */
   Evas_List *l;

   /* Clean up faces */
   for (l = volume->faces; l; l = evas_list_next(l))
     {
        Volume_Face *face;

        face = evas_list_data(l);
        e_volume_face_free(face);
     }
   evas_list_free(volume->faces);

   evas_list_free(volume->mixers);

   e_volume_config_shutdown();

   free(volume);

   return 1;
}

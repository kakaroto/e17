
#include <e.h>
#include <e_mod_main.h>
#include <e_mod_volume.h>
#include <e_mod_face.h>
#include <e_mod_menu.h>
#include <e_mod_util.h>
#include <e_mod_slider.h>

#include <e_mixer.h>
#include <e_mod_mixer.h>

/* Additional faces function: new face or remove */
static void
_volume_cb_menu_edit(void *data, E_Menu *mn __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   Volume_Face *face;

   face = data;

   e_gadman_mode_set(face->con->gadman, E_GADMAN_MODE_EDIT);
}

static void
_volume_remove_face_cb(void *data, E_Menu *mn __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   Volume_Face *face;
   Volume *volume;
   Evas_List *mixer, *system;

   face = data;
   volume = face->volume;

   volume->faces = evas_list_remove(volume->faces, face);
   face->con_config->faces = evas_list_remove(face->con_config->faces, face->conf);

   for (system = face->conf->systems; system; system = evas_list_next(system))
     {
        Config_Mixer_System *system_conf;

        system_conf = evas_list_data(system);
        for (mixer = system_conf->mixers; mixer; mixer = evas_list_next(mixer))
          {
             Evas_List *elem;
             Config_Mixer *mixer_conf;

             mixer_conf = evas_list_data(mixer);
             for (elem = mixer_conf->elems; elem; elem = evas_list_next(elem))
               {
                  Config_Mixer_Elem *el;

                  el = evas_list_data(elem);
                  evas_stringshare_del(el->name);
                  free(el);
               }

             evas_list_free(mixer_conf->elems);

             evas_stringshare_del(mixer_conf->real);
             evas_stringshare_del(mixer_conf->card);
             evas_stringshare_del(mixer_conf->system);

             free(mixer_conf);
          }
        evas_list_free(system_conf->mixers);

        if (system_conf->name)
           evas_stringshare_del(system_conf->name);

        if (system_conf->system)
           evas_stringshare_del(system_conf->system);

        free(system_conf);
     }
   evas_list_free(face->conf->systems);

   e_volume_face_free(face);
}

/* Orient callbacks */
static void
_volume_orient_horiz_cb(void *data, E_Menu *mn __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   Volume_Face *face;

//      int w, h;
   face = data;

   face->conf->orient = ORIENT_HORIZ;
//      e_volume_mixers_recreate(face);

#if 0
   /* swap geometry */
   e_gadman_client_geometry_get(face->gmc, NULL, NULL, &w, &h);
   face->gmc->use_autoh = face->gmc->use_autow = 1;
   e_gadman_client_auto_size_set(face->gmc, h, w);
#endif
}

static void
_volume_orient_vert_cb(void *data, E_Menu *mn __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   Volume_Face *face;

//      int w, h;
   face = data;

   face->conf->orient = ORIENT_VERT;
//      e_volume_mixers_recreate(face);

#if 0
   /* swap geometry */
   e_gadman_client_geometry_get(face->gmc, NULL, NULL, &w, &h);
   face->gmc->use_autoh = face->gmc->use_autow = 1;
   e_gadman_client_auto_size_set(face->gmc, h, w);
#endif
}

static void
_volume_cb_configure(void *data, E_Menu *mn __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   Volume_Face *face = data;

   e_volume_config_module(face->con, face);
}

/* Main function */
void
e_volume_face_menu_new(Volume_Face *face)
{
   char buf[1024];
   Evas_List *l;
   E_Menu *mn, *sm, *om;
   E_Menu_Item *mi;

   mn = e_menu_new();
   face->menu = mn;
   e_object_data_set(E_OBJECT(mn), face);

   {
      /* Face Menu */
      sm = e_menu_new();

      snprintf(buf, sizeof(buf), "%s/module_icon.png", module_root);
      mi = e_menu_item_new(mn);
      e_menu_item_label_set(mi, D_("Faces"));
      e_menu_item_icon_file_set(mi, buf);
      e_menu_item_submenu_set(mi, sm);

      mi = e_menu_item_new(sm);
      e_menu_item_label_set(mi, D_("Remove Face"));
      e_menu_item_callback_set(mi, _volume_remove_face_cb, (void *)face);
   }

   mi = e_menu_item_new(mn);
   e_menu_item_separator_set(mi, 1);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
   e_menu_item_callback_set(mi, _volume_cb_configure, (void *)face);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");
   e_menu_item_callback_set(mi, _volume_cb_menu_edit, (void *)face);   
}

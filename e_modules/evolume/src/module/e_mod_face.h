#ifndef E_MOD_FACE_INCLUDED
#define E_MOD_FACE_INCLUDED

#include "e_mod_volume.h"
#include <e.h>

typedef struct _Volume_Face Volume_Face;

struct _Volume_Face
{
   Evas_Object *main_object;
   Evas_Object *box_object;
   Evas_List *sliders;
   E_Container *con;

   E_Menu *menu;

   int entry_height;

   E_Gadman_Client *gmc;

   Config_Face *conf;
   Config_Container *con_config;
   Volume *volume;

   /* For mute */
   Evas_List *muted_melems;
};

Volume_Face *e_volume_face_new(E_Container *con, Volume *volume);
int e_volume_face_free(Volume_Face *face);

void e_volume_face_mixers_update(Volume_Face *face);
void e_volume_face_mixers_create(Volume_Face *face);
int e_volume_mixers_free(Volume_Face *face, Mixer *mixer, int unref);

void e_volume_face_mixer_elem_update(Volume_Face *face, Config_Mixer_Elem *elem_conf, Config_Mixer *mixer_conf);

#endif // E_MOD_FACE_INCLUDED


#include <e.h>
#include <e_mixer.h>
#include <e_mod_volume.h>
#include <e_mod_mixer.h>
#include <e_mod_util.h>

int
e_volume_mixer_close(Volume *volume, Mixer *mixer)
{
   Evas_List *l;

   l = evas_list_find_list(volume->mixers, mixer);
   if (!l)
      return 0;

   mixer_close(mixer);

   volume->mixers = evas_list_remove_list(volume->mixers, l);

   return 0;
}

int
e_volume_mixer_ref(Volume *volume, Mixer *mixer)
{
   Evas_List *l;
   int i;

   l = evas_list_find_list(volume->mixers, mixer);
   if (!l)
      return 0;

   i = mixer_ref(mixer);

   return i;
}

int
e_volume_mixer_unref(Volume *volume, Mixer *mixer)
{
   Evas_List *l;
   int i;

   l = evas_list_find_list(volume->mixers, mixer);
   if (!l)
      return 0;

   i = mixer_unref(mixer);

   return i;
}

int
e_volume_mixer_unref_close(Volume *volume, Mixer *mixer)
{
   Evas_List *l;
   int i;

   l = evas_list_find_list(volume->mixers, mixer);
   if (!l)
      return 0;

   if ((i = mixer_unref_close(mixer)) == 0)
      volume->mixers = evas_list_remove_list(volume->mixers, l);

   return i;
}

Mixer *
e_volume_mixer_open(Volume *volume, Mixer_Name *name)
{
   Mixer *mixer;

   /* First try to check if mixer already exists */
   mixer = e_util_search_mixer_by_name(volume->mixers, name);

   if (!mixer)
     {
        Mixer_System *ms = NULL;

        mixer = mixer_open(&ms, name->system_name, name->mixer_id);
        if (mixer)
           volume->mixers = evas_list_append(volume->mixers, mixer);

        mixer_set_callback(mixer, (void (*)())e_volume_mixers_update, volume);
     }

   return mixer;
}

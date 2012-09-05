#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <canberra.h>
#include <Evas.h>

#include "efbb.h"
#include "sound.h"

unsigned int SOUND_REPEAT;

static ca_context *_sound_context = NULL;
static Ecore_Event_Handler *_sound_repeat_handler = NULL;

typedef struct _Sound_Data Sound_Data;

struct _Sound_Data {
   int id;
   char *filename;
};

static Eina_Bool sound = EINA_TRUE;

static Eina_Bool
_sound_repeat_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Sound_Data *sound_data = event;

   sound_play_music(sound_data->filename, sound_data->id);
   INF("Repeating music %i", sound_data->id);

   return ECORE_CALLBACK_DONE;
}

static void
_sound_repeat_free(void *userdata __UNUSED__, void *data)
{
   Sound_Data *sound_data = data;

   free(sound_data->filename);
   free(sound_data);
}

static void
_sound_finished_cb(ca_context *c __UNUSED__, uint32_t id __UNUSED__,
                   int error_code, void *data)
{
   Sound_Data *sound_data = data;

   if (error_code != CA_SUCCESS)
     {
        INF("Sound didn't finish correctly. So, not repeating. %s",
            ca_strerror(error_code));
        free(sound_data->filename);
        free(sound_data);
        return;
     }

   ecore_event_add(SOUND_REPEAT, sound_data, _sound_repeat_free, NULL);
}

void
sound_mute_toggle(Eina_Bool islvl)
{
   sound = !sound;

   if (!sound)
     {
        if (sound_playing_get(MENU_MUSIC_ID))
          sound_stop(MENU_MUSIC_ID);
        if (sound_playing_get(GAME_MUSIC_ID))
          sound_stop(GAME_MUSIC_ID);
     }

   if (sound)
     {
        if (islvl)
          sound_play_music(GAME_MUSIC, GAME_MUSIC_ID);
        else
          sound_play_music(MENU_MUSIC, MENU_MUSIC_ID);
     }
}

void
sound_play_music(const char *filename, int id)
{
   ca_proplist *sound_props;
   Sound_Data *data;
   char buf[1024];
   int ret;

   if (!sound) return;

   if (id <= 0)
     {
        ERR("Id 0 is used for samples. Use id bigger than 0.");
        return;
     }

   snprintf(buf, sizeof(buf), "%s/%s", SOUND_DIR, filename);

   ret = ca_proplist_create(&sound_props);
   if (ret != CA_SUCCESS)
     {
        ERR("Couldn't create sound properties list: %s", ca_strerror(ret));
        return;
     }
   ca_proplist_sets(sound_props, CA_PROP_CANBERRA_CACHE_CONTROL, "permanent");
   ca_proplist_sets(sound_props, CA_PROP_MEDIA_FILENAME, buf);

   data = calloc(1, sizeof(Sound_Data));
   if (!data)
     {
        ERR("Failed to create sound data.");
        return;
     }

   data->id = id;
   data->filename = strdup(filename);
   if (!data->filename)
     {
        ERR("Failed to allocate filename on sound data.");
        free(data);
        return;
     }

   ret = ca_context_play_full(_sound_context, id, sound_props,
                              _sound_finished_cb, data);
   if (ret != CA_SUCCESS)
     ERR("Failed to play %s: %s", buf, ca_strerror(ret));

   ret = ca_proplist_destroy(sound_props);
   if (ret != CA_SUCCESS)
     ERR("Couldn't destroy sound properties list: %s", ca_strerror(ret));
}

void
sound_play(const char *filename)
{
   char buf[1024];
   int ret;

   if (!sound) return;

   snprintf(buf, sizeof(buf), "%s/%s", SOUND_DIR, filename);
   ret = ca_context_play(_sound_context, 0, CA_PROP_MEDIA_FILENAME, buf,
                         CA_PROP_CANBERRA_CACHE_CONTROL, "permanent", NULL);

   if (ret != CA_SUCCESS)
     ERR("Failed to play %s: %s", buf, ca_strerror(ret));
}

void
sound_play_cb(void *data __UNUSED__, Evas_Object *o __UNUSED__,
              const char *sig, const char *src __UNUSED__)
{
   if (!sound) return;

   sound_play(sig);
}

Eina_Bool
sound_playing_get(int id)
{
   int playing, ret;

   ret = ca_context_playing(_sound_context, id, &playing);
   if (ret != CA_SUCCESS)
     {
        WRN("Failed to get playing status from id = %i: %s", id,
            ca_strerror(ret));
        return EINA_FALSE;
     }

   return !!playing;
}

void
sound_stop(int id)
{
   int ret;

   ret = ca_context_cancel(_sound_context, id);
   if (ret != CA_SUCCESS)
     ERR("Failed to stop sounds with id = %i: %s", id, ca_strerror(ret));
}

Eina_Bool
sound_init(void)
{
   int ret;

   ret = ca_context_create(&_sound_context);
   if (ret != CA_SUCCESS)
     {
        ERR("Couldn't create context on libcanberra %s", ca_strerror(ret));
        return EINA_FALSE;
     }

   ret = ca_context_change_props(_sound_context, CA_PROP_APPLICATION_NAME,
                                 PACKAGE_NAME, CA_PROP_APPLICATION_VERSION,
                                 PACKAGE_VERSION, NULL);

   if (ret != CA_SUCCESS)
     {
        ERR("Couldn't set context properties: %s", ca_strerror(ret));
        return EINA_FALSE;
     }

   SOUND_REPEAT = ecore_event_type_new();
   _sound_repeat_handler = ecore_event_handler_add(SOUND_REPEAT,
                                                   _sound_repeat_cb, NULL);

   DBG("Sound system initialized");

   return EINA_TRUE;
}

Eina_Bool
sound_shutdown(void)
{
   int ret;

   ecore_event_handler_del(_sound_repeat_handler);

   ret = ca_context_destroy(_sound_context);
   if (ret != CA_SUCCESS)
     {
        ERR("Couldn't destroy sound context: %s", ca_strerror(ret));
        return EINA_FALSE;
     }
   _sound_context = NULL;

   DBG("Sound system shutdown");

   return EINA_TRUE;
}

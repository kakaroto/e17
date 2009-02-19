/* Interface */

#include "enna.h"
#include <Emotion.h>

#define ENNA_MODULE_NAME "emotion"

typedef struct _Enna_Module_Emotion
{
    Evas *evas;
    Evas_Object *o_emotion;
    Enna_Module *em;
    void (*event_cb)(void *data, enna_mediaplayer_event_t event);
    void *event_cb_data;
    const char *plugin;

} Enna_Module_Emotion;

static Enna_Module_Emotion *mod;

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static void _class_init(int dummy)
{
    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "class init");
}

static void _class_shutdown(int dummy)
{
    emotion_object_play_set(mod->o_emotion, 0);
    evas_object_del(mod->o_emotion);
}

static int _class_file_set(const char *uri, const char *label)
{
    emotion_object_file_set(mod->o_emotion, uri);
    return 0;
}

static int _class_play(void)
{
    emotion_object_play_set(mod->o_emotion, 1);
    return 0;
}

static int _class_seek(double percent)
{
    if (emotion_object_seekable_get(mod->o_emotion))
    {
        double length = emotion_object_play_length_get(mod->o_emotion);
        double sec = percent * length;
        emotion_object_position_set(mod->o_emotion, sec);
    }

    return 0;
}

static int _class_stop(void)
{
    emotion_object_play_set(mod->o_emotion, 0);
    emotion_object_position_set(mod->o_emotion, 0);
    return 0;
}

static int _class_pause(void)
{
    emotion_object_play_set(mod->o_emotion, 0);
    return 0;
}

static double _class_position_get()
{
    return emotion_object_position_get(mod->o_emotion);
}

static double _class_length_get()
{
    return emotion_object_play_length_get(mod->o_emotion);
}

static void _class_event_cb_set(void (*event_cb)(void *data, enna_mediaplayer_event_t event), void *data)
{
    mod->event_cb_data = data;
    mod->event_cb = event_cb;
}

static Evas_Object *_class_video_obj_get(void)
{
    return mod->o_emotion;
}

static void _eos_cb(void *data, Evas_Object * obj, void *event_info)
{
    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "End of stream");
    if (mod->event_cb)
	mod->event_cb(mod->event_cb_data, ENNA_MP_EVENT_EOF);
}

static Enna_Class_MediaplayerBackend class = {
    "emotion",
    1,
    {
	_class_init,
	_class_shutdown,
	_class_file_set,
	_class_play,
	_class_seek,
	_class_stop,
	_class_pause,
	_class_position_get,
	_class_length_get,
	NULL,
	_class_event_cb_set,
	_class_video_obj_get
    }
};

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_BACKEND,
    "backend_emotion"
};

void module_init(Enna_Module *em)
{
    Enna_Config_Data *cfgdata;
    char *value = NULL;

    if (!em)
	return;

    mod = calloc(1, sizeof(Enna_Module_Emotion));

    mod->plugin = NULL;

    /* Load Config file values */
    cfgdata = enna_config_module_pair_get("emotion");

    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "parameters:");

    if (cfgdata)
    {
	Eina_List *l;
	for (l = cfgdata->pair; l; l = l->next)
        {
            Config_Pair *pair = l->data;

            if (!strcmp("backend", pair->key))
            {
                enna_config_value_store(&value, "backend", ENNA_CONFIG_STRING,
                        pair);
                enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, " * type: %s", value);

                if ((!strcmp("gstreamer", value)) ||(!strcmp("xine", value)) || (!strcmp("vlc", value)))
		    mod->plugin = eina_stringshare_add(value);
		else
		{
                    enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                            "   - unknown type, 'gstreamer' used instead");
		    mod->plugin = eina_stringshare_add("gstreamer");
		}
            }

	}
    }

    mod->em = em;
    mod->evas = em->evas;
    mod->o_emotion = emotion_object_add(mod->evas);
    /* Fixme should come frome config */
    if (!emotion_object_init(mod->o_emotion, mod->plugin))
    {
        enna_log(ENNA_MSG_ERROR, ENNA_MODULE_NAME,
	    "could not initialize %s plugin for emotion", mod->plugin);
        return;
    }
    evas_object_smart_callback_add(mod->o_emotion, "decode_stop", _eos_cb, NULL);
    enna_mediaplayer_backend_register(&class);
}

void module_shutdown(Enna_Module *em)
{
    _class_shutdown(0);
    free(mod);
}

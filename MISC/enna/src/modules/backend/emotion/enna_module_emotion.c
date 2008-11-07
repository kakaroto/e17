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

static void _class_snapshot(const char *uri, const char *file)
{
#if 0
   Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *o_emotion;

    if (!uri || !file)
        return;
    printf("uri : %s, file : %s\n", uri, file);
    if (!ecore_file_exists(file))
    {

       Evas_Object *o_img, *o_rect;
       Evas_Coord ow, oh;
       Evas_Pixel_Import_Source *pixels;
	ecore_evas_init();

	ee = ecore_evas_buffer_new(32, 32);
	evas = ecore_evas_get(ee);
	o_emotion = emotion_object_add(evas);
	ecore_evas_data_set(ee, "emotion", o_emotion);
	emotion_object_init(o_emotion, mod->plugin);
	emotion_object_file_set(o_emotion, uri);
	emotion_object_play_set(o_emotion, 1);
	emotion_object_audio_mute_set(o_emotion, 1);
	emotion_object_position_set(o_emotion, 825.0);
	emotion_object_size_get(o_emotion, &ow, &oh);
	evas_object_resize(o_emotion, ow, oh);
	evas_object_move(o_emotion, 0, 0);
	evas_object_show(o_emotion);
	printf("%d %d\n", ow, oh);
	ecore_evas_resize(ee, ow, oh);
	if (emotion_object_seekable_get(o_emotion))
	  {
	     double length = emotion_object_play_length_get(o_emotion);
	     double sec = 0.15 * length;
	     emotion_object_position_set(o_emotion, sec);
	     printf("set position %3.3f\n", sec);
	  }

	o_rect = evas_object_rectangle_add(evas);

	evas_object_resize(o_rect, ow, oh);
	evas_object_move(o_rect, 0, 0);
	evas_object_color_set(o_rect, 255, 0, 0,255);
	evas_object_show(o_rect);

	ecore_evas_show(ee);

	//	o_img = ecore_evas_object_image_new(ee);
	o_img = evas_object_image_add(enna->evas);
	pixels = calloc(1, sizeof(Evas_Pixel_Import_Source));
	pixels->format = EVAS_PIXEL_FORMAT_ARGB32;
	pixels->w = ow;
	pixels->h = oh;
	pixels->rows = ecore_evas_buffer_pixels_get(ee);
	printf("pixels : %p\n", pixels->rows);
	evas_object_image_data_set(o_img,pixels->rows);
	evas_object_image_pixels_dirty_set(o_img, 1);
	evas_object_image_alpha_set(o_img, 1);
	evas_object_image_size_set(o_img, ow, oh);
	evas_object_image_fill_set(o_img, 0, 0, ow, oh);
	evas_object_move(o_img, 0, 0);
	evas_object_resize(o_img, ow, oh);
	evas_object_show(o_img);
	evas_object_image_save(o_img, "/home/nico/toto.png", NULL, NULL);

	evas_object_del(o_img);
	evas_object_del(o_emotion);
	ecore_evas_free(ee);

    }
#endif

}

static Enna_Metadata *_class_metadata_get(void)
{
    Enna_Metadata *m;
    const char *track;
    const char *tmp;

    m = enna_metadata_new();

    tmp = emotion_object_file_get(mod->o_emotion);
    if (tmp)
	m->uri = strdup(tmp);

    tmp = emotion_object_meta_info_get(mod->o_emotion,
	EMOTION_META_INFO_TRACK_TITLE);
    if (tmp)
	m->title = strdup(tmp);

    tmp = emotion_object_meta_info_get(mod->o_emotion,
	EMOTION_META_INFO_TRACK_ALBUM);
    if (tmp)
	m->music->album = strdup(tmp);

    tmp = emotion_object_meta_info_get(mod->o_emotion,
	EMOTION_META_INFO_TRACK_ARTIST);
    if (tmp)
	m->music->artist = strdup(tmp);

    tmp = emotion_object_meta_info_get(mod->o_emotion,
	EMOTION_META_INFO_TRACK_GENRE);
    if (tmp)
	m->music->genre = strdup(tmp);

    tmp = emotion_object_meta_info_get(mod->o_emotion,
	EMOTION_META_INFO_TRACK_YEAR);
    if (tmp)
	m->music->year = strdup(tmp);

    tmp = emotion_object_meta_info_get(mod->o_emotion,
	EMOTION_META_INFO_TRACK_COMMENT);
    if (tmp)
	m->music->comment = strdup(tmp);

    tmp = emotion_object_meta_info_get(mod->o_emotion,
	EMOTION_META_INFO_TRACK_DISC_ID);
    if (tmp)
	m->music->discid = strdup(tmp);

    track = emotion_object_meta_info_get(mod->o_emotion,
	EMOTION_META_INFO_TRACK_COUNT);
    m->music->track = track ? atoi(track) : 0;
    return m;

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
	_class_snapshot,
	_class_metadata_get,
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
    "emotion"
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

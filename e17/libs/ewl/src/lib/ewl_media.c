#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_media_size_update(Ewl_Media *m);
static void ewl_media_update_timer_cb(void *data, Evas_Object *obj, void
							    *event_info);

/**
 * @param media: the media to be played or NULL
 * @return Returns a pointer to a new media on success, NULL on failure.
 * @brief Allocate a new media widget
 */
Ewl_Widget	 *ewl_media_new(char *media)
{
	Ewl_Media   *m;

	DENTER_FUNCTION(DLEVEL_STABLE);

	m = NEW(Ewl_Media, 1);
	if (!m)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_media_init(m, media);

	DRETURN_PTR(EWL_WIDGET(m), DLEVEL_STABLE);
}

/**
 * @param m: the media area to be initialized
 * @param media: the media to be played or NULL
 * @return Returns no value.
 * @brief Initialize the fields and callbacks of a media object
 *
 * Sets the internal fields and callbacks of a media object to there defaults.
 */
void ewl_media_init(Ewl_Media *m, char *media)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);

	w = EWL_WIDGET(m);

	ewl_widget_init(EWL_WIDGET(w), "media");
	ewl_widget_inherit(EWL_WIDGET(w), "media");

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, ewl_media_realize_cb,
				NULL);
	ewl_callback_append(w, EWL_CALLBACK_UNREALIZE,
				ewl_media_unrealize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				ewl_media_configure_cb, NULL);

	if (media)
		ewl_media_media_set(m, media);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media area widget to set the media
 * @param media: the media to set in the media widget @a m 
 * @return Returns no value.
 * @brief Set the media of a media widget
 *
 * Sets the media of the media widget @a m 
 */
void ewl_media_media_set(Ewl_Media * m, char *media)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("m", m);
	DCHECK_PARAM_PTR("media", media);

	IF_FREE(m->media);
	m->media = strdup(media);

	/*
	 * Update the emotion to the new file
	 */
	if (m->video) {
		emotion_object_file_set(m->video, m->media);
		ewl_media_size_update(m);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to retrieve media contents
 * @return Returns a copy of the media in @a m on success, NULL on failure.
 * @brief Retrieve the media of a media widget
 */
char *ewl_media_media_get(Ewl_Media * m)
{
	char *txt = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, NULL);

	if (m->media)
		txt = strdup(m->media);

	DRETURN_PTR(txt, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to retrieve length from
 * @return Returns the length of the media contained in the widget.
 * @brief Retrieve the length of the media displayed by the media widget.
 */
int ewl_media_length_get(Ewl_Media *m)
{
	int length = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("m", m, 0);

	if (m->video)
		length = emotion_object_play_length_get(m->video); 

	DRETURN_INT(length, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param h: hours variable
 * @param min: minutes variable
 * @param s: seconds variable
 * @brief Puts the length of the video into the @a h, @a m, @a s variables
 */
void ewl_media_length_time_get(Ewl_Media *m, int *h, int *min, double *s)
{
    double pos = ewl_media_length_get(m);

    /* stolen from envision by benr */
    *h = (int)pos / (60 * 60);
    *min = ((int)pos / 60) - (*h * 60);
    *s = pos - (*h * 60 * 60) - (*min * 60);
}

/**
 * @param m: the media widget to act upon
 * @param p: the value to set play too
 * @return Returns no value
 * @brief Sets the media widget into the given state
 */
void ewl_media_play_set(Ewl_Media *m, int p) 
{
	DENTER_FUNCTION(DLEVEL_STABLE)
	DCHECK_PARAM_PTR("m", m);

	if (m->video)
		emotion_object_play_set(m->video, p);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @return Returns if the media area is seekable
 * @brief Returns if the media area is seekable
 */
int ewl_media_seekable_get(Ewl_Media *m)
{
	int seekable = 0;

	DENTER_FUNCTION(DLEVEL_STABLE)
	DCHECK_PARAM_PTR_RET("m", m, 0);

	if (m->video && !m->block_seek)
	    seekable = emotion_object_seekable_get(m->video);

	DRETURN_INT(seekable, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @return Returns the current media position
 * @brief Returns the position of the current media
 */
double ewl_media_position_get(Ewl_Media *m)
{
	double p = 0.0;

	DENTER_FUNCTION(DLEVEL_STABLE)
	DCHECK_PARAM_PTR_RET("m", m, 0);

	if (m->video)
	    p = emotion_object_position_get(m->video);

	DRETURN_FLOAT(p, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param h: hours variable
 * @param min: minutes variable
 * @param s: seconds variable
 * @brief Puts the position of the video into the @a h, @a m, @a s variables
 */
void ewl_media_position_time_get(Ewl_Media *m, int *h, int *min, double *s)
{
    double pos = ewl_media_position_get(m);

    /* stolen from envision by benr */
    *h = (int)pos / (60 * 60);
    *min = ((int)pos / 60) - (*h * 60);
    *s = pos - (*h * 60 * 60) - (*min * 60);
}

/**
 * @param m: the media widget to act upon
 * @param p: the positon to seek too
 * @return Returns no value
 * @brief Sets the media widget to the specified position
 */
void ewl_media_position_set(Ewl_Media *m, double p)
{
	DENTER_FUNCTION(DLEVEL_STABLE)
	DCHECK_PARAM_PTR("m", m);

	if (m->video && ewl_media_seekable_get(m)) {
		m->block_seek = 1;
		emotion_object_position_set(m->video, p);
		m->block_seek = 0;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @return Returns if the media widget is muted
 * @brief Checks if the media widget is muted
 */
int ewl_media_audio_mute_get(Ewl_Media *m)
{
	int mute = 0;

	DENTER_FUNCTION(DLEVEL_STABLE)
	DCHECK_PARAM_PTR_RET("m", m, 0);

	if (m->video)
	    mute = emotion_object_audio_mute_get(m->video);

	DRETURN_INT(mute, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param mute: boolean to indicate if the sound should be muted
 * @return Returns no value
 * @brief Mutes the media widget
 */
void ewl_media_audio_mute_set(Ewl_Media *m, int mute) 
{
	DENTER_FUNCTION(DLEVEL_STABLE)
	DCHECK_PARAM_PTR("m", m);

	if (m->video)
	    emotion_object_audio_mute_set(m->video, mute);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @return Returns the media widget volume
 * @brief Gets the current volume from the media widget
 */
double ewl_media_audio_volume_get(Ewl_Media *m) 
{
	double v = 0.0;

	DENTER_FUNCTION(DLEVEL_STABLE)
	DCHECK_PARAM_PTR_RET("m", m, 0);

	if (m->video)
	    emotion_object_audio_volume_get(m->video);

	DRETURN_FLOAT(v, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param v: the volume to set the widget too
 * @return Returns no value
 * @brief Sets the media widget to the given volume
 */
void ewl_media_audio_volume_set(Ewl_Media *m, double v )
{
	DENTER_FUNCTION(DLEVEL_STABLE)
	DCHECK_PARAM_PTR("m", m);

	if (m->video)
	    emotion_object_audio_volume_set(m->video, v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void ewl_media_realize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Media  *m;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	m = EWL_MEDIA(w);

	/*
	 * Find the embed so we know which evas to draw onto.
	 */
	emb = ewl_embed_widget_find(w);

	/*
	 * Create the emotion
	 */
	m->video = emotion_object_add(emb->evas);
	if (m->media) {
		emotion_object_file_set(m->video, m->media);
		ewl_media_size_update(m);
	}

	if (w->fx_clip_box)
		evas_object_clip_set(m->video, w->fx_clip_box);

	/*
	 * Now set the media and display it.
	 */
	evas_object_show(m->video);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_media_unrealize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Media   *m;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	m = EWL_MEDIA(w);

	evas_object_clip_unset(m->video);
	evas_object_del(m->video);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_media_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Media   *m;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	m = EWL_MEDIA(w);

	/*
	 * Update the emotion position and size.
	 */
	if (m->video) {
		evas_object_move(m->video, CURRENT_X(w), CURRENT_Y(w));
		evas_object_resize(m->video, CURRENT_W(w), CURRENT_H(w));
		evas_object_layer_set(m->video, ewl_widget_layer_sum_get(w));
		evas_object_smart_callback_add(m->video, "frame_decode", 
					 ewl_media_update_timer_cb, m);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_media_size_update(Ewl_Media *m)
{
	int width, height;
	emotion_object_size_get(m->video, &width, &height);
	if (width && height)
		ewl_object_preferred_inner_size_set(EWL_OBJECT(m), width, height);
}

static void ewl_media_update_timer_cb(void *data,
					Evas_Object *obj __UNUSED__,
					void *event_info __UNUSED__)
{
	Ewl_Widget *m = (Ewl_Widget *)data;
	EWL_MEDIA(m)->block_seek = 1;
	ewl_callback_call(m,  EWL_CALLBACK_VALUE_CHANGED);
	EWL_MEDIA(m)->block_seek = 0;
}




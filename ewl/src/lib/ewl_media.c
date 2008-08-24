/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_media.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

#include <Evas.h>

#ifdef BUILD_EMOTION_SUPPORT
#include <Emotion.h>
#endif

static void ewl_media_size_update(Ewl_Media *m);
static void ewl_media_cb_update_timer(void *data, Evas_Object *obj, void
                *event_info);

/**
 * @return Returns a pointer to a new media on success, NULL on failure.
 * @brief Allocate a new media widget
 */
Ewl_Widget *
ewl_media_new(void)
{
        Ewl_Media *m = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

#ifdef BUILD_EMOTION_SUPPORT
        m = NEW(Ewl_Media, 1);
        if (!m)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_media_init(m)) {
                ewl_widget_destroy(EWL_WIDGET(m));
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }
#endif

        DRETURN_PTR(EWL_WIDGET(m), DLEVEL_STABLE);
}

/**
 * @param m: the media area to be initialized
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the fields and callbacks of a media object
 *
 * Sets the internal fields and callbacks of a media object to there defaults.
 */
int
ewl_media_init(Ewl_Media *m)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, FALSE);

        w = EWL_WIDGET(m);

        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_MEDIA_TYPE);
        ewl_widget_inherit(EWL_WIDGET(w), EWL_MEDIA_TYPE);

        ewl_callback_append(w, EWL_CALLBACK_REALIZE,
                                ewl_media_cb_realize, NULL);
        ewl_callback_append(w, EWL_CALLBACK_REVEAL,
                                ewl_media_cb_reveal, NULL);
        ewl_callback_append(w, EWL_CALLBACK_UNREALIZE,
                                ewl_media_cb_unrealize, NULL);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                                ewl_media_cb_configure, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns TRUE if the media widget is available, FALSE otherwise
 * @brief This will check if the modules required for the media widget were
 * available at compile time or not.
 */
unsigned int
ewl_media_is_available(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

#ifdef BUILD_EMOTION_SUPPORT
        DRETURN_INT(TRUE, DLEVEL_STABLE);
#else
        DRETURN_INT(FALSE, DLEVEL_STABLE);
#endif
}

/**
 * @param m: the media area widget to set the module
 * @param module: the module to set in the media widget @a m
 * @return Returns FALSE if we failed to load the module, TRUE otherwise.
 * @brief Set the module of a media widget
 *
 * Sets the module of the media widget @a m
 */
int
ewl_media_module_set(Ewl_Media *m, Ewl_Media_Module_Type module)
{
        int ret = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, FALSE);
        DCHECK_TYPE_RET(m, EWL_MEDIA_TYPE, FALSE);

        m->module = module;

#ifdef BUILD_EMOTION_SUPPORT
        /*
         * Initialize emotion
         */
        if (m->video)
        {
                switch (module)
                {
                        case EWL_MEDIA_MODULE_GSTREAMER:
                                ret = emotion_object_init(m->video,
                                                "gstreamer");
                                break;

                        case EWL_MEDIA_MODULE_XINE:
                        default:
                                ret = emotion_object_init(m->video,
                                                "xine");
                                break;
                }
        }
#endif

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to retrieve module contents
 * @return Returns a the module associated with the media object
 * @brief Retrieve the module of a media widget
 */
Ewl_Media_Module_Type
ewl_media_module_get(Ewl_Media *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, EWL_MEDIA_MODULE_XINE);
        DCHECK_TYPE_RET(m, EWL_MEDIA_TYPE, EWL_MEDIA_MODULE_XINE);

        DRETURN_INT(m->module, DLEVEL_STABLE);
}

/**
 * @param m: the media area widget to set the media
 * @param media: the media to set in the media widget @a m
 * @return Returns no value.
 * @brief Set the media of a media widget
 *
 * Sets the media of the media widget @a m
 */
void
ewl_media_media_set(Ewl_Media *m, const char *media)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_PARAM_PTR(media);
        DCHECK_TYPE(m, EWL_MEDIA_TYPE);

        IF_FREE(m->media);
        m->media = strdup(media);

#ifdef BUILD_EMOTION_SUPPORT
        /*
         * Update the emotion to the new file
         */
        if (m->video)
                emotion_object_file_set(m->video, m->media);
#endif

        /*
         * Move this check outside the optional build to avoid warnings, and
         * to keep it generic in the event something other than emotion is
         * added.
         */
        if (m->video)
                ewl_media_size_update(m);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to retrieve media contents
 * @return Returns a copy of the media in @a m on success, NULL on failure.
 * @brief Retrieve the media of a media widget
 */
const char *
ewl_media_media_get(Ewl_Media *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, NULL);
        DCHECK_TYPE_RET(m, EWL_MEDIA_TYPE, NULL);

        DRETURN_PTR(m->media, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to retrieve length from
 * @return Returns the length of the media contained in the widget.
 * @brief Retrieve the length of the media displayed by the media widget.
 */
int
ewl_media_length_get(Ewl_Media *m)
{
        int length = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, 0);
        DCHECK_TYPE_RET(m, EWL_MEDIA_TYPE, 0);

#ifdef BUILD_EMOTION_SUPPORT
        if (m->video)
                length = emotion_object_play_length_get(m->video);
#endif

        DRETURN_INT(length, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param h: hours variable
 * @param min: minutes variable
 * @param s: seconds variable
 * @brief Puts the length of the video into the @a h, @a m, @a s variables
 */
void
ewl_media_length_time_get(Ewl_Media *m, int *h, int *min, double *s)
{
        double pos;
        int mh, mm;
        double ms;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MEDIA_TYPE);

        pos = ewl_media_length_get(m);

        /* stolen from envision by benr */
        mh = (int)pos / (60 * 60);
        mm = ((int)pos / 60) - (mh * 60);
        ms = pos - (mh * 60 * 60) - (mm * 60);

        if (h) *h = mh;
        if (min) *min = mm;
        if (s) *s = ms;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param p: the value to set play too
 * @return Returns no value
 * @brief Sets the media widget into the given state
 */
void
ewl_media_play_set(Ewl_Media *m, int p)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MEDIA_TYPE);

#ifdef BUILD_EMOTION_SUPPORT
        if (m->video)
                emotion_object_play_set(m->video, p);
#endif

        DRETURN(DLEVEL_STABLE);
        p = p;
}

/**
 * @param m: the media widget to act upon
 * @return Returns if the media area is seekable
 * @brief Returns if the media area is seekable
 */
int
ewl_media_seekable_get(Ewl_Media *m)
{
        int seekable = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, 0);
        DCHECK_TYPE_RET(m, EWL_MEDIA_TYPE, 0);

#ifdef BUILD_EMOTION_SUPPORT
        if (m->video && !m->block_seek)
                seekable = emotion_object_seekable_get(m->video);
#endif

        DRETURN_INT(seekable, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @return Returns the current media position
 * @brief Returns the position of the current media
 */
double
ewl_media_position_get(Ewl_Media *m)
{
        double p = 0.0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, 0);
        DCHECK_TYPE_RET(m, EWL_MEDIA_TYPE, 0);

#ifdef BUILD_EMOTION_SUPPORT
        if (m->video)
                p = emotion_object_position_get(m->video);
#endif

        DRETURN_FLOAT(p, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param h: hours variable
 * @param min: minutes variable
 * @param s: seconds variable
 * @brief Puts the position of the video into the @a h, @a m, @a s variables
 */
void
ewl_media_position_time_get(Ewl_Media *m, int *h, int *min, double *s)
{
        double pos;
        int mh, mm;
        double ms;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MEDIA_TYPE);

        pos = ewl_media_position_get(m);
        /* stolen from envision by benr */
        mh = (int)pos / (60 * 60);
        mm = ((int)pos / 60) - (mh * 60);
        ms = pos - (mh * 60 * 60) - (mm * 60);

        if (h) *h = mh;
        if (min) *min = mm;
        if (s) *s = ms;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param p: the positon to seek too
 * @return Returns no value
 * @brief Sets the media widget to the specified position
 */
void
ewl_media_position_set(Ewl_Media *m, double p)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MEDIA_TYPE);

        if (p == m->position)
                DRETURN(DLEVEL_STABLE);

        m->position = p;

#ifdef BUILD_EMOTION_SUPPORT
        if (m->video && ewl_media_seekable_get(m)) {
                m->block_seek = 1;
                emotion_object_position_set(m->video, m->position);
                m->block_seek = 0;
        }
#endif

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @return Returns if the media widget is muted
 * @brief Checks if the media widget is muted
 */
int
ewl_media_audio_mute_get(Ewl_Media *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, 0);
        DCHECK_TYPE_RET(m, EWL_MEDIA_TYPE, 0);

        DRETURN_INT(m->mute, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param mute: boolean to indicate if the sound should be muted
 * @return Returns no value
 * @brief Mutes the media widget
 */
void
ewl_media_audio_mute_set(Ewl_Media *m, int mute)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MEDIA_TYPE);

        if (m->mute == !!mute)
                DRETURN(DLEVEL_STABLE);

        m->mute = !!mute;

#ifdef BUILD_EMOTION_SUPPORT
        if (m->video)
                emotion_object_audio_mute_set(m->video, m->mute);
#endif

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @return Returns the media widget volume
 * @brief Gets the current volume from the media widget
 */
double
ewl_media_audio_volume_get(Ewl_Media *m)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, 0);
        DCHECK_TYPE_RET(m, EWL_MEDIA_TYPE, 0.0);

        DRETURN_FLOAT(m->volume, DLEVEL_STABLE);
}

/**
 * @param m: the media widget to act upon
 * @param v: the volume to set the widget too
 * @return Returns no value
 * @brief Sets the media widget to the given volume
 */
void
ewl_media_audio_volume_set(Ewl_Media *m, double v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MEDIA_TYPE);

        if (m->volume == v)
                DRETURN(DLEVEL_STABLE);

        m->volume = v;

#ifdef BUILD_EMOTION_SUPPORT
        if (m->video)
                emotion_object_audio_volume_set(m->video, m->volume);
#endif

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The realize callback
 */
void
ewl_media_cb_realize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Media *m;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MEDIA_TYPE);

        m = EWL_MEDIA(w);

        /*
         * Find the embed so we know which canvas to draw onto.
         */
        emb = ewl_embed_widget_find(w);

#ifdef BUILD_EMOTION_SUPPORT
        /*
         * Create the emotion
         */
        m->video = emotion_object_add(emb->canvas);
        if (ewl_media_module_set(m, m->module) && m->media) {
                emotion_object_file_set(m->video, m->media);
                ewl_media_size_update(m);
        }
#endif
        /*
         * Now set the media and display it.
         */
        evas_object_show(m->video);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The reveal callback
 */
void
ewl_media_cb_reveal(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Media *m;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MEDIA_TYPE);

        m = EWL_MEDIA(w);

        if (m->video) {
                evas_object_smart_member_add(m->video, w->smart_object);
                if (w->fx_clip_box) {
                        evas_object_clip_set(m->video, w->fx_clip_box);
                        evas_object_stack_above(m->video, w->fx_clip_box);
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The unrealize callback
 */
void
ewl_media_cb_unrealize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Media *m;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MEDIA_TYPE);

        m = EWL_MEDIA(w);

        evas_object_clip_unset(m->video);
        evas_object_del(m->video);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_media_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                void *user_data __UNUSED__)
{
        Ewl_Media *m;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MEDIA_TYPE);

        m = EWL_MEDIA(w);

        /*
         * Update the emotion position and size.
         */
        if (m->video) {
                evas_object_move(m->video, CURRENT_X(w), CURRENT_Y(w));
                evas_object_resize(m->video, CURRENT_W(w), CURRENT_H(w));
                evas_object_smart_callback_add(m->video, "frame_decode",
                                ewl_media_cb_update_timer, m);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_media_size_update(Ewl_Media *m)
{
        int width = 0, height = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MEDIA_TYPE);

#ifdef BUILD_EMOTION_SUPPORT
        emotion_object_size_get(m->video, &width, &height);
#endif

        if (width && height)
                ewl_object_preferred_inner_size_set(EWL_OBJECT(m), width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_media_cb_update_timer(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
        Ewl_Media *m;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_MEDIA_TYPE);

        m = data;

        m->block_seek = 1;
        ewl_callback_call(EWL_WIDGET(m),  EWL_CALLBACK_VALUE_CHANGED);
        m->block_seek = 0;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


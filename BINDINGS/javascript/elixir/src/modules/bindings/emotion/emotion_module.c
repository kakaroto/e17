#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <Emotion.h>

#include "Elixir.h"

#ifdef ELIXIR_EMOTION_SECURITY
# include <fnmatch.h>
#endif

static elixir_parameter_t               _evas_parameter = {
  "Evas", JOBJECT, NULL
};
static elixir_parameter_t               _evas_object_parameter = {
  "Evas_Object", JOBJECT, NULL
};
static elixir_parameter_t               _emotion_object_parameter = {
  "Emotion_Object", JOBJECT, NULL
};


static const elixir_parameter_t*        _evas_params[2] = {
   &_evas_parameter,
   NULL
};
static const elixir_parameter_t*        _emotion_object_params[2] = {
   &_emotion_object_parameter,
   NULL
};
static const elixir_parameter_t*        _emotion_object_bool_params[3] = {
   &_emotion_object_parameter,
   &boolean_parameter,
   NULL
};
static const elixir_parameter_t*        _emotion_object_double_params[3] = {
   &_emotion_object_parameter,
   &double_parameter,
   NULL
};
static const elixir_parameter_t*        _emotion_object_int_params[3] = {
   &_emotion_object_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _emotion_object_2strings_params[4] = {
   &_emotion_object_parameter,
   &string_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _emotion_object_string_params[3] = {
   &_emotion_object_parameter,
   &string_parameter,
   NULL
};

static char **_elixir_emotion_regex = NULL;
static char **_elixir_emotion_blacklist = NULL;

static JSBool
elixir_emotion_object_add(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *obj = NULL;
   Evas *evas = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);

   obj = emotion_object_add(evas);

   return evas_object_to_jsval(cx, obj, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_emotion_object_module_option_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *obj;
   const char *option;
   const char *value;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _emotion_object_2strings_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, obj);
   option = elixir_get_string_bytes(val[1].v.str, NULL);
   value = elixir_get_string_bytes(val[2].v.str, NULL);
   emotion_object_module_option_set(obj, option, value);
   return JS_TRUE;
}

static JSBool
elixir_emotion_object_init(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *obj;
   const char *module_filename;
   Eina_Bool ret;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _emotion_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, obj);
   module_filename = elixir_get_string_bytes(val[1].v.str, NULL);
   ret = emotion_object_init(obj, module_filename);
   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret));
   return JS_TRUE;
}

static JSBool
elixir_emotion_object_file_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *obj;
   const char *uri;
   char tmp[PATH_MAX];
   elixir_value_t val[2];
#ifdef ELIXIR_EMOTION_SECURITY
   int i;
#endif

   if (!elixir_params_check(cx, _emotion_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, obj);
   uri = elixir_get_string_bytes(val[1].v.str, NULL);

   if (!strstr(uri, "://"))
     {
	char *file;

	file = elixir_file_canonicalize(uri);

	strcpy(tmp, "file://");
	strncat(tmp, file, PATH_MAX - 8);

	free(file);
     }
   else if (!strncmp(uri, "file://", 7))
     {
	char *file;
	file = elixir_file_canonicalize(uri + 7);

	strcpy(tmp, "file://");
	strncat(tmp, file, PATH_MAX - 8);

	free(file);
     }
   else
     {
	strncpy(tmp, uri, PATH_MAX - 1);
     }

#ifdef ELIXIR_EMOTION_SECURITY
   for (i = 0; _elixir_emotion_regex[i] != NULL; ++i)
     if (!fnmatch(_elixir_emotion_regex[i], tmp, FNM_CASEFOLD))
       break;

   if (_elixir_emotion_regex[i])
     {
        for (i = 0; _elixir_emotion_blacklist[i] != NULL; ++i)
          if (!fnmatch(_elixir_emotion_blacklist[i], tmp, FNM_CASEFOLD))
            break;

        if (!_elixir_emotion_blacklist[i])
          {
#endif
             emotion_object_file_set(obj, tmp);
             return JS_TRUE;
#ifdef ELIXIR_EMOTION_SECURITY
          }
     }

   JS_ReportError(cx, "URI [%s] is not authorized by current security rules.", tmp);
   return JS_FALSE;
#endif
}


static JSBool
elixir_string_params_emotion_object(const char* (*func)(const Evas_Object *obj),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *ret;
   elixir_value_t       val[1];

   if (!elixir_params_check(cx, _emotion_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   ret = func(eo);

   elixir_return_str(cx, vp, ret);
   return JS_TRUE;
}

FAST_CALL_PARAMS(emotion_object_file_get, elixir_string_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_progress_info_get, elixir_string_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_title_get, elixir_string_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_ref_file_get, elixir_string_params_emotion_object);

static JSBool
elixir_void_params_emotion_object_bool(void (*func)(Evas_Object *obj, Eina_Bool bol),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   elixir_value_t       val[2];

   if (!elixir_params_check(cx, _emotion_object_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   func(eo, val[1].v.bol);

   return JS_TRUE;
}

FAST_CALL_PARAMS(emotion_object_play_set, elixir_void_params_emotion_object_bool);
FAST_CALL_PARAMS(emotion_object_smooth_scale_set, elixir_void_params_emotion_object_bool);
FAST_CALL_PARAMS(emotion_object_audio_mute_set, elixir_void_params_emotion_object_bool);
FAST_CALL_PARAMS(emotion_object_video_mute_set, elixir_void_params_emotion_object_bool);
FAST_CALL_PARAMS(emotion_object_spu_mute_set, elixir_void_params_emotion_object_bool);

static JSBool
elixir_bool_params_emotion_object(Eina_Bool (*func)(const Evas_Object *obj),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Eina_Bool ret;
   elixir_value_t       val[1];

   if (!elixir_params_check(cx, _emotion_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   ret = func(eo);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret));
   return JS_TRUE;
}

FAST_CALL_PARAMS(emotion_object_play_get, elixir_bool_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_video_handled_get, elixir_bool_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_audio_handled_get, elixir_bool_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_seekable_get, elixir_bool_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_smooth_scale_get, elixir_bool_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_audio_mute_get, elixir_bool_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_video_mute_get, elixir_bool_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_spu_mute_get, elixir_bool_params_emotion_object);

static JSBool
elixir_void_params_emotion_object_double(void (*func)(Evas_Object *obj, double d),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   elixir_value_t       val[2];

   if (!elixir_params_check(cx, _emotion_object_double_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   func(eo, val[1].v.dbl);

   return JS_TRUE;
}

FAST_CALL_PARAMS(emotion_object_position_set, elixir_void_params_emotion_object_double);
FAST_CALL_PARAMS(emotion_object_audio_volume_set, elixir_void_params_emotion_object_double);
FAST_CALL_PARAMS(emotion_object_play_speed_set, elixir_void_params_emotion_object_double);

static JSBool
elixir_double_params_emotion_object(double (*func)(const Evas_Object *obj),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   double ret;
   elixir_value_t       val[1];

   if (!elixir_params_check(cx, _emotion_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   ret = func(eo);

   return JS_NewNumberValue(cx, ret, &(JS_RVAL(cx, vp)));
}

FAST_CALL_PARAMS(emotion_object_position_get, elixir_double_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_play_length_get, elixir_double_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_ratio_get, elixir_double_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_audio_volume_get, elixir_double_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_play_speed_get, elixir_double_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_progress_status_get, elixir_double_params_emotion_object);

static JSBool
elixir_emotion_object_size_get(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *js_obj;
   Evas_Object *eo;
   int iw;
   int ih;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _emotion_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   emotion_object_size_get(eo, &iw, &ih);
   js_obj = elixir_new_size(cx, iw, ih);
   if (!js_obj)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));

   return JS_TRUE;
}

static JSBool
elixir_int_params_emotion_object(int (*func)(const Evas_Object *obj),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   int ret;
   elixir_value_t       val[1];

   if (!elixir_params_check(cx, _emotion_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   ret = func(eo);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ret));
   return JS_TRUE;
}

FAST_CALL_PARAMS(emotion_object_audio_channel_count, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_audio_channel_get, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_video_channel_count, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_video_channel_get, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_spu_channel_count, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_spu_channel_get, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_chapter_count, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_chapter_get, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_ref_num_get, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_spu_button_count_get, elixir_int_params_emotion_object);
FAST_CALL_PARAMS(emotion_object_spu_button_get, elixir_int_params_emotion_object);

/* int _elixir_emotion_object_vis_get(const Evas_Object *obj) */
/* { */
/*    return (int)emotion_object_vis_get(obj); */
/* } */

/* FAST_CALL_PARAMS_CAST(emotion_object_vis_get, elixir_int_params_emotion_object); */

static JSBool
elixir_string_params_emotion_object_int(const char* (*func)(const Evas_Object *obj, int i),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *ret;
   elixir_value_t       val[2];

   if (!elixir_params_check(cx, _emotion_object_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   ret = func(eo, val[1].v.num);

   elixir_return_str(cx, vp, ret);
   return JS_TRUE;
}

FAST_CALL_PARAMS(emotion_object_audio_channel_name_get, elixir_string_params_emotion_object_int);
FAST_CALL_PARAMS(emotion_object_video_channel_name_get, elixir_string_params_emotion_object_int);
FAST_CALL_PARAMS(emotion_object_spu_channel_name_get, elixir_string_params_emotion_object_int);
FAST_CALL_PARAMS(emotion_object_chapter_name_get, elixir_string_params_emotion_object_int);

const char* _elixir_emotion_object_meta_info_get(const Evas_Object *obj, int i)
{
   return emotion_object_meta_info_get(obj, (Emotion_Meta_Info)i);
}

FAST_CALL_PARAMS_CAST(emotion_object_meta_info_get, elixir_string_params_emotion_object_int);

static JSBool
elixir_void_params_emotion_object_int(void (*func)(Evas_Object *obj, int d),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   elixir_value_t       val[2];

   if (!elixir_params_check(cx, _emotion_object_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   func(eo, val[1].v.num);

   return JS_TRUE;
}

FAST_CALL_PARAMS(emotion_object_audio_channel_set, elixir_void_params_emotion_object_int);
FAST_CALL_PARAMS(emotion_object_video_channel_set, elixir_void_params_emotion_object_int);
FAST_CALL_PARAMS(emotion_object_spu_channel_set, elixir_void_params_emotion_object_int);
FAST_CALL_PARAMS(emotion_object_chapter_set, elixir_void_params_emotion_object_int);

void _elixir_emotion_object_event_simple_send(Evas_Object *obj, int ev)
{
   emotion_object_event_simple_send(obj, (Emotion_Event)ev);
}

FAST_CALL_PARAMS_CAST(emotion_object_event_simple_send, elixir_void_params_emotion_object_int);

/* void _elixir_emotion_object_vis_set(Evas_Object *obj, int visualization) */
/* { */
/*    emotion_object_vis_set(obj, (Emotion_Vis)visualization); */
/* } */

/* FAST_CALL_PARAMS_CAST(emotion_object_vis_set, elixir_void_params_emotion_object_int); */

static JSBool
elixir_emotion_object_eject(JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   elixir_value_t       val[1];

   if (!elixir_params_check(cx, _emotion_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   emotion_object_eject(eo);

   return JS_TRUE;
}

/* static JSBool */
/* elixir_emotion_object_vis_supported(JSContext* cx, uintN argc, jsval *vp) */
/* { */
/*    Evas_Object *eo; */
/*    Eina_Bool ret; */
/*    elixir_value_t       val[2]; */

/*    if (!elixir_params_check(cx, _emotion_object_int_params, val, argc, JS_ARGV(cx, vp))) */
/*      return JS_FALSE; */

/*    GET_PRIVATE(cx, val[0].v.obj, eo); */

/*    ret = emotion_object_vis_supported(eo, val[1].v.num); */

/*    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret)); */
/*    return JS_TRUE; */
/* } */

static JSFunctionSpec   emotion_functions[] = {
   ELIXIR_FN(emotion_object_add, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_module_option_set, 3, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_init, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_file_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_file_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_play_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_play_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_position_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_position_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_video_handled_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_handled_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_seekable_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_play_length_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_size_get, 3, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_smooth_scale_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_smooth_scale_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_ratio_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_event_simple_send, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_volume_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_volume_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_mute_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_mute_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_channel_count, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_channel_name_get, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_channel_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_audio_channel_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_video_mute_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_video_mute_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_video_channel_count, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_video_channel_name_get, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_video_channel_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_video_channel_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_spu_mute_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_spu_mute_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_spu_channel_count, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_spu_channel_name_get, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_spu_channel_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_spu_channel_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_chapter_count, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_chapter_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_chapter_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_chapter_name_get, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_play_speed_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_play_speed_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_eject, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_title_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_progress_info_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_progress_status_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_ref_file_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_ref_num_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_spu_button_count_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_spu_button_get, 1, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(emotion_object_meta_info_get, 2, JSPROP_ENUMERATE, 0 ),
   /* ELIXIR_FN(emotion_object_vis_set, 2, JSPROP_ENUMERATE, 0 ), */
   /* ELIXIR_FN(emotion_object_vis_get, 1, JSPROP_ENUMERATE, 0 ), */
   /* ELIXIR_FN(emotion_object_vis_supported, 2, JSPROP_ENUMERATE, 0 ), */
   JS_FS_END
};


#define EMOTION_DEFINE(Const) { #Const, Const }

static const struct {
   const char*  name;
   int          value;
} emotion_const_properties[] = {
   EMOTION_DEFINE(EMOTION_EVENT_MENU1),
   EMOTION_DEFINE(EMOTION_EVENT_MENU2),
   EMOTION_DEFINE(EMOTION_EVENT_MENU3),
   EMOTION_DEFINE(EMOTION_EVENT_MENU4),
   EMOTION_DEFINE(EMOTION_EVENT_MENU5),
   EMOTION_DEFINE(EMOTION_EVENT_MENU6),
   EMOTION_DEFINE(EMOTION_EVENT_MENU7),
   EMOTION_DEFINE(EMOTION_EVENT_UP),
   EMOTION_DEFINE(EMOTION_EVENT_DOWN),
   EMOTION_DEFINE(EMOTION_EVENT_LEFT),
   EMOTION_DEFINE(EMOTION_EVENT_RIGHT),
   EMOTION_DEFINE(EMOTION_EVENT_SELECT),
   EMOTION_DEFINE(EMOTION_EVENT_NEXT),
   EMOTION_DEFINE(EMOTION_EVENT_PREV),
   EMOTION_DEFINE(EMOTION_EVENT_ANGLE_NEXT),
   EMOTION_DEFINE(EMOTION_EVENT_ANGLE_PREV),
   EMOTION_DEFINE(EMOTION_EVENT_FORCE),
   EMOTION_DEFINE(EMOTION_EVENT_0),
   EMOTION_DEFINE(EMOTION_EVENT_1),
   EMOTION_DEFINE(EMOTION_EVENT_2),
   EMOTION_DEFINE(EMOTION_EVENT_3),
   EMOTION_DEFINE(EMOTION_EVENT_4),
   EMOTION_DEFINE(EMOTION_EVENT_5),
   EMOTION_DEFINE(EMOTION_EVENT_6),
   EMOTION_DEFINE(EMOTION_EVENT_7),
   EMOTION_DEFINE(EMOTION_EVENT_8),
   EMOTION_DEFINE(EMOTION_EVENT_9),
   EMOTION_DEFINE(EMOTION_EVENT_10),
   EMOTION_DEFINE(EMOTION_META_INFO_TRACK_TITLE),
   EMOTION_DEFINE(EMOTION_META_INFO_TRACK_ARTIST),
   EMOTION_DEFINE(EMOTION_META_INFO_TRACK_ALBUM),
   EMOTION_DEFINE(EMOTION_META_INFO_TRACK_YEAR),
   EMOTION_DEFINE(EMOTION_META_INFO_TRACK_GENRE),
   EMOTION_DEFINE(EMOTION_META_INFO_TRACK_COMMENT),
   EMOTION_DEFINE(EMOTION_META_INFO_TRACK_DISC_ID),
   EMOTION_DEFINE(EMOTION_META_INFO_TRACK_COUNT),
   EMOTION_DEFINE(EMOTION_VIS_NONE),
   EMOTION_DEFINE(EMOTION_VIS_GOOM),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_BUMPSCOPE),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_CORONA),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_DANCING_PARTICLES),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_GDKPIXBUF),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_G_FORCE),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_GOOM),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_INFINITE),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_JAKDAW),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_JESS),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_LV_ANALYSER),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_LV_FLOWER),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_LV_GLTEST),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_LV_SCOPE),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_MADSPIN),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_NEBULUS),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_OINKSIE),
   EMOTION_DEFINE(EMOTION_VIS_LIBVISUAL_PLASMA),
   { NULL, 0 }
};

static const struct {
   const char *extention;
   const char *type_name;
} emotion_types[] = {
   { ".avi", "Video AVI" },
   { ".mkv", "Video MKV" },
   { ".mpg", "Video MPEG" },
   { ".wmv", "Video WMV" },
   { ".ts",  "Video MPEG" }
};

static Eina_Bool
module_open(Elixir_Module* em, JSContext* cx, JSObject* parent)
{
   void *tmp;
   unsigned int i = 0;
   jsval property;

   if (em->data)
     return EINA_TRUE;

   em->data = parent;
   tmp = &em->data;
   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), emotion_functions))
     goto on_error;

   while (emotion_const_properties[i].name != NULL)
     {
        property = INT_TO_JSVAL(emotion_const_properties[i].value);
        if (!JS_DefineProperty(cx, parent,
                               emotion_const_properties[i].name,
                               property,
                               NULL, NULL,
                               JSPROP_ENUMERATE | JSPROP_READONLY))
          goto on_error;
        ++i;
     }

   for (i = 0; i < sizeof (emotion_types) / sizeof(*emotion_types); i++)
     elixir_file_register(emotion_types[i].extention, emotion_types[i].type_name);

   _evas_parameter.class = elixir_class_request("evas", NULL);
   _evas_object_parameter.class = elixir_class_request("evas_object", NULL);
   _emotion_object_parameter.class = elixir_class_request("emotion_object", "evas_object_smart");

   _elixir_emotion_regex = malloc(sizeof (char**));
   _elixir_emotion_regex[0] = NULL;

   _elixir_emotion_blacklist = malloc(sizeof (char **));
   _elixir_emotion_blacklist[0] = NULL;

#ifdef ELIXIR_EMOTION_SECURITY
   if (getenv("ELIXIR_EMOTION_MATCH"))
     {
        char *copy;
        unsigned int j = 0;

        copy = strdup(getenv("ELIXIR_EMOTION_MATCH"));
        while (copy)
          {
             _elixir_emotion_regex = realloc(_elixir_emotion_regex, sizeof (char**) * (++j + 1));
             _elixir_emotion_regex[j - 1] = strsep(&copy, ",");
             _elixir_emotion_regex[j] = NULL;
          }
     }

   if (getenv("ELIXIR_EMOTION_BLACKLIST"))
     {
        char *copy;
        unsigned int j = 0;

        copy = strdup(getenv("ELIXIR_EMOTION_BLACKLIST"));
        while (copy)
          {
             _elixir_emotion_blacklist = realloc(_elixir_emotion_blacklist, sizeof (char**) * (++j + 1));
             _elixir_emotion_blacklist[j - 1] = strsep(&copy, ",");
             _elixir_emotion_blacklist[j] = NULL;
          }
     }
#endif

   return EINA_TRUE;

  on_error:
   if (tmp)
     elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;
   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   JSObject *parent;
   void **tmp;
   unsigned int i;

   if (!em->data)
     return EINA_FALSE;

   parent = (JSObject*) em->data;
   tmp = &em->data;

   i = 0;
   while (emotion_functions[i].name != NULL)
     JS_DeleteProperty(cx, parent, emotion_functions[i++].name);

   i = 0;
   while (emotion_const_properties[i].name != NULL)
     JS_DeleteProperty(cx, parent, emotion_const_properties[i++].name);

   for (i = 0; i < sizeof (emotion_types) / sizeof(*emotion_types); i++)
     elixir_file_unregister(emotion_types[i].extention, emotion_types[i].type_name);

   elixir_object_unregister(cx, (JSObject**) tmp);

   if (_elixir_emotion_regex)
     {
        free(_elixir_emotion_regex[0]);
        free(_elixir_emotion_regex);
        _elixir_emotion_regex = NULL;
     }

   if (_elixir_emotion_blacklist)
     {
        free(_elixir_emotion_blacklist[0]);
        free(_elixir_emotion_blacklist);
        _elixir_emotion_blacklist = NULL;
     }

   em->data = NULL;

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "emotion",
   "Cedric SCHIELI <cschieli@gmail.com>"
};

static Elixir_Module em_emotion = {
  &module_api_elixir,
  NULL,
  EINA_FALSE,
  {
    module_open,
    NULL,
    module_close
  }
};

Eina_Bool
emotion_binding_init(void)
{
   return elixir_modules_register(&em_emotion);
}

void
emotion_binding_shutdown(void)
{
   elixir_modules_unregister(&em_emotion);
}

#ifndef EINA_STATIC_BUILD_EMOTION
EINA_MODULE_INIT(emotion_binding_init);
EINA_MODULE_SHUTDOWN(emotion_binding_shutdown);
#endif

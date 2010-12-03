#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <Ecore.h>

#include "SDL.h"
#include "SDL_mixer.h"

#include "Elixir.h"

static elixir_parameter_t               _mix_chunk_parameter = {
   "Mix_Chunk", JOBJECT, NULL
};
static const elixir_parameter_t*        _4int_params[5] = {
   &int_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _2int_params[3] = {
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _mix_chunk_params[2] = {
   &_mix_chunk_parameter,
   NULL
};
static const elixir_parameter_t*        _mix_chunk_int_params[3] = {
   &_mix_chunk_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _int_mix_chunk_int_params[4] = {
   &int_parameter,
   &_mix_chunk_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _int_mix_chunk_2int_params[5] = {
   &int_parameter,
   &_mix_chunk_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};

static void *callback_context = NULL;
static Ecore_Pipe *callback_pipe = NULL;

static JSBool
elixir_void_int_params(void (*func)(int channel),
                       JSContext *cx, uintN argc, jsval *vp)
{
   int channel;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   channel = val[0].v.num;

   func(channel);

   return JS_TRUE;
}

FAST_CALL_PARAMS(Mix_Pause, elixir_void_int_params);
FAST_CALL_PARAMS(Mix_Resume, elixir_void_int_params);

static JSBool
elixir_int_int_params(int (*func)(int n),
                      JSContext *cx, uintN argc, jsval *vp)
{
   int value;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   value = val[0].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(value)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(Mix_HaltChannel, elixir_int_int_params);
/* FAST_CALL_PARAMS(Mix_VolumeMusic, elixir_int_int_params); */
FAST_CALL_PARAMS(Mix_AllocateChannels, elixir_int_int_params);

static JSBool
elixir_int_2int_params(int (*func)(int channel, int n),
		       JSContext *cx, uintN argc, jsval *vp)
{
   int channel;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _2int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   channel = val[0].v.num;
   value = val[1].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(channel, value)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(Mix_Volume, elixir_int_2int_params);
FAST_CALL_PARAMS(Mix_ExpireChannel, elixir_int_2int_params);
FAST_CALL_PARAMS(Mix_FadeOutChannel, elixir_int_2int_params);

static JSBool
elixir_Mix_OpenAudio(JSContext *cx, uintN argc, jsval *vp)
{
   Uint16 format;
   int frequency;
   int channels;
   int chunksize;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _4int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   frequency = val[0].v.num;
   format = val[1].v.num;
   channels = val[2].v.num;
   chunksize = val[3].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(Mix_OpenAudio(frequency, format, channels, chunksize)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(Mix_CloseAudio, elixir_void_params_void);

static JSBool
elixir_Mix_LoadWAV(JSContext *cx, uintN argc, jsval *vp)
{
   void *ret;
   char *filename;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   filename = elixir_file_canonicalize(elixir_get_string_bytes(val[0].v.str, NULL));

   ret = Mix_LoadWAV(filename);

   free(filename);

   elixir_return_ptr(cx, vp, ret, elixir_class_request("Mix_Chunk", NULL));
   return JS_TRUE;
}

static JSBool
elixir_Mix_PlayChannel(JSContext *cx, uintN argc, jsval *vp)
{
   Mix_Chunk *chunk;
   int channel;
   int loops;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _int_mix_chunk_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   channel = val[0].v.num;
   GET_PRIVATE(cx, val[1].v.obj, chunk);
   loops = val[2].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(Mix_PlayChannel(channel, chunk, loops)));
   return JS_TRUE;
}

static JSBool
elixir_Mix_FreeChunk(JSContext *cx, uintN argc, jsval *vp)
{
   Mix_Chunk *chunk;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _mix_chunk_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, chunk);

   Mix_FreeChunk(chunk);

   return JS_TRUE;
}

static JSBool
elixir_Mix_VolumeChunk(JSContext *cx, uintN argc, jsval *vp)
{
   Mix_Chunk *chunk;
   int volume;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _mix_chunk_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, chunk);
   volume = val[1].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(Mix_VolumeChunk(chunk, volume)));
   return JS_TRUE;
}

static JSBool
elixir_Mix_FadeInChannel(JSContext *cx, uintN argc, jsval *vp)
{
   Mix_Chunk *chunk;
   int channel;
   int loops;
   int ms;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _int_mix_chunk_2int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   channel = val[0].v.num;
   GET_PRIVATE(cx, val[1].v.obj, chunk);
   loops = val[2].v.num;
   ms = val[3].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(Mix_FadeInChannel(channel, chunk, loops, ms)));
   return JS_TRUE;
}

static void
_channel_callback(__UNUSED__ void *data, void *buffer, unsigned int nbyte)
{
   JSFunction *fct;
   JSContext *cx;
   JSObject *parent;
   jsval argv[1];
   jsval js_return;
   int channel;

   if (!callback_context) return ;

   if (nbyte != sizeof (int)) return ;
   channel = *(int*) buffer;

   fct = elixir_void_get_private(callback_context);
   if (fct)
     {
	Eina_Bool suspended;

        parent = elixir_void_get_parent(callback_context);
        cx = elixir_void_get_cx(callback_context);
        if (!parent || !cx)
          return ;

	suspended = elixir_function_suspended(cx);
	if (suspended)
	  elixir_function_start(cx);

        argv[0] = INT_TO_JSVAL(channel);
        elixir_function_run(cx, fct, parent,  1, argv, &js_return);

	if (suspended)
	  elixir_function_stop(cx);
     }

   return ;
}

static void
_channel_finished_cb(int channel)
{
   ecore_pipe_write(callback_pipe, &channel, sizeof (int));
}

static JSBool
elixir_Mix_ChannelFinished(JSContext *cx, uintN argc, jsval *vp)
{
   elixir_value_t val[1];

   if (!elixir_params_check(cx, function_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   if (callback_context)
     elixir_void_free(callback_context);
   callback_context = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), JSVAL_VOID, val[0].v.fct);

   Mix_ChannelFinished(_channel_finished_cb);

   return JS_TRUE;
}

static const struct {
   const char*  name;
   int          value;
} mixer_const_properties[] = {
   { "AUDIO_U8", AUDIO_U8 },
   { "AUDIO_S8", AUDIO_S8 },
   { "AUDIO_U16LSB", AUDIO_U16LSB },
   { "AUDIO_S16LSB", AUDIO_S16LSB },
   { "AUDIO_U16MSB", AUDIO_U16MSB },
   { "AUDIO_S16MSB", AUDIO_S16MSB },
   { "AUDIO_U16", AUDIO_U16 },
   { "AUDIO_S16", AUDIO_S16 },
   { "AUDIO_U16SYS", AUDIO_U16SYS },
   { "AUDIO_S16SYS", AUDIO_S16SYS },
   { NULL, 0 }
};

static JSFunctionSpec           mixer_functions[] = {
  ELIXIR_FN(Mix_Pause, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_Resume, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_HaltChannel, 1, JSPROP_ENUMERATE, 0 ),
  /*    ELIXIR_FN(Mix_VolumeMusic, 1, JSPROP_ENUMERATE, 0 ), */
  ELIXIR_FN(Mix_AllocateChannels, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_Volume, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_ExpireChannel, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_FadeOutChannel, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_OpenAudio, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_CloseAudio, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_LoadWAV, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_PlayChannel, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_FreeChunk, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_VolumeChunk, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_FadeInChannel, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(Mix_ChannelFinished, 1, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

static const struct {
   const char *extention;
   const char *type_name;
} mix_types[] = {
   { ".wav", "Sample WAV" },
   { ".ogg", "Sample OGG" },
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp = NULL;
   unsigned int i = 0;
   jsval property;

   if (em->data)
     return EINA_FALSE;

   ecore_init();

   callback_pipe = ecore_pipe_add(_channel_callback, NULL);
   if (!callback_pipe)
     goto on_error;

   em->data = parent;
   tmp = &em->data;
   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), mixer_functions))
     goto on_error;

   while (mixer_const_properties[i].name)
     {
        property = INT_TO_JSVAL(mixer_const_properties[i].value);
        if (!JS_DefineProperty(cx, parent,
			       mixer_const_properties[i].name,
			       property,
			       NULL, NULL,
			       JSPROP_ENUMERATE | JSPROP_READONLY))
          return EINA_FALSE;
        ++i;
     }

   _mix_chunk_parameter.class = elixir_class_request("Mix_Chunk", NULL);

   SDL_InitSubSystem(SDL_INIT_AUDIO);

   for (i = 0; i < sizeof (mix_types) / sizeof(*mix_types); i++)
     elixir_file_register(mix_types[i].extention, mix_types[i].type_name);

   return EINA_TRUE;

  on_error:
   if (tmp)
     elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   if (callback_pipe)
     ecore_pipe_del(callback_pipe);
   callback_pipe = NULL;

   ecore_shutdown();

   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   void **tmp;
   unsigned int i = 0;

   if (!em->data)
     return EINA_FALSE;

   while (mixer_functions[i].name)
     JS_DeleteProperty(cx, (JSObject*) em->data, mixer_functions[i++].name);

   i = 0;
   while (mixer_const_properties[i].name)
     JS_DeleteProperty(cx, (JSObject*) em->data, mixer_const_properties[i++].name);

   if (callback_context)
     elixir_void_free(callback_context);
   callback_context = NULL;

   for (i = 0; i < sizeof (mix_types) / sizeof(*mix_types); i++)
     elixir_file_unregister(mix_types[i].extention, mix_types[i].type_name);

   tmp = &em->data;

   elixir_object_unregister(cx, (JSObject**) tmp);

   em->data = NULL;

   SDL_QuitSubSystem(SDL_INIT_AUDIO);

   ecore_pipe_del(callback_pipe);
   callback_pipe = NULL;

   ecore_shutdown();

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "mix",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_mixer = {
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
mixer_binding_init(void)
{
   return elixir_modules_register(&em_mixer);
}

void
mixer_binding_shutdown(void)
{
   elixir_modules_unregister(&em_mixer);
}

#ifndef EINA_STATIC_BUILD_MIX
EINA_MODULE_INIT(mixer_binding_init);
EINA_MODULE_SHUTDOWN(mixer_binding_shutdown);
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libgen.h>
#include <pthread.h>

#include <jsxdrapi.h>

#include "Elixir.h"
#include "config.h"

#ifdef BUILD_MODULE_EDJE
# include <Edje.h>
#endif

#define RUNTIME_MEM     (1024L * 1024L)
#define STACK_MEM       (32L * 1024L)

#define MAGIC_GCCX      0x42775502
typedef struct gc_cx_s  gc_cx_t;
struct gc_cx_s
{
   JSContext *cx;
   JSContext *main;

   Eina_List *obj;

   int magic;
   int count;
   int nested;
   int save;

   Eina_Bool dogc : 1;
   Eina_Bool delete : 1;
   Eina_Bool dying : 1;
};

static elixir_virtual_chroot_t  _evct = ELIXIR_VCHROOT_ALL;
static Eina_List *suspended_cx = NULL;
static pthread_mutex_t suspended_lock = PTHREAD_MUTEX_INITIALIZER;
static int running_thread = 0;

static unsigned int _elixir_api_version = 0;

EAPI FILE *tracker = NULL;

extern char*    files_root;
extern int      files_root_length;

static gc_cx_t*
elixir_get_gccx(JSContext *cx)
{
   gc_cx_t      *gccx;

   gccx = JS_GetContextPrivate(cx);
   if (!gccx)
     return NULL;

   if (gccx->magic != MAGIC_GCCX)
     return NULL;

   return gccx;
}

static void
elixir_register_cx(JSContext *cx)
{
   gc_cx_t      *gccx;

   if (!cx)
     return ;

   gccx = malloc(sizeof(gc_cx_t));
   if (!gccx)
     return ;

   gccx->magic = MAGIC_GCCX;
   gccx->cx = cx;
   gccx->main = NULL;
   gccx->obj = NULL;
   gccx->count = 0;
   gccx->nested = 0;
   gccx->delete = EINA_FALSE;
   gccx->save = -1;
   gccx->dogc = EINA_FALSE;
   gccx->dying = EINA_FALSE;

   JS_SetContextPrivate(cx, gccx);
}

static JSContext *
elixir_unregister_cx(JSContext *cx, Eina_Bool force)
{
   gc_cx_t      *gccx;

   if (!cx)
     return NULL;

   gccx = elixir_get_gccx(cx);
   if (!gccx)
     return NULL;

   if (gccx->dying)
     return NULL;
   gccx->dying = EINA_TRUE;

   while (gccx->obj && gccx->main)
     {
#ifndef NDEBUG
	void *tmp = gccx->obj;
#endif

	elixir_void_move(gccx->main, eina_list_data_get(gccx->obj));

	assert(tmp != gccx->obj);
     }

   if (force == EINA_TRUE || (gccx->count == 0 && gccx->nested == 0)) // gccx->main
     {
	if (!(gccx->count == 0 && gccx->nested == 0))
	  elixir_debug_print("%i with still %i living objects and nested %i times in %p.", force, gccx->count, gccx->nested);
	if (gccx->save >= 0)
	  {
	     JS_ResumeRequest(cx, gccx->save);
	     pthread_mutex_lock(&suspended_lock);
	     suspended_cx = eina_list_remove(suspended_cx, cx);
	     pthread_mutex_unlock(&suspended_lock);
	  }
	while (gccx->nested-- > 0)
	  JS_EndRequest(cx);

	if (gccx->main)
	  {
	     gc_cx_t *gccxm;

	     gccxm = elixir_get_gccx(gccx->main);
	     if (gccxm) gccxm->dogc = EINA_TRUE;
	  }

	JS_SetContextThread(cx);
	JS_DestroyContext(cx);
        gccx->magic = 0;
        free(gccx);

        return NULL;
     }
   else
     {
        elixir_debug_print("Still %i living objects and nested %i time in %p.", gccx->count, gccx->nested, cx);
        gccx->delete = EINA_TRUE;
     }

   return cx;
}

static void
elixir_attach_cx(JSContext *parent, JSContext *clone)
{
   gc_cx_t *gccx;
   gc_cx_t *gccxl;

   gccx = elixir_get_gccx(parent);
   if (!gccx) return ;

   if (gccx->main)
     {
	elixir_attach_cx(gccx->main, clone);
	return ;
     }

   gccxl = elixir_get_gccx(clone);
   if (!gccxl) return ;

   gccxl->main = parent;
}

void
elixir_void_register(JSContext *cx, void *data)
{
   gc_cx_t *gccx;

   gccx = elixir_get_gccx(cx);
   gccx->obj = eina_list_append(gccx->obj, data);
}

void
elixir_void_unregister(JSContext *cx, void *data)
{
   gc_cx_t *gccx;

   gccx = elixir_get_gccx(cx);
   gccx->obj = eina_list_remove(gccx->obj, data);
}

void
elixir_lock_cx(JSContext *cx)
{
   gc_cx_t *gccx;

   gccx = elixir_get_gccx(cx);
   assert(gccx->save == -1);
   if (gccx->nested++ == 0)
     JS_SetContextThread(cx);
   JS_BeginRequest(cx);
}

void
elixir_unlock_cx(JSContext *cx)
{
   gc_cx_t *gccx;

   JS_EndRequest(cx);

   gccx = elixir_get_gccx(cx);
   if (gccx)
     {
	assert(gccx->save == -1);
	if (--gccx->nested == 0)
	  JS_ClearContextThread(cx);
	if (gccx->delete && gccx->count == 0)
	  elixir_unregister_cx(cx, EINA_FALSE);
     }
}

void
elixir_thread_new(void)
{
   running_thread++;
}

void
elixir_thread_del(void)
{
   running_thread--;
}

Eina_Bool
elixir_function_suspended(JSContext *cx)
{
   gc_cx_t *gccx;

   gccx = elixir_get_gccx(cx);
   if (gccx && gccx->save >= 0)
     return EINA_TRUE;
   return EINA_FALSE;
}

void
elixir_function_start(JSContext *cx)
{
   gc_cx_t *gccx;

   gccx = elixir_get_gccx(cx);
   if (gccx && gccx->save >= 0)
     {
	pthread_mutex_lock(&suspended_lock);
	suspended_cx = eina_list_remove(suspended_cx, cx);
	pthread_mutex_unlock(&suspended_lock);
	JS_SetContextThread(cx);
	JS_ResumeRequest(cx, gccx->save);
	gccx->save = -1;
     }
   elixir_lock_cx(cx);
}

void
elixir_function_stop(JSContext *cx)
{
   gc_cx_t *gccx;

   elixir_unlock_cx(cx);

   gccx = elixir_get_gccx(cx);
   if (gccx) {
      JS_SetContextThread(cx);
      gccx->save = JS_SuspendRequest(cx);
      JS_ClearContextThread(cx);
      pthread_mutex_lock(&suspended_lock);
      suspended_cx = eina_list_append(suspended_cx, cx);
      pthread_mutex_unlock(&suspended_lock);
   }
}

void
elixir_suspended_gc(void)
{
   Eina_List *l;
   JSContext *cx;

   if (running_thread > 0)
     return ;

   pthread_mutex_lock(&suspended_lock);
   EINA_LIST_FOREACH(suspended_cx, l, cx)
     {
	JS_SetContextThread(cx);
	JS_MaybeGC(cx);
	JS_ClearContextThread(cx);
     }
   pthread_mutex_unlock(&suspended_lock);
}

void
elixir_function_gc(JSContext *cx)
{
   gc_cx_t *gccx;

   gccx = elixir_get_gccx(cx);
   if (gccx && gccx->dogc)
     {
	JS_SetContextThread(cx);
	JS_GC(cx);
	JS_ClearContextThread(cx);
	gccx->dogc = EINA_FALSE;
     }
}

void
elixir_decrease_count(JSContext *cx)
{
   gc_cx_t      *gccx;

   gccx = elixir_get_gccx(cx);
   if (!gccx)
     return ;

   gccx->count--;
   if (gccx->delete && gccx->count == 0)
     elixir_unregister_cx(cx, EINA_FALSE);
}

void
elixir_increase_count(JSContext *cx)
{
   gc_cx_t      *gccx;

   gccx = elixir_get_gccx(cx);
   if (!gccx)
     return ;

   gccx->count++;
}

Eina_Bool
elixir_string_register(JSContext *cx, JSString **string)
{
   JSBool ret;

   if (!string || !*string)
     return EINA_FALSE;

   elixir_lock_cx(cx);
   ret = JS_AddRoot(cx, string);
   elixir_unlock_cx(cx);

   if (!ret)
     return EINA_FALSE;

   elixir_increase_count(cx);

   return EINA_TRUE;
}

Eina_Bool
elixir_string_unregister(JSContext *cx, JSString **string)
{
   JSBool ret;

   if (!string || !*string)
     return EINA_FALSE;

   elixir_lock_cx(cx);
   ret = JS_RemoveRoot(cx, string);
   elixir_unlock_cx(cx);

   if (!ret)
     return EINA_FALSE;

   elixir_decrease_count(cx);

   return EINA_TRUE;
}

Eina_Bool
elixir_object_named_register(JSContext *cx, const char *name, JSObject **obj, void *data)
{
   if (!obj || !*obj)
     return EINA_FALSE;

   elixir_lock_cx(cx);
   if (!JS_AddNamedRoot(cx, obj, name))
     goto on_first_error;
   if (data)
     if (!JS_SetPrivate(cx, *obj, data))
       goto on_error;
   elixir_unlock_cx(cx);

   elixir_increase_count(cx);

   return EINA_TRUE;

 on_error:
   JS_RemoveRoot(cx, obj);
 on_first_error:
   elixir_unlock_cx(cx);

   return EINA_FALSE;
}

Eina_Bool
elixir_object_unregister(JSContext *cx, JSObject **obj)
{
   JSBool ret;

   if (!obj || !*obj)
     return EINA_FALSE;

   elixir_lock_cx(cx);
   ret = JS_RemoveRoot(cx, obj);
   elixir_unlock_cx(cx);

   if (!ret)
     return EINA_FALSE;

   elixir_decrease_count(cx);

   return EINA_TRUE;
}

Eina_Bool
elixir_rval_named_new(JSContext *cx, const char *name, JSClass *class, void *data, jsval *rval)
{
   JSObject *new;
   Eina_Bool leave;

   leave = EINA_FALSE;

   *rval = JSVAL_NULL;

   elixir_lock_cx(cx);

   new = JS_NewObject(cx, class, NULL, NULL);
   if (!new)
     goto on_first_error;
   if (!JS_AddNamedRoot(cx, &new, name))
     goto on_first_error;
   if (data)
     if (!JS_SetPrivate(cx, new, data))
       goto on_error;

   *rval = OBJECT_TO_JSVAL(new);
   if (!JS_AddNamedRoot(cx, rval, name))
     goto on_error;

   elixir_increase_count(cx);

   leave = EINA_TRUE;

 on_error:
   JS_RemoveRoot(cx, &new);
 on_first_error:
   elixir_unlock_cx(cx);

   return leave;
}

Eina_Bool
elixir_rval_named_register(JSContext *cx, const char *name, jsval *rval)
{
   JSBool ret;

   if (!rval)
     return EINA_FALSE;

   elixir_lock_cx(cx);
   ret = JS_AddNamedRoot(cx, rval, name);
   elixir_unlock_cx(cx);

   if (!ret)
     return EINA_FALSE;

   elixir_increase_count(cx);

   return EINA_TRUE;
}

Eina_Bool
elixir_rval_delete(JSContext *cx, jsval *rval)
{
   JSBool ret;

   if (!rval)
     return EINA_FALSE;

   elixir_lock_cx(cx);
   ret = JS_RemoveRoot(cx, rval);
   elixir_unlock_cx(cx);

   if (!ret)
     return EINA_FALSE;

   elixir_decrease_count(cx);
   return EINA_TRUE;
}

static void
error_report(JSContext *cx, const char *message, JSErrorReport *report)
{
   (void) cx;

   if (report->linebuf)
     elixir_debug_print("%s:%u:(%s) %s", report->filename, report->lineno, report->linebuf, message);
   else
     elixir_debug_print("%s:%u: %s", report->filename, report->lineno, message);
}

Eina_Bool
elixir_function_run(JSContext *cx, JSFunction *callback, JSObject *parent, int argc, jsval *argv, jsval *rval)
{
   JSObject *obj;
   int i;
   Eina_Bool ret = EINA_TRUE;

   *rval = JSVAL_NULL;

   elixir_lock_cx(cx);

   elixir_object_register(cx, &parent, NULL);
   for (i = 0; i < argc; ++i)
     elixir_rval_register(cx, argv + i);
   elixir_rval_register(cx, rval);

   obj = JS_GetFunctionObject(callback);
   elixir_object_register(cx, &obj, NULL);

   if (!JS_GetParent(cx, obj))
     JS_SetParent(cx, obj, parent);

   if (JS_CallFunction(cx, parent, callback, argc, argv, rval) == JS_FALSE)
     {
	JS_ReportPendingException(cx);
	JS_ReportError(cx, "inside %s", JS_GetFunctionName(callback));
	JS_ReportPendingException(cx);
        ret = EINA_FALSE;
	JS_GetPendingException(cx, rval);
	JS_ClearPendingException(cx);
     }

   elixir_object_unregister(cx, &obj);

   for (i = 0; i < argc; ++i)
     elixir_rval_delete(cx, argv + i);
   elixir_rval_delete(cx, rval);
   elixir_object_unregister(cx, &parent);

   elixir_unlock_cx(cx);

   return ret;
}

Elixir_Runtime*
elixir_init(void)
{
   Elixir_Runtime       *result;

   result = malloc(sizeof (Elixir_Runtime));
   if (!result)
     return NULL;

   result->clone = 0;
   result->em = NULL;
   result->cx = NULL;
   result->root = NULL;
   result->rt = JS_NewRuntime(RUNTIME_MEM);
   if (!result->rt)
     goto first;

   result->cx = JS_NewContext(result->rt, STACK_MEM);
   if (!result->cx)
     goto first;
   elixir_register_cx(result->cx);

   JS_SetVersion(result->cx, JSVERSION_1_8);
   JS_SetErrorReporter(result->cx, error_report);
   elixir_lock_cx(result->cx);

   result->root = JS_NewObject(result->cx, elixir_class_request("elixir", NULL), NULL, NULL);
   if (!result->root)
     goto error;

   if (JS_AddRoot(result->cx, &result->root) == JS_FALSE)
     goto error;

   if (JS_InitStandardClasses(result->cx, result->root) == JS_FALSE)
     goto error;

   result->em = elixir_modules_find("elixir");
   if (!result->em)
     goto error;

   if (elixir_modules_load(result->em, result->cx, result->root))
     goto error;

   elixir_unlock_cx(result->cx);

   JS_SetRuntimePrivate(result->rt, eina_array_new(4));

   return result;

  error:
   if (result->root)
     JS_RemoveRoot(result->cx, &result->root);
   elixir_unlock_cx(result->cx);

 first:
   elixir_unregister_cx(result->cx, EINA_FALSE);
   if (result->rt)
     JS_DestroyRuntime(result->rt);
   free(result);

   return NULL;
}

Elixir_Runtime*
elixir_clone(JSContext *cx, JSObject *root)
{
   Elixir_Runtime       *result;

   result = malloc(sizeof (Elixir_Runtime));
   if (!result)
     return NULL;

   result->clone = 1;
   result->em = NULL;
   result->rt = JS_GetRuntime(cx);
   result->cx = JS_NewContext(result->rt, STACK_MEM);
   if (!result->cx)
     {
        free(result);
        return NULL;
     }
   elixir_register_cx(result->cx);
   JS_SetVersion(result->cx, JS_GetVersion(cx));
   JS_SetErrorReporter(result->cx, error_report);
   elixir_lock_cx(result->cx);

   result->root = root;
   JS_AddRoot(result->cx, &result->root);

   elixir_unlock_cx(result->cx);

   elixir_attach_cx(cx, result->cx);

   return result;
};

void
elixir_shutdown(Elixir_Runtime *er)
{
   if (!er)
     return ;

/*    if (elixir_function_suspended(er->cx)) */
/*      elixir_function_start(er->cx); */
/*    else */
     elixir_lock_cx(er->cx);

   if (!er->clone)
     if (er->em)
       elixir_modules_unload(er->em, er->cx);

   if (er->root)
     JS_RemoveRoot(er->cx, &er->root);
   elixir_unlock_cx(er->cx);

   elixir_unregister_cx(er->cx, er->clone ? EINA_TRUE : EINA_FALSE);

   if (!er->clone)
     {
        if (er->rt)
	  {
	     eina_array_free(JS_GetRuntimePrivate(er->rt));
	     JS_DestroyRuntime(er->rt);
	  }
     }


   free(er);
}

void
elixir_maybe_shutdown(Elixir_Runtime *er)
{
   if (!er)
     return ;

   elixir_lock_cx(er->cx);

   if (!er->clone)
     if (er->em)
       elixir_modules_unload(er->em, er->cx);

   if (er->root)
     JS_RemoveRoot(er->cx, &er->root);
   elixir_unlock_cx(er->cx);

   elixir_unregister_cx(er->cx, EINA_FALSE);

   if (!er->clone)
     {
        if (er->rt)
          JS_DestroyRuntime(er->rt);
     }
   free(er);
}

void
elixir_file_virtual_chroot_directive_set(elixir_virtual_chroot_t evct)
{
   _evct = evct;
}

EAPI char *
elixir_exe_canonicalize(const char *filename)
{
   switch (_evct)
     {
      case ELIXIR_VCHROOT_NOEXEC:
	 if (strncmp(files_root, filename, files_root_length) == 0)
	   return NULL;
      case ELIXIR_VCHROOT_DATA_ONLY:
	 return strdup(filename);
      case ELIXIR_VCHROOT_ALL:
	 return elixir_file_canonicalize(filename);
      default:
	 return NULL;
     }
}

static Eina_Bool
elixir_script_compile(Elixir_Script* es, const char *bytes, int length, const char *filename)
{
   Eina_Bool ret = EINA_FALSE;

   elixir_lock_cx(es->er->cx);

   es->script = JS_CompileScript(es->er->cx, es->er->root, bytes, length, filename, 1);
   if (!es->script) goto on_error;

   es->jscript = JS_NewScriptObject(es->er->cx, es->script);
   if (!es->jscript) goto on_error;

   if (!elixir_object_register(es->er->cx, &es->jscript, NULL))
     goto on_error;

   ret = EINA_TRUE;

 on_error:
   elixir_unlock_cx(es->er->cx);
   return ret;
}

static int
elixir_script_precompiled(Elixir_Script *es, const char *bytes, int length, const char *filename)
{
#ifdef ELIXIR_USE_PRECOMPILED
   JSXDRState *xdr;

   xdr = JS_XDRNewMem(es->er->cx, JSXDR_DECODE);
   if (!xdr) return EINA_FALSE;

   JS_XDRMemSetData(xdr, (char*) bytes, length);
   if (!JS_XDRScript(xdr, &es->script))
     {
	JS_XDRDestroy(xdr);
	return EINA_FALSE;
     }

   JS_XDRMemSetData(xdr, NULL, 0);
   JS_XDRDestroy(xdr);

   return EINA_TRUE;
#else
   return EINA_FALSE;
#endif
}

Elixir_Script*
elixir_script_file(Elixir_Runtime *er, int param, const char **params)
{
   const unsigned char *bytes;
   Elixir_Loaded_File *file;
   Elixir_Script *es;
   unsigned int length;

   file = elixir_loader_load(param, params);
   if (!file) return NULL;

   es = calloc(1, sizeof (Elixir_Script));
   if (!es) goto on_error;

   es->file = file;
   es->out = stderr;
   es->er = er;

   if (elixir_id_push(es->file) == EINA_FALSE) goto on_error;
   es->push = 1;

   bytes = elixir_loader_compiled(file, &length);
   if (bytes)
     {
	elixir_debug_print("Trying compiled version.");
	if (elixir_script_precompiled(es,
				      (const char *) bytes, length,
				      elixir_loader_filename(es->file)))
	  {
	     elixir_debug_print("Done.");
	     return es;
	  }
	elixir_debug_print("Failed.");
     }

   bytes = elixir_id_content(&length);
   if (!elixir_script_compile(es,
			      (const char *) bytes, length,
			      elixir_loader_filename(es->file)))
     goto on_error;

   return es;

 on_error:
   if (es) elixir_script_free(es);
   else elixir_loader_unload(file);
   return NULL;
}

Elixir_Script*
elixir_script_template(Elixir_Runtime *er, int param, const char **params)
{
   char                 *template = NULL;
   Elixir_Loaded_File   *file;
   const unsigned char  *bytes;
   Elixir_Script        *es;
   unsigned int          length;

   file = elixir_loader_load(param, params);
   if (!file) return NULL;

   es = calloc(1, sizeof (Elixir_Script));
   if (!es) goto on_error;

   es->file = file;
   es->out = stderr;
   es->er = er;

   if (elixir_id_push(es->file)) goto on_error;
   es->push = 1;

   bytes = elixir_id_content(&length);

   template = elixir_template(bytes, length, &length);
   if (!template) goto on_error;

   if (!elixir_script_compile(es,
			      template, length,
			      elixir_loader_filename(es->file)))
     goto on_error;

   free(template);

   return es;

 on_error:
   if (template) free(template);
   if (es) elixir_script_free(es);
   else elixir_loader_unload(file);
   return NULL;
}

char *
elixir_script_binary(Elixir_Script *es, unsigned int *size)
{
   char *result = NULL;
   JSXDRState *xdr;

   elixir_lock_cx(es->er->cx);

   xdr = JS_XDRNewMem(es->er->cx, JSXDR_ENCODE);

   elixir_unlock_cx(es->er->cx);
   if (!xdr) return NULL;

   if (JS_XDRScript(xdr, &es->script))
     result = (char*) JS_XDRMemGetData(xdr, size);

/*    JS_XDRDestroy(xdr); */

   return result;
}

void
elixir_script_set_out(Elixir_Script *es, FILE *lout)
{
   if (es)
     es->out = lout;
}

FILE*
elixir_script_get_current_out(void)
{
   return out;
}

Eina_Bool
elixir_script_run(Elixir_Script *es, jsval *rval)
{
   Eina_Array *exe;
   FILE *save;
   char *dupc;
   char *file;
   char *dot;
   JSBool status;
   jsval tmp;

   if (!es)
     return EINA_FALSE;

   /* Extract application name */
   dupc = strdupa(elixir_loader_filename(es->file));
   if (!dupc) return EINA_FALSE;

   file = basename(dupc);
   dot = strrchr(file, '.');
   if (!dot) return EINA_FALSE;
   *dot = '\0';
   /* *** */

   exe = JS_GetRuntimePrivate(es->er->rt);

   if (eina_array_count_get(exe) > 0
       && strcmp(eina_array_data_get(exe, eina_array_count_get(exe) - 1), file) == 0)
     file = NULL;

   if (file)
     {
	if (tracker) fprintf(tracker, "%s\n", file);

	eina_array_push(exe, strdup(file));
     }

   elixir_lock_cx(es->er->cx);

   save = out;
   out = es->out;
   status = JS_ExecuteScript(es->er->cx, es->er->root, es->script, &tmp);
   out = save;

   if (file)
     {
	free(eina_array_pop(exe));

	if (tracker && eina_array_count_get(exe) > 1)
	  fprintf(tracker, "%s\n", (char*) eina_array_data_get(exe, eina_array_count_get(exe) - 1));
     }

   if (status == JS_FALSE)
     {
	JS_ReportPendingException(es->er->cx);
	JS_ReportError(es->er->cx, "inside %s", file);
	JS_ReportPendingException(es->er->cx);
	elixir_unlock_cx(es->er->cx);

	return EINA_FALSE;
     }

   elixir_unlock_cx(es->er->cx);

   if (rval)
     *rval = tmp;

   return EINA_TRUE;
}

void
elixir_script_free(Elixir_Script *es)
{
   if (es)
     {
	if (es->file)
	  elixir_loader_unload(es->file);
	if (es->push)
	  elixir_id_pop();
        if (es->er->cx)
          if (es->jscript)
            elixir_object_unregister(es->er->cx, &es->jscript);
        free(es);
     }
}

Eina_Bool
elixir_reset(Elixir_Runtime *er)
{
   if (!er || !er->em)
     return EINA_FALSE;

   if (er->em->func.reset)
     return er->em->func.reset(er->em, er->cx);

   return EINA_FALSE;
}

unsigned int
elixir_api_version_get(void)
{
   return _elixir_api_version;
}

void
elixir_api_version_set(unsigned int version)
{
   if (version > _elixir_api_version)
     _elixir_api_version = version;
}



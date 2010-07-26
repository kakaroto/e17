#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <libgen.h>
#include <alloca.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <Ecore.h>
#include <Ecore_Download.h>

#include "Elixir.h"

static elixir_parameter_t               _ecore_download_file_parameter = {
  "Ecore_Download_File", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_download_dir_parameter = {
  "Ecore_Download_Dir", JOBJECT, NULL
};

static const elixir_parameter_t*        _3string_params[4] = {
  &string_parameter,
  &string_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_download_dir_params[2] = {
  &_ecore_download_dir_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_download_dir_any_params[3] = {
  &_ecore_download_dir_parameter,
  &any_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_download_dir_2string_params[4] = {
  &_ecore_download_dir_parameter,
  &string_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_download_file_params[2] = {
  &_ecore_download_file_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_download_file_any_params[3] = {
  &_ecore_download_file_parameter,
  &any_parameter,
  NULL
};

static Eina_Bool		_no_reuse = EINA_FALSE;

static int                      _ecore_download_init = 0;

static int                      ELIXIR_DOWNLOAD_EVENT_CANCEL = 0;
static int                      ELIXIR_DOWNLOAD_EVENT_COMPLETE = 0;
static int                      ELIXIR_DOWNLOAD_EVENT_PROGRESS = 0;

static Ecore_Event_Handler*     eeh_elixir_download_event[3] = { NULL, NULL, NULL };

static const struct
{
  const char*   name;
  int*          value;
} ecore_download_properties[] = {
  { "ECORE_DOWNLOAD_EVENT_CANCEL", &ELIXIR_DOWNLOAD_EVENT_CANCEL },
  { "ECORE_DOWNLOAD_EVENT_COMPLETE", &ELIXIR_DOWNLOAD_EVENT_COMPLETE },
  { "ECORE_DOWNLOAD_EVENT_PROGRESS", &ELIXIR_DOWNLOAD_EVENT_PROGRESS },
  { NULL, NULL }
};

static void
_ecore_event_func_free(__UNUSED__ void* data, void* ev)
{
   elixir_void_free(ev);
}

static Eina_Bool
_elixir_download_event_cancel_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Download_Event_Cancel          *edec;
   JSContext                            *cx;
   JSObject                             *obj_edec;
   JSObject                             *obj_edf;
   void                                 *private_data;

   edec = event;
   private_data = ecore_download_file_data_get(edec->edf);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_edec = JS_NewObject(cx, elixir_class_request("Ecore_Download_Event_Cancel", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_edec, NULL))
     goto on_finish;

   obj_edf = JS_DefineObject(cx, obj_edec, "edf", elixir_class_request("Ecore_Download_File", NULL), NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!elixir_object_register(cx, &obj_edf, edec->edf))
     goto on_error;

   if (!elixir_add_str_prop(cx, obj_edec, "url", edec->url))
     goto on_error;
   if (!elixir_add_int_prop(cx, obj_edec, "status", edec->status))
     goto on_error;

   ecore_event_add(ELIXIR_DOWNLOAD_EVENT_CANCEL,
                   elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_edec), NULL),
                   _ecore_event_func_free,
                   NULL);

 on_error:
   elixir_object_unregister(cx, &obj_edf);
   elixir_object_unregister(cx, &obj_edec);

 on_finish:
   elixir_function_stop(cx);

   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_elixir_download_event_complete_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Download_Event_Complete        *edec;
   JSContext                            *cx;
   JSObject                             *obj_edec;
   JSObject                             *obj_edf;
   void                                 *private_data;

   edec = event;
   private_data = ecore_download_file_data_get(edec->edf);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_edec = JS_NewObject(cx, elixir_class_request("Ecore_Download_Event_Complete", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_edec, NULL))
     goto on_finish;

   obj_edf = JS_DefineObject(cx, obj_edec, "edf", elixir_class_request("Ecore_Download_File", NULL), NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!elixir_object_register(cx, &obj_edf, edec->edf))
     goto on_error;

   if (!elixir_add_str_prop(cx, obj_edec, "file", edec->file))
     goto on_error;
   if (!elixir_add_int_prop(cx, obj_edec, "status", edec->status))
     goto on_error;

   ecore_event_add(ELIXIR_DOWNLOAD_EVENT_COMPLETE,
                   elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_edec), NULL),
                   _ecore_event_func_free,
                   NULL);

 on_error:
   elixir_object_unregister(cx, &obj_edf);
   elixir_object_unregister(cx, &obj_edec);

 on_finish:
   elixir_function_stop(cx);

   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_elixir_download_event_progress_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Download_Event_Progress        *edep;
   JSContext                            *cx;
   JSObject                             *obj_edep;
   JSObject                             *obj_edf;
   void                                 *private_data;

   edep = event;
   private_data = ecore_download_file_data_get(edep->edf);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_edep = JS_NewObject(cx, elixir_class_request("Ecore_Download_Event_Progress", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_edep, NULL))
     goto on_finish;

   obj_edf = JS_NewObject(cx, elixir_class_request("Ecore_Download_File", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_edf, edep->edf))
     goto on_error;

   if (!elixir_add_object_prop(cx, obj_edep, "edf", obj_edf))
     goto on_error;

   if (!elixir_add_str_prop(cx, obj_edep, "url", edep->url))
     goto on_error;
/*    if (!elixir_add_dbl_prop(cx, obj_edep, "total", edep->total)) */
/*      goto on_error; */
/*    if (!elixir_add_dbl_prop(cx, obj_edep, "now", edep->now)) */
/*      goto on_error; */

   ecore_event_add(ELIXIR_DOWNLOAD_EVENT_PROGRESS,
                   elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_edep), NULL),
                   _ecore_event_func_free,
                   NULL);

 on_error:
   elixir_object_unregister(cx, &obj_edf);
   elixir_object_unregister(cx, &obj_edep);

 on_finish:
   elixir_function_stop(cx);

   return ECORE_CALLBACK_DONE;
}

static JSBool
elixir_ecore_download_init(JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_TRUE;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ecore_download_init()));

   if (++_ecore_download_init == 1)
     {
        int             i = 0;

        ELIXIR_DOWNLOAD_EVENT_CANCEL = ecore_event_type_new();
        ELIXIR_DOWNLOAD_EVENT_COMPLETE = ecore_event_type_new();
        ELIXIR_DOWNLOAD_EVENT_PROGRESS = ecore_event_type_new();

        eeh_elixir_download_event[0] = ecore_event_handler_add(ECORE_DOWNLOAD_EVENT_CANCEL, _elixir_download_event_cancel_cb, NULL);
        eeh_elixir_download_event[1] = ecore_event_handler_add(ECORE_DOWNLOAD_EVENT_COMPLETE, _elixir_download_event_complete_cb, NULL);
        eeh_elixir_download_event[2] = ecore_event_handler_add(ECORE_DOWNLOAD_EVENT_PROGRESS, _elixir_download_event_progress_cb, NULL);

        while (ecore_download_properties[i].name != NULL)
          {
             jsval      property;

             JS_DeleteProperty(cx, JS_THIS_OBJECT(cx, vp), ecore_download_properties[i].name);
             property = INT_TO_JSVAL(*ecore_download_properties[i].value);
             JS_DefineProperty(cx, JS_THIS_OBJECT(cx, vp),
                               ecore_download_properties[i].name,
                               property,
                               NULL, NULL,
                               JSPROP_ENUMERATE | JSPROP_READONLY);
             ++i;
          }
     }

   return JS_TRUE;
}

static JSBool
elixir_ecore_download_shutdown(JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ecore_download_shutdown()));

   if (--_ecore_download_init == 0)
     {
        unsigned int    i;

        for (i = 0; i < sizeof(eeh_elixir_download_event) / sizeof (Ecore_Event_Handler*); ++i)
          ecore_event_handler_del(eeh_elixir_download_event[i]);

        for (i = 0; ecore_download_properties[i].name != NULL; ++i)
          JS_DeleteProperty(cx, JS_THIS_OBJECT(cx, vp), ecore_download_properties[i].name);
     }

   return JS_TRUE;
}

static JSBool
elixir_ecore_download_directory_new(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_Dir *edd;
   const char *prefix;
   char *files;
   char *temp;
   char *backup;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _3string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   files = elixir_file_canonicalize(elixir_get_string_bytes(val[0].v.str, NULL));
   temp = elixir_file_canonicalize(elixir_get_string_bytes(val[1].v.str, NULL));
   backup = strdupa(elixir_get_string_bytes(val[2].v.str, NULL));
   prefix = basename(backup);
   if (!prefix || strlen(prefix) == 0) prefix = "elixir-XXXXXX";

   edd = ecore_download_directory_new(files, temp, prefix);

   free(files);
   free(temp);

   elixir_return_ptr(cx, vp, edd, elixir_class_request("Ecore_Download_Dir", NULL));
   return JS_TRUE;
}

static JSBool
elixir_ecore_download_directory_destroy(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_Dir *edd;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_download_dir_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, edd);

   ecore_download_directory_destroy(edd);

   return JS_TRUE;
}

static JSBool
elixir_ecore_download_directory_data_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_Dir *edd;
   void *data;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_download_dir_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, edd);
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[1].v.any, NULL);

   elixir_void_free(ecore_download_directory_data_get(edd));
   ecore_download_directory_data_set(edd, data);

   return JS_TRUE;
}

static JSBool
elixir_ecore_download_directory_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_Dir *edd;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_download_dir_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, edd);

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(ecore_download_directory_data_get(edd)));
   return JS_TRUE;
}

static JSBool
elixir_ecore_download_file_add(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_Dir *dir;
   Ecore_Download_File *edf;
   const char *file;
   const char *url;
   char *backup;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_download_dir_2string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, dir);
   backup = strdupa(elixir_get_string_bytes(val[1].v.str, NULL));
   file = basename(backup);
   url = elixir_get_string_bytes(val[2].v.str, NULL);

   edf = ecore_download_file_add(dir, file, url);
   if (edf != NULL)
     ecore_download_file_data_set(edf, elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), JSVAL_NULL, NULL));

   elixir_return_ptr(cx, vp, edf, elixir_class_request("Ecore_Download_File", NULL));
   return JS_TRUE;
}

static JSBool
elixir_edfs_iedf(int (*func)(Ecore_Download_File *edf),
                 JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_File *edf;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_download_file_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, edf);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(edf)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_download_file_start, elixir_edfs_iedf);
FAST_CALL_PARAMS(ecore_download_file_stop, elixir_edfs_iedf);

static JSBool
elixir_ecore_download_file_destroy(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_File *edf;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_download_file_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, edf);

   elixir_void_free(ecore_download_file_data_get(edf));
   ecore_download_file_destroy(edf);

   return JS_TRUE;
}

static JSBool
elixir_ecore_download_file_data_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_File *edf;
   void *data;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_download_file_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, edf);
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[1].v.any, NULL);

   elixir_void_free(ecore_download_file_data_get(edf));
   ecore_download_file_data_set(edf, data);

   return JS_TRUE;
}

static JSBool
elixir_ecore_download_file_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Download_File *edf;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_download_file_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, edf);
   data = ecore_download_file_data_get(edf);

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(data));
   return JS_TRUE;
}

static JSBool
elixir_mkdir(JSContext *cx, uintN argc, jsval *vp)
{
   const char *path;
   Eina_Bool result = EINA_FALSE;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   path = elixir_get_string_bytes(val[0].v.str, NULL);
   if (path)
     if (mkdir(path, S_IRUSR | S_IWUSR | S_IXUSR
	       | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0)
       result = EINA_TRUE;

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(result));
   return JS_TRUE;
}

static JSBool
elixir_chdir(JSContext *cx, uintN argc, jsval *vp)
{
   const char *path;
   Eina_Bool result = EINA_FALSE;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   path = elixir_get_string_bytes(val[0].v.str, NULL);
   if (path)
     if (chdir(path) == 0)
       result = EINA_TRUE;

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(result));
   return JS_TRUE;
}

static JSFunctionSpec ecore_download_functions[];

static JSBool
elixir_ecore_download_no_reuse(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *parent;
   JSObject *func;
   jsval jsfn;
   unsigned int i;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   jsfn = JS_CALLEE(cx, vp);
   func = JSVAL_TO_OBJECT(jsfn);
   parent = JS_GetParent(cx, func);

   i = 0;
   while (ecore_download_functions[i].name != NULL)
     JS_DeleteProperty(cx, parent, ecore_download_functions[i++].name);

   return JS_TRUE;
}

static JSFunctionSpec           ecore_download_functions[] = {
  ELIXIR_FN(ecore_download_init, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_shutdown, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_directory_new, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_directory_destroy, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_directory_data_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_directory_data_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_file_add, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_file_start, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_file_stop, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_file_destroy, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_file_data_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_file_data_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(mkdir, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(chdir, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_download_no_reuse, 1, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp;

   if (em->data)
     return EINA_TRUE;

   if (_no_reuse)
     return EINA_FALSE;

   em->data = parent;
   tmp = &em->data;
   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), ecore_download_functions))
     goto on_error;

   _ecore_download_file_parameter.class = elixir_class_request("Ecore_Download_File", NULL);
   _ecore_download_dir_parameter.class = elixir_class_request("Ecore_Download_Dir", NULL);

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

   elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   if (_no_reuse)
     return EINA_TRUE;

   i = 0;
   while (ecore_download_functions[i].name != NULL)
     JS_DeleteProperty(cx, parent, ecore_download_functions[i++].name);

   return EINA_TRUE;
}

static const Elixir_Module_Api  module_api_elixir = {
  ELIXIR_MODULE_API_VERSION,
  ELIXIR_AUTH_REQUIRED,
  "ecore-download",
  "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_ecore_download = {
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
ecore_download_binding_init(void)
{
   return elixir_modules_register(&em_ecore_download);
}

void
ecore_download_binding_shutdown(void)
{
   elixir_modules_unregister(&em_ecore_download);
}

#ifndef EINA_STATIC_BUILD_ECORE_DOWNLOAD
EINA_MODULE_INIT(ecore_download_binding_init);
EINA_MODULE_SHUTDOWN(ecore_download_binding_shutdown);
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eio.h>

#include "Elixir.h"

static elixir_parameter_t _eio_file_parameter = {
  "Eio_File", JOBJECT, NULL
};

static elixir_parameter_t _eina_stat_parameter = {
  "Eina_Stat", JOBJECT, NULL
};

static elixir_parameter_t _eina_direct_parameter = {
  "Eina_Direct", JOBJECT, NULL
};

static elixir_parameter_t _struct_stat_parameter = {
  "struct stat", JOBJECT, NULL
};

static const elixir_parameter_t* _string_4func_any[7] = {
  &string_parameter,
  &function_parameter,
  &function_parameter,
  &function_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};

static const elixir_parameter_t* _string_2func_any[5] = {
  &string_parameter,
  &function_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};

static const elixir_parameter_t* _string_int_2func_any[6] = {
  &string_parameter,
  &int_parameter,
  &function_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};

static const elixir_parameter_t* _3string_2func_any[7] = {
  &string_parameter,
  &string_parameter,
  &string_parameter,
  &function_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};

static const elixir_parameter_t* _2string_3func_any[7] = {
  &string_parameter,
  &string_parameter,
  &function_parameter,
  &function_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};

static const elixir_parameter_t* _string_3func_any[6] = {
  &string_parameter,
  &function_parameter,
  &function_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};

static const elixir_parameter_t* _struct_stat_params[2] = {
  &_struct_stat_parameter,
  NULL
};

static const elixir_parameter_t* _eio_file_params[2] = {
  &_eio_file_parameter,
  NULL
};

FAST_CALL_PARAMS(eio_init, elixir_int_params_void);
FAST_CALL_PARAMS(eio_shutdown, elixir_int_params_void);

typedef struct _Elixir_EIO_Data Elixir_EIO_Data;
struct _Elixir_EIO_Data
{
   JSFunction *func_filter;
   JSFunction *func_main;
   JSFunction *func_done;
   JSFunction *func_error;
   Elixir_Runtime *runtime;
   JSObject *obj_filter;
   JSObject *obj_main;
   JSObject *obj_done;
   JSObject *obj_error;
   JSContext *main;
   JSObject *result;
};

#define ADDIPROP(Cx, Obj, Info, Name)                   \
  elixir_add_int_prop(Cx, Obj, #Name, Info->Name);

static Eina_Bool
elixir_new_eina_direct_info(JSContext *cx,
                            const Eina_File_Direct_Info *info,
                            jsval *rval)
{
   JSObject *obj;
   jsval propertie;
   char *tmp;
   const char *type = NULL;
   Eina_Bool ret = EINA_FALSE;

   tmp = elixir_file_canonicalize(info->path);
   if (tmp) type = elixir_file_type(tmp);
   if (!info || !tmp || !type)
     {
        *rval = JSVAL_NULL;
        return EINA_FALSE;
     }

   elixir_lock_cx(cx);

   obj = JS_NewObject(cx, elixir_class_request("Eina_File_Direct_Info", NULL), NULL, NULL);
   if (!obj) goto on_error;

   if (!elixir_object_register(cx, &obj, NULL)) goto on_error;

   *rval = OBJECT_TO_JSVAL(obj);

   /* build eina_file_direct_info content */
   ADDIPROP(cx, obj, info, path_length);
   ADDIPROP(cx, obj, info, name_length);
   ADDIPROP(cx, obj, info, name_start);
   ADDIPROP(cx, obj, info, type);

   propertie = STRING_TO_JSVAL(elixir_dup(cx, tmp));
   JS_SetProperty(cx, obj, "path", &propertie);

   /* add an elixir specific content */
   propertie = STRING_TO_JSVAL(elixir_dup(cx, type));
   JS_SetProperty(cx, obj, "type", &propertie);

   ret = EINA_TRUE;

   elixir_object_unregister(cx, &obj);

 on_error:
   elixir_unlock_cx(cx);

   free(tmp);
   return ret;
}

static Eina_Bool
elixir_new_stat(JSContext *cx,
                const struct stat *st,
                jsval *rval)
{
   JSObject *obj;
   double tmp;
   Eina_Bool ret = EINA_FALSE;

   elixir_lock_cx(cx);

   obj = JS_NewObject(cx, elixir_class_request("struct stat", NULL), NULL, NULL);
   if (!obj) goto on_error;

   if (!elixir_object_register(cx, &obj, (struct stat*) st)) goto on_error;

   *rval = OBJECT_TO_JSVAL(obj);

   ADDIPROP(cx, obj, st, st_dev);
   ADDIPROP(cx, obj, st, st_ino);
   ADDIPROP(cx, obj, st, st_mode);
   ADDIPROP(cx, obj, st, st_nlink);
   ADDIPROP(cx, obj, st, st_uid);
   ADDIPROP(cx, obj, st, st_rdev);

   tmp = (double) st->st_size;
   elixir_add_dbl_prop(cx, obj, "st_size", tmp);

   ret = EINA_TRUE;
   elixir_object_unregister(cx, &obj);

 on_error:
   elixir_unlock_cx(cx);

   return ret;
}

static Eina_Bool
_eio_jsval_to_boolean(JSContext *cx, jsval val)
{
   Eina_Bool ret = EINA_FALSE;

   if (JSVAL_IS_BOOLEAN(val))
     {
	ret = JSVAL_TO_BOOLEAN(val);
     }
   else if (JSVAL_IS_INT(val)
	    || JSVAL_IS_STRING(val))
     {
	int tmp;

        if (JS_ValueToInt32(cx, val, &tmp) == JS_FALSE)
          tmp = EINA_FALSE;

	ret = tmp ? EINA_TRUE : EINA_FALSE;
     }
   else if (JSVAL_IS_DOUBLE(val))
     {
        jsdouble        dbl;

        if (JS_ValueToNumber(cx, val, &dbl) == JS_TRUE)
	  ret = dbl;
     }

   return ret;
}

static Eina_Bool
_elixir_eio_filter_direct_cb(void *data, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   Elixir_EIO_Data *dt;
   JSContext *cx;
   JSObject *parent;
   jsval argv[3];
   jsval rval;
   Eina_Bool result = EINA_FALSE;

   cx = elixir_void_get_cx(data);
   if (!cx) return EINA_FALSE;

   elixir_lock_cx(cx);

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);

   if (!dt->func_filter) goto on_error;
   if (!parent || !dt) goto on_error;

   argv[0] = elixir_void_get_jsval(data);
   elixir_return_ptr(cx, &argv[1], handler, elixir_class_request("Eio_File", NULL));
   if (!elixir_new_eina_direct_info(cx, info, &argv[2])) goto on_error;
   rval = JSVAL_VOID;

   if (elixir_function_run(cx, dt->func_filter, parent, 3, argv, &rval))
     result = _eio_jsval_to_boolean(cx, rval);

 on_error:
   elixir_unlock_cx(cx);
   return result;
}

static void
_elixir_eio_main_direct_cb(void *data, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   Elixir_EIO_Data *dt;
   JSContext *cx;
   JSObject *parent;
   jsval argv[3];
   jsval rval;

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);
   if (!parent || !dt)
     return ;

   cx = dt->main;

   elixir_function_start(cx);

   argv[0] = elixir_void_get_jsval(data);
   argv[1] = OBJECT_TO_JSVAL(dt->result);
   if (!elixir_new_eina_direct_info(cx, info, &argv[2])) goto on_error;
   rval = JSVAL_VOID;

   elixir_function_run(cx, dt->func_main, parent, 3, argv, &rval);

 on_error:
   elixir_function_stop(cx);
}

static Eina_Bool
_elixir_eio_filter_cb(void *data, Eio_File *handler __UNUSED__, const char *file)
{
   Elixir_EIO_Data *dt;
   JSContext *cx;
   JSObject *parent;
   JSString *str;
   const char *type;
   char *tmp = NULL;
   jsval argv[2];
   jsval rval;
   Eina_Bool result = EINA_FALSE;

   if (eina_stringshare_strlen(file) > 4096) return EINA_FALSE;

   tmp = elixir_file_canonicalize(file);
   if (!tmp) return EINA_FALSE;

   type = elixir_file_type(tmp);
   if (!type) return EINA_FALSE;

   cx = elixir_void_get_cx(data);
   if (!cx)
     {
        free(tmp);
        return EINA_FALSE;
     }

   elixir_lock_cx(cx);

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);

   if (!dt->func_filter) goto on_error;

   if (!parent || !dt) goto on_error;

   str = elixir_dup(cx, tmp);

   argv[0] = elixir_void_get_jsval(data);
   /* To avoid race condition and other nasty cross thread problem */
   elixir_return_ptr(cx, &argv[1], handler, elixir_class_request("Eio_File", NULL));
   argv[2] = STRING_TO_JSVAL(str);
   rval = JSVAL_VOID;

   if (elixir_function_run(cx, dt->func_filter, parent, 3, argv, &rval))
     result = _eio_jsval_to_boolean(cx, rval);

 on_error:
   elixir_unlock_cx(cx);

   free(tmp);
   return result;
}

static void
_elixir_eio_main_cb(void *data, Eio_File *handler __UNUSED__, const char *file)
{
   Elixir_EIO_Data *dt;
   JSString *str;
   JSContext *cx;
   JSObject *parent;
   char *canon;
   jsval argv[3];
   jsval rval;

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);
   if (!parent || !dt)
     return ;

   canon = elixir_file_canonicalize(file);
   if (!canon) return ;

   cx = dt->main;

   elixir_function_start(cx);

   str = elixir_dup(cx, canon);

   argv[0] = elixir_void_get_jsval(data);
   argv[1] = OBJECT_TO_JSVAL(dt->result);
   argv[2] = STRING_TO_JSVAL(str);
   rval = JSVAL_VOID;

   elixir_function_run(cx, dt->func_main, parent, 3, argv, &rval);

   elixir_function_stop(cx);
}

static void
_elixir_eio_data_free(Elixir_EIO_Data *dt)
{
   elixir_object_unregister(dt->runtime->cx, &dt->obj_filter);
   elixir_object_unregister(dt->runtime->cx, &dt->obj_main);
   elixir_object_unregister(dt->runtime->cx, &dt->obj_done);
   elixir_object_unregister(dt->runtime->cx, &dt->obj_error);

   if (dt->result)
     {
        JS_SetPrivate(dt->runtime->cx, dt->result, NULL);
        elixir_object_unregister(dt->runtime->cx, &dt->result);
     }

   if (dt->main)
     {
        elixir_function_stop(dt->main);
        elixir_function_gc(dt->main);
     }

   elixir_shutdown(dt->runtime);
   free(dt);
}

static void
_elixir_eio_done_cb(void *data, Eio_File *handler __UNUSED__)
{
   Elixir_EIO_Data *dt;
   JSContext *cx;
   JSObject *parent;
   jsval argv[2];
   jsval rval;

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);
   if (!parent || !dt)
     return ;

   cx = dt->main;

   elixir_function_start(cx);

   JS_SetContextThread(dt->runtime->cx);

   argv[0] = elixir_void_get_jsval(data);
   argv[1] = OBJECT_TO_JSVAL(dt->result);
   rval = JSVAL_VOID;

   elixir_function_run(cx, dt->func_done, parent, 2, argv, &rval);

   elixir_void_free(data);

   _elixir_eio_data_free(dt);
   elixir_thread_del();
}

static void
_elixir_eio_error_cb(void *data, Eio_File *handler __UNUSED__, int error)
{
   Elixir_EIO_Data *dt;
   JSContext *cx;
   JSObject *parent;
   jsval argv[3];
   jsval rval;

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);
   if (!parent || !dt)
     return ;

   cx = dt->main;

   elixir_function_start(cx);

   JS_SetContextThread(dt->runtime->cx);

   argv[2] = INT_TO_JSVAL(error);
   argv[0] = elixir_void_get_jsval(data);
   argv[1] = OBJECT_TO_JSVAL(dt->result);
   rval = JSVAL_VOID;

   elixir_function_run(cx, dt->func_error, parent, 3, argv, &rval);

   _elixir_eio_data_free(dt);

   elixir_void_free(data);
   elixir_thread_del();
}

static JSBool
elixir_eio_file_ls(JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_EIO_Data *dt;
   Eio_File *result;
   void *new;
   elixir_value_t val[6];

   if (!elixir_params_check(cx, _string_4func_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   dt = malloc(sizeof (Elixir_EIO_Data));
   if (!dt) return JS_FALSE;

   dt->runtime = elixir_clone(cx, JS_THIS_OBJECT(cx, vp));
   if (!dt->runtime)
     {
        free(dt);
        return JS_FALSE;
     }

   dt->func_filter = val[1].v.fct;
   dt->func_main = val[2].v.fct;
   dt->func_done = val[3].v.fct;
   dt->func_error = val[4].v.fct;
   dt->obj_filter = JS_GetFunctionObject(dt->func_filter);
   dt->obj_main = JS_GetFunctionObject(dt->func_main);
   dt->obj_done = JS_GetFunctionObject(dt->func_done);
   dt->obj_error = JS_GetFunctionObject(dt->func_error);
   dt->main = cx;

   elixir_object_register(dt->runtime->cx, &dt->obj_filter, NULL);
   elixir_object_register(dt->runtime->cx, &dt->obj_main, NULL);
   elixir_object_register(dt->runtime->cx, &dt->obj_done, NULL);
   elixir_object_register(dt->runtime->cx, &dt->obj_error, NULL);

   if (!JS_GetParent(cx, dt->obj_filter))
     JS_SetParent(cx, dt->obj_filter, JS_THIS_OBJECT(cx, vp));

   new = elixir_void_new(dt->runtime->cx, JS_THIS_OBJECT(cx, vp), val[5].v.any, dt);
   if (!new)
     {
        dt->main = NULL;
        _elixir_eio_data_free(dt);
        return JS_FALSE;
     }

   elixir_function_stop(cx);

   elixir_thread_new();
   result = eio_file_ls(elixir_get_string_bytes(val[0].v.str, NULL),
                        _elixir_eio_filter_cb,
                        _elixir_eio_main_cb,
                        _elixir_eio_done_cb,
                        _elixir_eio_error_cb,
                        new);

   elixir_function_start(cx);

   if (result)
     {
        dt->result = elixir_return_ptr(cx, vp, result, elixir_class_request("Eio_File", NULL));
        elixir_object_register(cx, &dt->result, NULL);
     }
   else
     {
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
     }
   return JS_TRUE;
}

static JSBool
elixir_eio_file_any_ls(Eio_File *(*func)(const char *dir,
                                         Eio_Filter_Direct_Cb filter_cb,
                                         Eio_Main_Direct_Cb main_cb,
                                         Eio_Done_Cb done_cb,
                                         Eio_Error_Cb error_cb,
                                         const void *data),
                       JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_EIO_Data *dt;
   Eio_File *result;
   void *new;
   elixir_value_t val[6];

   if (!elixir_params_check(cx, _string_4func_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   dt = malloc(sizeof (Elixir_EIO_Data));
   if (!dt) return JS_FALSE;

   dt->runtime = elixir_clone(cx, JS_THIS_OBJECT(cx, vp));
   if (!dt->runtime)
     {
        free(dt);
        return JS_FALSE;
     }

   dt->func_filter = val[1].v.fct;
   dt->func_main = val[2].v.fct;
   dt->func_done = val[3].v.fct;
   dt->func_error = val[4].v.fct;
   dt->obj_filter = JS_GetFunctionObject(dt->func_filter);
   dt->obj_main = JS_GetFunctionObject(dt->func_main);
   dt->obj_done = JS_GetFunctionObject(dt->func_done);
   dt->obj_error = JS_GetFunctionObject(dt->func_error);
   dt->main = cx;

   elixir_object_register(dt->runtime->cx, &dt->obj_filter, NULL);
   elixir_object_register(dt->runtime->cx, &dt->obj_main, NULL);
   elixir_object_register(dt->runtime->cx, &dt->obj_done, NULL);
   elixir_object_register(dt->runtime->cx, &dt->obj_error, NULL);

   if (!JS_GetParent(cx, dt->obj_filter))
     JS_SetParent(cx, dt->obj_filter, JS_THIS_OBJECT(cx, vp));

   new = elixir_void_new(dt->runtime->cx, JS_THIS_OBJECT(cx, vp), val[5].v.any, dt);
   if (!new)
     {
        dt->main = NULL;
        _elixir_eio_data_free(dt);
        return JS_FALSE;
     }

   elixir_function_stop(cx);

   elixir_thread_new();
   result = func(elixir_get_string_bytes(val[0].v.str, NULL),
                 _elixir_eio_filter_direct_cb,
                 _elixir_eio_main_direct_cb,
                 _elixir_eio_done_cb,
                 _elixir_eio_error_cb,
                 new);

   elixir_function_start(cx);

   if (result)
     {
        dt->result = elixir_return_ptr(cx, vp, result, elixir_class_request("Eio_File", NULL));
        elixir_object_register(cx, &dt->result, NULL);
     }
   else
     {
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
     }
   return JS_TRUE;
}

FAST_CALL_PARAMS(eio_file_direct_ls, elixir_eio_file_any_ls);
FAST_CALL_PARAMS(eio_file_stat_ls, elixir_eio_file_any_ls);

static JSBool
elixir_eio_filter_add(JSContext *cx, uintN argc, jsval *vp)
{
   const char     *filter;
   elixir_value_t  val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   filter = elixir_get_string_bytes(val[0].v.str, NULL);
   elixir_filter_register(filter);

   return JS_TRUE;
}

static JSBool
elixir_eio_filter_del(JSContext *cx, uintN argc, jsval *vp)
{
   const char     *filter;
   elixir_value_t  val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   filter = elixir_get_string_bytes(val[0].v.str, NULL);
   elixir_filter_unregister(filter);

   return JS_TRUE;
}

typedef struct _Elixir_EIO_Stat Elixir_EIO_Stat;
struct _Elixir_EIO_Stat
{
   JSFunction *func_done;
   JSFunction *func_error;
   JSObject *obj_done;
   JSObject *obj_error;
   JSObject *result;
};

static void
_elixir_eio_stat_free(JSContext *cx, Elixir_EIO_Stat *st)
{
   elixir_object_unregister(cx, &st->obj_done);
   elixir_object_unregister(cx, &st->obj_error);

   JS_SetPrivate(cx, st->result, NULL);
   elixir_object_unregister(cx, &st->result);

   elixir_function_stop(cx);
   free(st);
}

static void
_elixir_eio_file_error_cb(void *data, Eio_File *handler __UNUSED__, int error)
{
   Elixir_EIO_Stat *st;
   JSContext *cx;
   JSObject *parent;
   jsval argv[3];
   jsval rval;

   parent = elixir_void_get_parent(data);
   cx = elixir_void_get_cx(data);
   st = elixir_void_get_private(data);
   if (!parent || !st || !cx)
     return ;

   elixir_function_start(cx);

   argv[2] = INT_TO_JSVAL(error);
   argv[0] = elixir_void_get_jsval(data);
   argv[1] = OBJECT_TO_JSVAL(st->result);

   elixir_function_run(cx, st->func_error, parent, 2, argv, &rval);

   elixir_void_free(data);
   _elixir_eio_stat_free(cx, st);
}

static void
_elixir_eio_direct_stat_done(void *data, Eio_File *handler __UNUSED__, const struct stat *stat)
{
   Elixir_EIO_Stat *st;
   JSContext *cx;
   JSObject *parent;
   jsval argv[3];
   jsval rval;

   parent = elixir_void_get_parent(data);
   cx = elixir_void_get_cx(data);
   st = elixir_void_get_private(data);
   if (!parent || !st || !cx)
     return ;

   elixir_function_start(cx);

   if (!elixir_new_stat(cx, stat, &argv[2]))
     goto on_error;
   argv[0] = elixir_void_get_jsval(data);
   argv[1] = OBJECT_TO_JSVAL(st->result);

   elixir_function_run(cx, st->func_done, parent, 3, argv, &rval);

   elixir_void_free(data);
   _elixir_eio_stat_free(cx, st);

   return ;

 on_error:
   elixir_function_stop(cx);

   _elixir_eio_file_error_cb(data, handler, 0);
}

static JSBool
elixir_eio_file_direct_stat(JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_EIO_Stat *st = NULL;
   Eio_File *result;
   char *tmp = NULL;
   void *n;
   jsval argv[3];
   jsval rval;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _string_2func_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   tmp = elixir_file_canonicalize(elixir_get_string_bytes(val[0].v.str, NULL));
   if (tmp && !elixir_file_type(tmp))
     {
        free(tmp);
        tmp = NULL;
     }

   if (!tmp) goto on_error;

   st = malloc(sizeof (Elixir_EIO_Stat));
   if (!st) goto on_error;

   st->func_done = val[1].v.fct;
   st->func_error = val[2].v.fct;
   st->obj_done = JS_GetFunctionObject(st->func_done);
   st->obj_error = JS_GetFunctionObject(st->func_error);

   elixir_object_register(cx, &st->obj_done, NULL);
   elixir_object_register(cx, &st->obj_error, NULL);

   n = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, st);
   if (!n) goto on_error;

   elixir_function_stop(cx);
   result = eio_file_direct_stat(tmp,
                                 _elixir_eio_direct_stat_done,
                                 _elixir_eio_file_error_cb,
                                 n);
   elixir_function_start(cx);

   if (result)
     {
        st->result = elixir_return_ptr(cx, vp, result, elixir_class_request("Eio_File", NULL));
        elixir_object_register(cx, &st->result, NULL);
     }
   else
     {
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
     }

   free(tmp);

   return JS_TRUE;

 on_error:
   if (tmp) free(tmp);
   if (st)
     {
        elixir_object_unregister(cx, &st->obj_done);
        elixir_object_unregister(cx, &st->obj_error);
        free(st);
     }

   argv[2] = INT_TO_JSVAL(0);
   argv[1] = JSVAL_NULL;
   argv[0] = val[3].v.any;

   elixir_function_run(cx, val[2].v.fct, JS_THIS_OBJECT(cx, vp), 3, argv, &rval);

   JS_SET_RVAL(cx, vp, JSVAL_NULL);
   return JS_TRUE;
}

static JSBool
elixir_double_eio_stat(double (*func)(const struct stat *stat),
                       JSContext *cx, uintN argc, jsval *vp)
{
   const struct stat *stat;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _struct_stat_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stat);

   return JS_NewNumberValue(cx, func(stat), &(JS_RVAL(cx, vp)));
}

FAST_CALL_PARAMS(eio_file_atime, elixir_double_eio_stat);
FAST_CALL_PARAMS(eio_file_mtime, elixir_double_eio_stat);

static JSBool
elixir_eio_file_size(JSContext *cx, uintN argc, jsval *vp)
{
   const struct stat *stat;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _struct_stat_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stat);

   return JS_NewNumberValue(cx, eio_file_size(stat), &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_boolean_eio_stat(Eina_Bool (*func)(const struct stat *stat),
                        JSContext *cx, uintN argc, jsval *vp)
{
   const struct stat *stat;
   Eina_Bool ret;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _struct_stat_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stat);
   ret = func(stat);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ret));
   return JS_TRUE;
}

FAST_CALL_PARAMS(eio_file_is_dir, elixir_boolean_eio_stat);
FAST_CALL_PARAMS(eio_file_is_lnk, elixir_boolean_eio_stat);

static void
_elixir_eio_file_done_cb(void *data, Eio_File *handler)
{
  Elixir_EIO_Stat *st;
  JSContext *cx;
  JSObject *parent;
  jsval argv[2];
  jsval rval;

  parent = elixir_void_get_parent(data);
  cx = elixir_void_get_cx(data);
  st = elixir_void_get_private(data);
  if (!parent || !st || !cx)
    return ;

  elixir_function_start(cx);
  argv[0] = elixir_void_get_jsval(data);
  argv[1] = OBJECT_TO_JSVAL(st->result);

  elixir_function_run(cx, st->func_error, parent, 2, argv, &rval);

  elixir_void_free(data);
  _elixir_eio_stat_free(cx, st);
}

static JSBool
elixir_eio_file_unlink(JSContext *cx, uintN argc, jsval *vp)
{
  Elixir_EIO_Stat *st = NULL;
  Eio_File *result;
  char *tmp;
  void *n;
  jsval argv[3];
  jsval rval;
  elixir_value_t val[4];

  if (!elixir_params_check(cx, _string_2func_any, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  tmp = elixir_file_canonicalize(elixir_get_string_bytes(val[0].v.str, NULL));
  if (tmp && !elixir_file_type(tmp))
    {
      free(tmp);
      tmp = NULL;
    }

  if (!tmp) goto on_error;

  st = malloc(sizeof (Elixir_EIO_Stat));
  if (!st) goto on_error;

  st->func_done = val[1].v.fct;
  st->func_error = val[2].v.fct;
  st->obj_done = JS_GetFunctionObject(st->func_done);
  st->obj_error = JS_GetFunctionObject(st->func_error);

  elixir_object_register(cx, &st->obj_done, NULL);
  elixir_object_register(cx, &st->obj_error, NULL);

  n = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, st);
  if (!n) goto on_error;

  elixir_function_stop(cx);
  result = eio_file_unlink(tmp,
			   _elixir_eio_file_done_cb,
			   _elixir_eio_file_error_cb,
			   n);
  elixir_function_start(cx);

  if (result)
    {
      st->result = elixir_return_ptr(cx, vp, result, elixir_class_request("Eio_File", NULL));
      elixir_object_register(cx, &st->result, NULL);
    }
  else
    {
      JS_SET_RVAL(cx, vp, JSVAL_NULL);
    }

  free(tmp);

  return JS_TRUE;

 on_error:
  if (tmp) free(tmp);
  if (st)
    {
      elixir_object_unregister(cx, &st->obj_done);
      elixir_object_unregister(cx, &st->obj_error);
      free(st);
    }

  argv[2] = INT_TO_JSVAL(0);
  argv[1] = JSVAL_NULL;
  argv[0] = val[3].v.any;

  elixir_function_run(cx, val[2].v.fct, JS_THIS_OBJECT(cx, vp), 3, argv, &rval);

  JS_SET_RVAL(cx, vp, JSVAL_NULL);
  return JS_TRUE;
}

static JSBool
elixir_eio_file_cancel(JSContext *cx, uintN argc, jsval *vp)
{
  Eio_File *ls;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, _eio_file_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, ls);

  JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(eio_file_cancel(ls)));
  return JS_TRUE;
}

static JSFunctionSpec eio_functions[] = {
  ELIXIR_FN(eio_init, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_shutdown, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_direct_stat, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_ls, 6, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_direct_ls, 6, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_stat_ls, 6, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_filter_add, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_filter_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_atime, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_mtime, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_size, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_is_dir, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_is_lnk, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_unlink, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(eio_file_cancel, 1, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

static const struct {
   const char *name;
   int value;
} eio_const_properties[] = {
  { "EIO_FILE_COPY", EIO_FILE_COPY },
  { "EIO_FILE_MOVE", EIO_FILE_MOVE },
  { "EIO_DIR_COPY", EIO_DIR_COPY },
  { "EIO_DIR_MOVE", EIO_DIR_MOVE },
  { "EIO_UNLINK", EIO_UNLINK },
  { "EIO_FILE_GETPWNAM", EIO_FILE_GETPWNAM },
  { "EIO_FILE_GETGRNAM", EIO_FILE_GETGRNAM },
  { "EINA_FILE_UNKNOWN", EINA_FILE_UNKNOWN },
  { "EINA_FILE_FIFO", EINA_FILE_FIFO },
  { "EINA_FILE_CHR", EINA_FILE_CHR },
  { "EINA_FILE_DIR", EINA_FILE_DIR },
  { "EINA_FILE_BLK", EINA_FILE_BLK },
  { "EINA_FILE_REG", EINA_FILE_REG },
  { "EINA_FILE_LNK", EINA_FILE_LNK },
  { "EINA_FILE_SOCK", EINA_FILE_SOCK },
  { "EINA_FILE_WHT", EINA_FILE_WHT },
  { NULL, 0 }
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp;
   unsigned int i = 0;
   jsval property;

   if (em->data)
     return EINA_TRUE;

   em->data = parent;
   tmp = &em->data;
   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), eio_functions))
     goto on_error;

   while (eio_const_properties[i].name)
     {
        property = INT_TO_JSVAL(eio_const_properties[i].value);
        if (!JS_DefineProperty(cx, parent,
			       eio_const_properties[i].name,
			       property,
			       NULL, NULL,
			       JSPROP_ENUMERATE | JSPROP_READONLY))
          goto on_error;
        ++i;
     }

   _eio_file_parameter.class = elixir_class_request("Eio_File", NULL);
   _eina_stat_parameter.class = elixir_class_request("Eina_Stat", NULL);
   _eina_direct_parameter.class = elixir_class_request("Eina_Direct", NULL);
   _struct_stat_parameter.class = elixir_class_request("struct stat", NULL);

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
   while (eio_functions[i].name)
     JS_DeleteProperty(cx, parent, eio_functions[i++].name);

   i = 0;
   while (eio_const_properties[i].name)
     JS_DeleteProperty(cx, parent, eio_const_properties[i++].name);

   elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   return EINA_TRUE;
}

static const Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "eio",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_eio = {
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
eio_binding_init(void)
{
   return elixir_modules_register(&em_eio);
}

void
eio_binding_shutdown(void)
{
   elixir_modules_unregister(&em_eio);
}

#ifndef EINA_STATIC_BUILD_EIO
EINA_MODULE_INIT(eio_binding_init);
EINA_MODULE_SHUTDOWN(eio_binding_shutdown);
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "Elixir.h"

Eina_Bool load_sqlite_const_binding(JSContext* cx, JSObject* parent);
Eina_Bool unload_sqlite_const_binding(JSContext* cx, JSObject* parent);

static elixir_parameter_t               _sqlite3_parameter = {
  "SQLite3", JOBJECT, NULL
};
static elixir_parameter_t               _sqlite3_stmt_parameter = {
  "SQLite3_Stmt", JOBJECT, NULL
};
static elixir_parameter_t               _sqlite3_context_parameter = {
  "SQLite3_Context", JOBJECT, NULL
};
static elixir_parameter_t               _sqlite3_value_parameter = {
  "SQLite3_Value", JOBJECT, NULL
};

static const elixir_parameter_t*	_string_int_string[4] = {
  &string_parameter,
  &int_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _sqlite3_stmt_params[2] = {
   &_sqlite3_stmt_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_stmt_int_params[3] = {
   &_sqlite3_stmt_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_value_params[2] = {
   &_sqlite3_value_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_params[2] = {
   &_sqlite3_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_int_params[3] = {
   &_sqlite3_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_string_params[3] = {
   &_sqlite3_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_3string_params[5] = {
   &_sqlite3_parameter,
   &string_parameter,
   &string_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_stmt_string_params[3] = {
   &_sqlite3_stmt_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _double_sqlite3_stmt_params[3] = {
   &_sqlite3_stmt_parameter,
   &_sqlite3_stmt_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_stmt_double_int_params[4] = {
   &_sqlite3_stmt_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_stmt_int_dbl_params[4] = {
   &_sqlite3_stmt_parameter,
   &int_parameter,
   &double_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_stmt_int_string_params[4] = {
   &_sqlite3_stmt_parameter,
   &int_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_context_params[2] = {
   &_sqlite3_context_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_context_int_params[3] = {
   &_sqlite3_context_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_context_dbl_params[3] = {
   &_sqlite3_context_parameter,
   &double_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_context_sqlite3_value_params[3] = {
   &_sqlite3_context_parameter,
   &_sqlite3_value_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_context_string_params[3] = {
   &_sqlite3_context_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_function_any[4] = {
   &_sqlite3_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_string_function_any[5] = {
   &_sqlite3_parameter,
   &string_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_int_function_any[5] = {
   &_sqlite3_parameter,
   &int_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_create_function_scalar[8] = {
   &_sqlite3_parameter,
   &string_parameter,
   &int_parameter,
   &any_parameter,
   &function_parameter,
   &null_parameter,
   &null_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_create_function_aggreagate[8] = {
   &_sqlite3_parameter,
   &string_parameter,
   &int_parameter,
   &any_parameter,
   &null_parameter,
   &function_parameter,
   &function_parameter,
   NULL
};
static const elixir_parameter_t*        _sqlite3_create_function_reset[8] = {
   &_sqlite3_parameter,
   &string_parameter,
   &int_parameter,
   &any_parameter,
   &null_parameter,
   &null_parameter,
   &null_parameter,
   NULL
};

static JSBool
elixir_int_sqlite3_stmt_params(int (*func)(sqlite3_stmt *stmt),
                               JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_stmt_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(stmt)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(sqlite3_bind_parameter_count, elixir_int_sqlite3_stmt_params);
FAST_CALL_PARAMS(sqlite3_clear_bindings, elixir_int_sqlite3_stmt_params);
FAST_CALL_PARAMS(sqlite3_data_count, elixir_int_sqlite3_stmt_params);
FAST_CALL_PARAMS(sqlite3_column_count, elixir_int_sqlite3_stmt_params);
FAST_CALL_PARAMS(sqlite3_expired, elixir_int_sqlite3_stmt_params);
FAST_CALL_PARAMS(sqlite3_finalize, elixir_int_sqlite3_stmt_params);
FAST_CALL_PARAMS(sqlite3_reset, elixir_int_sqlite3_stmt_params);
FAST_CALL_PARAMS(sqlite3_step, elixir_int_sqlite3_stmt_params);

static JSBool
elixir_int_sqlite3_stmt_int_params(int (*func)(sqlite3_stmt *stmt, int n),
                                   JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_stmt_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   value = val[1].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(stmt, value)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(sqlite3_bind_null, elixir_int_sqlite3_stmt_int_params);
FAST_CALL_PARAMS(sqlite3_column_bytes, elixir_int_sqlite3_stmt_int_params);
FAST_CALL_PARAMS(sqlite3_column_int, elixir_int_sqlite3_stmt_int_params);
FAST_CALL_PARAMS(sqlite3_column_type, elixir_int_sqlite3_stmt_int_params);

static JSBool
elixir_char_sqlite3_stmt_int_params(const char* (*func)(sqlite3_stmt *stmt, int n),
                                    JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_stmt_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   value = val[1].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(stmt, value)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(sqlite3_bind_parameter_name, elixir_char_sqlite3_stmt_int_params);
FAST_CALL_PARAMS(sqlite3_column_decltype, elixir_char_sqlite3_stmt_int_params);
FAST_CALL_PARAMS(sqlite3_column_name, elixir_char_sqlite3_stmt_int_params);
FAST_CALL_PARAMS(sqlite3_column_database_name, elixir_char_sqlite3_stmt_int_params);
FAST_CALL_PARAMS(sqlite3_column_origin_name, elixir_char_sqlite3_stmt_int_params);
FAST_CALL_PARAMS(sqlite3_column_table_name, elixir_char_sqlite3_stmt_int_params);

static JSBool
elixir_sqlite3_column_text(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_stmt_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   value = val[1].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_column_text(stmt, value)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_sql(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _sqlite3_stmt_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_sql(stmt)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_column_double(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_stmt_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   value = val[1].v.num;

   return JS_NewNumberValue(cx, sqlite3_column_double(stmt, value), &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_sqlite3_db_handle(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _sqlite3_stmt_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);

   elixir_return_ptr(cx, vp, sqlite3_db_handle(stmt), elixir_class_request("sqlite3", NULL));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_open(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   char *filename;
   const char *tmp;
   size_t length;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx,vp)))
     return JS_FALSE;

   tmp = elixir_get_string_bytes(val[0].v.str, &length);
   if (tmp && length != strlen(tmp))
     return JS_FALSE;
   filename = elixir_file_canonicalize(tmp);

   value = sqlite3_open(filename, &db);
   free(filename);

   if (value == SQLITE_OK)
     elixir_return_ptr(cx, vp, db, elixir_class_request("sqlite3", NULL));
   else
     JS_SET_RVAL(cx, vp, INT_TO_JSVAL(value));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_open_v2(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   const char *tmp;
   char *filename;
   size_t length;
   int value;
   int flags;
   const char *vfs;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _string_int_string, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   tmp = elixir_get_string_bytes(val[0].v.str, &length);
   if (tmp && length != strlen(tmp))
     return JS_FALSE;
   filename = elixir_file_canonicalize(tmp);
   flags = val[1].v.num;
   vfs = elixir_get_string_bytes(val[2].v.str, NULL);

   value = sqlite3_open_v2(filename, &db, flags, vfs);
   free(filename);

   if (value == SQLITE_OK)
     elixir_return_ptr(cx, vp, db, elixir_class_request("sqlite3", NULL));
   else
     JS_SET_RVAL(cx, vp, INT_TO_JSVAL(value));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_bind_parameter_index(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   const char *name;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_stmt_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   name = elixir_get_string_bytes(val[1].v.str, NULL);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_bind_parameter_index(stmt, name)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_transfer_bindings(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt1 = NULL;
   sqlite3_stmt *stmt2 = NULL;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _double_sqlite3_stmt_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt1);
   GET_PRIVATE(cx, val[1].v.obj, stmt2);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_transfer_bindings(stmt1, stmt2)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_bind_double(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   int value_int;
   double value_dbl;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_stmt_int_dbl_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   value_int = val[1].v.num;
   value_dbl = val[2].v.dbl;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_bind_double(stmt, value_int, value_dbl)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_bind_int(JSContext *cx,uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   int value1;
   int value2;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_stmt_double_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   value1 = val[1].v.num;
   value2 = val[2].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_bind_int(stmt, value1, value2)));
   return JS_TRUE;
}

static JSBool
elixir_int_sqlite3_int_params(int (*func)(sqlite3 *stmt, int value),
                              JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);
   value = val[1].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(db, value)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(sqlite3_busy_timeout, elixir_int_sqlite3_int_params);
FAST_CALL_PARAMS(sqlite3_extended_result_codes, elixir_int_sqlite3_int_params);

static JSBool
elixir_int_sqlite3_params(int (*func)(sqlite3 *db),
                          JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   int result;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   elixir_function_stop(cx);
   result = func(db);
   elixir_function_start(cx);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(result));
   return JS_TRUE;
}

FAST_CALL_PARAMS(sqlite3_changes, elixir_int_sqlite3_params);
/* FIXME: Need to also free all related callback data. */
FAST_CALL_PARAMS(sqlite3_errcode, elixir_int_sqlite3_params);
FAST_CALL_PARAMS(sqlite3_get_autocommit, elixir_int_sqlite3_params);
FAST_CALL_PARAMS(sqlite3_total_changes, elixir_int_sqlite3_params);

static JSBool
elixir_sqlite3_close(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   int result;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   elixir_function_stop(cx);
   result = sqlite3_close(db);
   elixir_function_start(cx);

   JS_SetPrivate(cx, val[0].v.obj, NULL);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(result));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_interrupt(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _sqlite3_value_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   sqlite3_interrupt(db);

   return JS_TRUE;
}

static JSBool
elixir_sqlite3_errmsg(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _sqlite3_value_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   elixir_return_str(cx, vp, sqlite3_errmsg(db));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_libversion(JSContext* cx, uintN argc, jsval *vp)
{
   elixir_value_t val[1];

   if (!elixir_params_check(cx, void_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   elixir_return_str(cx, vp, sqlite3_libversion());
   return JS_TRUE;
}

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

/* FAST_CALL_PARAMS(sqlite3_release_memory, elixir_int_int_params); */
FAST_CALL_PARAMS(sqlite3_enable_shared_cache, elixir_int_int_params);
FAST_CALL_PARAMS(sqlite3_sleep, elixir_int_int_params);

static JSBool
elixir_sqlite3_complete(JSContext *cx, uintN argc, jsval *vp)
{
   const char *sql = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   sql = elixir_get_string_bytes(val[0].v.str, NULL);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_complete(sql)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_thread_cleanup(JSContext *cx, uintN argc, jsval *vp)
{
   elixir_value_t val[1];

   if (!elixir_params_check(cx, void_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   sqlite3_thread_cleanup();

   return JS_TRUE;
}

static JSBool
elixir_sqlite3_get_table(JSContext* cx, uintN argc, jsval *vp)
{
   const char *sql = NULL;
   sqlite3 *db = NULL;
   char *errmsg = NULL;
   char **resultp;
   JSObject *result;
   JSObject *array;
   int nrow = 0;
   int ncolumn = 0;
   int error;
   int i;
   JSBool ret = JS_FALSE;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);
   sql = elixir_get_string_bytes(val[1].v.str, NULL);

   error = sqlite3_get_table(db, sql, &resultp, &nrow, &ncolumn, &errmsg);

   result = JS_NewObject(cx, elixir_class_request("elixir", NULL), NULL, NULL);
   if (!result)
     goto on_error;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(result));

   if (!elixir_add_int_prop(cx, result, "error", error))
     goto on_error;
   if (!elixir_add_str_prop(cx, result, "errmsg", errmsg))
     goto on_error;
   if (!elixir_add_int_prop(cx, result, "nrow", nrow))
     goto on_error;
   if (!elixir_add_int_prop(cx, result, "ncolumn", ncolumn))
     goto on_error;

   array = JS_NewArrayObject(cx, (nrow + 1) * ncolumn, NULL);
   if (!array)
     goto on_error;
   if (!elixir_add_object_prop(cx, result, "resultp", array))
     goto on_error;

   if (resultp)
     for (i = 0; i < (nrow + 1) * ncolumn; ++i)
       {
          jsval js_str;

	  js_str = resultp[i] ?
	    STRING_TO_JSVAL(elixir_ndup(cx, resultp[i], strlen(resultp[i]))) :
	    JSVAL_NULL;
          if (!JS_SetElement(cx, array, i, &js_str))
            goto on_error;
       }

   ret = JS_TRUE;

 on_error:
   if (errmsg)
     free(errmsg);
   if (resultp)
     sqlite3_free_table(resultp);
   return ret;
}

static JSBool
elixir_sqlite3_bind_text(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   const char *string;
   int index;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_stmt_int_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   index = val[1].v.num;
   string = elixir_get_string_bytes(val[2].v.str, NULL);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_bind_text(stmt, index, string, -1, SQLITE_TRANSIENT)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_result_text(JSContext *cx,uintN argc, jsval *vp)
{
   sqlite3_context *ctx = NULL;
   const char *string;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_context_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ctx);
   string = elixir_get_string_bytes(val[2].v.str, NULL);

   sqlite3_result_text(ctx, string, -1, SQLITE_TRANSIENT);

   return JS_TRUE;
}

static void
void_cb_cdata(void* cdata)
{
   JSContext *cx;
   JSObject *parent;
   JSFunction *fct;
   jsval argv[1];
   jsval js_return;

   fct = elixir_void_get_private(cdata);
   if (fct)
     {
        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return ;

	elixir_function_start(cx);

        argv[0] = elixir_void_get_jsval(cdata);
        elixir_function_run(cx, fct, parent, 1, argv, &js_return);

	elixir_function_stop(cx);
     }
}

static JSBool
elixir_sqlite3_rollback_hook(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   void *cdata = NULL;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);
   cdata = sqlite3_rollback_hook(db, void_cb_cdata, cdata);

   JS_SET_RVAL(cx, vp, cdata ? elixir_void_free(cdata) : JSVAL_NULL);
   return JS_TRUE;
}

static int
int_cb_cdata(void* cdata)
{
   JSContext *cx;
   JSObject *parent;
   JSFunction *fct;
   jsval argv[1];
   jsval js_return;
   int result = -1;

   fct = elixir_void_get_private(cdata);
   if (fct)
     {
        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return -1;

	elixir_function_start(cx);

        argv[0] = elixir_void_get_jsval(cdata);
        elixir_function_run(cx, fct, parent, 1, argv, &js_return);
        if (JS_ValueToInt32(cx, js_return, &result) == JS_FALSE)
	  result = -1;

	elixir_function_stop(cx);
     }
   return result;
}

static JSBool
elixir_sqlite3_progress_handler(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   void *cdata = NULL;
   int n;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _sqlite3_int_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);
   n = val[1].v.num;

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, val[2].v.fct);
   sqlite3_progress_handler(db, n, int_cb_cdata, cdata);

   return JS_TRUE;
}

static int
int_cb_cdata_int(void* cdata, int count)
{
   JSFunction *fct;
   JSContext *cx;
   JSObject *parent;
   jsval js_return;
   int result;
   jsval argv[2];

   fct = elixir_void_get_private(cdata);
   if (fct)
     {
        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return -1;

	elixir_function_start(cx);

        argv[0] = elixir_void_get_jsval(cdata);
        argv[1] = INT_TO_JSVAL(count);
        if (elixir_function_run(cx, fct, parent, 2, argv, &js_return))
          if (!JS_ValueToInt32(cx, js_return, &result))
	    result = -1;

	elixir_function_stop(cx);

        return result;
     }
   return -1;
}

static JSBool
elixir_sqlite3_busy_handler(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   void *cdata = NULL;
   int error;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);
   /* FIXME: We have a memory leak here. */
   error = sqlite3_busy_handler(db, int_cb_cdata_int, cdata);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(error));
   return JS_FALSE;
}

static int
sqlite3_cb(void* cdata, int columns, char** values, char** name)
{
   JSFunction *fct;
   JSContext *cx = NULL;
   JSObject *row = NULL;
   JSObject *parent;
   jsval js_return;
   int result;
   int i;
   jsval argv[2];

   result = -1;

   fct = elixir_void_get_private(cdata);
   if (fct)
     {
        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          goto on_error;

	elixir_function_start(cx);

        row = JS_NewObject(cx, elixir_class_request("sqlite3_row", NULL), NULL, NULL);
        if (!elixir_object_register(cx, &row, NULL))
          goto on_finish;

        for (i = 0; i < columns; ++i)
          if (!elixir_add_str_prop(cx, row, name[i], values[i]))
            goto on_finish;

        argv[0] = elixir_void_get_jsval(cdata);
        argv[1] = OBJECT_TO_JSVAL(row);

        if (elixir_function_run(cx, fct, parent, 2, argv, &js_return))
	  if (!JS_ValueToInt32(cx, js_return, &result))
	    result = -1;

     on_finish:
	elixir_object_unregister(cx, &row);
	elixir_function_stop(cx);
     }

 on_error:
   return result;
}

static JSBool
elixir_sqlite3_exec(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *retour;
   sqlite3 *db = NULL;
   void *cdata = NULL;
   char *errmsg = NULL;
   const char *sql;
   int error;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _sqlite3_string_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);
   sql = elixir_get_string_bytes(val[1].v.str, NULL);

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, val[2].v.fct);
   elixir_function_stop(cx);
   error = sqlite3_exec(db, sql, sqlite3_cb, cdata, &errmsg);
   elixir_function_start(cx);
   elixir_void_free(cdata);

   retour = JS_NewObject(cx, elixir_class_request("elixir", NULL), NULL, NULL);
   if (!retour)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(retour));
   if (elixir_add_int_prop(cx, retour, "error", error) == JS_FALSE)
     return JS_FALSE;
   if (elixir_add_str_prop(cx, retour, "errmsg", errmsg) == JS_FALSE)
     return JS_FALSE;
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_bind_int64(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   sqlite_int64 i64;
   double value_dbl;
   int value_int;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_stmt_int_dbl_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   value_int = val[1].v.num;
   value_dbl = val[2].v.dbl;

   i64 = value_dbl;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(sqlite3_bind_int64(stmt, value_int, i64)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_column_int64(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *stmt = NULL;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_stmt_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, stmt);
   value = val[1].v.num;

   JS_NewNumberValue(cx, sqlite3_column_double(stmt, value), &(JS_RVAL(cx, vp)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_result_int64(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_context *context = NULL;
   sqlite_int64 value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_context_dbl_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, context);
   value = val[1].v.dbl;

   sqlite3_result_int64(context, value);

   return JS_TRUE;
}

static JSBool
elixir_sqlite3_value_int64(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_value *value = NULL;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_value_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, value);

   JS_NewNumberValue(cx, sqlite3_value_int64(value), &(JS_RVAL(cx, vp)));
   return JS_TRUE;
}

struct callback_function_s
{
   JSFunction *func;
   JSFunction *step;
   JSFunction *final;
};

static void
all_cb(void* cdata, JSFunction* fct, sqlite3_context* ctx, int argc, sqlite3_value **argv)
{
   JSContext *cx;
   JSObject *js_array = NULL;
   JSObject *parent;
   jsval js_return;
   int i;
   jsval js_argv[2];

   if (fct)
     {
        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return ;

	elixir_function_start(cx);

        if (!elixir_rval_new(cx, elixir_class_request("sqlite3_context", NULL), ctx, js_argv + 0))
          goto on_error;

        js_array = JS_NewArrayObject(cx, argc, NULL);
        if (!elixir_object_register(cx, &js_array, NULL))
          goto on_error;

        for (i = 0; i < argc; ++i)
          {
             JSObject*  value;
             jsval      js_value;

             value = JS_NewObject(cx, elixir_class_request("sqlite3_value", NULL), NULL, NULL);
             if (!JS_SetPrivate(cx, value, argv[i]))
               goto on_error;

             js_value = OBJECT_TO_JSVAL(value);
             if (!JS_SetElement(cx, js_array, i, &js_value))
               goto on_error;
          }

        js_argv[1] = OBJECT_TO_JSVAL(js_array);
        elixir_rval_register(cx, js_argv + 1);

        elixir_function_run(cx, fct, parent, 2, js_argv, &js_return);

        elixir_rval_delete(cx, js_argv + 1);

     on_error:
        elixir_rval_delete(cx, js_argv + 0);
        elixir_object_unregister(cx, &js_array);
	elixir_function_stop(cx);
     }
}

#define GCALLBACK(Func)							\
  static void								\
    cb_##Func(sqlite3_context* ctx, int argc, sqlite3_value **argv)	\
  {									\
     void *cdata;							\
     struct callback_function_s *cfs;					\
									\
     cdata = sqlite3_user_data(ctx);					\
     cfs = elixir_void_get_private(cdata);				\
     if (!cfs)								\
       return ;								\
     all_cb(cdata, cfs->Func, ctx, argc, argv);				\
  }

GCALLBACK(func);
GCALLBACK(step);

static void
cb_final(sqlite3_context* ctx)
{
   struct callback_function_s *cfs;
   JSFunction *fct;
   JSContext *cx;
   JSObject *parent;
   void *cdata;
   jsval js_return;
   jsval js_argv[1];

   cdata = sqlite3_user_data(ctx);
   cfs = elixir_void_get_private(cdata);
   if (!cfs)
     return ;
   fct = cfs->final;
   if (fct)
     {
        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return ;

	elixir_function_start(cx);

        if (!elixir_rval_new(cx, elixir_class_request("sqlite3_context", NULL), ctx, js_argv + 0))
          goto on_error;

        elixir_function_run(cx, fct, parent, 1, js_argv, &js_return);

        elixir_rval_delete(cx, js_argv + 0);

     on_error:
	elixir_function_stop(cx);
     }
}

static JSBool
elixir_sqlite3_create_function(JSContext *cx, uintN argc, jsval *vp)
{
   JSFunction *func = NULL;
   JSFunction *step = NULL;
   JSFunction *final = NULL;
   sqlite3 *db = NULL;
   struct callback_function_s *cfs;
   const char*function_name;
   void *cdata;
   int param_count;
   int error;
   elixir_value_t val[7];

   if (elixir_params_check(cx, _sqlite3_create_function_scalar, val, argc, JS_ARGV(cx, vp)))
     func = val[4].v.fct;

   if (elixir_params_check(cx, _sqlite3_create_function_aggreagate, val, argc, JS_ARGV(cx, vp)))
     {
        step = val[5].v.fct;
        final = val[6].v.fct;
     }

   if (!elixir_params_check(cx, _sqlite3_create_function_reset, val, argc, JS_ARGV(cx, vp))
       && !func
       && !step
       && !final)
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);
   function_name = elixir_get_string_bytes(val[1].v.str, NULL);
   param_count = val[2].v.num;

   /* FIXME: Some memory leak here. */
   cfs = malloc(sizeof(struct callback_function_s));
   if (!cfs)
     return JS_FALSE;
   cfs->func = func;
   cfs->step = step;
   cfs->final = final;
   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, cfs);

   error = sqlite3_create_function(db, function_name, param_count, SQLITE_UTF8, cdata,
                                   func ? cb_func : NULL,
                                   step ? cb_step : NULL,
                                   final ? cb_final : NULL);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(error));
   return JS_TRUE;
}

static int
commit_cb(void *cdata)
{
   JSFunction *fct;
   JSContext *cx;
   JSObject *parent;
   jsval argv[1];
   jsval js_return;
   int result = 1;

   fct = elixir_void_get_private(cdata);
   if (fct)
     {
        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return 1;

        argv[0] = elixir_void_get_jsval(cdata);

	elixir_function_start(cx);

        if (elixir_function_run(cx, fct, parent, 1, argv, &js_return))
          if (!JS_ValueToInt32(cx, js_return, &result))
	    result = 1;

	elixir_function_stop(cx);
     }
   return result;
}

static JSBool
elixir_sqlite3_commit_hook(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   void *cdata = NULL;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);
   cdata = sqlite3_commit_hook(db, commit_cb, cdata);

   JS_SET_RVAL(cx, vp, cdata ? elixir_void_free(cdata) : JSVAL_NULL);
   return JS_TRUE;
}

static void
hook_cb(void *cdata, int op, const char* db_name, const char* table_name, sqlite_int64 rowid)
{
   JSContext *cx;
   JSObject *parent;
   JSFunction *fct;
   jsval argv[5];
   jsval js_return;

   fct = elixir_void_get_private(cdata);
   if (fct)
     {
        JSString *dbn;
        JSString *tn;
        double rid;

        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return ;

	elixir_function_start(cx);

        dbn = elixir_dup(cx, db_name);
        tn = elixir_dup(cx, table_name);
        rid = rowid;

        argv[0] = elixir_void_get_jsval(cdata);
        argv[1] = INT_TO_JSVAL(op);
        argv[2] = dbn ? STRING_TO_JSVAL(dbn) : JSVAL_NULL;
        argv[3] = tn ? STRING_TO_JSVAL(tn) : JSVAL_NULL;

	JS_NewNumberValue(cx, rid, argv + 4);

        elixir_function_run(cx, fct, parent, 5, argv, &js_return);

	elixir_function_stop(cx);
     }
}

static JSBool
elixir_sqlite3_update_hook(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   void *cdata = NULL;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);
   cdata = sqlite3_update_hook(db, hook_cb, cdata);

   JS_SET_RVAL(cx, vp, cdata ? elixir_void_free(cdata) : JSVAL_NULL);
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_last_insert_rowid(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _sqlite3_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   JS_NewNumberValue(cx, sqlite3_last_insert_rowid(db), &(JS_RVAL(cx, vp)));
   return JS_TRUE;
}

static void
xtrace_cb(void* cdata, const char* sql)
{
   JSFunction *fct;
   JSContext *cx;
   JSObject *parent;
   jsval argv[2];
   jsval js_return;

   fct = elixir_void_get_private(cdata);
   if (fct)
     {
        JSString *s;

        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return ;

	elixir_function_start(cx);

        s = elixir_dup(cx, sql);
        argv[0] = elixir_void_get_jsval(cdata);
        argv[1] = s ? STRING_TO_JSVAL(s) : JSVAL_NULL;

        elixir_function_run(cx, fct, parent, 2, argv, &js_return);

	elixir_function_stop(cx);
     }
}

static JSBool
elixir_sqlite3_trace(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   void *cdata = NULL;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);
   cdata = sqlite3_trace(db, xtrace_cb, cdata);

   JS_SET_RVAL(cx, vp, cdata ? elixir_void_free(cdata) : JSVAL_NULL);
   return JS_TRUE;
}

static int
xauth_cb(void* cdata, int type, const char* name1, const char* name2, const char* db_name, const char* trigger_name)
{
   JSContext *cx;
   JSObject *parent;
   JSFunction *fct;
   jsval argv[6];
   jsval js_return;
   int error = SQLITE_OK;;

   fct = elixir_void_get_private(cdata);
   if (fct)
     {
        JSString *n1;
        JSString *n2;
        JSString *dbn;
        JSString *tn;

        parent = elixir_void_get_parent(cdata);
        cx = elixir_void_get_cx(cdata);
        if (!parent || !cx)
          return SQLITE_OK;

	elixir_function_start(cx);

        n1 = elixir_dup(cx, name1);
        n2 = elixir_dup(cx, name2);
        dbn = elixir_dup(cx, db_name);
        tn = elixir_dup(cx, trigger_name);

        argv[0] = elixir_void_get_jsval(cdata);
        argv[1] = INT_TO_JSVAL(type);
        argv[2] = n1 ? STRING_TO_JSVAL(n1) : JSVAL_NULL;
        argv[3] = n2 ? STRING_TO_JSVAL(n2) : JSVAL_NULL;
        argv[4] = dbn ? STRING_TO_JSVAL(dbn) : JSVAL_NULL;
        argv[5] = tn ? STRING_TO_JSVAL(tn) : JSVAL_NULL;

        if (!elixir_function_run(cx, fct, parent, 6, argv, &js_return))
          goto on_error;

        if (!JS_ValueToInt32(cx, js_return, &error))
          goto on_error;

	elixir_function_stop(cx);
     }

   return error;

  on_error:
   elixir_function_stop(cx);

   return SQLITE_DENY;
}

static JSBool
elixir_sqlite3_set_authorizer(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3 *db = NULL;
   void *cdata = NULL;
   int error;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _sqlite3_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);

   /* FIXME: Some memory leak append just here. */
   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);
   elixir_function_stop(cx);
   error = sqlite3_set_authorizer(db, xauth_cb, cdata);
   elixir_function_start(cx);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(error));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_prepare_v2(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_stmt *statement = NULL;
   const char *sql_tail = NULL;
   sqlite3 *db = NULL;
   JSObject *result;
   JSObject *js_stmt;
   const char *sql;
   int error;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);
   sql = elixir_get_string_bytes(val[1].v.str, NULL);

   error = sqlite3_prepare_v2(db, sql, -1, &statement, &sql_tail);

   result = JS_NewObject(cx, elixir_class_request("elixir", NULL), NULL, NULL);
   if (!result)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(result));

   if (!elixir_add_int_prop(cx, result, "error", error))
     return JS_FALSE;

   js_stmt = JS_DefineObject(cx, result, "statement",
                             elixir_class_request("sqlite3_stmt", NULL), NULL,
                             JSPROP_ENUMERATE | JSPROP_READONLY);

   if (!js_stmt)
     return JS_FALSE;
   if (!JS_SetPrivate(cx, js_stmt, statement))
     return JS_FALSE;
   if (!elixir_add_str_prop(cx, result, "tail", sql_tail))
     return JS_FALSE;
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_table_column_metadata(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *result;
   sqlite3 *db = NULL;
   const char *db_name;
   const char *table_name;
   const char *column_name;
   const char *data_type;
   const char *coll_seq;
   int not_null;
   int primary_key;
   int auto_inc;
   int error;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _sqlite3_3string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, db);
   db_name = elixir_get_string_bytes(val[1].v.str, NULL);
   table_name = elixir_get_string_bytes(val[2].v.str, NULL);
   column_name = elixir_get_string_bytes(val[3].v.str, NULL);

   error = sqlite3_table_column_metadata(db, db_name, table_name, column_name,
                                         &data_type, &coll_seq, &not_null, &primary_key, &auto_inc);

   result = JS_NewObject(cx, elixir_class_request("elixir", NULL), NULL, NULL);
   if (!result)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(result));

   if (!elixir_add_int_prop(cx, result, "error", error))
     return JS_FALSE;
   if (!elixir_add_str_prop(cx, result, "data_type", data_type))
     return JS_FALSE;
   if (!elixir_add_str_prop(cx, result, "collation_sequence", coll_seq))
     return JS_FALSE;
   if (!elixir_add_int_prop(cx, result, "not_null", not_null))
     return JS_FALSE;
   if (!elixir_add_int_prop(cx, result, "primary_key", primary_key))
     return JS_FALSE;
   if (!elixir_add_int_prop(cx, result, "auto_increment", auto_inc))
     return JS_FALSE;
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_result_value(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_context *context = NULL;
   sqlite3_value *value = NULL;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_context_sqlite3_value_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, context);
   GET_PRIVATE(cx, val[1].v.obj, value);

   sqlite3_result_value(context, value);

   return JS_TRUE;
}

static JSBool
elixir_sqlite3_result_double(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_context *context = NULL;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_context_dbl_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, context);

   sqlite3_result_double(context, val[1].v.dbl);

   return JS_TRUE;
}

static JSBool
elixir_sqlite3_result_error(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_context *context = NULL;
   const char *value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_context_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, context);
   value = elixir_get_string_bytes(val[1].v.str, NULL);

   sqlite3_result_error(context, value, -1);

   return JS_TRUE;
}

static JSBool
elixir_sqlite3_result_int(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_context *context = NULL;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_context_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, context);
   value = val[1].v.num;

   sqlite3_result_int(context, value);

   return JS_TRUE;
}

static JSBool
elixir_sqlite3_result_null(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_context *context = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _sqlite3_context_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, context);

   sqlite3_result_null(context);

   return JS_TRUE;
}

static JSBool
elixir_sqlite3_user_data(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_context *context = NULL;
   void *cdata = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _sqlite3_context_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, context);

   cdata = sqlite3_user_data(context);

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(cdata));
   return JS_TRUE;
}

static JSBool
elixir_int_sqlite3_value_params(int (*func)(sqlite3_value *val),
                                JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_value *value = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _sqlite3_value_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, value);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(value)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(sqlite3_value_bytes, elixir_int_sqlite3_value_params);
FAST_CALL_PARAMS(sqlite3_value_int, elixir_int_sqlite3_value_params);
FAST_CALL_PARAMS(sqlite3_value_type, elixir_int_sqlite3_value_params);

static JSBool
elixir_sqlite3_value_double(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_value *value = NULL;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_value_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, value);

   JS_NewNumberValue(cx, sqlite3_value_double(value), &(JS_RVAL(cx, vp)));
   return JS_TRUE;
}

static JSBool
elixir_sqlite3_value_text(JSContext *cx, uintN argc, jsval *vp)
{
   sqlite3_value *value = NULL;
   const char *str;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _sqlite3_value_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, value);
   str = (const char*) sqlite3_value_text(value);

   elixir_return_str(cx, vp, str);
   return JS_TRUE;
}

/* FIXME: Add sqlite3_create_collation_v2 binding when we know what it is. */
/* FIXME: Add sqlite3_next_stmt */

static JSFunctionSpec           sqlite_functions[] = {
  ELIXIR_FN(sqlite3_bind_parameter_count, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_clear_bindings, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_data_count, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_count, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_expired, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_finalize, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_reset, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_step, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_bind_parameter_name, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_decltype, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_name, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_database_name, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_origin_name, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_table_name, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_text, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_double, 2, JSPROP_ENUMERATE, 1 ),
  ELIXIR_FN(sqlite3_changes, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_close, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_errcode, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_get_autocommit, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_total_changes, 1, JSPROP_ENUMERATE, 0 ),
/*    ELIXIR_FN(sqlite3_release_memory, 1, JSPROP_ENUMERATE, 0 ), */
  ELIXIR_FN(sqlite3_enable_shared_cache, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_sleep, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_value_bytes, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_value_int, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_value_type, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_value_text, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_value_double, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_busy_timeout, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_extended_result_codes, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_thread_cleanup, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_complete, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_transfer_bindings, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_bind_parameter_index, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_bind_double, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_bind_int, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_db_handle, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_open, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_open_v2, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_bind_text, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_result_text, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_rollback_hook, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_progress_handler, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_bind_null, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_bytes, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_int, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_type, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_interrupt, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_errmsg, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_libversion, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_get_table, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_busy_handler, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_exec, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_result_null, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_result_int, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_result_error, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_result_double, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_result_value, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_table_column_metadata, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_prepare_v2, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_set_authorizer, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_trace, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_update_hook, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_commit_hook, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_bind_int64, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_column_int64, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_value_int64, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_result_int64, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_user_data, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_create_function, 7, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(sqlite3_last_insert_rowid, 1, JSPROP_ENUMERATE, 0),
  ELIXIR_FN(sqlite3_sql, 1, JSPROP_ENUMERATE, 0),
  JS_FS_END
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp = NULL;

   if (em->data)
     return EINA_FALSE;

   if (!load_sqlite_const_binding(cx, parent))
     goto on_error;

   em->data = parent;
   tmp = &em->data;
   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, parent, sqlite_functions))
     goto on_error;

   _sqlite3_parameter.class = elixir_class_request("sqlite3", NULL);
   _sqlite3_stmt_parameter.class = elixir_class_request("sqlite3_stmt", NULL);
   _sqlite3_context_parameter.class = elixir_class_request("sqlite3_context", NULL);
   _sqlite3_value_parameter.class = elixir_class_request("sqlite3_value", NULL);

   return EINA_TRUE;

  on_error:
   unload_sqlite_const_binding(cx, parent);
   if (tmp)
     elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;
   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   void **tmp;
   int i = 0;

   if (!em->data)
     return EINA_FALSE;

   /* FIXME: Destroy all callback and their refering data. */

   while (sqlite_functions[i].name)
     JS_DeleteProperty(cx, (JSObject*) em->data, sqlite_functions[i++].name);

   tmp = &em->data;
   elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "sqlite",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_sqlite = {
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
sqlite_binding_init(void)
{
   return elixir_modules_register(&em_sqlite);
}

void
sqlite_binding_shutdown(void)
{
   elixir_modules_unregister(&em_sqlite);
}

#ifndef EINA_STATIC_BUILD_SQLITE3
EINA_MODULE_INIT(sqlite_binding_init);
EINA_MODULE_SHUTDOWN(sqlite_binding_shutdown);
#endif

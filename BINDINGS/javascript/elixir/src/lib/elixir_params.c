#include <stdlib.h>
#include <string.h>

#include "elixir_private.h"

#include "Elixir.h"

EAPI const elixir_parameter_t	int_parameter = { "int", JINT, NULL };
EAPI const elixir_parameter_t	boolean_parameter = { "boolean", JBOOLEAN, NULL };
EAPI const elixir_parameter_t	string_parameter = { "string", JSTRING, NULL };
EAPI const elixir_parameter_t	double_parameter = { "double", JDOUBLE, NULL };
EAPI const elixir_parameter_t	jsobject_parameter = { "JSObject", JOBJECT, NULL };
EAPI const elixir_parameter_t	any_parameter = { "any", JANY, NULL };
EAPI const elixir_parameter_t	function_parameter = { "function", JFUNCTION, NULL };
EAPI const elixir_parameter_t   null_parameter = { "NULL", JNULL, NULL };

EAPI elixir_parameter_t         color_parameter = { "color", JOBJECT, NULL };
EAPI elixir_parameter_t         size_parameter = { "size", JOBJECT, NULL };
EAPI elixir_parameter_t         geometry_parameter = { "geometry", JOBJECT, NULL };
EAPI elixir_parameter_t         position_parameter = { "position", JOBJECT, NULL };
EAPI elixir_parameter_t         eet_parameter = { "eet_data", JOBJECT, NULL };

EAPI const elixir_parameter_t*	void_params[1] = {
   NULL
};
EAPI const elixir_parameter_t*	string_params[2] = {
   &string_parameter,
   NULL
};
EAPI const elixir_parameter_t*	int_params[2] = {
   &int_parameter,
   NULL
};
EAPI const elixir_parameter_t*	boolean_params[2] = {
   &boolean_parameter,
   NULL
};
EAPI const elixir_parameter_t*  function_params[2] = {
   &function_parameter,
   NULL
};
EAPI const elixir_parameter_t*	double_params[2] = {
   &double_parameter,
   NULL
};
EAPI const elixir_parameter_t*  jsobject_params[2] = {
   &jsobject_parameter,
   NULL
};

elixir_parameter_type_t
elixir_params_get_type(jsval arg)
{
   if (JSVAL_IS_BOOLEAN(arg) == JS_TRUE)
     return JBOOLEAN;
   if (JSVAL_IS_INT(arg) == JS_TRUE)
     return JINT;
   if (JSVAL_IS_DOUBLE(arg) == JS_TRUE)
     return JDOUBLE;
   if (JSVAL_IS_NULL(arg) == JS_TRUE)
     return JNULL;
   if (JSVAL_IS_STRING(arg) == JS_TRUE)
     return JSTRING;
   if (JSVAL_IS_OBJECT(arg) == JS_TRUE)
     return JOBJECT;
   return JANY;
}

static int
elixir_params_get_boolean(JSContext* cx, jsval arg, int* bool, int index)
{
   const char*  cstr;
   JSString*    jstr;
   int		localbool;
   int		localnum;
   jsdouble	localdbl;

   switch (elixir_params_get_type(arg))
     {
     case JBOOLEAN:
	localbool = JSVAL_TO_BOOLEAN(arg);
	break;
     case JINT:
	if (JS_ValueToInt32(cx, arg, &localnum) == JS_FALSE)
          goto on_error;
	localbool = localnum;
	break;
     case JDOUBLE:
	if (JS_ValueToNumber(cx, arg, &localdbl) == JS_FALSE)
	  goto on_error;
	localbool = localdbl;
	break;
     case JSTRING:
        jstr = JS_ValueToString(cx, arg);
        cstr = elixir_get_string_bytes(jstr, NULL);
        if (strcmp(cstr, "true") == 0)
          localbool = 1;
        else
          if (strcmp(cstr, "false") == 0)
            localbool = 0;
          else
            goto on_error;
        break;
     default:
        goto on_error;
     }

   if (bool)
     *bool = localbool;
   return 1;

  on_error:
   JS_ReportError(cx, "cannot convert arg %i from %s to boolean",
                  index, JS_GetTypeName(cx, JS_TypeOfValue(cx, arg)));
   return 0;
}

static int
elixir_params_get_int(JSContext* cx, jsval arg, int* num, int index)
{
   int		localnum;
   jsdouble	localdbl;

   switch (elixir_params_get_type(arg))
     {
     case JINT:
     case JSTRING:
	if (JS_ValueToInt32(cx, arg, &localnum) == JS_FALSE)
	  goto on_error;
	break;
     case JDOUBLE:
	if (JS_ValueToNumber(cx, arg, &localdbl) == JS_FALSE)
          goto on_error;
	localnum = localdbl;
	break;
     default:
        goto on_error;
     }

   if (num)
     *num = localnum;
   return 1;

  on_error:
   JS_ReportError(cx, "cannot convert arg %i from %s to int",
                  index, JS_GetTypeName(cx, JS_TypeOfValue(cx, arg)));
   return 0;
}

static int
elixir_params_get_double(JSContext* cx, jsval arg, jsdouble* dbl, int index)
{
   int		localnum;
   jsdouble	localdbl;

   switch (elixir_params_get_type(arg))
     {
     case JINT:
	if (JS_ValueToInt32(cx, arg, &localnum) == JS_FALSE)
          goto on_error;
	localdbl = localnum;
	break;
     case JSTRING:
     case JDOUBLE:
	if (JS_ValueToNumber(cx, arg, &localdbl) == JS_FALSE)
          goto on_error;
	break;
     default:
        goto on_error;
     }

   if (dbl)
     *dbl = localdbl;
   return 1;

  on_error:
   JS_ReportError(cx, "cannot convert arg %i from %s to double",
                  index, JS_GetTypeName(cx, JS_TypeOfValue(cx, arg)));
   return 0;
}

static int
elixir_params_get_string(JSContext* cx, jsval arg, JSString** str, int index)
{
   JSString*	lstr = NULL;
   char		buffer[32];
   jsdouble	localdbl;
   int		localnum;

   switch (elixir_params_get_type(arg))
     {
     case JDOUBLE:
	if (JS_ValueToNumber(cx, arg, &localdbl) == JS_FALSE)
          goto on_error;

	slprintf(buffer, sizeof (buffer), "%f", localdbl);
	lstr = elixir_dup(cx, buffer);
	break;
     case JINT:
	if (JS_ValueToInt32(cx, arg, &localnum) == JS_FALSE)
          goto on_error;

	slprintf(buffer, sizeof (buffer), "%i", localnum);
	lstr = elixir_dup(cx, buffer);
	break;
     case JBOOLEAN:
	lstr = elixir_dup(cx, JSVAL_TO_BOOLEAN(arg) ? "true" : "false");
	break;
     case JNULL:
	lstr = NULL;
	break;
     case JSTRING:
	lstr = JS_ValueToString(cx, arg);
	break;
     default:
        goto on_error;
     }

   if (str)
     *str = lstr;
   return 1;

  on_error:
   JS_ReportError(cx, "cannot convert arg %i from %s to string",
                  index, JS_GetTypeName(cx, JS_TypeOfValue(cx, arg)));
   return 0;
}

int
elixir_params_check_class(const JSClass *object, const JSClass *match)
{
   if (!object)
     return -1;
   if (!match)
     return -1;

   if (object != match)
     {
        JSClass    *parent;

        parent = elixir_class_request_parent(object);
        if (parent)
          return elixir_params_check_class(parent, match);
        return 0;
     }
   return 1;
}

static int
elixir_params_get_object(JSContext* cx, jsval arg, const JSClass *class, JSObject** obj, int index)
{
   JSObject     *lobj = NULL;
   JSClass      *object_class = NULL;

   if (JSVAL_IS_OBJECT(arg) == JS_FALSE)
     {
	JS_ReportError(cx, "cannot convert arg %i from %s to object",
                       index, JS_GetTypeName(cx, JS_TypeOfValue(cx, arg)));
        return 0;
     }

   if (JS_ValueToObject(cx, arg, &lobj) == JS_FALSE)
     return 0;

   if (class)
     {
        if (!lobj)
          {
             JS_ReportError(cx, "Arg %i is NULL instead of %s", index, class->name);
             return 0;
          }

        object_class = JS_GET_CLASS(cx, lobj);

        if (object_class)
          if (elixir_params_check_class(object_class, class) == 0)
            {
               JS_ReportError(cx, "Class of arg %i (%s) does not match (%s)", index, object_class->name, class->name);
               return 0;
            }
     }

   if (obj)
     *obj = lobj;
   return 1;
}

static int
elixir_params_check_arg(JSContext* cx,
                        const elixir_parameter_t* params[], elixir_value_t* values,
                        uintN argc, jsval* argv)
{
   unsigned int			i;

   elixir_lock_cx(cx);

   for (i = 0; i < argc; ++i)
     {
	switch (params[i]->type)
	  {
	  case JBOOLEAN:
	     if (elixir_params_get_boolean(cx, argv[i], values ? &values[i].v.bol : NULL, i))
	       break;
	     goto ok;
	  case JINT:
	     if (elixir_params_get_int(cx, argv[i], values ? &values[i].v.num : NULL, i))
	       break;
	     goto ok;
	  case JDOUBLE:
	     if (elixir_params_get_double(cx, argv[i], values ? &values[i].v.dbl : NULL, i))
	       break;
	     goto ok;
	  case JSTRING:
	     if (elixir_params_get_string(cx, argv[i], values ? &values[i].v.str : NULL, i))
	       break;
	     goto ok;
	  case JOBJECT:
             if (elixir_params_get_object(cx, argv[i], params[i]->class, values ? &values[i].v.obj : NULL, i))
               break;
	     goto ok;
	  case JANY:
	     if (values)
	       values[i].v.any = argv[i];
	     break;
	  case JFUNCTION:
             if (argv[i] == JSVAL_NULL)
               {
                  if (values)
                    values[i].v.fct = NULL;
                  break;
               }

	     if (JSVAL_IS_OBJECT(argv[i]) == JS_FALSE)
	       {
		  JS_ReportError(cx, "cannot convert arg %i from %s to object", i, JS_GetTypeName(cx, JS_TypeOfValue(cx, argv[i])));
		  goto ok;
	       }
	     if (values)
	       values[i].v.fct = JS_ValueToFunction(cx, argv[i]);
	     break;
          case JNULL:
             if (elixir_params_get_type(argv[i]) == JNULL)
               {
                  values[i].v.obj = NULL;
                  break;
               }
             JS_ReportError(cx, "Arg %i is not null", i);
             goto ok;
	  default:
	     abort();
	  };
     }
   elixir_unlock_cx(cx);
   return 1;

 ok:
   elixir_unlock_cx(cx);
   return 0;
}

static int
elixir_params_count(const elixir_parameter_t* params[])
{
   const elixir_parameter_t**	walk = params;
   unsigned int			count = 0;

   if (!walk)
     return 0;

   for (count = 0; *walk; ++walk, ++count)
     ;

   return count;
}

int
elixir_params_check(JSContext* cx,
                    const elixir_parameter_t* params[], elixir_value_t* values,
                    uintN argc, jsval* argv)
{
   unsigned int			count = 0;


   count = elixir_params_count(params);

   if (count == 0 && argc == 0)
     return 1;

   if (count != argc)
     return 0;

   return elixir_params_check_arg(cx, params, values, argc, argv);
}

int
elixir_params_check_with_options(JSContext* cx,
                                 const elixir_parameter_t* params[], elixir_value_t* values,
                                 uintN argc, jsval* argv,
                                 unsigned int optional)
{
   unsigned int			count = 0;

   count = elixir_params_count(params);

   if (count == 0 && argc == 0)
     return 1;

   if (argc <= count && count < optional)
     {
	elixir_lock_cx(cx);
	JS_ReportError(cx, "Wrong arguments count : %i (%i < %i)", count, optional, argc);
	elixir_unlock_cx(cx);
        return 0;
     }

   return elixir_params_check_arg(cx, params, values, argc, argv);
}

JSBool
elixir_int_params_void(int (*func)(void),
                       JSContext *cx, uintN argc, jsval *vp)
{
   int result;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   elixir_function_stop(cx);
   result = func();
   elixir_function_start(cx);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(result));
   return JS_TRUE;
}

JSBool
elixir_void_params_void(void (*func)(void),
                        JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   elixir_function_stop(cx);
   func();
   elixir_function_start(cx);

   return JS_TRUE;
}


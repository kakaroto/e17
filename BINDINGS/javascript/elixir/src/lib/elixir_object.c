#include <stdlib.h>
#include <string.h>

#include "elixir_private.h"

#include "Elixir.h"

JSObject*
elixir_object_create(Elixir_Runtime* er, JSObject* parent, const char* name,
                     void* (*callback)(void* handle, const char** key, const char** value, void* context),
                     void* context)
{
   void*        handle = NULL;
   const char*  key = NULL;
   const char*  value = NULL;
   JSObject*    obj;

   if (!name)
     return NULL;

   if (!parent)
     parent = er->root;

   elixir_lock_cx(er->cx);

   obj = JS_DefineObject(er->cx, parent, name, elixir_class_request("elixir", NULL), NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!elixir_object_register(er->cx, &obj, NULL))
     goto on_error;

   if (callback)
     while (1)
       {
          handle = callback(handle, &key, &value, context);
          if (!key)
            break ;
          if (elixir_add_str_prop(er->cx, obj, key, value) == JS_FALSE)
            break ;
          if (!handle)
            break ;
       }

   elixir_object_unregister(er->cx, &obj);

   return obj;

 on_error:
   elixir_unlock_cx(er->cx);
   return NULL;
}

JSObject*
elixir_object_get_object(JSContext* cx, JSObject* parent, const char* name)
{
   JSObject*    obj;
   jsval        propertie;

   if (JS_GetProperty(cx, parent, name, &propertie) == JS_FALSE)
     return NULL;

   if (JSVAL_IS_OBJECT(propertie) == JS_FALSE)
     return NULL;

   if (JS_ValueToObject(cx, propertie, &obj) == JS_FALSE)
     return NULL;

   return obj;
}

Eina_Bool
elixir_object_get_int(JSContext *cx, JSObject *obj, const char *name, int *value)
{
   jsval propertie;
   jsdouble localdbl;

   if (!JS_GetProperty(cx, obj, name, &propertie))
     return EINA_FALSE;

   if (JSVAL_IS_INT(propertie)
       || JSVAL_IS_STRING(propertie))
     {
        if (!JS_ValueToInt32(cx, propertie, value))
          return EINA_FALSE;
     }
   else
     if (JSVAL_IS_DOUBLE(propertie))
       {
          if (!JS_ValueToNumber(cx, propertie, &localdbl))
            return EINA_FALSE;
          *value = localdbl;
       }
     else
       if (JSVAL_IS_BOOLEAN(propertie))
         {
            *value = JSVAL_TO_BOOLEAN(propertie);
         }
       else
         return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
elixir_object_get_str(JSContext *cx, JSObject *obj, const char *name, const char **value)
{
   JSString *str;
   jsval propertie;

   if (!JS_GetProperty(cx, obj, name, &propertie))
     return EINA_FALSE;

   str = JS_ValueToString(cx, propertie);
   *value = elixir_get_string_bytes(str, NULL);

   return EINA_TRUE;
}

Eina_Bool
elixir_object_get_dbl(JSContext* cx, JSObject* obj, const char* name, double* value)
{
   jsval propertie;
   int localnum;

   if (!JS_GetProperty(cx, obj, name, &propertie))
     return EINA_FALSE;

   if (JSVAL_IS_DOUBLE(propertie)
       || JSVAL_IS_STRING(propertie))
     {
        if (!JS_ValueToNumber(cx, propertie, value))
          return EINA_FALSE;
     }
   else
     if (JSVAL_IS_INT(propertie))
       {
          if (!JS_ValueToInt32(cx, propertie, &localnum))
            return EINA_FALSE;
          *value = localnum;
       }
     else
       if (JSVAL_IS_BOOLEAN(propertie))
         {
            *value = JSVAL_TO_BOOLEAN(propertie);
         }
       else
         return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
elixir_object_get_fct(JSContext *cx, JSObject *obj, const char *name, JSFunction **fct)
{
   jsval propertie;

   if (!JS_GetProperty(cx, obj, name, &propertie))
     return EINA_FALSE;

   *fct = elixir_get_fct(cx, propertie);
   if (!*fct)
     return EINA_FALSE;
   return EINA_TRUE;
}

char*
elixir_get_string(JSContext *cx, jsval arg)
{
   char *ret = NULL;

   switch (elixir_params_get_type(arg))
     {
     case JDOUBLE:
     {
        jsdouble ldbl;

        if (JS_ValueToNumber(cx, arg, &ldbl))
          {
             ret = malloc(sizeof (char) * 33);
             slprintf(ret, 32, "%f", ldbl);
          }

        break;
     }
     case JINT:
     {
        int lnum;

        if (JS_ValueToInt32(cx, arg, &lnum))
          {
             ret = malloc(sizeof (char) * 33);
             slprintf(ret, 32, "%i", lnum);
          }

        break;
     }
     case JBOOLEAN:
     {
        ret = strdup(JSVAL_TO_BOOLEAN(arg) ? "true" : "false");

        break;
     }
     case JSTRING:
     {
        JSString *lstr;

        lstr = JS_ValueToString(cx, arg);
        if (lstr)
          ret = strdup(elixir_get_string_bytes(lstr, NULL));

        break;
     }
     case JNULL:
     default:
        ret = strdup("null");
        break;
     }
   return ret;
}

int
elixir_get_int(JSContext *cx, jsval arg)
{
   int ret = 0;

   switch (elixir_params_get_type(arg))
     {
     case JINT:
     case JSTRING:
        JS_ValueToInt32(cx, arg, &ret);
        break;
     case JDOUBLE:
     {
        jsdouble ldbl;

        if (JS_ValueToNumber(cx, arg, &ldbl))
          ret = ldbl;

        break;
     }
     default:
        break;
     }
   return ret;
}

double
elixir_get_dbl(JSContext *cx, jsval arg)
{
   double ret = 0;

   switch (elixir_params_get_type(arg))
     {
     case JINT:
     {
        int     lnum;

        if (JS_ValueToInt32(cx, arg, &lnum))
          ret = lnum;

        break;
     }
     case JDOUBLE:
     case JSTRING:
        JS_ValueToNumber(cx, arg, &ret);
        break;
     default:
        break;
     }

   return ret;
}

JSObject*
elixir_get_obj(JSContext *cx, jsval arg)
{
   JSObject *ret = NULL;

   if (JSVAL_IS_OBJECT(arg))
     JS_ValueToObject(cx, arg, &ret);

   return ret;
}

JSFunction*
elixir_get_fct(JSContext *cx, jsval arg)
{
   return JS_ValueToFunction(cx, arg);
}

Eina_List *
elixir_jsmap_add(Eina_List *list, JSContext *cx, jsval val, void *data, int type)
{
   Elixir_Jsmap *m;

   m = malloc(sizeof (Elixir_Jsmap));
   if (!m) return list;

   m->val = val;
   m->data = data;
   m->type = type;

   if (!elixir_rval_register(cx, &m->val))
     {
	free(m);
	return list;
     }

   return eina_list_append(list, m);
}

Eina_List *
elixir_jsmap_del(Eina_List *list, JSContext *cx, jsval val, int type)
{
   Elixir_Jsmap *m;
   Eina_List *l;

   EINA_LIST_FOREACH(list, l, m)
     if (m->val == val && m->type == type)
       {
	  elixir_rval_delete(cx, &m->val);
	  free(m);

	  return eina_list_remove_list(list, l);
       }

   return list;
}

void *
elixir_jsmap_find(Eina_List **list, jsval val, int type)
{
   Elixir_Jsmap *m;
   Eina_List *l;

   EINA_LIST_FOREACH(*list, l, m)
     if (m->val == val && m->type == type)
       {
	  *list = eina_list_promote_list(*list, l);
	  return m->data;
       }

   return NULL;
}

void
elixir_jsmap_free(Eina_List *list, JSContext *cx)
{
   Elixir_Jsmap *m;

   EINA_LIST_FREE(list, m)
     {
	elixir_rval_delete(cx, &m->val);
	elixir_void_free(m->data);
	free(m);
     }
}

static const struct {
  const char *evas_name;
  const char *elixir_name;
} _matching_evas_object[] = {
  { "rectangle", "evas_object_rectangle" },
  { "polygon", "evas_object_polygon" },
  { "textblock", "evas_object_textblock" },
  { "text", "evas_object_text" },
  { "line", "evas_object_line" },
  { "image", "evas_object_image" },
  { "edje", "edje_object" },
  { "emotion_object", "emotion_object" },
  { NULL, NULL }
};

#ifdef BUILD_MODULE_EVAS
const char*
evas_object_to_elixir_object(Evas_Object *obj)
{
   const char *evas_name;
   unsigned int  i;

   if (!obj)
     return "evas_object";

   evas_name = evas_object_type_get(obj);

   for (i = 0; _matching_evas_object[i].evas_name; ++i)
     if (strcmp(_matching_evas_object[i].evas_name, evas_name) == 0)
       return _matching_evas_object[i].elixir_name;

   return "evas_object_smart";
}

static void
_elixir_evas_object(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eina_List *jsmap;
   JSContext *cx;
   JSObject *js_obj;
   jsval *tmp;
   Eina_Bool suspended;

   (void) data;
   (void) e;
   (void) event_info;

   tmp = evas_object_data_del(obj, "elixir_jsval");
   if (!tmp) return ;

   cx = evas_object_event_callback_del(obj, EVAS_CALLBACK_FREE, _elixir_evas_object);

   suspended = elixir_function_suspended(cx);

   if (suspended) elixir_function_start(cx);

   js_obj = JSVAL_TO_OBJECT(*tmp);
   if (!js_obj) return ;
   JS_SetPrivate(cx, js_obj, NULL);

   elixir_rval_delete(cx, tmp);
   free(tmp);

   jsmap = evas_object_data_del(obj, "elixir_jsmap");
   elixir_jsmap_free(jsmap, cx);

   if (suspended) elixir_function_stop(cx);

   elixir_decrease_count(cx);
}
#endif

EAPI Eina_Bool
evas_object_to_jsval(JSContext *cx, Evas_Object *obj, jsval *rval)
{
#ifdef BUILD_MODULE_EVAS
   JSClass *evas_object_class;
   jsval *tmp;
   JSObject *jo;

   if (!obj)
     {
	*rval = JSVAL_NULL;
	return EINA_TRUE;
     }

   tmp = evas_object_data_get(obj, "elixir_jsval");
   if (tmp)
     {
	/* FIXME: Instruct tracker of returned object. */
	*rval = *tmp;
	return EINA_TRUE;
     }

   evas_object_class = elixir_class_request(evas_object_to_elixir_object(obj), "evas_object");

   jo = elixir_build_ptr(cx, obj, evas_object_class);
   if (!jo)
     return EINA_FALSE;
   *rval = OBJECT_TO_JSVAL(jo);

   tmp = malloc(sizeof (jsval));
   if (!tmp) return EINA_FALSE;

   *tmp = *rval;
   if (!elixir_rval_register(cx, tmp))
     {
	free(tmp);
	return EINA_FALSE;
     }

   evas_object_data_set(obj, "elixir_jsval", tmp);
   elixir_increase_count(cx);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, _elixir_evas_object, cx);

   return EINA_TRUE;
#else
   (void) cx;
   (void) obj;
   (void) rval;
   return EINA_FALSE;
#endif
}



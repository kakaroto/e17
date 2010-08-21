#include <string.h>
#include <stdlib.h>
#include <Evas.h>

#include "evas-bindings.h"

static elixir_parameter_t               _evas_smart_parameter = {
  "Evas_Smart", JOBJECT, NULL
};

static const elixir_parameter_t*        _evas_smart_params[2] = {
  &_evas_smart_parameter,
  NULL
};
static const elixir_parameter_t*        _evas_evas_smart_params[3] = {
  &evas_parameter,
  &_evas_smart_parameter,
  NULL
};
static const elixir_parameter_t*        _evas_object_smart_string_func_any_params[5] = {
  &evas_object_smart_parameter,
  &string_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};
static const elixir_parameter_t*        _evas_object_smart_string_func_params[4] = {
  &evas_object_smart_parameter,
  &string_parameter,
  &function_parameter,
  NULL
};
static const elixir_parameter_t*        _evas_object_smart_string_any_params[4] = {
  &evas_object_smart_parameter,
  &string_parameter,
  &any_parameter,
  NULL
};

typedef enum _evas_smart_callback_e     evas_smart_callback_t;
enum _evas_smart_callback_e
{
  ESC_ADD = 0,
  ESC_DEL,
  ESC_LAYER_SET,
  ESC_RAISE,
  ESC_LOWER,
  ESC_STACK_ABOVE,
  ESC_STACK_BELOW,
  ESC_MOVE,
  ESC_RESIZE,
  ESC_SHOW,
  ESC_HIDE,
  ESC_COLOR_SET,
  ESC_CLIP_SET,
  ESC_CLIP_UNSET,
  ESC_CALCULATE,
  ESC_MEMBER_ADD,
  ESC_MEMBER_DEL
};

typedef struct _evas_smart_data_s       evas_smart_data_t;
struct _evas_smart_data_s
{
  JSFunction*   func[17];
};

static void
func_get_data(Evas_Object* obj, JSContext** cx, JSObject** parent, evas_smart_data_t** esd)
{
   Evas_Smart*          es;
   void*                data;

   es = evas_object_smart_smart_get(obj);
   data = evas_smart_data_get(es);

   *esd = elixir_void_get_private(data);
   *cx = elixir_void_get_cx(data);
   *parent = elixir_void_get_parent(data);
}

static void
func_evas_object_call(evas_smart_callback_t index, Evas_Object* obj)
{
   evas_smart_data_t *esd;
   JSObject *parent;
   JSContext *cx;
   Eina_Bool suspended;
   jsval js_return;
   jsval argv[1];

   func_get_data(obj, &cx, &parent, &esd);
   if (!esd || !cx || !parent)
     return ;

   suspended = elixir_function_suspended(cx);
   if (suspended) elixir_function_start(cx);

   if (!evas_object_to_jsval(cx, obj, &argv[0]))
     return ;

   elixir_function_run(cx, esd->func[index], parent, 1, argv, &js_return);

   if (suspended) elixir_function_stop(cx);
}

#define FUNC_CALL_EO(Name) \
        void func_##Name(Evas_Object* obj) \
        { \
                func_evas_object_call(ESC_##Name, obj); \
        }

FUNC_CALL_EO(ADD);
FUNC_CALL_EO(DEL);
FUNC_CALL_EO(RAISE);
FUNC_CALL_EO(LOWER);
FUNC_CALL_EO(SHOW);
FUNC_CALL_EO(HIDE);
FUNC_CALL_EO(CLIP_UNSET);
FUNC_CALL_EO(CALCULATE);

static void
func_2evas_object_call(evas_smart_callback_t index, Evas_Object* obj1, Evas_Object* obj2)
{
   evas_smart_data_t *esd;
   JSContext *cx;
   JSObject *parent;
   Eina_Bool suspended;
   jsval js_return;
   jsval argv[2];

   func_get_data(obj1, &cx, &parent, &esd);
   if (!esd || !cx || !parent)
     return ;

   suspended = elixir_function_suspended(cx);
   if (suspended) elixir_function_start(cx);

   if (!evas_object_to_jsval(cx, obj1, &argv[0]))
     goto on_error;
   if (!evas_object_to_jsval(cx, obj2, &argv[1]))
     goto on_error;

   elixir_function_run(cx, esd->func[index], parent, 2, argv, &js_return);

 on_error:
   if (suspended) elixir_function_stop(cx);
}

#define FUNC_CALL_2EO(Name) \
        void func_##Name(Evas_Object* obj1, Evas_Object* obj2) \
        { \
                func_2evas_object_call(ESC_##Name, obj1, obj2); \
        }

FUNC_CALL_2EO(CLIP_SET);
FUNC_CALL_2EO(MEMBER_ADD);
FUNC_CALL_2EO(MEMBER_DEL);

static void
func_evas_object_2int_call(evas_smart_callback_t index, Evas_Object* obj, int i1, int i2)
{
   evas_smart_data_t *esd;
   JSObject *parent;
   JSContext *cx;
   Eina_Bool suspended;
   jsval js_return;
   jsval argv[3];

   func_get_data(obj, &cx, &parent, &esd);
   if (!esd || !cx || !parent)
     return ;

   suspended = elixir_function_suspended(cx);
   if (suspended) elixir_function_start(cx);

   if (!evas_object_to_jsval(cx, obj, &argv[0]))
     goto on_error;

   argv[1] = INT_TO_JSVAL(i1);
   argv[2] = INT_TO_JSVAL(i2);

   elixir_function_run(cx, esd->func[index], parent, 3, argv, &js_return);

 on_error:
   if (suspended) elixir_function_stop(cx);
}

#define FUNC_CALL_EO_2I(Name) \
        void func_##Name(Evas_Object* obj, Evas_Coord i1, Evas_Coord i2) \
        { \
                func_evas_object_2int_call(ESC_##Name, obj, i1, i2); \
        }

FUNC_CALL_EO_2I(MOVE);
FUNC_CALL_EO_2I(RESIZE);

static void
func_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   evas_smart_data_t *esd;
   JSObject *parent;
   JSContext *cx;
   Eina_Bool suspended;
   jsval js_return;
   jsval argv[5];

   func_get_data(obj, &cx, &parent, &esd);
   if (!esd || !cx || !parent)
     return ;

   suspended = elixir_function_suspended(cx);
   if (suspended) elixir_function_start(cx);

   if (!evas_object_to_jsval(cx, obj, &argv[0]))
     goto on_error;

   argv[1] = INT_TO_JSVAL(r);
   argv[2] = INT_TO_JSVAL(g);
   argv[3] = INT_TO_JSVAL(b);
   argv[4] = INT_TO_JSVAL(a);

   elixir_function_run(cx, esd->func[ESC_COLOR_SET], parent, 5, argv, &js_return);

 on_error:
   if (suspended) elixir_function_stop(cx);
}

#define ELX_CALL_CHECK(Func)			\
  if (!(Func)) return JS_FALSE;



static JSBool
elixir_evas_smart_class_new(JSContext *cx, uintN argc, jsval *vp)
{
   evas_smart_data_t *esd;
   Evas_Smart *es;
   JSClass *evas_smart_class;
   Evas_Smart_Class *fesc;
   Evas_Smart_Class esc;
   jsval any;
   JSFunction *fcts[17];
   elixir_value_t val[1];

   if (!elixir_params_check(cx, jsobject_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   ELX_CALL_CHECK(elixir_object_get_str(cx, val[0].v.obj, "name", &esc.name));
   ELX_CALL_CHECK(elixir_object_get_int(cx, val[0].v.obj, "version", &esc.version));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "add", fcts + ESC_ADD));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "del", fcts + ESC_DEL));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "move", fcts + ESC_MOVE));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "resize", fcts + ESC_RESIZE));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "show", fcts + ESC_SHOW));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "hide", fcts + ESC_HIDE));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "color_set", fcts + ESC_COLOR_SET));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "clip_set", fcts + ESC_CLIP_SET));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "clip_unset", fcts + ESC_CLIP_UNSET));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "calculate", fcts + ESC_CALCULATE));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "member_add", fcts + ESC_MEMBER_ADD));
   ELX_CALL_CHECK(elixir_object_get_fct(cx, val[0].v.obj, "member_del", fcts + ESC_MEMBER_DEL));

   if (JS_GetProperty(cx, val[0].v.obj, "data", &any) == JS_FALSE)
     any = JSVAL_NULL;

   /* FIXME: We don't take into account parent and callbacks right now. */
   if (esc.version != 4)
     {
	JS_ReportError(cx, "Wrong version for Smart Object Class.");
	return JS_FALSE;
     }

   esc.add = func_ADD;
   esc.del = func_DEL;
   esc.move = func_MOVE;
   esc.resize = func_RESIZE;
   esc.show = func_SHOW;
   esc.hide = func_HIDE;
   esc.color_set = func_color_set;
   esc.clip_set = func_CLIP_SET;
   esc.clip_unset = func_CLIP_UNSET;
   esc.calculate = func_CALCULATE;
   esc.member_add = func_MEMBER_ADD;
   esc.member_del = func_MEMBER_DEL;

   esd = malloc(sizeof (evas_smart_data_t) + sizeof (Evas_Smart_Class));
   if (!esd)
     return JS_FALSE;

   fesc = (Evas_Smart_Class*) (esd + 1);
   *fesc = esc;

   esd->func[ESC_ADD] = fcts[ESC_ADD];
   esd->func[ESC_DEL] = fcts[ESC_DEL];
   esd->func[ESC_LAYER_SET] = fcts[ESC_LAYER_SET];
   esd->func[ESC_RAISE] = NULL;
   esd->func[ESC_LOWER] = NULL;
   esd->func[ESC_STACK_ABOVE] = NULL;
   esd->func[ESC_STACK_BELOW] = NULL;
   esd->func[ESC_MOVE] = fcts[ESC_MOVE];
   esd->func[ESC_RESIZE] = fcts[ESC_RESIZE];
   esd->func[ESC_SHOW] = fcts[ESC_SHOW];
   esd->func[ESC_HIDE] = fcts[ESC_HIDE];
   esd->func[ESC_COLOR_SET] = fcts[ESC_COLOR_SET];
   esd->func[ESC_CLIP_SET] = fcts[ESC_CLIP_SET];
   esd->func[ESC_CLIP_UNSET] = fcts[ESC_CLIP_UNSET];
   esd->func[ESC_CALCULATE] = fcts[ESC_CALCULATE];
   esd->func[ESC_MEMBER_ADD] = fcts[ESC_MEMBER_ADD];
   esd->func[ESC_MEMBER_DEL] = fcts[ESC_MEMBER_DEL];

   fesc->data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), any, esd);
   fesc->name = malloc(strlen(esc.name) + 8);
   if (!fesc->name) fesc->name = esc.name;
   else sprintf((char*) fesc->name, "elixir/%s", esc.name);

   fesc->parent = NULL;
   fesc->callbacks = NULL;

   es = evas_smart_class_new(fesc);

   evas_smart_class = elixir_class_request("Evas_Smart", NULL);
   elixir_return_ptr(cx, vp, es, evas_smart_class);
   return JS_TRUE;
}

static JSBool
elixir_evas_smart_free(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Smart *es;
   void *data;
   evas_smart_data_t *esd;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_smart_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, es);

   data = evas_smart_data_get(es);
   esd = elixir_void_get_private(data);
   elixir_void_free(data);
   free(esd);

   evas_smart_free(es);

   return JS_TRUE;
}

static JSBool
elixir_evas_smart_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Smart *es;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_smart_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, es);

   data = evas_smart_data_get(es);

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(data));
   return JS_TRUE;
}

static JSBool
elixir_evas_object_smart_add(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Evas_Smart *es;
   Evas *e;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_evas_smart_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, e);
   GET_PRIVATE(cx, val[1].v.obj, es);

   eo = evas_object_smart_add(e, es);
   return evas_object_to_jsval(cx, eo, &(JS_RVAL(cx, vp)));
}

static void
_func_smart_cb(void *data, Evas_Object *obj, void *event_info)
{
   JSObject *parent;
   JSFunction *cb;
   JSContext *cx;
   Eina_Bool suspended;
   jsval js_return;
   jsval argv[3];

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);

   if (!cb || !cx || !parent)
     return ;

   suspended = elixir_function_suspended(cx);
   if (suspended) elixir_function_start(cx);

   argv[0] = elixir_void_get_jsval(data);

   if (!evas_object_to_jsval(cx, obj, &argv[1]))
     goto on_error;

   argv[2] = elixir_void_get_jsval(event_info);

   elixir_function_run(cx, cb, parent, 3, argv, &js_return);

 on_error:
   if (suspended) elixir_function_stop(cx);
}

static JSBool
elixir_evas_object_smart_callback_add(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *lst;
   Evas_Object *eo;
   const char *event;
   void *data;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _evas_object_smart_string_func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   event = elixir_get_string_bytes(val[1].v.str, NULL);
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, val[2].v.fct);

   /* Prevent leaking by linking the callback to the object lifetime */
   lst = evas_object_data_get(eo, "elixir_jsmap");
   lst = elixir_jsmap_add(lst, cx, elixir_void_get_jsval(data), data, -1);
   evas_object_data_set(eo, "elixir_jsmap", lst);

   evas_object_smart_callback_add(eo, event, _func_smart_cb, data);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_smart_callback_del(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *lst;
   Evas_Object *eo;
   const char *event;
   void *data;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _evas_object_smart_string_func_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   event = elixir_get_string_bytes(val[1].v.str, NULL);
   data = evas_object_smart_callback_del(eo, event, _func_smart_cb);

   /* Prevent leaking by linking the callback to the object lifetime */
   lst = evas_object_data_get(eo, "elixir_jsmap");
   lst = elixir_jsmap_del(lst, cx, elixir_void_get_jsval(data), -1);
   evas_object_data_set(eo, "elixir_jsmap", lst);

   JS_SET_RVAL(cx, vp, elixir_void_free(data));
   return JS_TRUE;
}

static JSBool
elixir_evas_object_smart_callback_call(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *event;
   void *event_info;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _evas_object_smart_string_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   event = elixir_get_string_bytes(val[1].v.str, NULL);
   event_info = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, NULL);

   evas_object_smart_callback_call(eo, event, event_info);

   elixir_void_free(event_info);

   return JS_TRUE;
}

static JSFunctionSpec   evas_smart_function[] = {
  ELIXIR_FN(evas_object_smart_callback_call, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_callback_del, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_callback_add, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_add, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_smart_data_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_smart_free, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_smart_class_new, 1, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_smart_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_smart_function))
     return EINA_FALSE;

   _evas_smart_parameter.class = elixir_class_request("Evas_Smart", NULL);

   return EINA_TRUE;
}

Eina_Bool
unload_evas_smart_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_smart_function[i].name)
     JS_DeleteProperty(cx, parent, evas_smart_function[i++].name);

   return EINA_TRUE;
}


#include <unistd.h>
#include <stdlib.h>
#include <Evas.h>

#include "evas-bindings.h"

#define STACK_MEM       (8L * 1024L)

struct _Mouse_Coord
{
   struct {
      int x, y;
   } output;
   struct {
      Evas_Coord x, y;
   } canvas;
};

static const elixir_parameter_t*	_evas_object_int_function_any[5] =
{
   &evas_object_parameter,
   &int_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_int_function_any[5] =
{
   &evas_parameter,
   &int_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};

#define	INTPROP(Var, Name, Inside) \
        JS_DefineProperty(cx, Inside, Name, INT_TO_JSVAL(Var), NULL, NULL, JSPROP_ENUMERATE | JSPROP_PERMANENT);
#define XYPROP(Struct, Name, Inside) \
	if (!elixir_new_xy(cx, Inside, Struct.x, Struct.y, Name)) \
		return EINA_FALSE;
#define DATAPROP(Data, Inside) \
	JS_DefineProperty(cx, Inside, "data", elixir_void_get_jsval(Data), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_PERMANENT);
#define BUTTONPROP(Var, Inside) INTPROP(Var, "button", Inside);
#define TIMESTAMPPROP(Var, Inside) INTPROP(Var, "timestamp", Inside);
#define FLAGSPROP(Var, Inside) INTPROP(Var, "flags", Inside);
#define OUTPUTPROP(Struct, Inside) XYPROP(Struct, "output", Inside);
#define CANVASPROP(Struct, Inside) XYPROP(Struct, "canvas", Inside);

static Eina_Bool
elixir_new_xy(JSContext *cx, JSObject *parent, int x, int y, const char *name)
{
   JSObject *js_obj;
   JSClass *evas_event_xy_class;

   evas_event_xy_class = elixir_class_request("evas_event_xy", NULL);

   js_obj = JS_NewObject(cx, evas_event_xy_class, NULL, NULL);
   if (!js_obj)
     return EINA_FALSE;
   JS_DefineProperty(cx, parent, name,
                     OBJECT_TO_JSVAL(js_obj),
                     NULL, NULL,
                     JSPROP_ENUMERATE | JSPROP_PERMANENT);

   INTPROP(x, "x", js_obj);
   INTPROP(y, "y", js_obj);

   return EINA_TRUE;
}

static Eina_Bool
elixir_new_event_mouse_coord(JSContext* cx, JSObject* parent, void* cd, const char* name)
{
   JSObject *js_eemmc;
   JSClass *evas_event_class;
   struct _Mouse_Coord*	coord = cd;

   evas_event_class = elixir_class_request("evas_event", NULL);

   js_eemmc = JS_NewObject(cx, evas_event_class, NULL, NULL);
   if (!js_eemmc)
     return EINA_FALSE;
   JS_DefineProperty(cx, parent, name,
                     OBJECT_TO_JSVAL(js_eemmc),
                     NULL, NULL,
                     JSPROP_ENUMERATE | JSPROP_PERMANENT);

   OUTPUTPROP(coord->output, js_eemmc);
   CANVASPROP(coord->canvas, js_eemmc);

   return EINA_TRUE;
}

static Eina_Bool
elixir_new_event_mouse_down(JSContext *cx, void *veemd, JSObject *js_obj)
{
   Evas_Event_Mouse_Down *eemd = veemd;

   BUTTONPROP(eemd->button, js_obj);
   OUTPUTPROP(eemd->output, js_obj);
   CANVASPROP(eemd->canvas, js_obj);
   DATAPROP(eemd->data, js_obj);
   FLAGSPROP(eemd->flags, js_obj);
   TIMESTAMPPROP(eemd->timestamp, js_obj);

   return EINA_TRUE;
}

static Eina_Bool
elixir_new_event_mouse_up(JSContext *cx, void *veemu, JSObject *js_obj)
{
   Evas_Event_Mouse_Up *eemu = veemu;

   BUTTONPROP(eemu->button, js_obj);
   OUTPUTPROP(eemu->output, js_obj);
   CANVASPROP(eemu->canvas, js_obj);
   DATAPROP(eemu->data, js_obj);
   FLAGSPROP(eemu->flags, js_obj);
   TIMESTAMPPROP(eemu->timestamp, js_obj);

   return EINA_TRUE;
}

static Eina_Bool
elixir_new_event_mouse_in(JSContext *cx, void *veemi, JSObject* js_obj)
{
   Evas_Event_Mouse_In *eemi = veemi;

   BUTTONPROP(eemi->buttons, js_obj);
   OUTPUTPROP(eemi->output, js_obj);
   CANVASPROP(eemi->canvas, js_obj);
   DATAPROP(eemi->data, js_obj);
   TIMESTAMPPROP(eemi->timestamp, js_obj);

   return EINA_TRUE;
}

static Eina_Bool
elixir_new_event_mouse_out(JSContext *cx, void *veemo, JSObject *js_obj)
{
   Evas_Event_Mouse_Out *eemo = veemo;

   BUTTONPROP(eemo->buttons, js_obj);
   OUTPUTPROP(eemo->output, js_obj);
   CANVASPROP(eemo->canvas, js_obj);
   DATAPROP(eemo->data, js_obj);
   TIMESTAMPPROP(eemo->timestamp, js_obj);

   return EINA_TRUE;
}

static Eina_Bool
elixir_new_event_mouse_move(JSContext *cx, void *veemm, JSObject *js_obj)
{
   Evas_Event_Mouse_Move *eemm = veemm;

   BUTTONPROP(eemm->buttons, js_obj);

   if (!elixir_new_event_mouse_coord(cx, js_obj, &(eemm->cur), "cur"))
     return EINA_FALSE;

   if (!elixir_new_event_mouse_coord(cx, js_obj, &(eemm->prev), "prev"))
     return EINA_FALSE;

   DATAPROP(eemm->data, js_obj);
   TIMESTAMPPROP(eemm->timestamp, js_obj);

   return EINA_TRUE;
}

static Eina_Bool
elixir_new_event_mouse_wheel(JSContext *cx, void *veemw, JSObject *js_obj)
{
   Evas_Event_Mouse_Wheel *eemw = veemw;

   INTPROP(eemw->direction, "direction", js_obj);
   INTPROP(eemw->z, "z", js_obj);
   OUTPUTPROP(eemw->output, js_obj);
   CANVASPROP(eemw->canvas, js_obj);
   DATAPROP(eemw->data, js_obj);
   TIMESTAMPPROP(eemw->timestamp, js_obj);

   return EINA_TRUE;
}

#define JS_CALL_CHECK(Func)			\
  if (Func == EINA_FALSE) return EINA_FALSE;

static Eina_Bool
elixir_new_event_key_down(JSContext *cx, void *veekd, JSObject *js_obj)
{
   Evas_Event_Key_Down *eekd = veekd;

   JS_CALL_CHECK(elixir_add_str_prop(cx, js_obj, "keyname", eekd->keyname));

   DATAPROP(eekd->data, js_obj);

   JS_CALL_CHECK(elixir_add_str_prop(cx, js_obj, "key", eekd->key));
   JS_CALL_CHECK(elixir_add_str_prop(cx, js_obj, "string", eekd->string));
   JS_CALL_CHECK(elixir_add_str_prop(cx, js_obj, "compose", eekd->compose));

   TIMESTAMPPROP(eekd->timestamp, js_obj);

   return EINA_TRUE;
}

static Eina_Bool
elixir_new_event_key_up(JSContext *cx, void *veeku, JSObject *js_obj)
{
   Evas_Event_Key_Up *eeku = veeku;

   JS_CALL_CHECK(elixir_add_str_prop(cx, js_obj, "keyname", eeku->keyname));

   DATAPROP(eeku->data, js_obj);

   JS_CALL_CHECK(elixir_add_str_prop(cx, js_obj, "key", eeku->key));
   JS_CALL_CHECK(elixir_add_str_prop(cx, js_obj, "string", eeku->string));
   JS_CALL_CHECK(elixir_add_str_prop(cx, js_obj, "compose", eeku->compose));

   TIMESTAMPPROP(eeku->timestamp, js_obj);

   return EINA_TRUE;
}

struct	_js_callback
{
   JSFunction*		fct;
   Evas_Callback_Type	type;
};

typedef Eina_Bool (*elixir_new_event)(JSContext *cx, void* ee, JSObject* js_obj);

static void
elixir_evas_event_callback(void *data, Evas *e, void *event_info)
{
   JSObject *obj_event = NULL;
   struct _js_callback *cb;
   JSObject *parent;
   JSContext *cx;
   Eina_Bool suspended;
   jsval js_return;
   jsval argv[3];

   cb = elixir_void_get_private(data);
   if (!cb)
     return ;

   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent)
     return ;

   suspended = elixir_function_suspended(cx);
   if (suspended)
     elixir_function_start(cx);

   argv[2] = JSVAL_NULL;
   elixir_rval_register(cx, argv + 2);

   switch (cb->type)
     {
     case EVAS_CALLBACK_CANVAS_FOCUS_IN:
     case EVAS_CALLBACK_CANVAS_FOCUS_OUT:
     case EVAS_CALLBACK_RENDER_FLUSH_PRE:
     case EVAS_CALLBACK_RENDER_FLUSH_POST:
	break;
     case EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_IN:
     case EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_OUT:
	if (!evas_object_to_jsval(cx, event_info, argv + 2))
	  goto on_error;
	break;
     case EVAS_CALLBACK_MOUSE_IN:
     case EVAS_CALLBACK_MOUSE_OUT:
     case EVAS_CALLBACK_MOUSE_DOWN:
     case EVAS_CALLBACK_MOUSE_UP:
     case EVAS_CALLBACK_MOUSE_MOVE:
     case EVAS_CALLBACK_MOUSE_WHEEL:
     case EVAS_CALLBACK_KEY_DOWN:
     case EVAS_CALLBACK_KEY_UP:
     case EVAS_CALLBACK_FREE:
     case EVAS_CALLBACK_FOCUS_IN:
     case EVAS_CALLBACK_FOCUS_OUT:
     case EVAS_CALLBACK_SHOW:
     case EVAS_CALLBACK_HIDE:
     case EVAS_CALLBACK_MOVE:
     case EVAS_CALLBACK_RESIZE:
     case EVAS_CALLBACK_RESTACK:
	JS_ReportError(cx, "Unauthorized callback type: %i", cb->type);
	goto on_error;
     default:
	JS_ReportError(cx, "Unknow callback type: %i", cb->type);
	goto on_error;
     }

   if (!evas_to_jsval(cx, e, argv + 1))
     goto on_error;
   elixir_rval_register(cx, argv + 1);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);

   elixir_function_run(cx, cb->fct, parent, 3, argv, &js_return);

   elixir_rval_delete(cx, argv + 1);
   elixir_rval_delete(cx, argv);

 on_error:
   elixir_rval_delete(cx, argv + 2);

   if (obj_event)
     elixir_object_unregister(cx, &obj_event);

   if (suspended)
     elixir_function_stop(cx);
}

static void
elixir_object_event_callback(void *data, Evas *e, Evas_Object *obj, void *event)
{
   elixir_new_event fct = NULL;
   JSObject *obj_event = NULL;
   JSClass *evas_event_class;
   struct _js_callback *cb;
   JSObject *parent;
   JSContext *cx;
   Eina_Bool suspended;
   jsval js_return;
   jsval argv[4];

   cb = elixir_void_get_private(data);
   if (!cb)
     return ;

   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent)
     return ;

   suspended = elixir_function_suspended(cx);
   if (suspended)
     elixir_function_start(cx);

   switch (cb->type)
     {
     case EVAS_CALLBACK_MOUSE_IN:
	fct = elixir_new_event_mouse_in;
        break;
     case EVAS_CALLBACK_MOUSE_OUT:
	fct = elixir_new_event_mouse_out;
        break;
     case EVAS_CALLBACK_MOUSE_DOWN:
	fct = elixir_new_event_mouse_down;
        break;
     case EVAS_CALLBACK_MOUSE_UP:
	fct = elixir_new_event_mouse_up;
        break;
     case EVAS_CALLBACK_MOUSE_MOVE:
	fct = elixir_new_event_mouse_move;
        break;
     case EVAS_CALLBACK_MOUSE_WHEEL:
	fct = elixir_new_event_mouse_wheel;
        break;
     case EVAS_CALLBACK_KEY_DOWN:
	fct = elixir_new_event_key_down;
        break;
     case EVAS_CALLBACK_KEY_UP:
	fct = elixir_new_event_key_up;
        break;
     /* Folowing event dont have an event parameter */
     case EVAS_CALLBACK_FOCUS_IN:
     case EVAS_CALLBACK_FOCUS_OUT:
     case EVAS_CALLBACK_FREE:
     case EVAS_CALLBACK_SHOW:
     case EVAS_CALLBACK_HIDE:
     case EVAS_CALLBACK_MOVE:
     case EVAS_CALLBACK_RESIZE:
     case EVAS_CALLBACK_RESTACK:
        break;
     case EVAS_CALLBACK_CANVAS_FOCUS_IN:
     case EVAS_CALLBACK_CANVAS_FOCUS_OUT:
     case EVAS_CALLBACK_RENDER_FLUSH_PRE:
     case EVAS_CALLBACK_RENDER_FLUSH_POST:
     case EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_IN:
     case EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_OUT:
	JS_ReportError(cx, "Unauthorized callback type: %i", cb->type);
	goto on_error_empty;
     default:
	JS_ReportError(cx, "Unknow callback type: %i", cb->type);
	goto on_error_empty;
     }

   if (!evas_to_jsval(cx, e, argv + 1))
     goto on_error_empty;
   elixir_rval_register(cx, argv + 1);

   if (!evas_object_to_jsval(cx, obj, argv + 2))
     goto on_error;
   elixir_rval_register(cx, argv + 2);

   argv[3] = JSVAL_NULL;
   if (fct)
     {
        evas_event_class = elixir_class_request("evas_object_event", NULL);

        obj_event = JS_NewObject(cx, evas_event_class, NULL, NULL);
        if (!elixir_object_register(cx, &obj_event, NULL))
          goto on_error2;

        if (!fct(cx, event, obj_event))
          goto on_error2;

        argv[3] = OBJECT_TO_JSVAL(obj_event);
     }
   elixir_rval_register(cx, argv + 3);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);

   elixir_function_run(cx, cb->fct, parent, 4, argv, &js_return);

   elixir_rval_delete(cx, argv + 3);
   elixir_rval_delete(cx, argv);

 on_error2:
   elixir_rval_delete(cx, argv + 2);
 on_error:
   elixir_rval_delete(cx, argv + 1);
 on_error_empty:
   if (obj_event)
     elixir_object_unregister(cx, &obj_event);

   if (suspended)
     elixir_function_stop(cx);
}

/* Crach when last arg is undefined. */
static JSBool
elixir_evas_object_event_callback_add(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *lst;
   Evas_Object *know = NULL;
   Evas_Callback_Type type;
   struct _js_callback *cb;
   void *cdata;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _evas_object_int_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   type = val[1].v.num;

   cb = malloc(sizeof (struct _js_callback));
   if (!cb) return JS_FALSE;
   cb->fct = val[2].v.fct;
   cb->type = type;

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, cb);

   /* Prevent leaking by linking the callback to the object lifetime */
   lst = evas_object_data_get(know, "elixir_jsmap");
   lst = elixir_jsmap_add(lst, cx, elixir_void_get_jsval(cdata), cdata, type);
   evas_object_data_set(know, "elixir_jsmap", lst);

   evas_object_event_callback_add(know, type, elixir_object_event_callback, cdata);

   return JS_TRUE;
}

static JSBool
elixir_evas_event_callback_add(JSContext *cx, uintN argc, jsval *vp)
{
   Evas *know = NULL;
   Evas_Callback_Type type;
   struct _js_callback *cb;
   void *cdata;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _evas_int_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   type = val[1].v.num;

   cb = malloc(sizeof (struct _js_callback));
   if (!cb) return JS_FALSE;
   cb->fct = val[2].v.fct;
   cb->type = type;

   cdata = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, cb);
   evas_event_callback_add(know, type, elixir_evas_event_callback, cdata);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_event_callback_del(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *lst;
   Evas_Object *know = NULL;
   void *cdata;
   Evas_Callback_Type type;
   jsval ret;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _evas_object_int_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   type = val[1].v.num;

   /* Prevent leaking */
   lst = evas_object_data_get(know, "elixir_jsmap");
   cdata = elixir_jsmap_find(&lst, val[3].v.any, type);

   cdata = evas_object_event_callback_del_full(know, type, elixir_object_event_callback, cdata);
   if (elixir_void_get_private(cdata))
     free(elixir_void_get_private(cdata));

   lst = elixir_jsmap_del(lst, cx, elixir_void_get_jsval(cdata), type);
   evas_object_data_set(know, "elixir_jsmap", lst);

   ret = elixir_void_free(cdata);

   JS_SET_RVAL(cx, vp, ret);
   return JS_TRUE;
}

static JSBool
elixir_evas_event_callback_del(JSContext *cx, uintN argc, jsval *vp)
{
   Evas *know = NULL;
   void *cdata;
   Evas_Callback_Type type;
   jsval ret;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _evas_int_function_any, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   type = val[1].v.num;

   cdata = evas_event_callback_del(know, type, elixir_evas_event_callback);
   if (elixir_void_get_private(cdata))
     free(elixir_void_get_private(cdata));

   ret = elixir_void_free(cdata);

   JS_SET_RVAL(cx, vp, ret);
   return JS_TRUE;
}

Eina_Bool
load_evas_event_binding(JSContext *cx, JSObject *parent)
{
   static JSFunctionSpec	evas_event_functions[] = {
     ELIXIR_FN(evas_object_event_callback_add, 4, JSPROP_ENUMERATE, 0 ),
     ELIXIR_FN(evas_object_event_callback_del, 3, JSPROP_ENUMERATE, 0 ),
     ELIXIR_FN(evas_event_callback_add, 4, JSPROP_ENUMERATE, 0 ),
     ELIXIR_FN(evas_event_callback_del, 4, JSPROP_ENUMERATE, 0 ),
     JS_FS_END
   };

   if (!JS_DefineFunctions(cx, parent, evas_event_functions))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_event_binding(JSContext *cx, JSObject *parent)
{
   JS_DeleteProperty(cx, parent, "evas_object_event_callback_add");
   JS_DeleteProperty(cx, parent, "evas_object_event_callback_del");
   JS_DeleteProperty(cx, parent, "evas_event_callback_add");
   JS_DeleteProperty(cx, parent, "evas_event_callback_del");

   return EINA_TRUE;
}


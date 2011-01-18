#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <Ecore.h>

#include "Elixir.h"

static elixir_parameter_t               _ecore_event_handler_parameter = {
   "Ecore_Event_Handler", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_event_parameter = {
   "Ecore_Event", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_event_filter_parameter = {
   "Ecore_Event_Filter", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_idler_parameter = {
   "Ecore_Idler", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_idle_enterer_parameter = {
   "Ecore_Idle_Enterer", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_idle_exiter_parameter = {
   "Ecore_Idle_Exiter", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_timer_parameter = {
   "Ecore_Timer", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_animator_parameter = {
   "Ecore_Animator", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_thread_parameter = {
   "Ecore_Thread", JOBJECT, NULL
};

static elixir_parameter_t               _ecore_job_parameter = {
   "Ecore_Job", JOBJECT, NULL
};

static const elixir_parameter_t*        _int_func_any_params[4] = {
   &int_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};

static const elixir_parameter_t*        _int_any_func_any_params[5] = {
   &int_parameter,
   &any_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};

static const elixir_parameter_t*        _3func_any_params[5] = {
   &function_parameter,
   &function_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};

static const elixir_parameter_t*        _func_any_params[3] = {
   &function_parameter,
   &any_parameter,
   NULL
};

static const elixir_parameter_t*        _double_func_any_params[4] = {
   &double_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_event_handler_params[2] = {
   &_ecore_event_handler_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_event_params[2] = {
   &_ecore_event_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_event_filter_params[2] = {
   &_ecore_event_filter_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_idler_params[2] = {
   &_ecore_idler_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_idle_enterer_params[2] = {
   &_ecore_idle_enterer_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_idle_exiter_params[2] = {
   &_ecore_idle_exiter_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_timer_params[2] = {
   &_ecore_timer_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_timer_double_params[3] = {
   &_ecore_timer_parameter,
   &double_parameter,
   NULL
};

static const elixir_parameter_t*        _ecore_animator_params[2] = {
   &_ecore_animator_parameter,
   NULL
};

static const elixir_parameter_t*	_ecore_thread_params[2] = {
  &_ecore_thread_parameter,
  NULL
};

static const elixir_parameter_t*              _ecore_job_params[2] = {
   &_ecore_job_parameter,
   NULL
};

/* Need to be correctly done. */
static JSBool
elixir_ecore_event_current_event_get(JSContext *cx, uintN argc, jsval *vp)
{
   void *data;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   data = ecore_event_current_event_get();

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(data));
   return JS_TRUE;
}

static Eina_Bool
_elixir_ecore_maybe_gc(__UNUSED__ void *data)
{
   elixir_suspended_gc();

   return ECORE_CALLBACK_RENEW;
}

static JSBool
elixir_ecore_init(JSContext *cx, uintN argc, jsval *vp)
{
   int r;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   r = ecore_init();

   ecore_idle_enterer_add(_elixir_ecore_maybe_gc, NULL);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(r));
   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_shutdown, elixir_int_params_void);
FAST_CALL_PARAMS(ecore_event_type_new, elixir_int_params_void);
FAST_CALL_PARAMS(ecore_event_current_type_get, elixir_int_params_void);

FAST_CALL_PARAMS(ecore_app_restart, elixir_void_params_void);
FAST_CALL_PARAMS(ecore_main_loop_iterate, elixir_void_params_void);
FAST_CALL_PARAMS(ecore_main_loop_begin, elixir_void_params_void);
FAST_CALL_PARAMS(ecore_main_loop_quit, elixir_void_params_void);

static JSBool
elixir_double_params_void(double (*func)(void),
                          JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   return JS_NewNumberValue(cx, func(), &(JS_RVAL(cx, vp)));
}

FAST_CALL_PARAMS(ecore_animator_frametime_get, elixir_double_params_void);
FAST_CALL_PARAMS(ecore_time_get, elixir_double_params_void);
FAST_CALL_PARAMS(ecore_loop_time_get, elixir_double_params_void);

static JSBool
elixir_void_params_animator(void (*func)(Ecore_Animator *anim),
			    JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Animator *anim;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_animator_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, anim);
   func(anim);

   return EINA_TRUE;
}

FAST_CALL_PARAMS(ecore_animator_freeze, elixir_void_params_animator);
FAST_CALL_PARAMS(ecore_animator_thaw, elixir_void_params_animator);

static JSBool
elixir_ecore_animator_frametime_set(JSContext *cx, uintN argc, jsval *vp)
{
   double frametime;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, double_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   frametime = val[0].v.dbl;

   ecore_animator_frametime_set(frametime);

   return JS_TRUE;
}

static Eina_Bool
_ecore_jsval_to_boolean(JSContext *cx, jsval val)
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
_ecore_event_handler_func(void *data, int type, void *event)
{
   JSFunction *cb;
   JSContext *cx;
   JSObject *parent;
   Eina_Bool ret = EINA_FALSE;
   Eina_Bool suspended;
   jsval js_return = JSVAL_NULL;
   jsval argv[3];

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);

   if (!cx || !parent || !cb)
     return 0;

   suspended = elixir_function_suspended(cx);
   if (suspended)
     elixir_function_start(cx);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);
   argv[1] = INT_TO_JSVAL(type);
   elixir_rval_register(cx, argv + 1);
   argv[2] = elixir_void_get_jsval(event);
   elixir_rval_register(cx, argv + 2);

   elixir_rval_register(cx, &js_return);

   if (elixir_function_run(cx, cb, parent, 3, argv, &js_return))
     ret = _ecore_jsval_to_boolean(cx, js_return);

   elixir_rval_delete(cx, &js_return);
   elixir_rval_delete(cx, argv + 2);
   elixir_rval_delete(cx, argv + 1);
   elixir_rval_delete(cx, argv);

  end:
   if (suspended)
     elixir_function_stop(cx);
   return ret;
}

static JSBool
elixir_ecore_event_handler_add(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Event_Handler *hdl;
   void *data;
   int type;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _int_func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   type = val[0].v.num;
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);
   hdl = ecore_event_handler_add(type, _ecore_event_handler_func, data);

   elixir_return_ptr(cx, vp, hdl, elixir_class_request("Ecore_Event_Handler", NULL));
   return JS_TRUE;
}

static JSBool
elixir_ecore_event_handler_del(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Event_Handler *hdl;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_event_handler_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, hdl);
   data = ecore_event_handler_del(hdl);

   JS_SET_RVAL(cx, vp, data ? elixir_void_free(data) : JSVAL_NULL);
   return JS_TRUE;
}

static void
_ecore_event_func_free(void* data, void* ev)
{
   JSFunction *cb;

   cb = elixir_void_get_private(ev);
   if (cb)
     {
        JSContext *cx;
        JSObject *parent;
        jsval js_return = JSVAL_NULL;
        jsval argv[2];

        cx = elixir_void_get_cx(ev);
        /* FIXME: Don't know if it's better to take the parent from ev or data. */
        parent = elixir_void_get_parent(ev);

	elixir_function_start(cx);

        argv[0] = elixir_void_get_jsval(data);
        elixir_rval_register(cx, argv);
        argv[1] = elixir_void_get_jsval(ev);
        elixir_rval_register(cx, argv + 1);

        elixir_function_run(cx, cb, parent, 2, argv, &js_return);

        elixir_rval_delete(cx, argv);
        elixir_rval_delete(cx, argv + 1);

	elixir_function_stop(cx);
     }

   elixir_void_free(data);
   elixir_void_free(ev);
}

static JSBool
elixir_ecore_event_add(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Event *ee;
   void *ev;
   void *data;
   int type;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _int_any_func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   type = val[0].v.num;
   ev = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[1].v.any, val[2].v.fct);
   if (val[3].v.any != JSVAL_NULL)
     data = NULL;
   else
     /* FIXME: This will generate memory leak, if the JavaScript ecore_event_del is not called. */
     data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, NULL);

   ee = ecore_event_add(type, ev, _ecore_event_func_free, data);

   elixir_return_ptr(cx, vp, ee, elixir_class_request("Ecore_Event", NULL));
   return JS_TRUE;
}

static JSBool
elixir_ecore_event_del(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Event *ee;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_event_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);
   data = ecore_event_del(ee);

   JS_SET_RVAL(cx, vp, data ? elixir_void_free(data) : JSVAL_NULL);
   return JS_TRUE;
}

struct _ecore_filter_func_s
{
   JSFunction *start;
   JSFunction *filter;
   JSFunction *end;
};

static void*
_ecore_filter_func_start(void *data)
{
   struct _ecore_filter_func_s *effs;
   JSContext *cx;
   JSObject *parent;
   void *res = NULL;
   jsval js_return;
   jsval argv[1];

   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent)
     return NULL;

   elixir_function_start(cx);

   effs = elixir_void_get_private(data);

   if (effs && effs->start)
     {
        argv[0] = elixir_void_get_jsval(data);
        elixir_rval_register(cx, argv);

        if (elixir_function_run(cx, effs->start, parent, 1, argv, &js_return))
	  /* Return loop_data. */
	  res = elixir_void_new(cx, parent, js_return, NULL);
        elixir_rval_delete(cx, argv);
     }

   elixir_function_stop(cx);

   return res;
}

static Eina_Bool
_ecore_filter_func_filter(void *data, void *loop_data, int type, void *event)
{
   struct _ecore_filter_func_s *effs;
   JSContext *cx;
   JSObject *parent;
   jsval js_return = JSVAL_NULL;
   jsval argv[4];
   Eina_Bool ret = EINA_TRUE;

   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent)
     return EINA_TRUE;

   elixir_function_start(cx);

   effs = elixir_void_get_private(data);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);
   argv[1] = elixir_void_get_jsval(loop_data);
   elixir_rval_register(cx, argv + 1);
   argv[2] = INT_TO_JSVAL(type);
   elixir_rval_register(cx, argv + 2);
   argv[3] = elixir_void_get_jsval(event);
   elixir_rval_register(cx, argv + 3);

   elixir_rval_register(cx, &js_return);

   if (effs && effs->filter)
     {
        ret = 0;

        if (elixir_function_run(cx, effs->filter, parent, 4, argv, &js_return))
	  ret = _ecore_jsval_to_boolean(cx, js_return);
     }

   /* If we return 0, the event will be deleted. Free data before loosing reference. */
   if (!ret) elixir_void_free(event);

   elixir_rval_delete(cx, &js_return);
   elixir_rval_delete(cx, argv + 3);
   elixir_rval_delete(cx, argv + 2);
   elixir_rval_delete(cx, argv + 1);
   elixir_rval_delete(cx, argv);

   elixir_function_stop(cx);

   return ret;
}

static void
_ecore_filter_func_end(void *data, void *loop_data)
{
   struct _ecore_filter_func_s *effs;
   JSContext *cx;
   JSObject *parent;
   jsval js_return;
   jsval argv[2];

   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent)
     return ;

   elixir_function_start(cx);

   effs = elixir_void_get_private(data);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);
   argv[1] = elixir_void_get_jsval(loop_data);
   elixir_rval_register(cx, argv + 1);

   if (effs && effs->end)
     elixir_function_run(cx, effs->end, parent, 2, argv, &js_return);

   elixir_rval_delete(cx, argv + 1);
   elixir_rval_delete(cx, argv);

   elixir_void_free(loop_data);

   elixir_function_stop(cx);
}

static JSBool
elixir_ecore_event_filter_add(JSContext *cx, uintN argc, jsval *vp)
{
   struct _ecore_filter_func_s *effs;
   Ecore_Event_Filter *eef;
   void *data;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _3func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   effs = malloc(sizeof(struct _ecore_filter_func_s));
   if (!effs)
     return JS_FALSE;

   effs->start = val[0].v.fct;
   effs->filter = val[1].v.fct;
   effs->end = val[2].v.fct;

   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, effs);

   eef = ecore_event_filter_add(_ecore_filter_func_start, _ecore_filter_func_filter, _ecore_filter_func_end, data);

   elixir_return_ptr(cx, vp, eef, elixir_class_request("Ecore_Event_Filter", NULL));
   return JS_TRUE;
}

static JSBool
elixir_ecore_event_filter_del(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Event_Filter *eef;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_event_filter_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eef);
   data = ecore_event_filter_del(eef);

   JS_SET_RVAL(cx, vp, data ? elixir_void_free(data) : JSVAL_NULL);
   return JS_TRUE;
}

typedef struct _Elixir_Ecore_Handler Elixir_Ecore_Handler;
struct _Elixir_Ecore_Handler
{
   JSFunction *cb;
   JSObject *obj;
   Eina_Bool running : 1;
   Eina_Bool delete : 1;
};

static Eina_Bool
_ecore_int_func_void(void* data)
{
   Elixir_Ecore_Handler *eeh;
   JSContext *cx;
   JSObject *parent;
   Eina_Bool ret = ECORE_CALLBACK_CANCEL;
   jsval js_return = JSVAL_NULL;
   jsval argv[1];

   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent)
     return ECORE_CALLBACK_CANCEL;

   elixir_function_start(cx);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);
   elixir_rval_register(cx, &js_return);

   eeh = elixir_void_get_private(data);
   if (eeh && eeh->cb)
     {
	eeh->running = EINA_TRUE;

	if (elixir_function_run(cx, eeh->cb, parent, 1, argv, &js_return))
	  ret = _ecore_jsval_to_boolean(cx, js_return);

	eeh->running = EINA_FALSE;
     }

   elixir_rval_delete(cx, argv);
   elixir_rval_delete(cx, &js_return);

   /* The event will be deleted after this call, cleaning memory is a good idea. */
   if (eeh && (!ret || eeh->delete))
     {
	JS_SetPrivate(cx, eeh->obj, NULL);

	elixir_object_unregister(cx, &eeh->obj);

	free(eeh);

	elixir_void_free(data);
     }

   elixir_function_stop(cx);
   return ret;
}

typedef void *(*Elixir_Ecore_Add)(Eina_Bool (*callback)(void *data), const void *data);
typedef void *(*Elixir_Ecore_Del)(void *handler);

static JSBool
elixir_ecore_add(JSContext *cx, uintN argc, jsval *vp,
		 Elixir_Ecore_Add func,
		 const char *parent_class)
{
   Elixir_Ecore_Handler *eeh;
   void *handler;
   void *data;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   eeh = malloc(sizeof (Elixir_Ecore_Handler));
   if (!eeh)
     return JS_FALSE;

   eeh->cb = val[0].v.fct;
   eeh->obj = NULL;
   eeh->running = EINA_FALSE;
   eeh->delete = EINA_FALSE;

   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[1].v.any, eeh);
   if (!data)
     {
	free(eeh);
	return JS_FALSE;
     }

   handler = func(_ecore_int_func_void, data);

   eeh->obj = elixir_return_ptr(cx, vp, handler, elixir_class_request(parent_class, NULL));
   elixir_object_register(cx, &eeh->obj, NULL);
   return JS_TRUE;
}

static JSBool
elixir_ecore_del(JSContext *cx, uintN argc, jsval *vp,
		 Elixir_Ecore_Del func,
		 const elixir_parameter_t *params[])
{
   Elixir_Ecore_Handler *eeh = NULL;
   void *data;
   void *handler;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, handler);
   if (handler) data = func(handler);

   eeh = elixir_void_get_private(data);
   if (eeh)
     {
	if (eeh->running)
	  {
	     eeh->delete = EINA_TRUE;
	     JS_SET_RVAL(cx, vp, JSVAL_NULL);
	  }
	else
	  {
	     JS_SetPrivate(cx, val[0].v.obj, NULL);

	     elixir_object_unregister(cx, &eeh->obj);

	     free(eeh);

	     JS_SET_RVAL(cx, vp, elixir_void_free(data));
	  }
     }
   else
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
     }

   return JS_TRUE;
}

static JSBool
elixir_ecore_idler_add(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_add(cx, argc, vp, (Elixir_Ecore_Add)ecore_idler_add, "Ecore_Idler");
}

static JSBool
elixir_ecore_idler_del(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_del(cx, argc, vp, (Elixir_Ecore_Del)ecore_idler_del, _ecore_idler_params);
}

static JSBool
elixir_ecore_idle_enterer_add(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_add(cx, argc, vp, (Elixir_Ecore_Add)ecore_idle_enterer_add, "Ecore_Idle_Enterer");
}

static JSBool
elixir_ecore_idle_enterer_del(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_del(cx, argc, vp, (Elixir_Ecore_Del)ecore_idle_enterer_del, _ecore_idle_enterer_params);
}

static JSBool
elixir_ecore_idle_exiter_add(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_add(cx, argc, vp, (Elixir_Ecore_Add)ecore_idle_exiter_add, "Ecore_Idle_Exiter");
}

static JSBool
elixir_ecore_idle_exiter_del(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_del(cx, argc, vp, (Elixir_Ecore_Del)ecore_idle_exiter_del, _ecore_idle_exiter_params);
}

static JSBool
elixir_ecore_animator_add(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_add(cx, argc, vp, (Elixir_Ecore_Add)ecore_animator_add, "Ecore_Animator");
}

static JSBool
elixir_ecore_animator_del(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_del(cx, argc, vp, (Elixir_Ecore_Del)ecore_animator_del, _ecore_animator_params);
}

static JSBool
elixir_ecore_timer_double_func_any(Ecore_Timer *(*func)(double dbl, Eina_Bool (*func)(void *data), const void *data),
				   JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_Ecore_Handler *eeh;
   Ecore_Timer *et;
   void *data;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _double_func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   eeh = malloc(sizeof (Elixir_Ecore_Handler));
   if (!eeh)
     return JS_FALSE;

   eeh->cb = val[1].v.fct;
   eeh->obj = NULL;
   eeh->running = EINA_FALSE;
   eeh->delete = EINA_FALSE;

   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, eeh);
   if (!data)
     {
	free(eeh);
	return JS_FALSE;
     }

   et = func(val[0].v.dbl, _ecore_int_func_void, data);
   eeh->obj = elixir_return_ptr(cx, vp, et, elixir_class_request("Ecore_Timer", NULL));
   elixir_object_register(cx, &eeh->obj, NULL);
   return JS_TRUE;

}

static JSBool
elixir_ecore_timer_add(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_timer_double_func_any(ecore_timer_add, cx, argc, vp);
}

static JSBool
elixir_ecore_timer_loop_add(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_timer_double_func_any(ecore_timer_loop_add, cx, argc, vp);
}

static JSBool
elixir_void_ecore_timer(void (*func)(Ecore_Timer *timer),
                        JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Timer *timer;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_timer_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, timer);

   func(timer);

   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_timer_freeze, elixir_void_ecore_timer);
FAST_CALL_PARAMS(ecore_timer_thaw, elixir_void_ecore_timer);

static JSBool
elixir_ecore_timer_del(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_ecore_del(cx, argc, vp, (Elixir_Ecore_Del)ecore_timer_del, _ecore_timer_params);
}

static JSBool
elixir_ecore_timer_pending_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Timer *et;
   double result;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_timer_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, et);

   result = ecore_timer_pending_get(et);

   return JS_NewNumberValue(cx, result, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_void_ecore_timer_double(void (*func)(Ecore_Timer *timer, double time),
			       JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Timer *et;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_timer_double_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, et);

   func(et, val[1].v.dbl);

   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_timer_interval_set, elixir_void_ecore_timer_double);
FAST_CALL_PARAMS(ecore_timer_delay, elixir_void_ecore_timer_double);

typedef struct _Elixir_Thread_Data Elixir_Thread_Data;
struct _Elixir_Thread_Data
{
   JSFunction *func_heavy;
   JSFunction *func_end;
   JSFunction *func_cancel;
   Elixir_Runtime *runtime;
   JSObject *obj_heavy;
   JSObject *obj_end;
   JSObject *obj_cancel;
   JSContext *main;
   JSObject *result;
};

static void
_elixir_func_heavy(void *data, Ecore_Thread *thread)
{
   Elixir_Thread_Data *dt;
   JSContext *cx;
   JSObject *parent;
   jsval argv[2];
   jsval rval = JSVAL_VOID;

   cx = elixir_void_get_cx(data);
   if (!cx) return ;

   elixir_lock_cx(cx);

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);

   if (!parent
       || !dt)
     goto on_error;

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);

   if (elixir_api_version_get() == 0)
     {
        elixir_function_run(cx, dt->func_heavy, parent, 1, argv, &rval);
     }
   else
     {
        elixir_return_ptr(cx, &argv[1], thread, elixir_class_request("Ecore_Thread", NULL));

        elixir_function_run(cx, dt->func_heavy, parent, 2, argv, &rval);
     }

   elixir_rval_delete(cx, argv);

 on_error:
   elixir_unlock_cx(cx);
}

static void
_elixir_func_cleanup(JSContext *cx, Elixir_Thread_Data *dt, void *data)
{
   elixir_object_unregister(dt->runtime->cx, &dt->obj_heavy);
   elixir_object_unregister(dt->runtime->cx, &dt->obj_end);
   elixir_object_unregister(dt->runtime->cx, &dt->obj_cancel);

   JS_SetPrivate(dt->runtime->cx, dt->result, NULL);
   elixir_object_unregister(dt->runtime->cx, &dt->result);

   elixir_void_free(data);

   elixir_function_stop(cx);

   elixir_function_gc(cx);

   elixir_shutdown(dt->runtime);
   free(dt);

   elixir_thread_del();
}

static void
_elixir_func_end(void *data, Ecore_Thread *thread)
{
   Elixir_Thread_Data *dt;
   JSContext *cx;
   JSObject *parent;
   jsval argv[2];
   jsval rval;

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);

   if (!parent
       || !dt)
     return ;

   cx = dt->main;

   elixir_function_start(cx);

   JS_SetContextThread(dt->runtime->cx);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);
   rval = JSVAL_VOID;
   elixir_rval_register(cx, &rval);

   if (elixir_api_version_get() == 0)
     {
        elixir_function_run(cx, dt->func_end, parent, 1, argv, &rval);
     }
   else
     {
        elixir_return_ptr(cx, &argv[1], thread, elixir_class_request("Ecore_Thread", NULL));
        elixir_rval_register(cx, argv + 1);

        elixir_function_run(cx, dt->func_end, parent, 2, argv, &rval);

        elixir_rval_delete(cx, argv + 1);
     }

   elixir_rval_delete(cx, argv);
   elixir_rval_delete(cx, &rval);

   _elixir_func_cleanup(cx, dt, data);
}

static void
_elixir_func_cancel(void *data, Ecore_Thread *thread)
{
   Elixir_Thread_Data *dt;
   JSContext *cx;
   JSObject *parent;
   jsval argv[2];
   jsval rval;

   parent = elixir_void_get_parent(data);
   dt = elixir_void_get_private(data);

   if (!parent
       || !dt)
     return ;

   cx = dt->main;

   elixir_function_start(cx);

   JS_SetContextThread(dt->runtime->cx);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);
   rval = JSVAL_VOID;
   elixir_rval_register(cx, &rval);

   if (elixir_api_version_get() == 0)
     {
        elixir_function_run(cx, dt->func_cancel, parent, 1, argv, &rval);
     }
   else
     {
        elixir_return_ptr(cx, &argv[1], thread, elixir_class_request("Ecore_Thread", NULL));
        elixir_rval_register(cx, argv + 1);

        elixir_function_run(cx, dt->func_cancel, parent, 2, argv, &rval);

        elixir_rval_delete(cx, argv + 1);
     }

   elixir_rval_delete(cx, argv);
   elixir_rval_delete(cx, &rval);

   _elixir_func_cleanup(cx, dt, data);
}

static JSBool
elixir_ecore_thread_run(JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_Thread_Data *dt;
   void *new;
   Ecore_Thread *result;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _3func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   dt = malloc(sizeof (Elixir_Thread_Data));
   if (!dt) return JS_FALSE;

   dt->runtime = elixir_clone(cx, JS_THIS_OBJECT(cx, vp));

   if (!dt->runtime)
     {
	free(dt);
	return JS_FALSE;
     }

   dt->func_heavy = val[0].v.fct;
   dt->func_end = val[1].v.fct;
   dt->func_cancel = val[2].v.fct;
   dt->obj_heavy = JS_GetFunctionObject(dt->func_heavy);
   dt->obj_end = JS_GetFunctionObject(dt->func_end);
   dt->obj_cancel = JS_GetFunctionObject(dt->func_cancel);
   dt->main = cx;

   elixir_object_register(dt->runtime->cx, &dt->obj_heavy, NULL);
   elixir_object_register(dt->runtime->cx, &dt->obj_end, NULL);
   elixir_object_register(dt->runtime->cx, &dt->obj_cancel, NULL);

   if (!JS_GetParent(cx, dt->obj_heavy))
     JS_SetParent(cx, dt->obj_heavy, JS_THIS_OBJECT(cx, vp));

   new = elixir_void_new(dt->runtime->cx, JS_THIS_OBJECT(cx, vp), val[3].v.any, dt);
   if (!new)
     {
        elixir_object_unregister(dt->runtime->cx, &dt->obj_heavy);
        elixir_object_unregister(dt->runtime->cx, &dt->obj_end);
        elixir_object_unregister(dt->runtime->cx, &dt->obj_cancel);
	free(dt);
	return JS_FALSE;
     }

   elixir_function_stop(cx);

   elixir_thread_new();

   result = ecore_thread_run(_elixir_func_heavy, _elixir_func_end, _elixir_func_cancel, new);

   elixir_function_start(cx);

   if (result)
     {
        dt->result = elixir_return_ptr(cx, vp, result, elixir_class_request("Ecore_Thread", NULL));
        elixir_object_register(cx, &dt->result, NULL);
     }
   else
     {
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
     }
   return JS_TRUE;
}

static JSBool
elixir_ecore_thread_cancel(JSContext *cx, uintN argc, jsval *vp)
{
   elixir_value_t val[1];
   Ecore_Thread *et;

   if (!elixir_params_check(cx, _ecore_thread_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, et);

   if (!et) JS_SET_RVAL(cx, vp, JSVAL_TRUE);
   else JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ecore_thread_cancel(et)));
   return JS_TRUE;
}

static void
_elixir_ecore_job_cb(void* data)
{
   JSObject*            parent;
   JSContext*           cx;
   JSFunction*          cb;
   jsval                js_return;
   jsval                argv[1];

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent || !cb)
     return ;

   elixir_function_start(cx);

   argv[0] = elixir_void_get_jsval(data);
   elixir_rval_register(cx, argv);

   elixir_function_run(cx, cb, parent, 1, argv, &js_return);

   elixir_rval_register(cx, argv);

   elixir_function_stop(cx);
}

static JSBool
elixir_ecore_job_add(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Job *ej;
   void *data;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   /* If you don't call ecore_job_del, this will leak. Perhaps better to implement ecore_job directly in JS. */
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[1].v.any, val[0].v.fct);

   ej = ecore_job_add(_elixir_ecore_job_cb, data);

   elixir_return_ptr(cx, vp, ej, elixir_class_request("ecore_job", NULL));
   return JS_TRUE;
}

static JSBool
elixir_ecore_job_del(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Job *ej;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_job_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ej);

   data = ecore_job_del(ej);

   JS_SET_RVAL(cx, vp, elixir_void_free(data));
   return JS_TRUE;
}

static JSFunctionSpec        ecore_functions[] = {
  ELIXIR_FN(ecore_init, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_shutdown, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_type_new, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_current_type_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_app_restart, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_main_loop_iterate, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_main_loop_begin, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_main_loop_quit, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_animator_frametime_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_time_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_loop_time_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_handler_add, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_handler_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_add, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_filter_add, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_filter_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_animator_frametime_set, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_idler_add, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_idler_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_idle_enterer_add, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_idle_enterer_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_idle_exiter_add, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_idle_exiter_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_animator_add, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_animator_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_timer_add, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_timer_loop_add, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_timer_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_timer_interval_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_timer_delay, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_timer_pending_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_timer_freeze, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_timer_thaw, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_event_current_event_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_thread_run, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_thread_cancel, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_animator_freeze, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_animator_thaw, 1, JSPROP_ENUMERATE, 0 ),
#if 0
  ELIXIR_FN(ecore_job_add, 2, JSPROP_READONLY, 0 ),
  ELIXIR_FN(ecore_job_del, 1, JSPROP_READONLY, 0 ),
#endif
  JS_FS_END
};

static const struct {
   const char*  name;
   int          value;
} ecore_const_properties[] = {
   { "ECORE_EVENT_NONE", ECORE_EVENT_NONE },
   { "ECORE_EVENT_SIGNAL_USER", ECORE_EVENT_SIGNAL_USER },
   { "ECORE_EVENT_SIGNAL_HUP", ECORE_EVENT_SIGNAL_HUP },
   { "ECORE_EVENT_SIGNAL_EXIT", ECORE_EVENT_SIGNAL_EXIT },
   { "ECORE_EVENT_SIGNAL_POWER", ECORE_EVENT_SIGNAL_POWER },
   { "ECORE_EVENT_SIGNAL_REALTIME", ECORE_EVENT_SIGNAL_REALTIME },
   { "ECORE_EVENT_COUNT", ECORE_EVENT_COUNT },
   { "ECORE_CALLBACK_CANCEL", ECORE_CALLBACK_CANCEL },
   { "ECORE_CALLBACK_RENEW", ECORE_CALLBACK_RENEW },
   { "ECORE_CALLBACK_PASS_ON", ECORE_CALLBACK_PASS_ON },
   { "ECORE_CALLBACK_DONE", ECORE_CALLBACK_DONE },
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

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), ecore_functions))
     goto on_error;

   while (ecore_const_properties[i].name)
     {
        property = INT_TO_JSVAL(ecore_const_properties[i].value);
        if (!JS_DefineProperty(cx, parent,
			       ecore_const_properties[i].name,
			       property,
			       NULL, NULL,
			       JSPROP_ENUMERATE | JSPROP_READONLY))
          goto on_error;
        ++i;
     }

   _ecore_event_handler_parameter.class = elixir_class_request("Ecore_Event_Handler", NULL);
   _ecore_event_parameter.class = elixir_class_request("Ecore_Event", NULL);
   _ecore_event_filter_parameter.class = elixir_class_request("Ecore_Event_Filter", NULL);
   _ecore_idler_parameter.class = elixir_class_request("Ecore_Idler", NULL);
   _ecore_idle_enterer_parameter.class = elixir_class_request("Ecore_Idle_Enterer", NULL);
   _ecore_idle_exiter_parameter.class = elixir_class_request("Ecore_Idle_Exiter", NULL);
   _ecore_timer_parameter.class = elixir_class_request("Ecore_Timer", NULL);
   _ecore_animator_parameter.class = elixir_class_request("Ecore_Animator", NULL);
   _ecore_job_parameter.class = elixir_class_request("ecore_job", NULL);

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
   while (ecore_functions[i].name)
     JS_DeleteProperty(cx, parent, ecore_functions[i++].name);

   i = 0;
   while (ecore_const_properties[i].name)
     JS_DeleteProperty(cx, parent, ecore_const_properties[i++].name);

   elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   return EINA_TRUE;
}

static const Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "ecore",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_ecore = {
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
ecore_binding_init(void)
{
   return elixir_modules_register(&em_ecore);
}

void
ecore_binding_shutdown(void)
{
   elixir_modules_unregister(&em_ecore);
}

#ifndef EINA_STATIC_BUILD_ECORE
EINA_MODULE_INIT(ecore_binding_init);
EINA_MODULE_SHUTDOWN(ecore_binding_shutdown);
#endif

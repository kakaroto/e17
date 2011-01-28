#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <alloca.h>
#include <arpa/inet.h>
#include <assert.h>

#include <Ecore.h>
#include <Ecore_Con.h>

#include "Elixir.h"

#define MAGIC_EET_DATA_PACKET 0xEE70ACE1

static elixir_parameter_t               _ecore_con_server_parameter = {
  "Ecore_Con_Server", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_con_client_parameter = {
  "Ecore_Con_Client", JOBJECT, NULL
};
static elixir_parameter_t               _ecore_con_url_parameter = {
  "Ecore_Con_Url", JOBJECT, NULL
};

static const elixir_parameter_t*        _2string_params[3] = {
  &string_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _int_string_int_any_params[5] = {
  &int_parameter,
  &string_parameter,
  &int_parameter,
  &any_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_server_params[2] = {
  &_ecore_con_server_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_server_string_params[3] = {
  &_ecore_con_server_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_server_int_bool_params[4] = {
  &_ecore_con_server_parameter,
  &int_parameter,
  &boolean_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_client_string_params[3] = {
  &_ecore_con_client_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_client_params[2] = {
  &_ecore_con_client_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_client_any_params[3] = {
  &_ecore_con_client_parameter,
  &any_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_url_params[2] = {
  &_ecore_con_url_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_url_any_params[3] = {
  &_ecore_con_url_parameter,
  &any_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_url_string_params[3] = {
  &_ecore_con_url_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*	_ecore_con_url_bool_params[3] = {
  &_ecore_con_url_parameter,
  &boolean_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_url_2int_params[4] = {
  &_ecore_con_url_parameter,
  &int_parameter,
  &int_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_url_2string_params[4] = {
  &_ecore_con_url_parameter,
  &string_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_url_4string_params[6] = {
  &_ecore_con_url_parameter,
  &string_parameter,
  &string_parameter,
  &string_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_server_eet_data_params[3] = {
  &_ecore_con_server_parameter,
  &eet_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_con_client_eet_data_params[3] = {
  &_ecore_con_client_parameter,
  &eet_parameter,
  NULL
};
static const elixir_parameter_t *_string_func_any_params[4] = {
  &string_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};

static int                      _elixir_ecore_con_init = 0;
static int                      _elixir_ecore_con_url_init = 0;

static int                      ELIXIR_CON_EVENT_URL_DATA = 0;
static int                      ELIXIR_CON_EVENT_URL_COMPLETE = 0;
static int                      ELIXIR_CON_EVENT_URL_PROGRESS = 0;

static Ecore_Event_Handler *eeh_elixir_con_url_event[3] = { NULL, NULL, NULL };

static int                      ELIXIR_CON_EVENT_CLIENT_DEL = 0;
static int                      ELIXIR_CON_EVENT_SERVER_ADD = 0;
static int                      ELIXIR_CON_EVENT_SERVER_DEL = 0;
static int                      ELIXIR_CON_EVENT_CLIENT_DATA = 0;
static int                      ELIXIR_CON_EVENT_SERVER_DATA = 0;
static int                      ELIXIR_CON_EVENT_CLIENT_ADD = 0;

static Ecore_Event_Handler *eeh_elixir_con_event[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

typedef struct _Elixir_Con_Data Elixir_Con_Data;
struct _Elixir_Con_Data
{
   int size;
   int current;

   char *data;
};

static const struct {
  const char*   name;
  int*          value;
} ecore_con_event_properties[] = {
  { "ECORE_CON_EVENT_CLIENT_ADD", &ELIXIR_CON_EVENT_CLIENT_ADD },
  { "ECORE_CON_EVENT_CLIENT_DEL", &ELIXIR_CON_EVENT_CLIENT_DEL },
  { "ECORE_CON_EVENT_SERVER_ADD", &ELIXIR_CON_EVENT_SERVER_ADD },
  { "ECORE_CON_EVENT_SERVER_DEL", &ELIXIR_CON_EVENT_SERVER_DEL },
  { "ECORE_CON_EVENT_CLIENT_DATA", &ELIXIR_CON_EVENT_CLIENT_DATA },
  { "ECORE_CON_EVENT_SERVER_DATA", &ELIXIR_CON_EVENT_SERVER_DATA },
  { NULL, NULL }
};

static const struct {
  const char*   name;
  int*          value;
} ecore_con_url_event_properties[] = {
  { "ECORE_CON_EVENT_URL_DATA", &ELIXIR_CON_EVENT_URL_DATA },
  { "ECORE_CON_EVENT_URL_COMPLETE", &ELIXIR_CON_EVENT_URL_COMPLETE },
  { "ECORE_CON_EVENT_URL_PROGRESS", &ELIXIR_CON_EVENT_URL_PROGRESS },
  { NULL, NULL }
};

static int
_elixir_con_data_handler(void *private_data, int *data, int size)
{
   Elixir_Con_Data *ecd;

   ecd = elixir_void_get_private(private_data);
   if (ecd)
     {
	/* We are in the process of receiving an Eet_Data packet. */
	if (ecd->current + size > ecd->size)
	  {
	     /* Too much data ! */
	     free(ecd->data);
	     free(ecd);
	     elixir_void_set_private(private_data, NULL);
	     return 0;
	  }

	memcpy(ecd->data + ecd->current, data, size);

	ecd->current += size;

	if (ecd->current < ecd->size)
	  /* All data where not received. */
	  return -1;
     }
   else
     {
	/* Detect if it's a string or a possible Eet_Data packet start. */
	if (size >= 4 && ntohl(data[0]) == MAGIC_EET_DATA_PACKET)
	  {
	     uint32_t count;

	     /*
	       All Eet_Data communication start with the expected size of Eet_Data.
	       This integer always start with two 0x0 as we don't expect more than
	       2^16 bytes for an Eet_Data. More will certainly mean death of the
	       connection.
	      */
	     count = ntohl(data[1]);

	     if (count < size - sizeof (int) * 2)
	       return 0;

	     ecd = malloc(sizeof (Elixir_Con_Data));
	     if (!ecd) return 0;

	     ecd->data = malloc(count);
	     if (!ecd->data)
	       {
		  free(ecd);
		  return 0;
	       }

	     ecd->size = count;
	     ecd->current = size - sizeof (int) * 2;

	     memcpy(ecd->data, data + 2, ecd->current);

	     elixir_void_set_private(private_data, ecd);

	     if (ecd->current < ecd->size)
	       /* All data where not received. */
	       return -1;
	  }
     }

   return 1;
}

static void
_ecore_event_func_free(void *data, void *ev)
{
   (void) data;

   elixir_void_free(ev);
}

static JSObject *
_elixir_ecore_con_client_get(JSContext *cx, Ecore_Con_Client *clt)
{
   JSObject *result;
   void *data;

   data = ecore_con_client_data_get(clt);
   if (!data)
     {
	result = elixir_build_ptr(cx, clt, elixir_class_request("Ecore_Con_Client", NULL));
	if (!result) return NULL;

	data = elixir_void_new(cx, result, JSVAL_NULL, NULL);
	if (!data) return NULL;
	ecore_con_client_data_set(clt, data);
     }

   return elixir_void_get_parent(data);
}

static Eina_Bool
_elixir_con_event_url_data_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Url_Data *eceud;
   Elixir_Con_Data *ecd;
   JSContext *cx;
   JSObject *obj_eceud;
   JSString *src_data;
   void *new;
   void *private_data;
   jsval tmp;
   Eina_Bool ret = ECORE_CALLBACK_PASS_ON;

   eceud = event;

   private_data = ecore_con_url_data_get(eceud->url_con);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   ret = _elixir_con_data_handler(private_data, (int*) eceud->data, eceud->size);
   if (ret != 1)
     return ret;
   ret = 0;

   ecd = elixir_void_get_private(private_data);

   elixir_function_start(cx);

   obj_eceud = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Url_Data", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_eceud, NULL))
     goto on_finish;

   if (!JS_DefineProperty(cx, obj_eceud, "url_con",
			  OBJECT_TO_JSVAL(elixir_void_get_parent(private_data)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   if (!elixir_add_int_prop(cx, obj_eceud, "size", eceud->size))
     goto on_error;

   if (ecd)
     {
	JSObject *eet_data;

	eet_data = elixir_eet_data_new(cx, ecd->data, ecd->size);
	if (!eet_data)
	  goto on_error;

	tmp = OBJECT_TO_JSVAL(eet_data);
     }
   else
     {
	src_data = elixir_ndup(cx, (char*) eceud->data, eceud->size);
	if (!src_data)
	  goto on_error;

	tmp = STRING_TO_JSVAL(src_data);
     }

   if (!JS_DefineProperty(cx, obj_eceud, "data",
			  tmp,
			  NULL, NULL,
			  JSPROP_ENUMERATE |
			  JSPROP_READONLY))
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_eceud), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_URL_DATA, new, _ecore_event_func_free, NULL);

 on_error:
   if (ecd)
     {
	/* FIXME: What will happen when we have multiple handler on one connection ? */
	free(ecd);
	elixir_void_set_private(private_data, NULL);
     }

   elixir_object_unregister(cx, &obj_eceud);

 on_finish:
   elixir_function_stop(cx);

   return ret;
}

static Eina_Bool
_elixir_con_event_url_complete_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Url_Complete *eceuc;
   JSContext *cx;
   JSObject *obj_eceuc;
   void *new;
   void *private_data;
   Eina_Bool ret = ECORE_CALLBACK_PASS_ON;

   eceuc = event;

   private_data = ecore_con_url_data_get(eceuc->url_con);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_eceuc = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Url_Complete", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_eceuc, NULL))
     goto on_finish;

   if (!JS_DefineProperty(cx, obj_eceuc, "url_con",
			  OBJECT_TO_JSVAL(elixir_void_get_parent(private_data)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   if (!elixir_add_int_prop(cx, obj_eceuc, "status", eceuc->status))
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_eceuc), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_URL_COMPLETE, new, _ecore_event_func_free, NULL);

 on_error:
   elixir_object_unregister(cx, &obj_eceuc);

 on_finish:
   elixir_function_stop(cx);

   return ret;
}

static Eina_Bool
_elixir_con_event_url_progress_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Url_Progress *eceup;
   JSContext *cx;
   JSObject *obj_eceup;
   JSObject *obj_download;
   JSObject *obj_upload;
   void *new;
   void *private_data;
   Eina_Bool ret = ECORE_CALLBACK_PASS_ON;

   eceup = event;

   private_data = ecore_con_url_data_get(eceup->url_con);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_eceup = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Url_Progress", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_eceup, NULL))
     goto on_finish;

   if (!JS_DefineProperty(cx, obj_eceup, "url_con",
			  OBJECT_TO_JSVAL(elixir_void_get_parent(private_data)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   obj_download = JS_DefineObject(cx, obj_eceup, "down", NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!elixir_object_register(cx, &obj_download, NULL))
     goto on_error;
   if (!elixir_add_dbl_prop(cx, obj_download, "total", eceup->down.total))
     goto on_error;
   if (!elixir_add_dbl_prop(cx, obj_download, "now", eceup->down.now))
     goto on_error;

   obj_upload = JS_DefineObject(cx, obj_eceup, "up", NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!elixir_object_register(cx, &obj_upload, NULL))
     goto on_error;
   if (!elixir_add_dbl_prop(cx, obj_upload, "total", eceup->up.total))
     goto on_error;
   if (!elixir_add_dbl_prop(cx, obj_upload, "now", eceup->up.now))
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_eceup), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_URL_PROGRESS, new, _ecore_event_func_free, NULL);

 on_error:
   elixir_object_unregister(cx, &obj_download);
   elixir_object_unregister(cx, &obj_upload);
   elixir_object_unregister(cx, &obj_eceup);

   ret = ECORE_CALLBACK_DONE;

 on_finish:
   elixir_function_stop(cx);
   return ret;
}

static Eina_Bool
_elixir_con_event_client_add_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Client_Add*  ececa;
   Ecore_Con_Server*            server;
   JSContext*                   cx;
   JSObject*                    obj_ececa;
   void*                        new;
   void*                        private_data;
   Eina_Bool                    ret = ECORE_CALLBACK_PASS_ON;

   ececa = event;

   server = ecore_con_client_server_get(ececa->client);
   private_data = ecore_con_server_data_get(server);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_ececa = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Client_Add", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_ececa, NULL))
     goto on_finish;

   if (!JS_DefineProperty(cx, obj_ececa, "client",
			  OBJECT_TO_JSVAL(_elixir_ecore_con_client_get(cx, ececa->client)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_ececa), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_CLIENT_ADD, new, _ecore_event_func_free, NULL);

 on_error:
   elixir_object_unregister(cx, &obj_ececa);

   ret = ECORE_CALLBACK_DONE;

 on_finish:
   elixir_function_stop(cx);

   return ret;
}

static Eina_Bool
_elixir_con_event_client_del_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Client_Del *ececd;
   Ecore_Con_Server *server;
   JSContext *cx;
   JSObject *obj_ececd;
   void *new;
   void *private_data;
   Eina_Bool ret = ECORE_CALLBACK_PASS_ON;

   ececd = event;

   if (!ececd->client) return ECORE_CALLBACK_PASS_ON;

   server = ecore_con_client_server_get(ececd->client);
   private_data = ecore_con_server_data_get(server);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_ececd = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Client_Del", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_ececd, NULL))
     goto on_finish;

   if (!JS_DefineProperty(cx, obj_ececd, "client",
			  OBJECT_TO_JSVAL(_elixir_ecore_con_client_get(cx, ececd->client)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_ececd), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_CLIENT_DEL, new, _ecore_event_func_free, NULL);

 on_error:
   elixir_object_unregister(cx, &obj_ececd);

   ret = ECORE_CALLBACK_DONE;

 on_finish:
   elixir_function_stop(cx);

   return ret;
}

static Eina_Bool
_elixir_con_event_server_add_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Server_Add   *ecesa;
   JSContext                    *cx;
   JSObject                     *obj_ecesa;
   void                         *new;
   void                         *private_data;
   Eina_Bool                     ret = ECORE_CALLBACK_PASS_ON;

   ecesa = event;
   private_data = ecore_con_server_data_get(ecesa->server);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_ecesa = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Server_Add", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_ecesa, NULL))
     goto on_finish;

   if (!JS_DefineProperty(cx, obj_ecesa, "server",
			  OBJECT_TO_JSVAL(elixir_void_get_parent(private_data)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_ecesa), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_SERVER_ADD, new, _ecore_event_func_free, NULL);

 on_error:
   elixir_object_unregister(cx, &obj_ecesa);

   ret = ECORE_CALLBACK_DONE;

 on_finish:
   elixir_function_stop(cx);

   return ret;
}

static Eina_Bool
_elixir_con_event_server_del_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Server_Del   *ecesd;
   JSContext                    *cx;
   JSObject                     *obj_ecesd;
   void                         *new;
   void                         *private_data;

   ecesd = event;
   private_data = ecore_con_server_data_get(ecesd->server);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   elixir_function_start(cx);

   obj_ecesd = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Server_Del", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_ecesd, NULL))
     {
	elixir_function_stop(cx);
	return ECORE_CALLBACK_DONE;
     }

   if (!JS_DefineProperty(cx, obj_ecesd, "server",
			  OBJECT_TO_JSVAL(elixir_void_get_parent(private_data)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_ecesd), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_SERVER_DEL, new, _ecore_event_func_free, NULL);

 on_error:
   elixir_object_unregister(cx, &obj_ecesd);
   elixir_function_stop(cx);
   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_elixir_con_event_client_data_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Client_Data *ececd;
   Ecore_Con_Server *server;
   Elixir_Con_Data *ecd;
   JSContext *cx;
   JSObject *obj_ececd;
   JSString *src_data;
   void *new;
   void *private_data;
   jsval tmp;

   ececd = event;

   server = ecore_con_client_server_get(ececd->client);
   private_data = ecore_con_server_data_get(server);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   if (_elixir_con_data_handler(private_data, (int*) ececd->data, ececd->size) != 1)
     return ECORE_CALLBACK_DONE;

   ecd = elixir_void_get_private(private_data);

   elixir_function_start(cx);

   obj_ececd = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Client_Data", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_ececd, NULL))
     {
	elixir_function_stop(cx);
	return ECORE_CALLBACK_DONE;
     }

   if (!JS_DefineProperty(cx, obj_ececd, "client",
			  OBJECT_TO_JSVAL(_elixir_ecore_con_client_get(cx, ececd->client)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   if (!elixir_add_int_prop(cx, obj_ececd, "size", ececd->size))
     goto on_error;

   if (ecd)
     {
	JSObject *eet_data;

	eet_data = elixir_eet_data_new(cx, ecd->data, ecd->size);
	if (!eet_data) goto on_error;

	tmp = OBJECT_TO_JSVAL(eet_data);
     }
   else
     {
	/* It's a string */
	src_data = elixir_ndup(cx, ececd->data, ececd->size);
	if (!src_data)
	  goto on_error;

	tmp = STRING_TO_JSVAL(src_data);
     }

   if (JS_DefineProperty(cx, obj_ececd, "data",
			 tmp,
			 NULL, NULL,
			 JSPROP_ENUMERATE |
			 JSPROP_READONLY) == JS_FALSE)
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_ececd), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_CLIENT_DATA, new, _ecore_event_func_free, NULL);

 on_error:
   if (ecd)
     {
	/* FIXME: What will happen when we have multiple handler on one connection ? */
	free(ecd);
	elixir_void_set_private(private_data, NULL);
     }

   elixir_object_unregister(cx, &obj_ececd);

   elixir_function_stop(cx);

   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_elixir_con_event_server_data_cb(__UNUSED__ void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Server_Data *ececd;
   Elixir_Con_Data *ecd;
   JSContext *cx;
   JSObject *obj_ececd;
   JSString *src_data;
   void *new;
   void *private_data;
   jsval tmp;

   ececd = event;
   private_data = ecore_con_server_data_get(ececd->server);
   cx = elixir_void_get_cx(private_data);
   if (!cx) return ECORE_CALLBACK_PASS_ON;

   if (_elixir_con_data_handler(private_data, (int*) ececd->data, ececd->size) != 1)
     return ECORE_CALLBACK_PASS_ON;

   ecd = elixir_void_get_private(private_data);

   elixir_function_start(cx);

   obj_ececd = JS_NewObject(cx, elixir_class_request("Ecore_Con_Event_Server_Data", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj_ececd, NULL))
     goto on_finish;

   if (!JS_DefineProperty(cx, obj_ececd, "server",
			  OBJECT_TO_JSVAL(elixir_void_get_parent(private_data)),
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   if (!elixir_add_int_prop(cx, obj_ececd, "size", ececd->size))
     goto on_error;

   if (ecd)
     {
	JSObject *eet_data;

	eet_data = elixir_eet_data_new(cx, ecd->data, ecd->size);
	if (!eet_data)
	  goto on_error;

	tmp = OBJECT_TO_JSVAL(eet_data);
     }
   else
     {
	src_data = elixir_ndup(cx, ececd->data, ececd->size);
	if (!src_data)
	  goto on_error;

	tmp = STRING_TO_JSVAL(src_data);
     }

   if (!JS_DefineProperty(cx, obj_ececd, "data",
			  tmp,
			  NULL, NULL,
			  JSPROP_ENUMERATE |
			  JSPROP_READONLY))
     goto on_error;

   new = elixir_void_new(cx, NULL, OBJECT_TO_JSVAL(obj_ececd), NULL);
   ecore_event_add(ELIXIR_CON_EVENT_SERVER_DATA, new, _ecore_event_func_free, NULL);

 on_error:
   if (ecd)
     {
	/* FIXME: What will happen when we have multiple handler on one connection ? */
	free(ecd);
	elixir_void_set_private(private_data, NULL);
     }

   elixir_object_unregister(cx, &obj_ececd);

 on_finish:
   elixir_function_stop(cx);

   return ECORE_CALLBACK_DONE;
}

static JSBool
elixir_ecore_con_init(JSContext *cx, uintN argc, jsval *vp)
{
   unsigned int i = 0;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   if (++_elixir_ecore_con_init == 1)
     {
	ecore_init();
	ecore_con_init();

        ELIXIR_CON_EVENT_CLIENT_ADD = ecore_event_type_new();
        ELIXIR_CON_EVENT_CLIENT_DATA = ecore_event_type_new();
        ELIXIR_CON_EVENT_CLIENT_DEL = ecore_event_type_new();
        ELIXIR_CON_EVENT_SERVER_ADD = ecore_event_type_new();
        ELIXIR_CON_EVENT_SERVER_DATA = ecore_event_type_new();
        ELIXIR_CON_EVENT_SERVER_DEL = ecore_event_type_new();

        eeh_elixir_con_event[0] = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD, _elixir_con_event_client_add_cb, NULL);
        eeh_elixir_con_event[1] = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA, _elixir_con_event_client_data_cb, NULL);
        eeh_elixir_con_event[2] = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL, _elixir_con_event_client_del_cb, NULL);
        eeh_elixir_con_event[3] = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _elixir_con_event_server_add_cb, NULL);
        eeh_elixir_con_event[4] = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, _elixir_con_event_server_data_cb, NULL);
        eeh_elixir_con_event[5] = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _elixir_con_event_server_del_cb, NULL);

        while (ecore_con_event_properties[i].name)
          {
             jsval        property;

             JS_DeleteProperty(cx, JS_THIS_OBJECT(cx, vp), ecore_con_event_properties[i].name);

             property = INT_TO_JSVAL(*ecore_con_event_properties[i].value);
             JS_DefineProperty(cx, JS_THIS_OBJECT(cx, vp),
                               ecore_con_event_properties[i].name,
                               property,
                               NULL, NULL,
                               JSPROP_ENUMERATE | JSPROP_READONLY);
             ++i;
          }
     }

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ecore_con_init()));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_shutdown(JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   if (--_elixir_ecore_con_init == 0)
     {
        unsigned        i;

        for (i = 0; i < sizeof (eeh_elixir_con_event) / sizeof (Ecore_Event_Handler*); ++i)
          ecore_event_handler_del(eeh_elixir_con_event[i]);

        for (i = 0; ecore_con_event_properties[i].name; ++i)
          JS_DeleteProperty(cx, JS_THIS_OBJECT(cx, vp), ecore_con_event_properties[i].name);

	ecore_con_shutdown();
	ecore_shutdown();
     }

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ecore_con_shutdown()));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_init(JSContext *cx, uintN argc, jsval *vp)
{
   unsigned int i = 0;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   if (++_elixir_ecore_con_url_init == 1)
     {
	ecore_init();
	ecore_con_url_init();

        ELIXIR_CON_EVENT_URL_DATA = ecore_event_type_new();
        ELIXIR_CON_EVENT_URL_COMPLETE = ecore_event_type_new();
        ELIXIR_CON_EVENT_URL_PROGRESS = ecore_event_type_new();

        eeh_elixir_con_url_event[0] = ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA, _elixir_con_event_url_data_cb, NULL);
        eeh_elixir_con_url_event[1] = ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE, _elixir_con_event_url_complete_cb, NULL);
        eeh_elixir_con_url_event[2] = ecore_event_handler_add(ECORE_CON_EVENT_URL_PROGRESS, _elixir_con_event_url_progress_cb, NULL);

        while (ecore_con_url_event_properties[i].name)
          {
             jsval        property;

             JS_DeleteProperty(cx, JS_THIS_OBJECT(cx, vp), ecore_con_url_event_properties[i].name);

             property = INT_TO_JSVAL(*ecore_con_url_event_properties[i].value);
             JS_DefineProperty(cx, JS_THIS_OBJECT(cx, vp),
                               ecore_con_url_event_properties[i].name,
                               property,
                               NULL, NULL,
                               JSPROP_ENUMERATE | JSPROP_READONLY);
             ++i;
          }
     }

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ecore_con_url_init()));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_shutdown(JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   if (--_elixir_ecore_con_url_init == 0)
     {
        unsigned int    i;

        for (i = 0; i < sizeof (eeh_elixir_con_url_event) / sizeof (Ecore_Event_Handler*); ++i)
          ecore_event_handler_del(eeh_elixir_con_url_event[i]);

	i = 0;
        while (ecore_con_url_event_properties[i].name)
          {
             JS_DeleteProperty(cx, JS_THIS_OBJECT(cx, vp), ecore_con_url_event_properties[i].name);
             ++i;
          }

	ecore_con_url_shutdown();
	ecore_shutdown();
     }

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ecore_con_url_shutdown()));
   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_con_ssl_available_get, elixir_int_params_void);

static JSBool
elixir_ecs_isia(Ecore_Con_Server* (*func)(Ecore_Con_Type type, const char *name, int port, const void *data),
                JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Server *ecs;
   const char *con_name;
   const void *data;
   JSObject *result;
   Ecore_Con_Type type;
   int port;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _int_string_int_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   type = val[0].v.num;
   con_name = elixir_get_string_bytes(val[1].v.str, NULL);
   port = val[2].v.num;
   data = elixir_void_new(cx, NULL, val[3].v.any, NULL);

   ecs = func(type, con_name, port, data);

   result = elixir_return_ptr(cx, vp, ecs, elixir_class_request("Ecore_Con_Server", NULL));
   elixir_void_set_parent(data, result);
   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_con_server_add, elixir_ecs_isia);
FAST_CALL_PARAMS(ecore_con_server_connect, elixir_ecs_isia);

static JSBool
elixir_ecore_con_server_del(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Server *svr;
   void *any;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_server_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, svr);

   any = ecore_con_server_del(svr);

   JS_SET_RVAL(cx, vp, elixir_void_free(any));

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_server_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Server *svr;
   void *any;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_server_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, svr);

   any = ecore_con_server_data_get(svr);
   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(any));

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_server_connected_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Server *svr;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_server_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, svr);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ecore_con_server_connected_get(svr)));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_server_ip_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Server *svr;
   const char *ip;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_server_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, svr);

   ip = ecore_con_server_ip_get(svr);

   elixir_return_str(cx, vp, ip);
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_server_flush(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Server *svr;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_server_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, svr);

   ecore_con_server_flush(svr);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_server_clients_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Client *clt;
   Ecore_Con_Server *svr;
   Eina_List *lst;
   JSObject *array;
   jsval jelt;
   int i = 0;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_server_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, svr);

   lst = ecore_con_server_clients_get(svr);

   array = elixir_return_array(cx, vp);
   if (!array) return JS_FALSE;

   EINA_LIST_FOREACH(lst, lst, clt)
     {
	jelt = OBJECT_TO_JSVAL(_elixir_ecore_con_client_get(cx, clt));
	JS_DefineElement(cx, array, i, jelt, NULL, NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);
	i++;
     }

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_server_send(JSContext *cx, uintN argc, jsval *vp)
{
   const char *data = NULL;
   Ecore_Con_Server *svr;
   size_t length;
   int send;
   elixir_value_t val[2];

   if (elixir_params_check(cx, _ecore_con_server_string_params, val, argc, JS_ARGV(cx, vp)))
     {
	data = elixir_get_string_bytes(val[1].v.str, &length);
     }
   else if (elixir_params_check(cx, _ecore_con_server_eet_data_params, val, argc, JS_ARGV(cx, vp)))
     {
	Elixir_Eet_Data *eed = NULL;
	int *tmp;

	GET_PRIVATE(cx, val[1].v.obj, eed);
	length = sizeof (int) + sizeof (int) + eed->count;

	data = malloc(length);
	if (!data) return JS_FALSE;

	tmp = (int*) data;
	tmp[0] = htonl(MAGIC_EET_DATA_PACKET);
	tmp[1] = htonl(eed->count);
	memcpy(tmp + 2, eed->data, eed->count);
     }
   else
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, svr);

   send = ecore_con_server_send(svr, data, length);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(send));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_server_client_limit_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Server *svr;
   int client_limit;
   char reject_excess_clients;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_con_server_int_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, svr);
   client_limit = val[1].v.num;
   reject_excess_clients = val[2].v.bol;

   ecore_con_server_client_limit_set(svr, client_limit, reject_excess_clients);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_client_send(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Client *clt;
   const char *data;
   size_t length;
   int size;
   elixir_value_t val[2];

   if (elixir_params_check(cx, _ecore_con_client_string_params, val, argc, JS_ARGV(cx, vp)))
     {
	data = elixir_get_string_bytes(val[1].v.str, &length);
     }
   else if (elixir_params_check(cx, _ecore_con_client_eet_data_params, val, argc, JS_ARGV(cx, vp)))
     {
	Elixir_Eet_Data *eed = NULL;
	int *tmp;

	GET_PRIVATE(cx, val[1].v.obj, eed);
	length = sizeof (int) + eed->count;

	data = malloc(length);
	if (!data) return JS_FALSE;

	tmp = (int*) data;
	*tmp = htonl(eed->count);
	memcpy(tmp + 1, eed->data, eed->count);
     }
   else
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, clt);

   size = ecore_con_client_send(clt, data, length);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(size));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_client_server_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Client *clt;
   Ecore_Con_Server *srv;
   elixir_value_t val[1];
   void *data;

   if (!elixir_params_check(cx, _ecore_con_client_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, clt);

   srv = ecore_con_client_server_get(clt);

   if (!srv)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   data = ecore_con_server_data_get(srv);
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(elixir_void_get_parent(data)));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_client_del(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Client *clt;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_client_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, clt);

   data = ecore_con_client_data_get(clt);
   ecore_con_client_data_set(clt, NULL);

   ecore_con_client_del(clt);
   JS_SET_RVAL(cx, vp, elixir_void_free(data));

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_client_data_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Client *clt;
   void *old;
   void *data;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_con_client_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, clt);
   old = ecore_con_client_data_get(clt);

   data = elixir_void_new(cx, elixir_void_get_parent(old), val[1].v.any, NULL);
   elixir_void_free(old);

   ecore_con_client_data_set(clt, data);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_client_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Client *clt;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_client_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, clt);

   data = ecore_con_client_data_get(clt);

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(data));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_client_ip_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Client *clt;
   const char *ip;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_client_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, clt);

   ip = ecore_con_client_ip_get(clt);

   elixir_return_str(cx, vp, ip);
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_client_flush(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Client *clt;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_client_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, clt);

   ecore_con_client_flush(clt);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_new(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   const char *url;
   JSObject *result;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   url = elixir_get_string_bytes(val[0].v.str, NULL);

   curl = ecore_con_url_new(url);

   result = elixir_return_ptr(cx, vp, curl, elixir_class_request("Ecore_Con_Url", NULL));
   if (curl)
     ecore_con_url_data_set(curl, elixir_void_new(cx, result, JSVAL_NULL, NULL));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_custom_new(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   const char *url;
   const char *custom_request;
   JSObject *result;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _2string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   url = elixir_get_string_bytes(val[0].v.str, NULL);
   custom_request = elixir_get_string_bytes(val[1].v.str, NULL);

   curl = ecore_con_url_custom_new(url, custom_request);

   result = elixir_return_ptr(cx, vp, curl, elixir_class_request("Ecore_Con_Url", NULL));
   if (curl)
     ecore_con_url_data_set(curl, elixir_void_new(cx, result, JSVAL_NULL, NULL));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_free(JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_Con_Data *ecd;
   Ecore_Con_Url *curl;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_url_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);

   ecd = elixir_void_get_private(ecore_con_url_data_get(curl));
   if (ecd)
     {
	free(ecd->data);
	free(ecd);
     }

   JS_SetPrivate(cx, val[0].v.obj, NULL);

   elixir_void_free(ecore_con_url_data_get(curl));
   ecore_con_url_data_set(curl, NULL);
   ecore_con_url_free(curl);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_data_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   void *data;
   void *old;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_con_url_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);

   old = ecore_con_url_data_get(curl);

   data = elixir_void_new(cx, elixir_void_get_parent(old), val[1].v.any, elixir_void_get_private(old));

   ecore_con_url_data_set(curl, data);
   elixir_void_free(old);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_url_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);

   data = ecore_con_url_data_get(curl);

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(data));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_additional_header_add(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   const char *key;
   const char *value;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_con_url_2string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);
   key = elixir_get_string_bytes(val[1].v.str, NULL);
   value = elixir_get_string_bytes(val[2].v.str, NULL);

   ecore_con_url_additional_header_add(curl, key, value);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_additional_headers_clear(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_con_url_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);

   ecore_con_url_additional_headers_clear(curl);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_response_headers_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   const Eina_List *headers;
   const char *header;
   const Eina_List *l;
   JSObject *array;
   jsval jelt;
   int i = 0;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_con_url_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);

   headers = ecore_con_url_response_headers_get(curl);

   array = elixir_return_array(cx, vp);
   if (!array) return JS_FALSE;

   EINA_LIST_FOREACH(headers, l, header)
     {
	jelt = STRING_TO_JSVAL(elixir_dup(cx, header));
	JS_DefineElement(cx, array, i, jelt, NULL, NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);
	i++;
     }

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_received_bytes_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_con_url_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ecore_con_url_received_bytes_get(curl)));

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_url_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   const char *url;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_con_url_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);
   url = elixir_get_string_bytes(val[1].v.str, NULL);

   ecore_con_url_url_set(curl, url);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_send(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   const char *data;
   const char *content_type;
   size_t length;
   int size;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_con_url_2string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);
   data = elixir_get_string_bytes(val[1].v.str, &length);
   content_type = elixir_get_string_bytes(val[2].v.str, NULL);

   size = ecore_con_url_send(curl, data, length, content_type);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(size));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   int size;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_con_url_2string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);

   size = ecore_con_url_get(curl);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(size));
   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_post(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   const char *data;
   const char *content_type;
   size_t length;
   int size;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_con_url_2string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);
   data = elixir_get_string_bytes(val[1].v.str, &length);
   content_type = elixir_get_string_bytes(val[2].v.str, NULL);

   size = ecore_con_url_post(curl, data, length, content_type);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(size));
   return JS_TRUE;
}

static JSBool
elixir_void_params_ecore_con_url_bool(void (*func)(Ecore_Con_Url*, Eina_Bool),
				     JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   Eina_Bool arg;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_con_url_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);
   arg = val[1].v.bol;

   func(curl, arg);

   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_con_url_verbose_set, elixir_void_params_ecore_con_url_bool);
FAST_CALL_PARAMS(ecore_con_url_ftp_use_epsv_set, elixir_void_params_ecore_con_url_bool);

static JSBool
elixir_ecore_con_url_time(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   Ecore_Con_Url_Time condition;
   time_t tm;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_con_url_2int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);
   condition = val[1].v.num;
   tm = val[2].v.num;

   ecore_con_url_time(curl, condition, tm);

   return JS_TRUE;
}

static JSBool
elixir_ecore_con_url_ftp_upload(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Con_Url *curl;
   const char *filename;
   const char *user;
   const char *pass;
   const char *upload_dir;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _ecore_con_url_4string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, curl);
   filename = elixir_file_canonicalize(elixir_get_string_bytes(val[1].v.str, NULL));
   user = elixir_get_string_bytes(val[2].v.str, NULL);
   pass = elixir_get_string_bytes(val[3].v.str, NULL);
   upload_dir = elixir_get_string_bytes(val[4].v.str, NULL);

   JS_SET_RVAL(cx, vp, ecore_con_url_ftp_upload(curl, filename, user, pass, upload_dir));

   return JS_TRUE;
}

static void
_elixir_ecore_con_lookup_cb(const char *canonname,
			    const char *ip,
			    __UNUSED__ struct sockaddr *addr,
			    __UNUSED__ int addrlen,
			    void *data)
{
   JSFunction *cb;
   JSContext *cx;
   JSObject *parent;
   JSString *js_canonname;
   JSString *js_ip;
   jsval js_return;
   jsval argv[3];

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);

   if (!cx || !parent || !cb)
     return ;

   elixir_function_start(cx);

   js_canonname = elixir_dup(cx, canonname);
   if (!elixir_string_register(cx, &js_canonname))
     goto on_canon_error;

   js_ip =  elixir_dup(cx, ip);
   if (!elixir_string_register(cx, &js_ip))
     goto on_ip_error;

   argv[0] = STRING_TO_JSVAL(js_canonname);
   argv[1] = STRING_TO_JSVAL(js_ip);
   argv[2] = elixir_void_get_jsval(data);

   elixir_function_run(cx, cb, parent, 4, argv, &js_return);

   elixir_string_unregister(cx, &js_ip);
 on_ip_error:
   elixir_string_unregister(cx, &js_canonname);

 on_canon_error:
   elixir_function_stop(cx);
}

static JSBool
elixir_ecore_con_lookup(JSContext *cx, uintN argc, jsval *vp)
{
   const char *name;
   void *data;
   Eina_Bool res;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _string_func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   name = elixir_get_string_bytes(val[0].v.str, NULL);

   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);
   res = ecore_con_lookup(name, _elixir_ecore_con_lookup_cb, data);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(res));
   return JS_TRUE;
}


static JSFunctionSpec           ecore_con_functions[] = {
  ELIXIR_FN(ecore_con_init, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_shutdown, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_ssl_available_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_init, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_shutdown, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_add, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_connect, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_data_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_connected_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_ip_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_flush, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_clients_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_send, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_server_client_limit_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_client_send, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_client_server_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_client_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_client_data_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_client_data_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_client_ip_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_client_flush, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_new, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_custom_new, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_free, 1, JSPROP_ENUMERATE, 0 ),
  JS_FN("ecore_con_url_destroy", elixir_ecore_con_url_free, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_url_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_send, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_post, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_verbose_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_ftp_use_epsv_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_data_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_data_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_additional_header_add, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_additional_headers_clear, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_response_headers_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_received_bytes_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_time, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_url_ftp_upload, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_con_lookup, 3, JSPROP_ENUMERATE, 0),
  JS_FS_END
};

static const struct {
  const char*   name;
  int           value;
} ecore_con_properties[] = {
  { "ECORE_CON_LOCAL_USER", ECORE_CON_LOCAL_USER },
  { "ECORE_CON_LOCAL_SYSTEM", ECORE_CON_LOCAL_SYSTEM },
  { "ECORE_CON_LOCAL_ABSTRACT", ECORE_CON_LOCAL_ABSTRACT },
  { "ECORE_CON_REMOTE_SYSTEM", ECORE_CON_REMOTE_SYSTEM },
  { "ECORE_CON_USE_SSL", ECORE_CON_USE_SSL },
  { "ECORE_CON_URL_TIME_NONE", ECORE_CON_URL_TIME_NONE },
  { "ECORE_CON_URL_TIME_IFMODSINCE", ECORE_CON_URL_TIME_IFMODSINCE },
  { "ECORE_CON_URL_TIME_IFUNMODSINCE", ECORE_CON_URL_TIME_IFUNMODSINCE },
  { "ECORE_CON_REMOTE_UDP", ECORE_CON_REMOTE_UDP },
  { "ECORE_CON_REMOTE_NODELAY", ECORE_CON_REMOTE_NODELAY },
  { "ECORE_CON_REMOTE_TCP", ECORE_CON_REMOTE_TCP },
  { "ECORE_CON_REMOTE_MCAST", ECORE_CON_REMOTE_MCAST },
  { "ECORE_CON_REMOTE_BROADCAST", ECORE_CON_REMOTE_BROADCAST },
  { "ECORE_CON_USE_SSL2", ECORE_CON_USE_SSL2 },
  { "ECORE_CON_USE_SSL3", ECORE_CON_USE_SSL3 },
  { "ECORE_CON_USE_TLS", ECORE_CON_USE_TLS },
  { NULL, 0 }
};

static Eina_Bool
module_open(Elixir_Module* em, JSContext* cx, JSObject* parent)
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

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), ecore_con_functions))
     goto on_error;

   while (ecore_con_properties[i].name)
     {
        property = INT_TO_JSVAL(ecore_con_properties[i].value);
        if (!JS_DefineProperty(cx, parent,
			       ecore_con_properties[i].name,
			       property,
			       NULL, NULL,
			       JSPROP_ENUMERATE | JSPROP_READONLY))
          goto on_error;
        ++i;
     }

   _ecore_con_server_parameter.class = elixir_class_request("Ecore_Con_Server", NULL);
   _ecore_con_client_parameter.class = elixir_class_request("Ecore_Con_Client", NULL);
   _ecore_con_url_parameter.class = elixir_class_request("Ecore_Con_Url", NULL);

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
   while (ecore_con_functions[i].name)
     JS_DeleteProperty(cx, parent, ecore_con_functions[i++].name);

   i = 0;
   while (ecore_con_properties[i].name)
     JS_DeleteProperty(cx, parent, ecore_con_properties[i++].name);

   elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;
   return EINA_TRUE;
}

static const Elixir_Module_Api  module_api_elixir = {
  ELIXIR_MODULE_API_VERSION,
  ELIXIR_GRANTED,
  "ecore-con",
  "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_ecore_con = {
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
ecore_con_binding_init(void)
{
   return elixir_modules_register(&em_ecore_con);
}

void
ecore_con_binding_shutdown(void)
{
   elixir_modules_unregister(&em_ecore_con);
}

#ifndef EINA_STATIC_BUILD_ECORE_CON
EINA_MODULE_INIT(ecore_con_binding_init);
EINA_MODULE_SHUTDOWN(ecore_con_binding_shutdown);
#endif


#include "E_DBus.h"
#include <Ecore_Data.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct E_DBus_Object_Method E_DBus_Object_Method;


Ecore_Strbuf * e_dbus_object_introspect(E_DBus_Object *obj);

static void e_dbus_object_unregister(DBusConnection *conn, void *user_data);
static DBusHandlerResult e_dbus_object_handler(DBusConnection *conn, DBusMessage *message, void *user_data);
static E_DBus_Object_Method *e_dbus_object_method_new(const char *interface, const char *member, const char *signature, const char *reply_signature, E_DBus_Object_Method_Cb func);
static void e_dbus_object_method_free(E_DBus_Object_Method *m);

static void _introspect_indent_append(Ecore_Strbuf *buf, int level);
static void _introspect_method_append(Ecore_Strbuf *buf, E_DBus_Object_Method *method, int level);
static void _introspect_arg_append(Ecore_Strbuf *buf, const char *type, const char *direction, int level);


//static Ecore_List *standard_methods = NULL;


static DBusObjectPathVTable vtable = (DBusObjectPathVTable){
  e_dbus_object_unregister,
  e_dbus_object_handler,
  NULL,
  NULL,
  NULL,
  NULL
};

struct E_DBus_Object
{
  DBusConnection *conn;
  char *path;
  Ecore_List *methods;

  void *data;
};

struct E_DBus_Object_Method
{
  char *interface;
  char *member;
  char *signature;
  char *reply_signature;
  E_DBus_Object_Method_Cb func;
};

static DBusMessage *
cb_introspect(E_DBus_Object *obj, DBusMessage *msg)
{
  DBusMessage *ret;
  Ecore_Strbuf *buf;
  const char *xml;

  buf = e_dbus_object_introspect(obj);
  if (!buf)
  {
    ret = dbus_message_new_error(msg, "org.e.NotIntrospectable", "This object does not provide introspection data");
    return ret;
    /* XXX send an error */
  }

  xml = ecore_strbuf_string_get(buf);
  printf("XML: \n\n%s\n\n", xml);
  ret = dbus_message_new_method_return(msg);
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &xml, DBUS_TYPE_INVALID);

  ecore_strbuf_free(buf);
  return ret;
}
#if 0
int
e_dbus_object_init(void)
{
  E_DBus_Method *m;

  if (standard_methods) return;
  standard_methods = ecore_list_new();

  m = e_dbus_object_method_new("org.freedesktop.DBus.Introspectable", "Introspect", "", "s", cb_introspect);

}
#endif
/**
 * Add a dbus object.
 */
E_DBus_Object *
e_dbus_object_add(DBusConnection *conn, const char *object_path, void *data)
{
  E_DBus_Object *obj;

  obj = calloc(1, sizeof(E_DBus_Object));
  if (!obj) return NULL;

  if (!dbus_connection_register_object_path(conn, object_path, &vtable, obj))
  {
    free(obj);
    return NULL;
  }

  obj->conn = conn;
  dbus_connection_ref(conn);
  obj->path = strdup(object_path);
  obj->data = data;
  obj->methods = ecore_list_new();
  ecore_list_set_free_cb(obj->methods, (Ecore_Free_Cb)e_dbus_object_method_free);

  e_dbus_object_method_add(obj, "org.freedesktop.DBus.Introspectable", "Introspect", NULL, "s", cb_introspect);

  return obj;
}

void
e_dbus_object_free(E_DBus_Object *obj)
{
  if (!obj) return;

  DEBUG(5, "e_dbus_object_free\n");
  dbus_connection_unregister_object_path(obj->conn, obj->path);
  dbus_connection_unref(obj->conn);

  if (obj->path) free(obj->path);
  ecore_list_destroy(obj->methods);

  free(obj);
}

/**
 * Add a method to an object
 *
 * @param obj the object
 * @param interface the interface name (e.g. org.freedesktop.DBus)
 * @param member the name of the method (e.g. AddMatch)
 * @param signature  an optional message signature. if provided, then messages
 *                   with invalid signatures will be automatically rejected 
 *                   (an Error response will be sent)
 *
 * @return 1 if successful, 0 if failed (e.g. no memory)
 */
int
e_dbus_object_method_add(E_DBus_Object *obj, const char *interface, const char *member, const char *signature, const char *reply_signature, E_DBus_Object_Method_Cb func)
{
  E_DBus_Object_Method *m;

  m = e_dbus_object_method_new(interface, member, signature, reply_signature, func);
  if (!m) return 0;

  if (obj)
    ecore_list_append(obj->methods, m);
  return 1;
}

static E_DBus_Object_Method *
e_dbus_object_method_new(const char *interface, const char *member, const char *signature, const char *reply_signature, E_DBus_Object_Method_Cb func)
{
  E_DBus_Object_Method *m;

  if (!interface || !member || !func) return NULL;

  if (signature && !dbus_signature_validate(signature, NULL)) return NULL;
  if (reply_signature && !dbus_signature_validate(reply_signature, NULL)) return NULL;
  m = calloc(1, sizeof(E_DBus_Object_Method));
  if (!m) return NULL;

  m->interface = strdup(interface);
  m->member = strdup(member);
  if (signature)
    m->signature = strdup(signature);
  if (reply_signature)
    m->reply_signature = strdup(reply_signature);
  m->func = func;

  return m;
}

static void
e_dbus_object_method_free(E_DBus_Object_Method *m)
{
  if (!m) return;
  if (m->interface) free(m->interface);
  if (m->member) free(m->member);
  if (m->signature) free(m->signature);
  if (m->reply_signature) free(m->reply_signature);

  free(m);
}

static E_DBus_Object_Method *
e_dbus_object_method_find(E_DBus_Object *obj, const char *interface, const char *member)
{
  E_DBus_Object_Method *m;
  if (!obj || !member) return NULL;

  ecore_list_goto_first(obj->methods);
  while ((m = ecore_list_next(obj->methods)))
  {
    if ( (!interface || !strcmp(interface, m->interface)) &&
         !strcmp(member, m->member))
    {
      return m;
    }
  }
  return NULL;
}

static DBusHandlerResult
e_dbus_object_handler(DBusConnection *conn, DBusMessage *message, void *user_data) 
{
  E_DBus_Object *obj;
  E_DBus_Object_Method *m;
  DBusMessage *reply;
  dbus_uint32_t serial;

  obj = user_data;
  if (!obj)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  m = e_dbus_object_method_find(obj, dbus_message_get_interface(message), dbus_message_get_member(message));

  /* XXX should this send an 'invalid method' error instead? */
  if (!m) 
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (m->signature && !dbus_message_has_signature(message, m->signature))
    reply = dbus_message_new_error_printf(message, "InvalidSignature", "Expected signature: %s", m->signature);
  else
    reply = m->func(obj, message);

  dbus_connection_send(conn, reply, &serial);
  dbus_message_unref(reply);

  return DBUS_HANDLER_RESULT_HANDLED;
}

static void
e_dbus_object_unregister(DBusConnection *conn, void *user_data)
{
  /* free up the object struct? */
}

Ecore_Strbuf *
e_dbus_object_introspect(E_DBus_Object *obj)
{
  Ecore_Strbuf *buf;
  char *current_interface = NULL;
  int level = 0;
  E_DBus_Object_Method *method;

  buf = ecore_strbuf_new();

  /* Doctype */
  ecore_strbuf_append(buf, "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n");

  ecore_strbuf_append(buf, "<node name=\"");
  ecore_strbuf_append(buf, obj->path);
  ecore_strbuf_append(buf, "\">\n");
  level++;
  /* XXX currently assumes methods grouped by interface. should probably sort first */
  ecore_list_goto_first(obj->methods);
  while ((method = ecore_list_next(obj->methods))) 
  {
    if (current_interface != method->interface)
    {
      if (current_interface)
      {
        level--;
        _introspect_indent_append(buf, level);
        ecore_strbuf_append(buf, "</interface>\n");
      }
      _introspect_indent_append(buf, level);
      ecore_strbuf_append(buf, "<interface name=\"");
      ecore_strbuf_append(buf, method->interface);
      ecore_strbuf_append(buf, "\">\n");
      level++;

      current_interface = method->interface;
    }

    _introspect_method_append(buf, method, level);
  }
  if (current_interface)
  {
    level--;
    _introspect_indent_append(buf, level);
    ecore_strbuf_append(buf, "</interface>\n");
  }
  ecore_strbuf_append(buf, "</node>\n");
  return buf;
}

static void
_introspect_indent_append(Ecore_Strbuf *buf, int level)
{
  /* XXX optimize this? */
  int i = level * 2;
  while (i-- > 0)
    ecore_strbuf_append_char(buf, ' ');
}
static void
_introspect_method_append(Ecore_Strbuf *buf, E_DBus_Object_Method *method, int level)
{
  DBusSignatureIter iter;
  char *type;

  _introspect_indent_append(buf, level);
  ecore_strbuf_append(buf, "<method name=\"");
  ecore_strbuf_append(buf, method->member);
  ecore_strbuf_append(buf, "\">\n");
  level++;

  /* append args */
  if (method->signature && dbus_signature_validate(method->signature, NULL))
  {
    dbus_signature_iter_init(&iter, method->signature);
    while ((type = dbus_signature_iter_get_signature(&iter)))
    {
      _introspect_arg_append(buf, type, "in", level);

      dbus_free(type);
      if (!dbus_signature_iter_next(&iter)) break;
    }
  }

  /* append reply args */
  if (method->reply_signature && dbus_signature_validate(method->reply_signature, NULL))
  {
    printf("valid reply sig: '%s'\n", method->reply_signature);
    dbus_signature_iter_init(&iter, method->reply_signature);
    while ((type = dbus_signature_iter_get_signature(&iter)))
    {
      printf("got type: '%s'\n", type);
      _introspect_arg_append(buf, type, "out", level);

      dbus_free(type);
      if (!dbus_signature_iter_next(&iter)) break;
    }
  }

  level--;
  _introspect_indent_append(buf, level);
  ecore_strbuf_append(buf, "</method>\n");
}

static void
_introspect_arg_append(Ecore_Strbuf *buf, const char *type, const char *direction, int level)
{
  _introspect_indent_append(buf, level);
  ecore_strbuf_append(buf, "<arg type=\"");
  ecore_strbuf_append(buf, type);
  ecore_strbuf_append(buf, "\" direction=\"");
  ecore_strbuf_append(buf, direction);
  ecore_strbuf_append(buf, "\">\n");
}


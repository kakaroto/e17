#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "e_dbus_private.h"

static E_DBus_Interface *introspectable_interface = NULL;
static E_DBus_Interface *properties_interface = NULL;

typedef struct E_DBus_Method E_DBus_Method;
typedef struct E_DBus_Signal E_DBus_Signal;

Eina_Strbuf * e_dbus_object_introspect(E_DBus_Object *obj);

static void e_dbus_object_unregister(DBusConnection *conn, void *user_data);
static DBusHandlerResult e_dbus_object_handler(DBusConnection *conn, DBusMessage *message, void *user_data);

static void e_dbus_interface_free(E_DBus_Interface *iface);

static E_DBus_Method *e_dbus_method_new(const char *member, const char *signature, const char *reply_signature, E_DBus_Method_Cb func);
static void e_dbus_object_method_free(E_DBus_Method *m);

static E_DBus_Signal *e_dbus_signal_new(const char *name, const char *signature);
static void e_dbus_object_signal_free(E_DBus_Signal *s);

static void _introspect_indent_append(Eina_Strbuf *buf, int level);
static void _introspect_interface_append(Eina_Strbuf *buf, E_DBus_Interface *iface, int level);
static void _introspect_method_append(Eina_Strbuf *buf, E_DBus_Method *method, int level);
static void _introspect_signal_append(Eina_Strbuf *buf, E_DBus_Signal *signal, int level);
static void _introspect_arg_append(Eina_Strbuf *buf, const char *type, const char *direction, int level);


//static Eina_List *standard_methods = NULL;


static DBusObjectPathVTable vtable = {
  e_dbus_object_unregister,
  e_dbus_object_handler,
  NULL,
  NULL,
  NULL,
  NULL
};

struct E_DBus_Object
{
  E_DBus_Connection *conn;
  char *path;
  Eina_List *interfaces;
  char *introspection_data;
  int introspection_dirty;

  E_DBus_Object_Property_Get_Cb cb_property_get;
  E_DBus_Object_Property_Set_Cb cb_property_set;

  void *data;
};

struct E_DBus_Interface
{
  char *name;
  Eina_List *methods;
  Eina_List *signals;
  int refcount;
};

struct E_DBus_Method
{
  char *member;
  char *signature;
  char *reply_signature;
  E_DBus_Method_Cb func;
};

struct E_DBus_Signal
{
  char *name;
  char *signature;
};

static DBusMessage *
cb_introspect(E_DBus_Object *obj, DBusMessage *msg)
{
  DBusMessage *ret;
  Eina_Strbuf *buf;

  if (obj->introspection_dirty || !obj->introspection_data)
  {
    buf = e_dbus_object_introspect(obj);
    if (!buf)
    {
      ret = dbus_message_new_error(msg, "org.enlightenment.NotIntrospectable", "This object does not provide introspection data");
      return ret;
    }

    if (obj->introspection_data) free(obj->introspection_data);
    obj->introspection_data = strdup(eina_strbuf_string_get(buf));
    eina_strbuf_free(buf);
  }
  //printf("XML: \n\n%s\n\n", obj->introspection_data);
  ret = dbus_message_new_method_return(msg);
  dbus_message_append_args(ret, DBUS_TYPE_STRING, &(obj->introspection_data), DBUS_TYPE_INVALID);

  return ret;
}

static DBusMessage *
cb_properties_get(E_DBus_Object *obj, DBusMessage *msg)
{
  DBusMessage *reply;
  DBusMessageIter iter, sub;
  DBusError err;
  int type;
  void *value;
  char *property;

  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);

  if (dbus_error_is_set(&err))
  {
    return dbus_message_new_error(msg, err.name, err.message);
  }

  obj->cb_property_get(obj, property, &type, &value);
  if (type == DBUS_TYPE_INVALID)
  {
    return dbus_message_new_error_printf(msg, "org.enlightenment.DBus.InvalidProperty", "The property '%s' does not exist on this object.", property);
  }

  if (dbus_type_is_basic(type))
  {
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(msg, &iter);
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, e_dbus_basic_type_as_string(type), &sub);
    dbus_message_iter_append_basic(&sub, type, &value);
    dbus_message_iter_close_container(&iter, &sub);
    return reply;
  }
  else
  {
    return dbus_message_new_error(msg, "org.enlightenment.DBus.UnsupportedType", "E_DBus currently only supports properties of a basic type.");
  }
}

static DBusMessage *
cb_properties_set(E_DBus_Object *obj, DBusMessage *msg)
{
  DBusMessageIter iter, sub;
  int type;
  void *value;
  char *property;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_get_basic(&iter, &property);
  dbus_message_iter_recurse(&iter, &sub);
  type = dbus_message_iter_get_arg_type(&sub);
  if (dbus_type_is_basic(type))
  {
    dbus_message_iter_get_basic(&sub, &value);
    if (obj->cb_property_set(obj, property, type, value))
    {
      return dbus_message_new_method_return(msg);
    }
    else
    {
      return dbus_message_new_error_printf(msg, "org.enlightenment.DBus.InvalidProperty", "The property '%s' does not exist on this object.", property);
    }
  }
  else
  {
    return dbus_message_new_error(msg, "org.enlightenment.DBus.UnsupportedType", "E_DBus currently only supports properties of a basic type.");
  }

}

int
e_dbus_object_init(void)
{
  introspectable_interface = e_dbus_interface_new("org.freedesktop.DBus.Introspectable");
  properties_interface = e_dbus_interface_new(E_DBUS_FDO_INTERFACE_PROPERTIES);
  if (!introspectable_interface || !properties_interface)
  {
    if (introspectable_interface) e_dbus_interface_unref(introspectable_interface);
    introspectable_interface = NULL;
    if (properties_interface) e_dbus_interface_unref(properties_interface);
    properties_interface = NULL;
    return 0;
  }

  e_dbus_interface_method_add(introspectable_interface, "Introspect", "", "s", cb_introspect);
  e_dbus_interface_method_add(properties_interface, "Get", "s", "v", cb_properties_get);
  e_dbus_interface_method_add(properties_interface, "Set", "sv", "", cb_properties_set);
  return 1;
}

void
e_dbus_object_shutdown(void)
{
  e_dbus_interface_unref(introspectable_interface);
  introspectable_interface = NULL;

  e_dbus_interface_unref(properties_interface);
  properties_interface = NULL;
}

EAPI E_DBus_Object *
e_dbus_object_add(E_DBus_Connection *conn, const char *object_path, void *data)
{
  E_DBus_Object *obj;

  EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);
  EINA_SAFETY_ON_NULL_RETURN_VAL(object_path, NULL);

  obj = calloc(1, sizeof(E_DBus_Object));
  if (!obj) return NULL;

  if (!dbus_connection_register_object_path(conn->conn, object_path, &vtable, obj))
  {
    free(obj);
    return NULL;
  }

  obj->conn = conn;
  e_dbus_connection_ref(conn);
  obj->path = strdup(object_path);
  obj->data = data;
  obj->interfaces = NULL;

  e_dbus_object_interface_attach(obj, introspectable_interface);

  return obj;
}

EAPI void
e_dbus_object_free(E_DBus_Object *obj)
{
  E_DBus_Interface *iface;

  if (!obj) return;

  DBG("e_dbus_object_free (%s)", obj->path);
  dbus_connection_unregister_object_path(obj->conn->conn, obj->path);
  e_dbus_connection_close(obj->conn);

  if (obj->path) free(obj->path);
  EINA_LIST_FREE(obj->interfaces, iface)
    e_dbus_interface_unref(iface);

  if (obj->introspection_data) free(obj->introspection_data);

  free(obj);
}

EAPI void *
e_dbus_object_data_get(E_DBus_Object *obj)
{
  return obj->data;
}

EAPI E_DBus_Connection *
e_dbus_object_conn_get(E_DBus_Object *obj)
{
  return obj->conn;
}

EAPI const char *
e_dbus_object_path_get(E_DBus_Object *obj)
{
  return obj->path;
}

EAPI const Eina_List *
e_dbus_object_interfaces_get(E_DBus_Object *obj)
{
  return obj->interfaces;
}

EAPI void
e_dbus_object_property_get_cb_set(E_DBus_Object *obj, E_DBus_Object_Property_Get_Cb func)
{
  obj->cb_property_get = func;
}

EAPI void
e_dbus_object_property_set_cb_set(E_DBus_Object *obj, E_DBus_Object_Property_Set_Cb func)
{
  obj->cb_property_set = func;
}

EAPI void
e_dbus_object_interface_attach(E_DBus_Object *obj, E_DBus_Interface *iface)
{
  EINA_SAFETY_ON_NULL_RETURN(obj);
  EINA_SAFETY_ON_NULL_RETURN(iface);

  e_dbus_interface_ref(iface);
  obj->interfaces = eina_list_append(obj->interfaces, iface);
  obj->introspection_dirty = 1;
  DBG("e_dbus_object_interface_attach (%s, %s) ", obj->path, iface->name);
}

EAPI void
e_dbus_object_interface_detach(E_DBus_Object *obj, E_DBus_Interface *iface)
{
  E_DBus_Interface *found;

  DBG("e_dbus_object_interface_detach (%s, %s) ", obj->path, iface->name);
  found = eina_list_data_find(obj->interfaces, iface);
  if (!found) return;

  obj->interfaces = eina_list_remove(obj->interfaces, iface);
  obj->introspection_dirty = 1;
  e_dbus_interface_unref(iface);
}

EAPI void
e_dbus_interface_ref(E_DBus_Interface *iface)
{
  iface->refcount++;
  DBG("e_dbus_interface_ref (%s) = %d", iface->name, iface->refcount);
}

EAPI void
e_dbus_interface_unref(E_DBus_Interface *iface)
{
  DBG("e_dbus_interface_unref (%s) = %d", iface->name, iface->refcount - 1);
  if (--(iface->refcount) == 0)
    e_dbus_interface_free(iface);
}

static void
e_dbus_interface_free(E_DBus_Interface *iface)
{
  E_DBus_Method *m;
  E_DBus_Signal *s;

  if (iface->name) free(iface->name);
  EINA_LIST_FREE(iface->methods, m)
    e_dbus_object_method_free(m);
  EINA_LIST_FREE(iface->signals, s)
    e_dbus_object_signal_free(s);
  free(iface);
}


EAPI int
e_dbus_interface_method_add(E_DBus_Interface *iface, const char *member, const char *signature, const char *reply_signature, E_DBus_Method_Cb func)
{
  E_DBus_Method *m;

  m = e_dbus_method_new(member, signature, reply_signature, func);
  DBG("E-dbus: Add method %s: %p", member, m);
  if (!m) return 0;

  iface->methods = eina_list_append(iface->methods, m);
  return 1;
}

EAPI int
e_dbus_interface_signal_add(E_DBus_Interface *iface, const char *name, const char *signature)
{
  E_DBus_Signal *s;

  s = e_dbus_signal_new(name, signature);
  DBG("E-dbus: Add signal %s: %p", name, s);
  if (!s) return 0;

  iface->signals = eina_list_append(iface->signals, s);
  return 1;
}

EAPI E_DBus_Interface *
e_dbus_interface_new(const char *interface)
{
  E_DBus_Interface *iface;

  if (!interface) return NULL;

  iface = calloc(1, sizeof(E_DBus_Interface));
  if (!iface) return NULL;

  iface->refcount = 1;
  iface->name = strdup(interface);
  iface->methods = NULL;
  iface->signals = NULL;

  return iface;
}

static E_DBus_Method *
e_dbus_method_new(const char *member, const char *signature, const char *reply_signature, E_DBus_Method_Cb func)
{
  E_DBus_Method *m;

  if (!member || !func) return NULL;

  if (signature && !dbus_signature_validate(signature, NULL)) return NULL;
  if (reply_signature && !dbus_signature_validate(reply_signature, NULL)) return NULL;
  m = calloc(1, sizeof(E_DBus_Method));
  if (!m) return NULL;

  m->member = strdup(member);
  if (signature)
    m->signature = strdup(signature);
  if (reply_signature)
    m->reply_signature = strdup(reply_signature);
  m->func = func;

  return m;
}

static void
e_dbus_object_method_free(E_DBus_Method *m)
{
  if (!m) return;
  if (m->member) free(m->member);
  if (m->signature) free(m->signature);
  if (m->reply_signature) free(m->reply_signature);

  free(m);
}

static E_DBus_Signal *
e_dbus_signal_new(const char *name, const char *signature)
{
  E_DBus_Signal *s;

  if (!name) return NULL;

  if (signature && !dbus_signature_validate(signature, NULL)) return NULL;
  s = calloc(1, sizeof(E_DBus_Signal));
  if (!s) return NULL;

  s->name = strdup(name);
  if (signature)
    s->signature = strdup(signature);

  return s;
}

static void
e_dbus_object_signal_free(E_DBus_Signal *s)
{
  if (!s) return;
  if (s->name) free(s->name);
  if (s->signature) free(s->signature);
  free(s);
}

static E_DBus_Method *
e_dbus_object_method_find(E_DBus_Object *obj, const char *interface, const char *member)
{
  E_DBus_Method *m;
  E_DBus_Interface *iface;
  Eina_List *l, *ll;

  if (!obj || !member) return NULL;

  EINA_LIST_FOREACH(obj->interfaces, l, iface)
  {
    if (strcmp(interface, iface->name)) continue;
    EINA_LIST_FOREACH(iface->methods, ll, m)
    {
      if (!strcmp(member, m->member))
        return m;
    }
  }
  return NULL;
}

static DBusHandlerResult
e_dbus_object_handler(DBusConnection *conn, DBusMessage *message, void *user_data) 
{
  E_DBus_Object *obj;
  E_DBus_Method *m;
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
    reply = dbus_message_new_error_printf(message, "org.enlightenment.InvalidSignature", "Expected signature: %s", m->signature);
  else
    reply = m->func(obj, message);

  /* user can choose reply later */
  if (!reply)
    return DBUS_HANDLER_RESULT_HANDLED;

  dbus_connection_send(conn, reply, &serial);
  dbus_message_unref(reply);

  return DBUS_HANDLER_RESULT_HANDLED;
}

static void
e_dbus_object_unregister(DBusConnection *conn __UNUSED__, void *user_data __UNUSED__)
{
  /* free up the object struct? */
}

Eina_Strbuf *
e_dbus_object_introspect(E_DBus_Object *obj)
{
  Eina_Strbuf *buf;
  int level = 0;
  E_DBus_Interface *iface;
  Eina_List *l;

  buf = eina_strbuf_new();

  /* Doctype */
  eina_strbuf_append(buf, "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n");

  eina_strbuf_append(buf, "<node name=\"");
  eina_strbuf_append(buf, obj->path);
  eina_strbuf_append(buf, "\">\n");
  level++;

  EINA_LIST_FOREACH(obj->interfaces, l, iface)
    _introspect_interface_append(buf, iface, level);

  eina_strbuf_append(buf, "</node>\n");
  return buf;
}

static void
_introspect_indent_append(Eina_Strbuf *buf, int level)
{
  /* XXX optimize this? */
  int i = level * 2;
  while (i-- > 0)
    eina_strbuf_append_char(buf, ' ');
}
static void
_introspect_interface_append(Eina_Strbuf *buf, E_DBus_Interface *iface, int level)
{
  E_DBus_Method *m;
  E_DBus_Signal *s;
  Eina_List *l;

  _introspect_indent_append(buf, level);
  eina_strbuf_append(buf, "<interface name=\"");
  eina_strbuf_append(buf, iface->name);
  eina_strbuf_append(buf, "\">\n");
  level++;

  DBG("introspect iface: %s", iface->name);
  EINA_LIST_FOREACH(iface->methods, l, m)
    _introspect_method_append(buf, m, level);
  EINA_LIST_FOREACH(iface->signals, l, s)
    _introspect_signal_append(buf, s, level);

  level--;
  _introspect_indent_append(buf, level);
  eina_strbuf_append(buf, "</interface>\n");
}
static void
_introspect_method_append(Eina_Strbuf *buf, E_DBus_Method *method, int level)
{
  DBusSignatureIter iter;
  char *type;

  _introspect_indent_append(buf, level);
  DBG("introspect method: %s\n", method->member);
  eina_strbuf_append(buf, "<method name=\"");
  eina_strbuf_append(buf, method->member);
  eina_strbuf_append(buf, "\">\n");
  level++;

  /* append args */
  if (method->signature && 
      method->signature[0] &&
      dbus_signature_validate(method->signature, NULL))
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
  if (method->reply_signature &&
      method->reply_signature[0] &&
      dbus_signature_validate(method->reply_signature, NULL))
  {
    dbus_signature_iter_init(&iter, method->reply_signature);
    while ((type = dbus_signature_iter_get_signature(&iter)))
    {
      _introspect_arg_append(buf, type, "out", level);

      dbus_free(type);
      if (!dbus_signature_iter_next(&iter)) break;
    }
  }

  level--;
  _introspect_indent_append(buf, level);
  eina_strbuf_append(buf, "</method>\n");
}

static void
_introspect_signal_append(Eina_Strbuf *buf, E_DBus_Signal *s, int level)
{
  DBusSignatureIter iter;
  char *type;

  _introspect_indent_append(buf, level);
  DBG("introspect signal: %s", s->name);
  eina_strbuf_append(buf, "<signal name=\"");
  eina_strbuf_append(buf, s->name);
  eina_strbuf_append(buf, "\">\n");
  level++;

  /* append args */
  if (s->signature &&
      s->signature[0] &&
      dbus_signature_validate(s->signature, NULL))
  {
    dbus_signature_iter_init(&iter, s->signature);
    while ((type = dbus_signature_iter_get_signature(&iter)))
    {
      _introspect_arg_append(buf, type, NULL, level);

      dbus_free(type);
      if (!dbus_signature_iter_next(&iter)) break;
    }
  }

  level--;
  _introspect_indent_append(buf, level);
  eina_strbuf_append(buf, "</signal>\n");
}

static void
_introspect_arg_append(Eina_Strbuf *buf, const char *type, const char *direction, int level)
{
  _introspect_indent_append(buf, level);
  eina_strbuf_append(buf, "<arg type=\"");
  eina_strbuf_append(buf, type);
  if (direction)
  {
    eina_strbuf_append(buf, "\" direction=\"");
    eina_strbuf_append(buf, direction);
  }
  eina_strbuf_append(buf, "\"/>\n");
}


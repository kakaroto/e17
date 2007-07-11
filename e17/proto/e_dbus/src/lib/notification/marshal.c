#include "E_Notify.h"
#include <string.h>


#define e_notification_call_new(member) dbus_message_new_method_call(E_NOTIFICATION_DESTINATION, E_NOTIFICATION_PATH, E_NOTIFICATION_INTERFACE, member)

Ecore_List *unmarshal_notify_actions(E_Notification *n, DBusMessageIter *iter);
Ecore_List *unmarshal_notify_hints(E_Notification *n, DBusMessageIter *iter);
void marshal_hint_image(DBusMessageIter *iter, E_Notification_Image *img);
E_Notification_Image *unmarshal_hint_image(DBusMessageIter *iter);

typedef void (*E_DBus_Variant_Marshaller) (DBusMessageIter *iter, void *data);
#define E_DBUS_VARIANT_MARSHALLER(x) ((E_DBus_Variant_Marshaller)(x))

static void
marshal_dict_variant(DBusMessageIter *iter, const char *key, char *type_str, E_DBus_Variant_Marshaller func, void *data)
{
  DBusMessageIter entry, variant;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, "sv", &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
  func(&entry, data);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, type_str, &variant);
  func(&variant, data);
  dbus_message_iter_close_container(&entry, &variant);
  dbus_message_iter_close_container(iter, &entry);
}

static void
marshal_dict_string(DBusMessageIter *iter, const char *key, const char *value)
{
  DBusMessageIter entry, variant;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, "sv", &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "s", &variant);
  dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &value);
  dbus_message_iter_close_container(&entry, &variant);
  dbus_message_iter_close_container(iter, &entry);
}

static void
marshal_dict_byte(DBusMessageIter *iter, const char *key, char value)
{
  DBusMessageIter entry, variant;

  if (!key || !value) return;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, "sv", &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "y", &variant);
  dbus_message_iter_append_basic(&variant, DBUS_TYPE_BYTE, &value);
  dbus_message_iter_close_container(&entry, &variant);
  dbus_message_iter_close_container(iter, &entry);
}

static void
marshal_dict_int(DBusMessageIter *iter, const char *key, int value)
{
  DBusMessageIter entry, variant;

  if (!key || !value) return;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, "sv", &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "i", &variant);
  dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &value);
  dbus_message_iter_close_container(&entry, &variant);
  dbus_message_iter_close_container(iter, &entry);
}

static void
marshal_string_list_as_array(DBusMessageIter *iter, Ecore_List *strings)
{
  const char *str;
  DBusMessageIter arr;

  dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY, "s", &arr);

  ecore_list_goto_first(strings);
  while((str = ecore_list_next(strings)))
    dbus_message_iter_append_basic(&arr, DBUS_TYPE_STRING, &str);

  dbus_message_iter_close_container(iter, &arr);
}

Ecore_List *
unmarshal_string_array_as_list(DBusMessageIter *iter)
{
  Ecore_List *strings;
  char *sig;
  int ret;
  DBusMessageIter arr;

  sig = dbus_message_iter_get_signature(iter);
  ret = !strcmp(sig, "as");
  dbus_free(sig);
  if (!ret) return NULL;

  strings = ecore_list_new();
  ecore_list_set_free_cb(strings, ECORE_FREE_CB(free)); //XXX use ecore_string_release?

  dbus_message_iter_recurse(iter, &arr);
  while(dbus_message_iter_has_next(&arr))
  {
    const char *str;
    dbus_message_get_basic(&arr, &str);
    ecore_list_append(strings, strdup(str)); //XXX use ecore_string_instance?
    dbus_message_iter_next(&arr);
  }
  return strings;
}



DBusMessage *
marshal_get_capabilities()
{
  DBusMessage *msg;

  msg = e_notification_call_new("GetCapabilities");
  return msg;
}

DBusMessage *
marshal_get_capabilities_return(DBusMessage *method_call, Ecore_List *capabilities)
{
  DBusMessage *msg;
  DBusMessageIter iter;

  msg = dbus_message_new_method_return(method_call);
  dbus_message_iter_init_append(msg, &iter);
  marshal_string_list_as_array(&iter, capabilities);

  return msg;
}

E_Notification_Return_Get_Capabilities *
unmarshal_get_capabilities_return(DBusMessage *msg)
{
  DBusMessageIter iter, arr;
  E_Notification_Return_Get_Capabilities *ret;
  Ecore_List *capabilities;

  if (!dbus_message_has_signature(msg, "as")) return NULL;

  ret = calloc(1, sizeof(E_Notification_Return_Get_Capabilities));
  dbus_message_iter_init(msg, &iter);
  ret->capabilities = unmarshal_string_array_as_list(&iter);

  return ret;
}

DBusMessage *
marshal_get_server_information()
{
  DBusMessage *msg;

  msg = e_notification_call_new("GetServerInformation");
  return msg;
}

DBusMessage *
marshal_get_server_information_return(DBusMessage *method_call, const char *name, const char *vendor, const char *version)
{
  DBusMessage *msg;
  msg = dbus_message_new_method_return(method_call);
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_STRING, &vendor, DBUS_TYPE_STRING, &version, DBUS_TYPE_INVALID);
  return msg;
}

E_Notification_Return_Get_Server_Information *
unmarshal_get_server_information_return(DBusMessage *msg)
{
  E_Notification_Return_Get_Server_Information *info;
  DBusError err;
  dbus_error_init(&err);
  if (!dbus_message_has_signature(msg, "sss")) return NULL;

  info = calloc(1, sizeof(E_Notification_Return_Get_Server_Information));
  dbus_message_get_args(msg, &err,
    DBUS_TYPE_STRING, &(info->name),
    DBUS_TYPE_STRING, &(info->vendor),
    DBUS_TYPE_STRING, &(info->version),
    DBUS_TYPE_INVALID
  );
  if (dbus_error_is_set(&err))
  {
    dbus_error_free(&err);
    return NULL;
  }

  return info;
}


DBusMessage *
marshal_close_notification(dbus_uint32_t id)
{
  DBusMessage *msg;

  msg = e_notification_call_new("CloseNotification");
  dbus_message_append_args(msg, DBUS_TYPE_UINT32, &id, DBUS_TYPE_INVALID);
  return msg;
}

dbus_uint32_t
unmarshal_close_notification(DBusMessage *msg)
{
  dbus_uint32_t id;
  DBusError err;
  dbus_error_init(&err);
  if (!dbus_message_has_signature(msg, "u")) return 0;
  dbus_message_get_args(msg, &err, DBUS_TYPE_UINT32, &id, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    dbus_error_free(&err);
    return 0;
  }
  return id;
}

DBusMessage *
marshal_notification_closed_signal(dbus_uint32_t id, dbus_uint32_t reason)
{
  DBusMessage *msg;
  msg = dbus_message_new_signal(E_NOTIFICATION_PATH, E_NOTIFICATION_INTERFACE, "NotificationClosed");
  dbus_message_append_args(msg, DBUS_TYPE_UINT32, &id, DBUS_TYPE_UINT32, &reason, DBUS_TYPE_INVALID);
  return msg;
}

E_Notification_Event_Notification_Closed *
unmarshal_notification_closed_signal(DBusMessage *msg)
{
  E_Notification_Event_Notification_Closed *ev;
  DBusError err;
  dbus_error_init(&err);

  if (!dbus_message_has_signature(msg, "uu")) return NULL;
  ev = calloc(1, sizeof(E_Notification_Event_Notification_Closed));
  dbus_message_get_args(msg, &err, DBUS_TYPE_UINT32, &(ev->notification_id), DBUS_TYPE_UINT32, &(ev->reason), DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    dbus_error_free(&err);
    return NULL;
  }
}

DBusMessage *
marshal_action_invoked_signal(dbus_uint32_t id, const char *action_id)
{
  DBusMessage *msg;
  msg = dbus_message_new_signal(E_NOTIFICATION_PATH, E_NOTIFICATION_INTERFACE, "ActionInvoked");
  dbus_message_append_args(msg, DBUS_TYPE_UINT32, &id, DBUS_TYPE_STRING, &action_id, DBUS_TYPE_INVALID);
  return msg;
}

E_Notification_Event_Action_Invoked *
unmarshal_action_invoked_signal(DBusMessage *msg)
{
  E_Notification_Event_Action_Invoked *ev;
  DBusError err;
  dbus_error_init(&err);

  if (!dbus_message_has_signature(msg, "us")) return NULL;
  ev = calloc(1, sizeof(E_Notification_Event_Action_Invoked));
  dbus_message_get_args(msg, &err, DBUS_TYPE_UINT32, &(ev->notification_id), DBUS_TYPE_STRING, &(ev->action_id), DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    dbus_error_free(&err);
    return NULL;
  }
  return ev;
}

DBusMessage *
marshal_notify(E_Notification *n)
{
  DBusMessage *msg;
  DBusMessageIter iter, sub;

  msg = e_notification_call_new("Notify");
  dbus_message_append_args(msg, 
    DBUS_TYPE_STRING, &(n->app_name),
    DBUS_TYPE_UINT32, &(n->replaces_id),
    DBUS_TYPE_STRING, &(n->app_icon),
    DBUS_TYPE_STRING, &(n->summary),
    DBUS_TYPE_STRING, &(n->body),
    DBUS_TYPE_INVALID
  );

  dbus_message_iter_init_append(msg, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &sub);
  if (n->actions)
  {
    E_Notification_Action *action;
    ecore_list_goto_first(n->actions);
    while ((action = ecore_list_next(n->actions)))
    {
      dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &(action->id));
      dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &(action->name));
    }
  }
  dbus_message_iter_close_container(&iter, &sub);

  /* hints */
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &sub);
  if (n->hints.urgency) /* we only need to send this if its non-zero*/
    marshal_dict_byte(&sub, "urgency", n->hints.urgency);
  if (n->hints.category)
    marshal_dict_string(&sub, "category", n->hints.category);
  if (n->hints.desktop)
    marshal_dict_string(&sub, "desktop_entry", n->hints.desktop);
  if (n->hints.image_data)
    marshal_dict_variant(&sub, "image_data", "(iiibiiay)", E_DBUS_VARIANT_MARSHALLER(marshal_hint_image), n->hints.image_data);
    //marshal_hint_image(&sub, "image_data", n->hints.image_data);
  if (n->hints.sound_file)
    marshal_dict_string(&sub, "sound_file", n->hints.sound_file);
  if (n->hints.suppress_sound) /* we only need to send this if its true */
    marshal_dict_byte(&sub, "suppress_sound", n->hints.suppress_sound);
  if (n->hints.x > -1 && n->hints.y > -1)
  {
    marshal_dict_int(&sub, "x", n->hints.x);
    marshal_dict_int(&sub, "y", n->hints.y);
  }

  dbus_message_iter_close_container(&iter, &sub);
  return msg;
}

E_Notification *
unmarshal_notify(DBusMessage *msg)
{
  E_Notification *n;
  const char *s_val;
  dbus_uint32_t u_val;
  dbus_int32_t i_val;
  DBusMessageIter iter, arr;
  if (!dbus_message_has_signature(msg, "susssasa{sv}i")) return NULL;

  n = e_notification_new();
  if (!n) return NULL;
  dbus_message_iter_init(msg, &iter);

  dbus_message_iter_get_basic(&iter, &s_val);
  e_notification_name_set(n, s_val); 
  dbus_message_iter_next(&iter);

  dbus_message_iter_get_basic(&iter, &u_val);
  e_notification_replaces_id_set(n, u_val); 
  dbus_message_iter_next(&iter);

  dbus_message_iter_get_basic(&iter, &s_val);
  e_notification_app_icon_set(n, s_val); 
  dbus_message_iter_next(&iter);

  dbus_message_iter_get_basic(&iter, &s_val);
  e_notification_summary_set(n, s_val); 
  dbus_message_iter_next(&iter);

  dbus_message_iter_get_basic(&iter, &s_val);
  e_notification_body_set(n, s_val); 
  dbus_message_iter_next(&iter);

  unmarshal_notify_actions(n, &iter);
  dbus_message_iter_next(&iter);

  unmarshal_notify_hints(n, &iter);
  dbus_message_iter_next(&iter);

  dbus_message_iter_get_basic(&iter, &i_val);
  e_notification_expire_timeout_set(n, i_val);
  
  return n;
}

DBusMessage *
marshal_notify_return(DBusMessage *method_call, dbus_uint32_t notification_id)
{
  DBusMessage *msg;
  msg = dbus_message_new_method_return(method_call);
  dbus_message_get_append_args(msg, DBUS_TYPE_UINT32, &notification_id, DBUS_TYPE_INVALID);
  return msg;
}

E_Notification_Return_Notify *
unmarshal_notify_return(DBusMessage *msg)
{
  E_Notification_Return_Notify *ret;
  DBusError err;
  dbus_error_init(&err);
  ret = calloc(1, sizeof(E_Notification_Return_Notify));
  dbus_message_get_args(msg, &err, DBUS_TYPE_UINT32, &(ret->notification_id), DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    dbus_error_free(&err);
    return NULL;
  }
  return ret;
}

Ecore_List *
unmarshal_notify_actions(E_Notification *n, DBusMessageIter *iter)
{
  DBusMessageIter arr;
  const char *id, *name;
  dbus_message_recurse(iter, &arr);
  while (dbus_message_iter_has_next(&arr))
  {
    dbus_message_iter_get_basic(&arr, &id);
    dbus_message_iter_next(&arr);
    dbus_message_iter_get_basic(&arr, &name);
    dbus_message_iter_next(&arr);
    e_notification_action_add(n, id, name);
  }
}

Ecore_List *
unmarshal_notify_hints(E_Notification *n, DBusMessageIter *iter)
{
  DBusMessageIter arr;
  const char *key;
  int x_set = 0, y_set = 0;
  int x, y;
  dbus_message_recurse(iter, &arr);
  while (dbus_message_iter_has_next(&arr))
  {
    DBusMessageIter dict;
    dbus_message_iter_recurse(&arr, &dict);
    while (dbus_message_iter_has_next(&dict))
    {
      DBusMessageIter variant;
      const char *s_val;
      char y_val;
      dbus_int32_t i_val;
      dbus_bool_t b_val;

      dbus_message_iter_get_basic(&dict, &key);
      dbus_message_iter_next(&dict);
      dbus_message_recurse(&dict, &variant);
      switch(dbus_message_iter_get_element_type(&variant))
      {
        case DBUS_TYPE_STRING:
          dbus_message_iter_get_basic(&variant, &s_val);
          break;
        case DBUS_TYPE_BOOLEAN:
          dbus_message_iter_get_basic(&variant, &b_val);
          break;
        case DBUS_TYPE_BYTE:
          dbus_message_iter_get_basic(&variant, &y_val);
          break;
        case DBUS_TYPE_INT32:
          dbus_message_iter_get_basic(&variant, &i_val);
          break;
        default:
          break;
      }
      if (!strcmp(key, "urgency"))
        e_notification_hint_urgency_set(n, y_val);
      else if (!strcmp(key, "category"))
        e_notification_hint_category_set(n, s_val);
      else if (!strcmp(key, "desktop-entry"))
        e_notification_hint_desktop_entry_set(n, s_val);
      else if (!strcmp(key, "sound-file"))
        e_notification_hint_sound_file_set(n, s_val);
      else if (!strcmp(key, "suppress-sound"))
        e_notification_hint_suppress_sound_set(n, b_val);
      else if (!strcmp(key, "x"))
      {
        x = i_val;
        x_set = 1;
      }
      else if (!strcmp(key, "y"))
      {
        y = i_val;
        y_set = 1;
      }
      else if (!strcmp(key, "image_data"))
        n->hints.image_data = unmarshal_hint_image(&variant);
    }
    dbus_message_iter_next(&arr);
  }

  if (x_set && y_set)
    e_notification_hint_xy_set(n, x, y);
}

void
marshal_hint_image(DBusMessageIter *iter, E_Notification_Image *img)
{
  DBusMessageIter sub, arr;
  int data_len = 0; 

  data_len = ((img->height - 1) * img->rowstride) + (img->width * ((img->channels * (img->bits_per_sample + 7)) / 8));
  dbus_message_iter_open_container(iter, DBUS_TYPE_STRUCT, NULL, &sub);
  dbus_message_iter_append_basic(&sub, DBUS_TYPE_INT32, &(img->width));
  dbus_message_iter_append_basic(&sub, DBUS_TYPE_INT32, &(img->height));
  dbus_message_iter_append_basic(&sub, DBUS_TYPE_INT32, &(img->rowstride));
  dbus_message_iter_append_basic(&sub, DBUS_TYPE_BOOLEAN, &(img->has_alpha));
  dbus_message_iter_append_basic(&sub, DBUS_TYPE_INT32, &(img->bits_per_sample));
  dbus_message_iter_append_basic(&sub, DBUS_TYPE_INT32, &(img->channels));
  dbus_message_iter_open_container(&sub, DBUS_TYPE_ARRAY, NULL, &arr);
  dbus_message_iter_append_fixed_array(&arr, DBUS_TYPE_BYTE, &(img->data), data_len);
  dbus_message_iter_close_container(&sub, &arr);
}

E_Notification_Image *
unmarshal_hint_image(DBusMessageIter *iter)
{
  DBusMessageIter sub, arr;
  dbus_int32_t i_val;
  char b_val;
  char *byte_array;
  int array_len;
  E_Notification_Image *img;
  char *sig;

  if (!dbus_message_iter_has_signature(iter, "(iiibiiay)")) return NULL;

  img = e_notification_image_new();
  if (!img) return NULL;

  dbus_message_iter_recurse(iter, &sub);
  dbus_message_iter_get_basic(&sub, &(img->width));
  dbus_message_iter_next(&sub);
  dbus_message_iter_get_basic(&sub, &(img->height));
  dbus_message_iter_next(&sub);
  dbus_message_iter_get_basic(&sub, &(img->rowstride));
  dbus_message_iter_next(&sub);
  dbus_message_iter_get_basic(&sub, &(img->has_alpha));
  dbus_message_iter_next(&sub);
  dbus_message_iter_get_basic(&sub, &(img->bits_per_sample));
  dbus_message_iter_next(&sub);
  dbus_message_iter_get_basic(&sub, &(img->channels));
  dbus_message_iter_next(&sub);

  dbus_message_iter_recurse(&sub, &arr);
  dbus_message_iter_get_fixed_array(&arr, &(byte_array), &array_len);
  img->data = malloc(array_len);
  memcpy(img->data, byte_array, array_len);

  return img;
}

/**** client api ****/

static void
cb_notify(void *data, DBusMessage *msg, DBusError *err)
{
  E_DBus_Callback *cb;
  E_Notification_Return_Notify *ret;
  cb = data;
  ret = unmarshal_notify_return(msg);
  e_dbus_callback_call(cb, ret);
}

void
e_notification_send(E_Notification_Context *ctx, E_Notification *n, E_DBus_Callback_Func func, void *data)
{
  DBusMessage *msg;
  E_DBus_Callback *cb;

  cb = e_dbus_callback_new(func, data);
  msg = marshal_notify(n);
  e_dbus_message_send(ctx->conn, msg, cb_notify, -1, cb);
}

static void
cb_get_capabilities(void *data, DBusMessage *msg, DBusError *err)
{
  E_DBus_Callback *cb;
  E_Notification_Return_Get_Capabilities *ret;
  cb = data;
  ret = unmarshal_get_capabilities_return(msg);
  e_dbus_callback_call(cb, ret);
}

void
e_notification_get_capabilities(E_Notification_Context *ctx, E_DBus_Callback_Func func, void *data)
{
  DBusMessage *msg;
  E_DBus_Callback *cb;

  cb = e_dbus_callback_new(func, data);
  msg = marshal_get_capabilities();
  e_dbus_message_send(ctx->conn, msg, cb_notify, -1, cb);
}

static void
cb_get_server_information(void *data, DBusMessage *msg, DBusError *err)
{
  E_DBus_Callback *cb;
  E_Notification_Return_Get_Server_Information *ret;
  cb = data;
  ret = unmarshal_get_server_information_return(msg);
  e_dbus_callback_call(cb, ret);
}

void
e_notification_get_server_information(E_Notification_Context *ctx, E_DBus_Callback_Func func, void *data)
{
  DBusMessage *msg;
  E_DBus_Callback *cb;

  cb = e_dbus_callback_new(func, data);
  msg = marshal_get_server_information();
  e_dbus_message_send(ctx->conn, msg, cb_notify, -1, cb);
}

/**** daemon api ****/
void
e_notification_signal_notification_closed(E_Notification_Daemon *daemon, unsigned int id, E_Notification_Closed_Reason reason)
{
  e_dbus_message_send(daemon->conn, 
                      marshal_notification_closed_signal(id, reason),
                      NULL, -1, NULL);
}

void
e_notification_signal_action_invoked(E_Notification_Daemon *daemon, unsigned int notification_id, const char *action_id)
{
  e_dbus_message_send(daemon->conn, 
                      marshal_notification_action_invoked_signal(notification_id, action_id),
                      NULL, -1, NULL);
}

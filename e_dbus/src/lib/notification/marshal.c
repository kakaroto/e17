#include "E_Notify.h"
#include <string.h>
#include "e_notify_private.h"

void
e_notify_marshal_dict_variant(DBusMessageIter *iter, const char *key, char *type_str, E_DBus_Variant_Marshaller func, void *data)
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

void
e_notify_marshal_dict_string(DBusMessageIter *iter, const char *key, const char *value)
{
  DBusMessageIter entry, variant;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, "sv", &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "s", &variant);
  dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &value);
  dbus_message_iter_close_container(&entry, &variant);
  dbus_message_iter_close_container(iter, &entry);
}

void
e_notify_marshal_dict_byte(DBusMessageIter *iter, const char *key, char value)
{
  DBusMessageIter entry, variant;

  if (!key || !value) return;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "y", &variant);
  dbus_message_iter_append_basic(&variant, DBUS_TYPE_BYTE, &value);
  dbus_message_iter_close_container(&entry, &variant);
  dbus_message_iter_close_container(iter, &entry);
}

void
e_notify_marshal_dict_int(DBusMessageIter *iter, const char *key, int value)
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

void
e_notify_marshal_string_array(DBusMessageIter *iter, const char **strings)
{
  const char **str;
  DBusMessageIter arr;

  dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY, "s", &arr);

  for (str = strings; *str; str++)
    dbus_message_iter_append_basic(&arr, DBUS_TYPE_STRING, str);

  dbus_message_iter_close_container(iter, &arr);
}

void
e_notify_marshal_string_list_as_array(DBusMessageIter *iter, Eina_List *strings)
{
  const char *str;
  DBusMessageIter arr;
  Eina_List *l;

  dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY, "s", &arr);

  EINA_LIST_FOREACH(strings, l, str)
    dbus_message_iter_append_basic(&arr, DBUS_TYPE_STRING, &str);

  dbus_message_iter_close_container(iter, &arr);
}

Eina_List *
e_notify_unmarshal_string_array_as_list(DBusMessageIter *iter, DBusError *err)
{
  Eina_List *strings;
  char *sig;
  int ret;
  DBusMessageIter arr;

  sig = dbus_message_iter_get_signature(iter);
  ret = !strcmp(sig, "as");
  dbus_free(sig);
  if (!ret) return NULL;

  strings = NULL;

  dbus_message_iter_recurse(iter, &arr);
  while(dbus_message_iter_has_next(&arr))
  {
    const char *str;
    dbus_message_iter_get_basic(&arr, &str);
    strings = eina_list_append(strings, strdup(str)); //XXX use eina_stringshare_instance?
    dbus_message_iter_next(&arr);
  }
  return strings;
}



DBusMessage *
e_notify_marshal_get_capabilities()
{
  DBusMessage *msg;

  msg = e_notification_call_new("GetCapabilities");
  return msg;
}

DBusMessage *
e_notify_marshal_get_capabilities_return(DBusMessage *method_call, const char **capabilities)
{
  DBusMessage *msg;
  DBusMessageIter iter;

  msg = dbus_message_new_method_return(method_call);
  dbus_message_iter_init_append(msg, &iter);
  e_notify_marshal_string_array(&iter, capabilities);

  return msg;
}

void *
e_notify_unmarshal_get_capabilities_return(DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter;
  E_Notification_Return_Get_Capabilities *ret;

  if (!dbus_message_has_signature(msg, "as")) return NULL;

  ret = calloc(1, sizeof(E_Notification_Return_Get_Capabilities));
  dbus_message_iter_init(msg, &iter);
  ret->capabilities = e_notify_unmarshal_string_array_as_list(&iter, err);

  return ret;
}

void
e_notify_free_get_capabilities_return(void *data)
{
  E_Notification_Return_Get_Capabilities *ret = data;

  if (!ret) return;
  eina_list_free(ret->capabilities);
  free(ret);
}

DBusMessage *
e_notify_marshal_get_server_information()
{
  DBusMessage *msg;

  msg = e_notification_call_new("GetServerInformation");
  return msg;
}

DBusMessage *
e_notify_marshal_get_server_information_return(DBusMessage *method_call, const char *name, const char *vendor, const char *version)
{
  DBusMessage *msg;
  msg = dbus_message_new_method_return(method_call);
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_STRING, &vendor, DBUS_TYPE_STRING, &version, DBUS_TYPE_INVALID);
  return msg;
}

void *
e_notify_unmarshal_get_server_information_return(DBusMessage *msg, DBusError *err)
{
  E_Notification_Return_Get_Server_Information *info;
  if (!dbus_message_has_signature(msg, "sss")) return NULL;

  info = calloc(1, sizeof(E_Notification_Return_Get_Server_Information));
  dbus_message_get_args(msg, err,
    DBUS_TYPE_STRING, &(info->name),
    DBUS_TYPE_STRING, &(info->vendor),
    DBUS_TYPE_STRING, &(info->version),
    DBUS_TYPE_INVALID
  );
  if (dbus_error_is_set(err))
  {
    free(info);
    return NULL;
  }

  return info;
}

void
e_notify_free_get_server_information_return(void *data)
{
  E_Notification_Return_Get_Server_Information *info = data;

  if (!info) return;
  free(info);
}

DBusMessage *
e_notify_marshal_close_notification(dbus_uint32_t id)
{
  DBusMessage *msg;

  msg = e_notification_call_new("CloseNotification");
  dbus_message_append_args(msg, DBUS_TYPE_UINT32, &id, DBUS_TYPE_INVALID);
  return msg;
}

dbus_uint32_t
e_notify_unmarshal_close_notification(DBusMessage *msg, DBusError *err)
{
  dbus_uint32_t id;
  if (!dbus_message_has_signature(msg, "u")) return 0;
  dbus_message_get_args(msg, err, DBUS_TYPE_UINT32, &id, DBUS_TYPE_INVALID);
  if (err && dbus_error_is_set(err))
    return 0;

  return id;
}

DBusMessage *
e_notify_marshal_notification_closed_signal(dbus_uint32_t id, dbus_uint32_t reason)
{
  DBusMessage *msg;
  msg = dbus_message_new_signal(E_NOTIFICATION_PATH, E_NOTIFICATION_INTERFACE, "NotificationClosed");
  dbus_message_append_args(msg, DBUS_TYPE_UINT32, &id, DBUS_TYPE_UINT32, &reason, DBUS_TYPE_INVALID);
  return msg;
}

E_Notification_Event_Notification_Closed *
e_notify_unmarshal_notification_closed_signal(DBusMessage *msg, DBusError *err)
{
  E_Notification_Event_Notification_Closed *ev;

  if (!dbus_message_has_signature(msg, "uu")) 
  {
    dbus_set_error(err, DBUS_ERROR_INVALID_SIGNATURE, "");
    return NULL;
  }
  ev = calloc(1, sizeof(E_Notification_Event_Notification_Closed));
  dbus_message_get_args(msg, err, DBUS_TYPE_UINT32, &(ev->notification_id), DBUS_TYPE_UINT32, &(ev->reason), DBUS_TYPE_INVALID);
  if (dbus_error_is_set(err))
  {
    free(ev);
    return NULL;
  }
  return ev;
}

DBusMessage *
e_notify_marshal_action_invoked_signal(dbus_uint32_t id, const char *action_id)
{
  DBusMessage *msg;
  msg = dbus_message_new_signal(E_NOTIFICATION_PATH, E_NOTIFICATION_INTERFACE, "ActionInvoked");
  dbus_message_append_args(msg, DBUS_TYPE_UINT32, &id, DBUS_TYPE_STRING, &action_id, DBUS_TYPE_INVALID);
  return msg;
}

E_Notification_Event_Action_Invoked *
e_notify_unmarshal_action_invoked_signal(DBusMessage *msg, DBusError *err)
{
  E_Notification_Event_Action_Invoked *ev;

  if (!dbus_message_has_signature(msg, "us")) 
  {
    dbus_set_error(err, DBUS_ERROR_INVALID_SIGNATURE, "");
    return NULL;
  }
  ev = calloc(1, sizeof(E_Notification_Event_Action_Invoked));
  dbus_message_get_args(msg, err, DBUS_TYPE_UINT32, &(ev->notification_id), DBUS_TYPE_STRING, &(ev->action_id), DBUS_TYPE_INVALID);
  if (dbus_error_is_set(err))
  {
    free(ev);
    return NULL;
  }
  return ev;
}

DBusMessage *
e_notify_marshal_notify(E_Notification *n)
{
  DBusMessage *msg;
  DBusMessageIter iter, sub;
  Eina_List *l;

  if (!n->app_name) n->app_name = strdup("");
  if (!n->app_icon) n->app_icon = strdup("");
  if (!n->summary) n->summary = strdup("");
  if (!n->body) n->body = strdup("");

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
    EINA_LIST_FOREACH(n->actions, l, action)
    {
      dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &(action->id));
      dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &(action->name));
    }
  }
  dbus_message_iter_close_container(&iter, &sub);

  /* hints */
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &sub);
  if (n->hints.urgency) /* we only need to send this if its non-zero*/
    e_notify_marshal_dict_byte(&sub, "urgency", n->hints.urgency);
  if (n->hints.category)
    e_notify_marshal_dict_string(&sub, "category", n->hints.category);
  if (n->hints.desktop)
    e_notify_marshal_dict_string(&sub, "desktop_entry", n->hints.desktop);
  if (n->hints.image_data)
    e_notify_marshal_dict_variant(&sub, "image_data", "(iiibiiay)", E_DBUS_VARIANT_MARSHALLER(e_notify_marshal_hint_image), n->hints.image_data);
  if (n->hints.icon_data)
    e_notify_marshal_dict_variant(&sub, "icon_data", "(iiibiiay)", E_DBUS_VARIANT_MARSHALLER(e_notify_marshal_hint_image), n->hints.icon_data);
  if (n->hints.sound_file)
    e_notify_marshal_dict_string(&sub, "sound_file", n->hints.sound_file);
  if (n->hints.suppress_sound) /* we only need to send this if its true */
    e_notify_marshal_dict_byte(&sub, "suppress_sound", n->hints.suppress_sound);
  if (n->hints.x > -1 && n->hints.y > -1)
  {
    e_notify_marshal_dict_int(&sub, "x", n->hints.x);
    e_notify_marshal_dict_int(&sub, "y", n->hints.y);
  }

  dbus_message_iter_close_container(&iter, &sub);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &(n->expire_timeout));
  return msg;
}

E_Notification *
e_notify_unmarshal_notify(DBusMessage *msg, DBusError *err)
{
  E_Notification *n;
  const char *s_val;
  dbus_uint32_t u_val;
  dbus_int32_t i_val;
  DBusMessageIter iter;
  if (!dbus_message_has_signature(msg, "susssasa{sv}i")) return NULL;

  n = e_notification_new();
  if (!n) return NULL;
  dbus_message_iter_init(msg, &iter);

  dbus_message_iter_get_basic(&iter, &s_val);
  e_notification_app_name_set(n, s_val); 
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

  e_notify_unmarshal_notify_actions(n, &iter);
  dbus_message_iter_next(&iter);

  e_notify_unmarshal_notify_hints(n, &iter);
  dbus_message_iter_next(&iter);

  dbus_message_iter_get_basic(&iter, &i_val);
  e_notification_timeout_set(n, i_val);
  
  return n;
}

DBusMessage *
e_notify_marshal_notify_return(DBusMessage *method_call, dbus_uint32_t notification_id)
{
  DBusMessage *msg;
  msg = dbus_message_new_method_return(method_call);
  dbus_message_append_args(msg, DBUS_TYPE_UINT32, &notification_id, DBUS_TYPE_INVALID);
  return msg;
}

void *
e_notify_unmarshal_notify_return(DBusMessage *msg, DBusError *err)
{
  E_Notification_Return_Notify *ret;
  ret = calloc(1, sizeof(E_Notification_Return_Notify));
  dbus_message_get_args(msg, err, DBUS_TYPE_UINT32, &(ret->notification_id), DBUS_TYPE_INVALID);
  if (dbus_error_is_set(err))
  {
    free(ret);
    return NULL;
  }
  return ret;
}

void
e_notify_free_notify_return(void *data)
{
  E_Notification_Return_Notify *ret = data;

  if (!ret) return;
  free(ret);
}

void
e_notify_unmarshal_notify_actions(E_Notification *n, DBusMessageIter *iter)
{
  DBusMessageIter arr;
  const char *id, *name;
  dbus_message_iter_recurse(iter, &arr);
  while (dbus_message_iter_has_next(&arr))
  {
    dbus_message_iter_get_basic(&arr, &id);
    dbus_message_iter_next(&arr);
    dbus_message_iter_get_basic(&arr, &name);
    dbus_message_iter_next(&arr);
    e_notification_action_add(n, id, name);
  }
}

void
e_notify_unmarshal_notify_hints(E_Notification *n, DBusMessageIter *iter)
{
  DBusMessageIter arr;
  const char *key;
  int x_set = 0, y_set = 0;
  int x, y;
  dbus_message_iter_recurse(iter, &arr);
  
  if (dbus_message_iter_get_arg_type(&arr) == DBUS_TYPE_INVALID)
    return;
  
  do
  {
    DBusMessageIter dict;
    dbus_message_iter_recurse(&arr, &dict);
    do
    {
      DBusMessageIter variant;
      const char *s_val;
      char y_val;
      dbus_bool_t b_val;

      dbus_message_iter_get_basic(&dict, &key);
      dbus_message_iter_next(&dict);
      dbus_message_iter_recurse(&dict, &variant);
     
      if (!strcmp(key, "urgency"))
        {
          dbus_message_iter_get_basic(&variant, &y_val);
          e_notification_hint_urgency_set(n, y_val);
        }
      else if (!strcmp(key, "category"))
        {
          dbus_message_iter_get_basic(&variant, &s_val);
          e_notification_hint_category_set(n, s_val);
        }
      else if (!strcmp(key, "desktop-entry"))
        {
          dbus_message_iter_get_basic(&variant, &s_val);
          e_notification_hint_desktop_set(n, s_val);
        }
      else if (!strcmp(key, "sound-file"))
        {
          dbus_message_iter_get_basic(&variant, &s_val);
          e_notification_hint_sound_file_set(n, s_val);
        }
      else if (!strcmp(key, "suppress-sound"))
        {
          dbus_message_iter_get_basic(&variant, &b_val);
          e_notification_hint_suppress_sound_set(n, b_val);
        }
      else if (!strcmp(key, "x"))
      {
        dbus_message_iter_get_basic(&variant, &x);
        x_set = 1;
      }
      else if (!strcmp(key, "y"))
      {
        dbus_message_iter_get_basic(&variant, &y);
        y_set = 1;
      }
      else if (!strcmp(key, "image_data"))
        {
          dbus_message_iter_recurse(&dict, &variant);
          n->hints.image_data = e_notify_unmarshal_hint_image(&variant);
        }
      else if (!strcmp(key, "icon_data"))
        {
          dbus_message_iter_recurse(&dict, &variant);
          n->hints.icon_data = e_notify_unmarshal_hint_image(&variant);
        }
    }
    while (dbus_message_iter_next(&dict));
  } 
  while (dbus_message_iter_next(&arr));

  if (x_set && y_set)
    e_notification_hint_xy_set(n, x, y);
}

void
e_notify_marshal_hint_image(DBusMessageIter *iter, E_Notification_Image *img)
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
e_notify_unmarshal_hint_image(DBusMessageIter *iter)
{
  DBusMessageIter sub, arr;
  char *byte_array;
  int array_len;
  E_Notification_Image *img;
  char *sig;
  int sig_matches;

  sig = dbus_message_iter_get_signature(iter);
  sig_matches = strcmp(sig, "(iiibiiay)");
  dbus_free(sig);
  if (sig_matches) return NULL;

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


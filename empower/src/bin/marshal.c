#include "marshal.h"

// Necessary to get username from /etc/passwd using uid
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

static void _marshal_dict_uint32(DBusMessageIter *itr, const char *key, uint32_t value);
static void _marshal_dict_string(DBusMessageIter *itr, const char *key, const char *value);
static void _marshal_dict_uint64(DBusMessageIter *itr, const char *key, uint64_t value);

static void _marshal_dict_string_string_free(void *data);

const char *_marshal_identity_kind_str[EMPOWER_IDENTITY_COUNT] = {"unix-user", "unix-group"};
const char *_marshal_subject_kind_str[EMPOWER_SUBJECT_COUNT] = {"unix-process", "unix-session", "system-bus-name"};

void marshal_subject(DBusMessageIter *itr, Empower_Subject *subject)
{
  DBusMessageIter s, a;

  if (dbus_message_iter_open_container(itr, DBUS_TYPE_STRUCT, NULL, &s))
  {
    dbus_message_iter_append_basic(&s, DBUS_TYPE_STRING, &_marshal_subject_kind_str[subject->kind]);
    if (dbus_message_iter_open_container(&s, DBUS_TYPE_ARRAY, "{sv}", &a))
    {
      switch (subject->kind)
      {
        case EMPOWER_SUBJECT_PROCESS:
          _marshal_dict_uint32(&a, "pid", subject->details.process.pid);
          _marshal_dict_uint64(&a, "start-time", subject->details.process.start_time);
          break;

        case EMPOWER_SUBJECT_SESSION:
          _marshal_dict_string(&a, "session-id", subject->details.session.id);
          break;

        case EMPOWER_SUBJECT_BUS:
          _marshal_dict_string(&a, "name", subject->details.bus.name);
          break;
      }
      dbus_message_iter_close_container(&s, &a);
    }
    dbus_message_iter_close_container(itr, &s);
  }
}

void marshal_identity(DBusMessageIter *itr, Empower_Identity *id)
{
  DBusMessageIter s, a;

  if (dbus_message_iter_open_container(itr, DBUS_TYPE_STRUCT, NULL, &s))
  {
    dbus_message_iter_append_basic(&s, DBUS_TYPE_STRING, &_marshal_identity_kind_str[id->kind]);
    if (dbus_message_iter_open_container(&s, DBUS_TYPE_ARRAY, "{sv}", &a))
    {
      switch (id->kind)
      {
        case EMPOWER_IDENTITY_USER:
          _marshal_dict_uint32(&a, "uid", id->details.user.uid);
          break;

        case EMPOWER_IDENTITY_GROUP:
          _marshal_dict_uint32(&a, "gid", id->details.group.gid);
          break;
      }
      dbus_message_iter_close_container(&s, &a);
    }
    dbus_message_iter_close_container(itr, &s);
  }
}

void unmarshal_identity(DBusMessageIter *itr, Empower_Identity *id)
{
  DBusMessageIter s, a, d, v;
  char *kind;
  char *key;
  uint32_t value;
  int type;
  struct passwd *user;
  struct group *group;

  if (!id) return;

  type = dbus_message_iter_get_arg_type(itr);
  if (type != DBUS_TYPE_STRUCT)
    return;

  dbus_message_iter_recurse(itr, &s);
  dbus_message_iter_get_basic(&s, &kind);
  dbus_message_iter_next(&s);

  if (!strcmp(kind, _marshal_identity_kind_str[EMPOWER_IDENTITY_USER]))
    id->kind = EMPOWER_IDENTITY_USER;
  else if(!strcmp(kind, _marshal_identity_kind_str[EMPOWER_IDENTITY_GROUP]))
    id->kind = EMPOWER_IDENTITY_GROUP;

  type = dbus_message_iter_get_arg_type(&s);
  if (type != DBUS_TYPE_ARRAY)
    return;

  dbus_message_iter_recurse(&s, &a);
  do
  {
    type = dbus_message_iter_get_arg_type(&a);
    if (type != DBUS_TYPE_DICT_ENTRY)
      return;

    dbus_message_iter_recurse(&a, &d);
    dbus_message_iter_get_basic(&d, &key);
    dbus_message_iter_next(&d);

    dbus_message_iter_recurse(&d, &v);
    dbus_message_iter_get_basic(&v, &value);

    switch (id->kind)
    {
      case EMPOWER_IDENTITY_USER:
        id->details.user.uid = value;
        user = getpwuid(value);
        if (user != NULL)
          id->details.user.name = eina_stringshare_add(user->pw_name);
        break;

      case EMPOWER_IDENTITY_GROUP:
        id->details.group.gid = value;
        group = getgrgid(value);
        if (group != NULL)
          id->details.group.name = eina_stringshare_add(group->gr_name);
        break;
    }
  } while (dbus_message_iter_next(&a));
}

Eina_Hash *
unmarshal_dict_string_string(DBusMessageIter *itr)
{
  DBusMessageIter a, d;
  Eina_Hash *h;
  char *key, *value;
  int type;

  // Confirm we are on a dict
  type = dbus_message_iter_get_arg_type(itr);
  if (type != DBUS_TYPE_ARRAY)
    return NULL;

  h = eina_hash_string_superfast_new(_marshal_dict_string_string_free);

  dbus_message_iter_recurse(itr, &a);
  do
  {
    dbus_message_iter_recurse(&a, &d);
    dbus_message_iter_get_basic(&d, &key);
    dbus_message_iter_get_basic(&d, &value);

    eina_hash_add(h, key, strdup(value));
  } while (dbus_message_iter_next(&a));

  return h;
}

static void
_marshal_dict_string_string_free(void *data)
{
  free(data);
}

static void
_marshal_dict_string(DBusMessageIter *itr, const char *key, const char *value)
{
   DBusMessageIter entry, variant;

   if (dbus_message_iter_open_container(itr, DBUS_TYPE_DICT_ENTRY, NULL, &entry))
     {
        dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
        if (dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "s", &variant))
          {
             dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &value);
             dbus_message_iter_close_container(&entry, &variant);
          }
        else
          {
             ERR("dbus_message_iter_open_container() failed");
          }
        dbus_message_iter_close_container(itr, &entry);
     }
   else
     {
        ERR("dbus_message_iter_open_container() failed");
     }
}

static void
_marshal_dict_uint64(DBusMessageIter *itr, const char *key, uint64_t value)
{
   DBusMessageIter entry, variant;

   if (!key || !value) return;

   if (dbus_message_iter_open_container(itr, DBUS_TYPE_DICT_ENTRY, NULL, &entry))
     {
        dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
        if (dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "t", &variant))
          {
             dbus_message_iter_append_basic(&variant, DBUS_TYPE_UINT64, &value);
             dbus_message_iter_close_container(&entry, &variant);
          }
        else
          {
             ERR("dbus_message_iter_open_container() failed");
          }
        dbus_message_iter_close_container(itr, &entry);
     }
   else
     {
        ERR("dbus_message_iter_open_container() failed");
     }
}

static void
_marshal_dict_uint32(DBusMessageIter *itr, const char *key, uint32_t value)
{
   DBusMessageIter entry, variant;

   if (!key || !value) return;

   if (dbus_message_iter_open_container(itr, DBUS_TYPE_DICT_ENTRY, NULL, &entry))
     {
        dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
        if (dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "u", &variant))
          {
             dbus_message_iter_append_basic(&variant, DBUS_TYPE_UINT32, &value);
             dbus_message_iter_close_container(&entry, &variant);
          }
        else
          {
             ERR("dbus_message_iter_open_container() failed");
          }
        dbus_message_iter_close_container(itr, &entry);
     }
   else
     {
        ERR("dbus_message_iter_open_container() failed");
     }
}

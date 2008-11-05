#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

static Property access_point_properties[] = {
  { .name = "HwAddress", .func = property_string, .offset = offsetof(E_NM_Device, wired.hw_address) },
  { .name = "Speed", .func = property_uint32, .offset = offsetof(E_NM_Device, wired.speed) },
  { .name = "Carrier", .func = property_bool, .offset = offsetof(E_NM_Device, wired.carrier) },
  { .name = NULL, .func = NULL, .offset = 0 }
};

static int
parse_properties(E_NM_Device_Internal *dev, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, a_iter;

  if (dbus_error_is_set(err))
    return 0;
  if (!dbus_message_has_signature(msg, "a{sv}"))
    return 0;

  dbus_message_iter_init(msg, &iter);

  dbus_message_iter_recurse(&iter, &a_iter);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    DBusMessageIter d_iter, v_iter;
    const char *name, *value;

    dbus_message_iter_recurse(&a_iter, &d_iter);
    dbus_message_iter_get_basic(&d_iter, &name);

    dbus_message_iter_next(&d_iter);
    dbus_message_iter_recurse(&d_iter, &v_iter);
    /* TODO */
    dbus_message_iter_next(&a_iter);
  }
  return 1;
}

static void
cb_properties_changed(void *data, DBusMessage *msg)
{
  E_NM_Device_Internal *dev;
  if (!msg || !data) return;

  dev = data;
  if (!parse_properties(dev, msg, NULL)) return;

  if (dev->properties_changed)
    dev->properties_changed(&(dev->dev));
}

EAPI int
e_nm_access_point_get(E_NM *nm, const char *access_point,
                      int (*cb_func)(void *data, void *reply),
                      void *data)
{
  E_NM_Internal *nmi;
  E_NM_Access_Point_Internal *ap;
  E_NM_Data     *d;

  nmi = (E_NM_Internal *)nm;
  ap = calloc(1, sizeof(E_NM_Access_Point_Internal));
  ap->nmi = nmi;
  d = calloc(1, sizeof(E_NM_Data));
  d->nmi = nmi;
  d->cb_func = cb_func;
  d->data = data;
  d->reply = ap;
  d->property = access_point_properties;
  d->object = strdup(access_point);

  ap->handlers = ecore_list_new();
  ecore_list_append(ap->handlers, e_nm_device_signal_handler_add(nmi->conn, access_point, "PropertiesChanged", cb_properties_changed, nmi));
 
  return e_nm_access_point_properties_get(nmi->conn, d->object, d->property->name, d->property->func, d) ? 1 : 0;
}

EAPI void
e_nm_access_point_free(E_NM_Access_Point *access_point)
{
  E_NM_Access_Point_Internal *ap;

  if (!access_point) return;
  ap = (E_NM_Access_Point_Internal *)access_point;
  /* TODO */
  free(ap);
}

EAPI void
e_nm_access_point_dump(E_NM_Access_Point *ap)
{
  if (!ap) return;
  printf("E_NM_Access_Point:\n");
  /* TODO */
  printf("\n");
}

EAPI void
e_nm_access_point_data_set(E_NM_Access_Point *access_point, void *data)
{
  E_NM_Access_Point_Internal *ap;

  ap = (E_NM_Access_Point_Internal *)access_point;
  ap->data = data;
}

EAPI void *
e_nm_access_point_data_get(E_NM_Access_Point *access_point)
{
  E_NM_Access_Point_Internal *ap;

  ap = (E_NM_Access_Point_Internal *)access_point;
  return ap->data;
}

EAPI void
e_nm_access_point_callback_properties_changed_set(E_NM_Access_Point *access_point, int (*cb_func)(E_NM_Access_Point *ap))
{
  E_NM_Access_Point_Internal *ap;

  ap = (E_NM_Access_Point_Internal *)access_point;
  ap->properties_changed = cb_func;
}

#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

static Property access_point_properties[] = {
  { .name = "Flags", .sig = "u", .offset = offsetof(E_NM_Access_Point, flags) },
  { .name = "WpaFlags", .sig = "u", .offset = offsetof(E_NM_Access_Point, wpa_flags) },
  { .name = "RsnFlags", .sig = "u", .offset = offsetof(E_NM_Access_Point, rsn_flags) },
  { .name = "Ssid", .sig = "ay", .offset = offsetof(E_NM_Access_Point, ssid) },
  { .name = "Frequency", .sig = "u", .offset = offsetof(E_NM_Access_Point, frequency) },
  { .name = "HwAddress", .sig = "s", .offset = offsetof(E_NM_Access_Point, hw_address) },
  { .name = "Mode", .sig = "u", .offset = offsetof(E_NM_Access_Point, mode) },
  { .name = "MaxBitrate", .sig = "u", .offset = offsetof(E_NM_Access_Point, max_bitrate) },
  { .name = "Strength", .sig = "y", .offset = offsetof(E_NM_Access_Point, strength) },
  { .name = NULL }
};

static void
cb_properties_changed(void *data, DBusMessage *msg)
{
  E_NM_Access_Point_Internal *ap;
  if (!msg || !data) return;

  ap = data;
  parse_properties(ap, access_point_properties, msg);

  if (ap->properties_changed)
    ap->properties_changed(&(ap->ap));
}

EAPI int
e_nm_access_point_get(E_NM *nm, const char *access_point,
                      int (*cb_func)(void *data, E_NM_Access_Point *ap),
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
  ecore_list_append(ap->handlers, e_nm_access_point_signal_handler_add(nmi->conn, access_point, "PropertiesChanged", cb_properties_changed, nmi));
 
  return e_nm_access_point_properties_get(nmi->conn, d->object, d->property->name, property, d) ? 1 : 0;
}

EAPI void
e_nm_access_point_free(E_NM_Access_Point *access_point)
{
  E_NM_Access_Point_Internal *ap;

  if (!access_point) return;
  ap = (E_NM_Access_Point_Internal *)access_point;
  /* TODO */
  if (ap->handlers)
  {
    E_DBus_Signal_Handler *sh;

    while ((sh = ecore_list_first_remove(ap->handlers)))
      e_dbus_signal_handler_del(ap->nmi->conn, sh);
    ecore_list_destroy(ap->handlers);
  }
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

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
  Property_Data     *d;

  if (!access_point) return 0;

  nmi = (E_NM_Internal *)nm;
  ap = calloc(1, sizeof(E_NM_Access_Point_Internal));
  ap->nmi = nmi;
  ap->ap.path = strdup(access_point);
  d = calloc(1, sizeof(Property_Data));
  d->nmi = nmi;
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;
  d->reply = ap;
  d->property = access_point_properties;
  d->object = strdup(access_point);

  ap->handlers = ecore_list_new();
  ecore_list_append(ap->handlers, e_nm_access_point_signal_handler_add(nmi->conn, access_point, "PropertiesChanged", cb_properties_changed, ap));
 
  return e_nm_access_point_properties_get(nmi->conn, d->object, d->property->name, property, d) ? 1 : 0;
}

EAPI void
e_nm_access_point_free(E_NM_Access_Point *access_point)
{
  E_NM_Access_Point_Internal *ap;

  if (!access_point) return;
  ap = (E_NM_Access_Point_Internal *)access_point;
  if (ap->ap.path) free(ap->ap.path);
  if (ap->ap.ssid) ecore_list_destroy(ap->ap.ssid);
  if (ap->ap.hw_address) free(ap->ap.hw_address);
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
  unsigned char *c;

  if (!ap) return;
  printf("E_NM_Access_Point:\n");
  printf("flags      :");
  if (ap->flags & E_NM_802_11_AP_FLAGS_PRIVACY)
    printf(" E_NM_802_11_AP_FLAGS_PRIVACY");
  if (ap->flags == E_NM_802_11_AP_FLAGS_NONE)
    printf(" E_NM_802_11_AP_FLAGS_NONE");
  printf("\n");
  printf("wpa_flags  :");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_PAIR_WEP40)
    printf(" E_NM_802_11_AP_SEC_PAIR_WEP40");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_PAIR_WEP104)
    printf(" E_NM_802_11_AP_SEC_PAIR_WEP104");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_PAIR_TKIP)
    printf(" E_NM_802_11_AP_SEC_PAIR_TKIP");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_PAIR_CCMP)
    printf(" E_NM_802_11_AP_SEC_PAIR_CCMP");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_GROUP_WEP40)
    printf(" E_NM_802_11_AP_SEC_GROUP_WEP40");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_GROUP_WEP104)
    printf(" E_NM_802_11_AP_SEC_GROUP_WEP104");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_GROUP_TKIP)
    printf(" E_NM_802_11_AP_SEC_GROUP_TKIP");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_GROUP_CCMP)
    printf(" E_NM_802_11_AP_SEC_GROUP_CCMP");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_KEY_MGMT_PSK)
    printf(" E_NM_802_11_AP_SEC_KEY_MGMT_PSK");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_KEY_MGMT_802_1X)
    printf(" E_NM_802_11_AP_SEC_KEY_MGMT_802_1X");
  if (ap->wpa_flags == E_NM_802_11_AP_SEC_NONE)
    printf(" E_NM_802_11_AP_SEC_NONE");
  printf("\n");
  printf("rsn_flags  :");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_PAIR_WEP40)
    printf(" E_NM_802_11_AP_SEC_PAIR_WEP40");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_PAIR_WEP104)
    printf(" E_NM_802_11_AP_SEC_PAIR_WEP104");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_PAIR_TKIP)
    printf(" E_NM_802_11_AP_SEC_PAIR_TKIP");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_PAIR_CCMP)
    printf(" E_NM_802_11_AP_SEC_PAIR_CCMP");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_GROUP_WEP40)
    printf(" E_NM_802_11_AP_SEC_GROUP_WEP40");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_GROUP_WEP104)
    printf(" E_NM_802_11_AP_SEC_GROUP_WEP104");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_GROUP_TKIP)
    printf(" E_NM_802_11_AP_SEC_GROUP_TKIP");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_GROUP_CCMP)
    printf(" E_NM_802_11_AP_SEC_GROUP_CCMP");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_KEY_MGMT_PSK)
    printf(" E_NM_802_11_AP_SEC_KEY_MGMT_PSK");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_KEY_MGMT_802_1X)
    printf(" E_NM_802_11_AP_SEC_KEY_MGMT_802_1X");
  if (ap->rsn_flags == E_NM_802_11_AP_SEC_NONE)
    printf(" E_NM_802_11_AP_SEC_NONE");
  printf("\n");
  printf("ssid       : ");
  if (ap->ssid)
  {
    ecore_list_first_goto(ap->ssid);
    while ((c = ecore_list_next(ap->ssid)))
      printf("%u", *c);
    printf("\n");
  }
  printf("frequency  : %u\n", ap->frequency);
  printf("hw_address : %s\n", ap->hw_address);
  printf("mode       : ");
  switch (ap->mode)
  {
    case E_NM_802_11_MODE_UNKNOWN:
      printf("E_NM_802_11_MODE_UNKNOWN\n");
      break;
    case E_NM_802_11_MODE_ADHOC:
      printf("E_NM_802_11_MODE_ADHOC\n");
      break;
    case E_NM_802_11_MODE_INFRA:
      printf("E_NM_802_11_MODE_INFRA\n");
      break;
  }
  printf("max_bitrate: %u\n", ap->max_bitrate);
  printf("strength   : %u\n", ap->strength);
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

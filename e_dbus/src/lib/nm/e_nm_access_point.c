#include "E_Nm.h"
#include "e_nm_private.h"
#include "e_dbus_private.h"
#include <string.h>

static const Property access_point_properties[] = {
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
  d->service = E_NM_SERVICE;
  d->object = strdup(access_point);
  d->interface = E_NM_INTERFACE_ACCESSPOINT;

  ap->handlers = eina_list_append(ap->handlers, e_nm_access_point_signal_handler_add(nmi->conn, access_point, "PropertiesChanged", cb_properties_changed, ap));
 
  return property_get(nmi->conn, d);
}

EAPI void
e_nm_access_point_free(E_NM_Access_Point *access_point)
{
  E_NM_Access_Point_Internal *ap;
  void *data;

  if (!access_point) return;
  ap = (E_NM_Access_Point_Internal *)access_point;
  if (ap->ap.path) free(ap->ap.path);
  EINA_LIST_FREE(ap->ap.ssid, data)
    free(data);
  if (ap->ap.hw_address) free(ap->ap.hw_address);
  EINA_LIST_FREE(ap->handlers, data)
    e_dbus_signal_handler_del(ap->nmi->conn, data);
  free(ap);
}

EAPI void
e_nm_access_point_dump(E_NM_Access_Point *ap)
{
  Eina_List *l;
  char buffer[1024];
  char *c;

  if (!ap) return;
  INFO("E_NM_Access_Point:");
  INFO("flags      :");
  if (ap->flags & E_NM_802_11_AP_FLAGS_PRIVACY)
    INFO(" E_NM_802_11_AP_FLAGS_PRIVACY");
  if (ap->flags == E_NM_802_11_AP_FLAGS_NONE)
    INFO(" E_NM_802_11_AP_FLAGS_NONE");
  INFO("wpa_flags  :");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_PAIR_WEP40)
    INFO(" E_NM_802_11_AP_SEC_PAIR_WEP40");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_PAIR_WEP104)
    INFO(" E_NM_802_11_AP_SEC_PAIR_WEP104");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_PAIR_TKIP)
    INFO(" E_NM_802_11_AP_SEC_PAIR_TKIP");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_PAIR_CCMP)
    INFO(" E_NM_802_11_AP_SEC_PAIR_CCMP");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_GROUP_WEP40)
    INFO(" E_NM_802_11_AP_SEC_GROUP_WEP40");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_GROUP_WEP104)
    INFO(" E_NM_802_11_AP_SEC_GROUP_WEP104");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_GROUP_TKIP)
    INFO(" E_NM_802_11_AP_SEC_GROUP_TKIP");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_GROUP_CCMP)
    INFO(" E_NM_802_11_AP_SEC_GROUP_CCMP");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_KEY_MGMT_PSK)
    INFO(" E_NM_802_11_AP_SEC_KEY_MGMT_PSK");
  if (ap->wpa_flags & E_NM_802_11_AP_SEC_KEY_MGMT_802_1X)
    INFO(" E_NM_802_11_AP_SEC_KEY_MGMT_802_1X");
  if (ap->wpa_flags == E_NM_802_11_AP_SEC_NONE)
    INFO(" E_NM_802_11_AP_SEC_NONE");
  INFO("rsn_flags  :");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_PAIR_WEP40)
    INFO(" E_NM_802_11_AP_SEC_PAIR_WEP40");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_PAIR_WEP104)
    INFO(" E_NM_802_11_AP_SEC_PAIR_WEP104");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_PAIR_TKIP)
    INFO(" E_NM_802_11_AP_SEC_PAIR_TKIP");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_PAIR_CCMP)
    INFO(" E_NM_802_11_AP_SEC_PAIR_CCMP");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_GROUP_WEP40)
    INFO(" E_NM_802_11_AP_SEC_GROUP_WEP40");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_GROUP_WEP104)
    INFO(" E_NM_802_11_AP_SEC_GROUP_WEP104");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_GROUP_TKIP)
    INFO(" E_NM_802_11_AP_SEC_GROUP_TKIP");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_GROUP_CCMP)
    INFO(" E_NM_802_11_AP_SEC_GROUP_CCMP");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_KEY_MGMT_PSK)
    INFO(" E_NM_802_11_AP_SEC_KEY_MGMT_PSK");
  if (ap->rsn_flags & E_NM_802_11_AP_SEC_KEY_MGMT_802_1X)
    INFO(" E_NM_802_11_AP_SEC_KEY_MGMT_802_1X");
  if (ap->rsn_flags == E_NM_802_11_AP_SEC_NONE)
    INFO(" E_NM_802_11_AP_SEC_NONE");
  strcpy(buffer, "ssid       : ");
  EINA_LIST_FOREACH(ap->ssid, l, c)
    strcat(buffer, c);
  INFO("%s", buffer);
  INFO("frequency  : %u", ap->frequency);
  INFO("hw_address : %s", ap->hw_address);
  INFO("mode       : ");
  switch (ap->mode)
  {
    case E_NM_802_11_MODE_UNKNOWN:
      INFO("E_NM_802_11_MODE_UNKNOWN");
      break;
    case E_NM_802_11_MODE_ADHOC:
      INFO("E_NM_802_11_MODE_ADHOC");
      break;
    case E_NM_802_11_MODE_INFRA:
      INFO("E_NM_802_11_MODE_INFRA");
      break;
  }
  INFO("max_bitrate: %u", ap->max_bitrate);
  INFO("strength   : %u", ap->strength);
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

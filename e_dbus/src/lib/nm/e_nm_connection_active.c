#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

static Property connection_active_properties[] = {
  { .name = "ServiceName", .sig = "s", .offset = offsetof(E_NM_Connection_Active, service_name) },
  { .name = "Connection", .sig = "o", .offset = offsetof(E_NM_Connection_Active, connection) },
  { .name = "SpecificObject", .sig = "o", .offset = offsetof(E_NM_Connection_Active, specific_object) },
  { .name = "Devices", .sig = "ao", .offset = offsetof(E_NM_Connection_Active, devices) },
  { .name = "State", .sig = "u", .offset = offsetof(E_NM_Connection_Active, state) },
  { .name = "Default", .sig = "b", .offset = offsetof(E_NM_Connection_Active, def) },
  { .name = NULL }
};


EAPI int
e_nm_connection_active_get(E_NM *nm, const char *connection,
                           int (*cb_func)(void *data, E_NM_Connection_Active *conn),
                           void *data)
{
  E_NM_Internal                   *nmi;
  E_NM_Connection_Active_Internal *conn;
  Property_Data                   *d;

  nmi = (E_NM_Internal *)nm;
  conn = calloc(1, sizeof(E_NM_Connection_Active_Internal));
  conn->nmi = nmi;
  d = calloc(1, sizeof(Property_Data));
  d->nmi = nmi;
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;
  d->reply = conn;
  d->property = connection_active_properties;
  d->object = strdup(connection);

  return e_nm_connection_active_properties_get(nmi->conn, d->object, d->property->name, property, d) ? 1 : 0;
}

EAPI void
e_nm_connection_active_free(E_NM_Connection_Active *conn)
{
  if (!conn) return;
#if 0
  if (dev->dev.udi) free(dev->dev.udi);
  if (dev->dev.interface) free(dev->dev.interface);
  if (dev->dev.driver) free(dev->dev.driver);
  if (dev->dev.ip4_config) free(dev->dev.ip4_config);
  if (dev->dev.dhcp4_config) free(dev->dev.dhcp4_config);
  switch (dev->dev.device_type)
  {
    case E_NM_DEVICE_TYPE_WIRED:
      if (dev->dev.wired.hw_address) free(dev->dev.wired.hw_address);
      break;
    case E_NM_DEVICE_TYPE_WIRELESS:
      if (dev->dev.wireless.hw_address) free(dev->dev.wireless.hw_address);
      if (dev->dev.wireless.active_access_point) free(dev->dev.wireless.active_access_point);
      break;
  }
  if (dev->handlers)
  {
    E_DBus_Signal_Handler *sh;

    while ((sh = ecore_list_first_remove(dev->handlers)))
      e_dbus_signal_handler_del(dev->nmi->conn, sh);
    ecore_list_destroy(dev->handlers);
  }
#endif
  free(conn);
}

EAPI void
e_nm_connection_active_dump(E_NM_Connection_Active *conn)
{
  if (!conn) return;
  printf("E_NM_Connection_Active:\n");
#if 0
  printf("udi                  : %s\n", dev->udi);
  printf("interface            : %s\n", dev->interface);
  printf("driver               : %s\n", dev->driver);
  printf("capabilities         :");
  if (dev->capabilities & E_NM_DEVICE_CAP_NM_SUPPORTED)
    printf(" E_NM_DEVICE_CAP_NM_SUPPORTED");
  if (dev->capabilities & E_NM_DEVICE_CAP_CARRIER_DETECT)
    printf(" E_NM_DEVICE_CAP_CARRIER_DETECT");
  if (dev->capabilities == E_NM_DEVICE_CAP_NONE)
    printf(" E_NM_DEVICE_CAP_NONE");
  printf("\n");
  printf("ip4_address          : %i.%i.%i.%i\n",
         ((dev->ip4_address      ) & 0xff),
         ((dev->ip4_address >> 8 ) & 0xff),
         ((dev->ip4_address >> 16) & 0xff),
         ((dev->ip4_address >> 24) & 0xff)
         );
  printf("state                : ");
  switch (dev->state)
  {
    case E_NM_DEVICE_STATE_UNKNOWN:
      printf("E_NM_DEVICE_STATE_UNKNOWN\n");
      break;
    case E_NM_DEVICE_STATE_UNMANAGED:
      printf("E_NM_DEVICE_STATE_UNMANAGED\n");
      break;
    case E_NM_DEVICE_STATE_UNAVAILABLE:
      printf("E_NM_DEVICE_STATE_UNAVAILABLE\n");
      break;
    case E_NM_DEVICE_STATE_DISCONNECTED:
      printf("E_NM_DEVICE_STATE_DISCONNECTED\n");
      break;
    case E_NM_DEVICE_STATE_PREPARE:
      printf("E_NM_DEVICE_STATE_PREPARE\n");
      break;
    case E_NM_DEVICE_STATE_CONFIG:
      printf("E_NM_DEVICE_STATE_CONFIG\n");
      break;
    case E_NM_DEVICE_STATE_NEED_AUTH:
      printf("E_NM_DEVICE_STATE_NEED_AUTH\n");
      break;
    case E_NM_DEVICE_STATE_IP_CONFIG:
      printf("E_NM_DEVICE_STATE_IP_CONFIG\n");
      break;
    case E_NM_DEVICE_STATE_ACTIVATED:
      printf("E_NM_DEVICE_STATE_ACTIVATED\n");
      break;
    case E_NM_DEVICE_STATE_FAILED:
      printf("E_NM_DEVICE_STATE_FAILED\n");
      break;
  }
  printf("ip4_config           : %s\n", dev->ip4_config);
  printf("dhcp4_config         : %s\n", dev->dhcp4_config);
  printf("managed              : %d\n", dev->managed);
  printf("device_type          : %u\n", dev->device_type);
  switch (dev->device_type)
  {
    case E_NM_DEVICE_TYPE_WIRED:
      printf("hw_address           : %s\n", dev->wired.hw_address);
      printf("speed                : %u\n", dev->wired.speed);
      printf("carrier              : %d\n", dev->wired.carrier);
      break;
    case E_NM_DEVICE_TYPE_WIRELESS:
      printf("hw_address           : %s\n", dev->wireless.hw_address);
      printf("mode                 : ");
      switch (dev->wireless.mode)
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
      printf("bitrate              : %u\n", dev->wireless.bitrate);
      printf("active_access_point  : %s\n", dev->wireless.active_access_point);
      printf("wireless_capabilities:");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_CIPHER_WEP40)
        printf(" E_NM_802_11_DEVICE_CAP_CIPHER_WEP40");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_CIPHER_WEP104)
        printf(" E_NM_802_11_DEVICE_CAP_CIPHER_WEP104");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_CIPHER_TKIP)
        printf(" E_NM_802_11_DEVICE_CAP_CIPHER_TKIP");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_CIPHER_CCMP)
        printf(" E_NM_802_11_DEVICE_CAP_CIPHER_CCMP");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_WPA)
        printf(" E_NM_802_11_DEVICE_CAP_WPA");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_RSN)
        printf(" E_NM_802_11_DEVICE_CAP_RSN");
      if (dev->wireless.wireless_capabilities == E_NM_802_11_DEVICE_CAP_NONE)
        printf(" E_NM_802_11_DEVICE_CAP_NONE");
      printf("\n");
      break;
  }
#endif
  printf("\n");
}


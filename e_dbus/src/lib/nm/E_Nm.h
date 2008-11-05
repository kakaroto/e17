#ifndef E_NM_H
#define E_NM_H

#include <E_DBus.h>
#include <Ecore_Data.h>

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#define E_NM_DEVICE_TYPE_WIRED 1
#define E_NM_DEVICE_TYPE_WIRELESS 2

typedef enum E_NM_State E_NM_State;
enum E_NM_State
{
  E_NM_STATE_UNKNOWN = 0,
  E_NM_STATE_ASLEEP = 1,
  E_NM_STATE_CONNECTING = 2,
  E_NM_STATE_CONNECTED = 3,
  E_NM_STATE_DISCONNECTED = 4
};

typedef enum E_NM_802_11_Ap_Flags E_NM_802_11_Ap_Flags;
enum E_NM_802_11_Ap_Flags
{
  E_NM_802_11_AP_FLAGS_NONE = 0x0,
  E_NM_802_11_AP_FLAGS_PRIVACY = 0x1
};

typedef enum E_NM_802_11_Ap_Sec E_NM_802_11_Ap_Sec;
enum E_NM_802_11_Ap_Sec
{
  E_NM_802_11_AP_SEC_NONE = 0x0,
  E_NM_802_11_AP_SEC_PAIR_WEP40 = 0x1,
  E_NM_802_11_AP_SEC_PAIR_WEP104 = 0x2,
  E_NM_802_11_AP_SEC_PAIR_TKIP = 0x4,
  E_NM_802_11_AP_SEC_PAIR_CCMP = 0x8,
  E_NM_802_11_AP_SEC_GROUP_WEP40 = 0x10,
  E_NM_802_11_AP_SEC_GROUP_WEP104 = 0x20,
  E_NM_802_11_AP_SEC_GROUP_TKIP = 0x40,
  E_NM_802_11_AP_SEC_GROUP_CCMP = 0x80,
  E_NM_802_11_AP_SEC_KEY_MGMT_PSK = 0x100,
  E_NM_802_11_AP_SEC_KEY_MGMT_802_1X = 0x200
};

typedef enum E_NM_Device_Cap E_NM_Device_Cap;
enum E_NM_Device_Cap
{
  E_NM_DEVICE_CAP_NONE = 0x0,
  E_NM_DEVICE_CAP_NM_SUPPORTED = 0x1,
  E_NM_DEVICE_CAP_CARRIER_DETECT = 0x2
};

typedef enum E_NM_Device_State E_NM_Device_State;
enum E_NM_Device_State
{
  E_NM_DEVICE_STATE_UNKNOWN = 0,
  E_NM_DEVICE_STATE_UNMANAGED = 1,
  E_NM_DEVICE_STATE_UNAVAILABLE = 2,
  E_NM_DEVICE_STATE_DISCONNECTED = 3,
  E_NM_DEVICE_STATE_PREPARE = 4,
  E_NM_DEVICE_STATE_CONFIG = 5,
  E_NM_DEVICE_STATE_NEED_AUTH = 6,
  E_NM_DEVICE_STATE_IP_CONFIG = 7,
  E_NM_DEVICE_STATE_ACTIVATED = 8,
  E_NM_DEVICE_STATE_FAILED = 9
};

typedef enum E_NM_Device_State_Reason E_NM_Device_State_Reason;
enum E_NM_Device_State_Reason
{
  E_NM_DEVICE_STATE_REASON_UNKNOWN = 0,
  E_NM_DEVICE_STATE_REASON_NONE = 1,
  E_NM_DEVICE_STATE_REASON_NOW_MANAGED = 2,
  E_NM_DEVICE_STATE_REASON_NOW_UNMANAGED = 3,
  E_NM_DEVICE_STATE_REASON_CONFIG_FAILED = 4,
  E_NM_DEVICE_STATE_REASON_CONFIG_UNAVAILABLE = 5,
  E_NM_DEVICE_STATE_REASON_CONFIG_EXPIRED = 6,
  E_NM_DEVICE_STATE_REASON_NO_SECRETS = 7,
  E_NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT = 8,
  E_NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED = 9,
  E_NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED = 10,
  E_NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT = 11,
  E_NM_DEVICE_STATE_REASON_PPP_START_FAILED = 12,
  E_NM_DEVICE_STATE_REASON_PPP_DISCONNECT = 13,
  E_NM_DEVICE_STATE_REASON_PPP_FAILED = 14,
  E_NM_DEVICE_STATE_REASON_DHCP_START_FAILED = 15,
  E_NM_DEVICE_STATE_REASON_DHCP_ERROR = 16,
  E_NM_DEVICE_STATE_REASON_DHCP_FAILED = 17,
  E_NM_DEVICE_STATE_REASON_SHARED_START_FAILED = 18,
  E_NM_DEVICE_STATE_REASON_SHARED_FAILED = 19,
  E_NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED = 20,
  E_NM_DEVICE_STATE_REASON_AUTOIP_ERROR = 21,
  E_NM_DEVICE_STATE_REASON_AUTOIP_FAILED = 22,
  E_NM_DEVICE_STATE_REASON_MODEM_BUSY = 23,
  E_NM_DEVICE_STATE_REASON_MODEM_NO_DIAL_TONE = 24,
  E_NM_DEVICE_STATE_REASON_MODEM_NO_CARRIER = 25,
  E_NM_DEVICE_STATE_REASON_MODEM_DIAL_TIMEOUT = 26,
  E_NM_DEVICE_STATE_REASON_MODEM_DIAL_FAILED = 27,
  E_NM_DEVICE_STATE_REASON_MODEM_INIT_FAILED = 28,
  E_NM_DEVICE_STATE_REASON_GSM_APN_FAILED = 29,
  E_NM_DEVICE_STATE_REASON_GSM_REGISTRATION_NOT_SEARCHING = 30,
  E_NM_DEVICE_STATE_REASON_GSM_REGISTRATION_DENIED = 31,
  E_NM_DEVICE_STATE_REASON_GSM_REGISTRATION_TIMEOUT = 32,
  E_NM_DEVICE_STATE_REASON_GSM_REGISTRATION_FAILED = 33,
  E_NM_DEVICE_STATE_REASON_GSM_PIN_CHECK_FAILED = 34,
  E_NM_DEVICE_STATE_REASON_FIRMWARE_MISSING = 35,
  E_NM_DEVICE_STATE_REASON_REMOVED = 36,
  E_NM_DEVICE_STATE_REASON_SLEEPING = 37,
  E_NM_DEVICE_STATE_REASON_CONNECTION_REMOVED = 38,
  E_NM_DEVICE_STATE_REASON_USER_REQUESTED = 39
};

typedef enum E_NM_802_11_Mode E_NM_802_11_Mode;
enum E_NM_802_11_Mode
{
  E_NM_802_11_MODE_UNKNOWN = 0,
  E_NM_802_11_MODE_ADHOC = 1,
  E_NM_802_11_MODE_INFRA = 2
};

typedef enum E_NM_802_11_Device_Cap E_NM_802_11_Device_Cap;
enum E_NM_802_11_Device_Cap
{
  E_NM_802_11_DEVICE_CAP_NONE = 0x0,
  E_NM_802_11_DEVICE_CAP_CIPHER_WEP40 = 0x1,
  E_NM_802_11_DEVICE_CAP_CIPHER_WEP104 = 0x2,
  E_NM_802_11_DEVICE_CAP_CIPHER_TKIP = 0x4,
  E_NM_802_11_DEVICE_CAP_CIPHER_CCMP = 0x8,
  E_NM_802_11_DEVICE_CAP_WPA = 0x10,
  E_NM_802_11_DEVICE_CAP_RSN = 0x20
};

typedef enum E_NMS_Context E_NMS_Context;
enum E_NMS_Context
{
  E_NMS_CONTEXT_USER,
  E_NMS_CONTEXT_SYSTEM,
};

typedef enum E_NM_Active_Connection_State E_NM_Active_Connection_State;
enum E_NM_Active_Connection_State
{
  E_NM_ACTIVE_CONNECTION_STATE_UNKNOWN = 0,
  E_NM_ACTIVE_CONNECTION_STATE_ACTIVATING = 1,
  E_NM_ACTIVE_CONNECTION_STATE_ACTIVATED = 2
};

typedef struct E_NM E_NM;
struct E_NM
{
  int         wireless_enabled; /* writeable*/
  int         wireless_hardware_enabled;
  Ecore_List *active_connections;
  E_NM_State  state;
};

typedef struct E_NM_Access_Point E_NM_Access_Point;
struct E_NM_Access_Point
{
  E_NM_802_11_Ap_Flags  flags;
  E_NM_802_11_Ap_Sec    wpa_flags;
  E_NM_802_11_Ap_Sec    rsn_flags;
  Ecore_List           *ssid; /* unsigned char */
  uint                  frequency;
  char                 *hw_address;
  E_NM_802_11_Mode      mode;
  uint                  max_bitrate;
  unsigned char         strength;
};

typedef struct E_NM_Device E_NM_Device;
struct E_NM_Device
{
  char              *udi; /* object_path */
  char              *interface;
  char              *driver;
  E_NM_Device_Cap    capabilities;
  unsigned int       ip4_address;
  E_NM_Device_State  state;
  char              *ip4_config; /* object_path */
  char              *dhcp4_config; /* object_path */
  int                managed;
  unsigned int       device_type;
  union
  {
    struct {
      char *hw_address;
      uint  speed;
      int   carrier;
    } wired;
    struct {
      char                   *hw_address;
      E_NM_802_11_Mode        mode;
      uint                    bitrate;
      char                   *active_access_point; /* object_path */
      E_NM_802_11_Device_Cap  wireless_capabilities;
    } wireless;
    /* TODO: cdma */
    /* TODO: gsm */
    /* TODO: serial */
  };
};

typedef struct E_NM_IP4_Config E_NM_IP4_Config;
struct E_NM_IP4_Config
{
  Ecore_List *addresses;  /* list uint */
  Ecore_List *nameservers;/* uint */
  Ecore_List *domains;    /* char* */
  Ecore_List *routes;     /* list uint */
};

/* TODO typedef struct E_NM_DHCP4_Config E_NM_DHCP4_Config; */

typedef struct E_NMS E_NMS;
/* No properties */

typedef struct E_NMS_Connection E_NMS_Connection;
/* No properties */

typedef struct E_NM_Active_Connection E_NM_Active_Connection;
struct E_NM_Active_Connection
{
  char                         *service_name;
  char                         *connection; /* object_path */
  char                         *specific_object; /* object_path */
  Ecore_List                   *devices; /* object_path */
  E_NM_Active_Connection_State  state;
  int                           def; /* default */
};

/* TODO typedef struct E_NM_VPN_Connection E_NM_VPN_Connection */
/* TODO typedef struct E_NM_VPN_Plugin E_NM_VPN_Plugin */

#ifdef __cplusplus
extern "C" {
#endif

   /* org.freedesktop.NetworkManager api */
   EAPI int   e_nm_get(int (*cb_func)(void *data, E_NM *nm), void *data);
   EAPI void  e_nm_free(E_NM *nm);
   EAPI void  e_nm_dump(E_NM *nm);

   /* TODO: e_nm_wireless_enabled_set */
   EAPI int   e_nm_get_devices(E_NM *nm, int (*cb_func)(void *data, Ecore_List *list), void *data);
   /* TODO: e_nm_activate_connection */
   /* TODO: e_nm_deactivate_connection */
   EAPI int   e_nm_sleep(E_NM *nm, int sleep);

   EAPI void  e_nm_data_set(E_NM *nm, void *data);
   EAPI void *e_nm_data_get(E_NM *nm);
   EAPI void  e_nm_callback_state_changed_set(E_NM *nm, int (*cb_func)(E_NM *nm, E_NM_State state));
   EAPI void  e_nm_callback_properties_changed_set(E_NM *nm, int (*cb_func)(E_NM *nm));
   EAPI void  e_nm_callback_device_added_set(E_NM *nm, int (*cb_func)(E_NM *nm, const char *device));
   EAPI void  e_nm_callback_device_removed_set(E_NM *nm, int (*cb_func)(E_NM *nm, const char *device));

   /* org.freedesktop.NetworkManager.AccessPoint api */
   EAPI int   e_nm_access_point_get(E_NM *nm, const char *access_point,
                                    int (*cb_func)(void *data, E_NM_Access_Point *ap),
                                    void *data);
   EAPI void  e_nm_access_point_free(E_NM_Access_Point *access_point);
   EAPI void  e_nm_access_point_dump(E_NM_Access_Point *access_point);

   EAPI void  e_nm_access_point_data_set(E_NM_Access_Point *access_point, void *data);
   EAPI void *e_nm_access_point_data_get(E_NM_Access_Point *access_point);
   EAPI void  e_nm_access_point_callback_properties_changed_set(E_NM_Access_Point *access_point, int (*cb_func)(E_NM_Access_Point *access_point));

   /* org.freedesktop.NetworkManager.Device(.*) api */
   EAPI int   e_nm_device_get(E_NM *nm, const char *device,
                              int (*cb_func)(void *data, E_NM_Device *dev),
                              void *data);
   EAPI void  e_nm_device_free(E_NM_Device *device);
   EAPI void  e_nm_device_dump(E_NM_Device *device);

   /* TODO: e_nm_device_wireless_get_access_points */

   EAPI void  e_nm_device_data_set(E_NM_Device *device, void *data);
   EAPI void *e_nm_device_data_get(E_NM_Device *device);
   EAPI void  e_nm_device_callback_state_changed_set(E_NM_Device *device, int (*cb_func)(E_NM_Device *device, E_NM_State new_state, E_NM_State old_state, E_NM_Device_State_Reason reason));
   EAPI void  e_nm_device_callback_properties_changed_set(E_NM_Device *device, int (*cb_func)(E_NM_Device *device));
   /* TODO: e_nm_device_wireless_callback_access_point_added_set */
   /* TODO: e_nm_device_wireless_callback_access_point_removed_set */

   /* org.freedesktop.NetworkManager.IP4Config api */
   EAPI int   e_nm_ip4_config_get(E_NM *nm, const char *config,
                                  int (*cb_func)(void *data, E_NM_IP4_Config *config),
                                  void *data);
   EAPI void  e_nm_ip4_config_free(E_NM_IP4_Config *config);
   EAPI void  e_nm_ip4_config_dump(E_NM_IP4_Config *config);

   /* TODO: org.freedesktop.NetworkManager.DHCP4Config api */

   /* org.freedesktop.NetworkManagerSettings api */
   EAPI int   e_nms_get(E_NM *nm, int (*cb_func)(void *data, E_NMS *nms), void *data);
   EAPI void  e_nms_free(E_NMS *nms);
   EAPI void  e_nms_dump(E_NMS *nms);
   EAPI int   e_nms_list_connections(E_NMS *nms,
                        int (*cb_func)(void *data, Ecore_List *list),
                        void *data);

   EAPI void  e_nms_callback_new_connection_set(E_NMS *nms, int (*cb_func)(E_NMS *nms, E_NMS_Context context, const char *connection));

   /* org.freedesktop.NetworkManagerSettings.Connection(.*) api */
   EAPI int   e_nms_connection_get(E_NMS *nms, E_NMS_Context context, const char *connection, int (*cb_func)(void *data, E_NMS_Connection *conn), void *data);
   EAPI void  e_nms_connection_free(E_NMS_Connection *conn);
   EAPI void  e_nms_connection_dump(E_NMS_Connection *conn);

   /* TODO: e_nms_connection_update */
   /* TODO: e_nms_connection_delete */
   /* TODO: e_nms_connection_get_settings */
   /* TODO: e_nms_connection_secrets_get_secrets */

   /* TODO: e_nms_connection_callback_updated_set */
   /* TODO: e_nms_connection_callback_removed_set */

   /* TODO: org.freedesktop.NetworkManager.Connection.Active api */
   EAPI int   e_nm_active_connection_get(E_NM *nm, const char *conn, int (*cb_func)(void *data, E_NM_Active_Connection *conn), void *data);
   EAPI void  e_nm_active_connection_free(E_NM_Active_Connection *conn);
   EAPI void  e_nm_active_connection_dump(E_NM_Active_Connection *conn);

   EAPI void  e_nm_active_connection_callback_properties_changed_set(E_NM_Active_Connection *conn, int (*cb_func)(E_NM_Active_Connection *conn));

   /* TODO: org.freedesktop.NetworkManager.VPN.Connection api */
   /* TODO: org.freedesktop.NetworkManager.VPN.Plugin api */

#ifdef __cplusplus
}
#endif

#endif

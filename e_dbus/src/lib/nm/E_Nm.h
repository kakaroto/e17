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

typedef struct E_NM E_NM;
struct E_NM
{
  int         wireless_enabled; /* writeable*/
  int         wireless_hardware_enabled;
  Ecore_List *active_connections;
  E_NM_State  state;
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
  /* TODO: Is it always 3 uints? */
  Ecore_List *addresses;  /* (uint, uint, uint)s */
  Ecore_List *nameservers;/* uints */
  Ecore_List *domains;    /* char* */
  /* TODO: Is it always 4 uints? */
  Ecore_List *routes;     /* (uint, uint, uint, uint)s */
};

typedef struct E_NM_Access_Point E_NM_Access_Point;
struct E_NM_Access_Point
{
  uint  flags;
  uint  wpaflags;
  uint  rsnflags;
  char *ssid;
  uint  frequency;
  char *hwaddress;
  int   mode;
  uint  rate;
  uint  strength;
};

#ifdef __cplusplus
extern "C" {
#endif

   /* org.freedesktop.NetworkManager api */
   EAPI int   e_nm_get(int (*cb_func)(void *data, void *reply), void *data);
   EAPI void  e_nm_free(E_NM *nm);
   EAPI void  e_nm_dump(E_NM *nm);

   /* TODO: e_nm_wireless_enabled_set */
   EAPI int   e_nm_get_devices(E_NM *nm, int (*cb_func)(void *data, void *reply), void *data);
   /* TODO: e_nm_activate_connection */
   /* TODO: e_nm_deactivate_connection */
   EAPI int   e_nm_sleep(E_NM *nm, int sleep);

   EAPI void  e_nm_data_set(E_NM *nm, void *data);
   EAPI void *e_nm_data_get(E_NM *nm);
   EAPI void  e_nm_callback_state_changed_set(E_NM *nm, int (*cb_func)(E_NM *nm, E_NM_State state));
   EAPI void  e_nm_callback_properties_changed_set(E_NM *nm, int (*cb_func)(E_NM *nm));
   EAPI void  e_nm_callback_device_added_set(E_NM *nm, int (*cb_func)(E_NM *nm, const char *device));
   EAPI void  e_nm_callback_device_removed_set(E_NM *nm, int (*cb_func)(E_NM *nm, const char *device));

   /* org.freedesktop.NetworkManager.Device api */
   EAPI int   e_nm_device_get(E_NM *nm, const char *device,
                              int (*cb_func)(void *data, void *reply),
                              void *data);
   EAPI void  e_nm_device_free(E_NM_Device *device);
   EAPI void  e_nm_device_dump(E_NM_Device *device);

   /* TODO: e_nm_device_wireless_get_access_points */

   EAPI void  e_nm_device_data_set(E_NM_Device *device, void *data);
   EAPI void *e_nm_device_data_get(E_NM_Device *device);
   EAPI void  e_nm_device_callback_state_changed_set(E_NM_Device *device, int (*cb_func)(E_NM_Device *device, E_NM_State state));
   EAPI void  e_nm_device_callback_properties_changed_set(E_NM_Device *device, int (*cb_func)(E_NM_Device *device));
   /* TODO: e_nm_device_wireless_callback_access_point_added_set */
   /* TODO: e_nm_device_wireless_callback_access_point_removed_set */

   /* org.freedesktop.NetworkManager.IP4Config api */
   EAPI int   e_nm_ip4_config_get(E_NM *nm, const char *device,
                                  int (*cb_func)(void *data, void *reply),
                                  void *data);
   EAPI void  e_nm_ip4_config_free(E_NM_IP4_Config *config);
   EAPI void  e_nm_ip4_config_dump(E_NM_IP4_Config *config);

#ifdef __cplusplus
}
#endif

#endif

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

typedef enum E_NM_Device_Cap _E_NM_Device_Cap;
enum _E_NM_Device_Cap
{
  E_NM_DEVICE_CAP_NONE = 0x0,
  E_NM_DEVICE_CAP_NM_SUPPORTED = 0x1,
  E_NM_DEVICE_CAP_CARRIER_DETECT = 0x2
};

typedef enum E_NM_Device_State _E_NM_Device_State;
enum _E_NM_Device_State
{
  E_NM_DEVICE_STATE_UNKNOWN = 0,
  E_NM_DEVICE_STATE_DOWN = 1,
  E_NM_DEVICE_STATE_DISCONNECTED = 2,
  E_NM_DEVICE_STATE_PREPARE = 3,
  E_NM_DEVICE_STATE_CONFIG = 4,
  E_NM_DEVICE_STATE_NEED_AUTH = 5,
  E_NM_DEVICE_STATE_IP_CONFIG = 6,
  E_NM_DEVICE_STATE_ACTIVATED = 7,
  E_NM_DEVICE_STATE_FAILED = 8,
  E_NM_DEVICE_STATE_CANCELLED = 9
};

typedef struct E_NM_Context E_NM_Context;

/**
 * Matching structs for the new "object" based DBus API
 * VPN is missing as this was still missing the redesign for the new 0.7 API
 */
typedef struct E_NM_Device E_NM_Device;
struct E_NM_Device
{
  char *udi; /* object_path */
  char *interface;
  char *driver;
  uint capabilities;
  int  ip4address;
  uint state;
  char *ip4config; /* object_path */
  int  carrier;
  uint type;
};

typedef struct E_NM_Device_Wireless E_NM_Device_Wireless;
struct E_NM_Device_Wireless
{
  char *hwaddress;
  int   mode;
  uint bitrate;
  char *activeaccesspoint; /* object_path */
  uint wirelesscapabilities;
};

typedef struct E_NM_Device_Wired E_NM_Device_Wired;
struct E_NM_Device_Wired
{
  char *hwaddress;
  uint speed;
};

typedef struct E_NM_Access_Point E_NM_Access_Point;
struct E_NM_Access_Point
{
  uint flags;
  uint wpaflags;
  uint rsnflags;
  char *ssid;
  uint frequency;
  char *hwaddress;
  int  mode;
  uint rate;
  uint strength;
};

typedef struct E_NM_IP4Config E_NM_IP4Config;
struct E_NM_IP4Config
{
  uint address;
  uint gateway;
  uint netmask;
  uint broadcast;
  char *hostname;
  Ecore_List *nameserver;  /* uints */
  Ecore_List *domains; /* char* */
  char *nisdomain;
  Ecore_List *nisserver; /* uints */
};

typedef struct E_NM_Manager E_NM_Manager;
struct E_NM_Manager
{
  int wirelessenabled; /* writeable*/
  int wirelesshardwareenabled;
  uint state;
};

typedef void (*E_NM_Cb_Manager_State_Change) (void *data, int state);
typedef void (*E_NM_Cb_Manager_Device_Added) (void *data, const char *device);
typedef void (*E_NM_Cb_Manager_Device_Removed) (void *data, const char *device);

#ifdef __cplusplus
extern "C" {
#endif

   EAPI E_NM_Context *e_nm_new(void);
   EAPI void          e_nm_free(E_NM_Context *ctx);


/**
 * A callback function for a Network Manager call
 * @param user_data the data passed in to the method call
 * @param event_data a struct containing the return data.
 *
 * For method calls, the return structs use the following naming convention:
 *   E_NM_<Interface>_<Method_Call_Name>_Return
 */

/* org.freedesktop.NetworkManager api */
/* methods */
   EAPI int e_nm_get_devices(E_NM_Context *ctx, E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_get_active_device(E_NM_Context *ctx, E_DBus_Callback_Func cb_func, void *data);
/* TODO: EAPI int e_nm_set_active_device() */
   EAPI int e_nm_status(E_NM_Context *ctx, E_DBus_Callback_Func cb_func, void *data);
/* signals */
/* TODO: These have changed! */
   /* deprecated */EAPI void e_nm_callback_manager_state_change_set(E_NM_Context *ctx, E_NM_Cb_Manager_State_Change cb_func, void *user_data);
   /* deprecated */EAPI void e_nm_callback_manager_device_added_set(E_NM_Context *ctx, E_NM_Cb_Manager_Device_Added cb_func, void *user_data);
   /* deprecated */EAPI void e_nm_callback_manager_device_removed_set(E_NM_Context *ctx, E_NM_Cb_Manager_Device_Removed cb_func, void *user_data);
/* TODO: EAPI void e_nm_callback_device_no_longer_active() */
/* TODO: EAPI void e_nm_callback_device_now_active() */
/* TODO: EAPI void e_nm_callback_device_activating() */
/* TODO: EAPI void e_nm_callback_devices_changed() */
/* TODO: EAPI void e_nm_callback_device_activation_failed() */
/* TODO: EAPI void e_nm_callback_device_strength_changed() */
/* TODO: EAPI void e_nm_callback_device_ipv4_address_changed() */
/* TODO: EAPI void e_nm_callback_wireless_network_appeared() */
/* TODO: EAPI void e_nm_callback_wireless_network_disappeared() */


/* org.freedesktop.NetworkManager.Device api */
   EAPI int e_nm_device_get_udi(E_NM_Context *ctx, const char *device,
                                E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_interface(E_NM_Context *ctx, const char *device,
                                      E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_driver(E_NM_Context *ctx, const char *device,
                                   E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_capabilities(E_NM_Context *ctx, const char *device,
                                         E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_ip4address(E_NM_Context *ctx, const char *device,
                                       E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_state(E_NM_Context *ctx, const char *device,
                                  E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_ip4config(E_NM_Context *ctx, const char *device,
                                      E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_carrier(E_NM_Context *ctx, const char *device,
                                    E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_type(E_NM_Context *ctx, const char *device,
                                 E_DBus_Callback_Func cb_func, void *data);

/* org.freedesktop.NetworkManager.Devices api */
/* TODO: EAPI int e_nm_network_get_name() */
/* TODO: EAPI int e_nm_network_get_address() */
/* TODO: EAPI int e_nm_network_get_strength() */
/* TODO: EAPI int e_nm_network_get_frequency() */
/* TODO: EAPI int e_nm_network_get_rate() */
/* TODO: EAPI int e_nm_network_get_encrypted() */

#ifdef __cplusplus
}
#endif

#endif

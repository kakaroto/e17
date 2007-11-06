#ifndef E_NM_H
#define E_HM_H

#include <E_DBus.h>

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

typedef struct E_NM_Context E_NM_Context;

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
   EAPI int e_nm_device_get_name(E_NM_Context *ctx, const char *device,
                                 E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_type(E_NM_Context *ctx, const char *device,
                                 E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_hal_udi(E_NM_Context *ctx, const char *device,
                                    E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_ip4_address(E_NM_Context *ctx, const char *device,
                                        E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_get_link_active(E_NM_Context *ctx, const char *device,
                                        E_DBus_Callback_Func cb_func, void *data);
   EAPI int e_nm_device_wireless_get_strength(E_NM_Context *ctx,
                                              const char *device,
                                              E_DBus_Callback_Func cb_func,
                                              void *data);
   EAPI int e_nm_device_wireless_get_active_network(E_NM_Context *ctx,
                                                    const char *device,
                                                    E_DBus_Callback_Func cb_func,
                                                    void *data);
   EAPI int e_nm_device_wireless_get_networks(E_NM_Context *ctx,
                                              const char *device,
                                              E_DBus_Callback_Func cb_func,
                                              void *data);

/* org.freedesktop.NetworkManager.Devices api */
/* TODO: EAPI int e_nm_network_get_name() */
/* TODO: EAPI int e_nm_network_get_address() */
/* TODO: EAPI int e_nm_network_get_strength() */
/* TODO: EAPI int e_nm_network_get_frequency() */
/* TODO: EAPI int e_nm_network_get_rate() */
/* TODO: EAPI int e_nm_network_get_encrypted() */

/* org.freedesktop.NetworkManagerInfo api */
/* signals */
/* TODO: EAPI void e_nmi_callback_trusted_network_update() */
/* TODO: EAPI void e_nmi_callback_preferred_network_update() */

#ifdef __cplusplus
}
#endif

#endif

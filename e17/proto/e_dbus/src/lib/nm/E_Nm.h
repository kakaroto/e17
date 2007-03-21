#ifndef E_NM_H
#define E_HM_H

#include <E_DBus.h>

typedef struct E_NM_Context E_NM_Context;

typedef void (*E_NM_Cb_Manager_State_Change) (void *data, int state);
typedef void (*E_NM_Cb_Manager_Device_Added) (void *data, const char *device);
typedef void (*E_NM_Cb_Manager_Device_Removed) (void *data, const char *device);

E_NM_Context * e_nm_new(void);
void e_nm_free(E_NM_Context *ctx);


/**
 * A callback function for a Network Manager call
 * @param user_data the data passed in to the method call
 * @param event_data a struct containing the return data.
 *
 * For method calls, the return structs use the following naming convention:
 *   E_NM_<Interface>_<Method_Call_Name>_Return
 */
typedef void (*E_NM_Callback_Func) (void *user_data, void *method_return, DBusError *error);

/* org.freedesktop.NetworkManager api */
int e_nm_get_devices(E_NM_Context *ctx, E_NM_Callback_Func cb_func, void *data);
int e_nm_sleep(E_NM_Context *ctx, E_NM_Callback_Func cb_func, void *data, int do_sleep);
#endif

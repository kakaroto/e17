#ifndef E_HAL_MANAGER_H
#define E_HAL_MANAGER_H

#include "E_Hal.h"

typedef struct E_Hal_String_List_Return E_Hal_Manager_Get_All_Devices_Return;
typedef struct E_Hal_Bool_Return E_Hal_Manager_Device_Exists_Return;
typedef struct E_Hal_String_List_Return E_Hal_Manager_Find_Device_String_Match_Return;
typedef struct E_Hal_String_List_Return E_Hal_Manager_Find_Device_By_Capability_Return;

typedef struct E_Hal_UDI_Return E_Hal_Manager_Device_Added;
typedef struct E_Hal_UDI_Return E_Hal_Manager_Device_Removed;
typedef struct E_Hal_Capability E_Hal_Manager_New_Capability;


int e_hal_manager_get_all_devices(E_DBus_Connection *conn, E_Hal_Callback_Func cb_func, void *data);
int e_hal_manager_device_exists(E_DBus_Connection *conn, const char *udi, E_Hal_Callback_Func cb_func, void *data);
int e_hal_manager_find_device_string_match(E_DBus_Connection *conn, const char *key, const char *value, E_Hal_Callback_Func cb_func, void *data);
int e_hal_manager_find_device_by_capability(E_DBus_Connection *conn, const char *capability, E_Hal_Callback_Func cb_func, void *data);

#endif

#ifndef E_HAL_DEVICE_H
#define E_HAL_DEVICE_H

#include "E_Hal.h"

typedef struct E_Hal_Properties E_Hal_Device_Get_All_Properties_Return;
typedef struct E_Hal_Property E_Hal_Device_Get_Property_Return;
typedef struct E_Hal_Bool_Return E_Hal_Device_Query_Capability_Return;

int e_hal_device_get_property(E_DBus_Connection *conn, const char *udi, const char *property, E_Hal_Callback_Func cb_func, void *data);
int e_hal_device_get_all_properties(E_DBus_Connection *conn, const char *udi, E_Hal_Callback_Func cb_func, void *data);
int e_hal_device_query_capability(E_DBus_Connection *conn, const char *udi, const char *capability, E_Hal_Callback_Func cb_func, void *data);

#endif

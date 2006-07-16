#ifndef __EDVI_DEVICE_H__
#define __EDVI_DEVICE_H__


#include "edvi_forward.h"


Edvi_Device *edvi_device_new (int h_dpi,
                              int v_dpi);

void         edvi_device_delete (Edvi_Device *device);


#endif /* __EDVI_DEVICE_H__ */

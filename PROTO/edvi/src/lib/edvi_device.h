#ifndef __EDVI_DEVICE_H__
#define __EDVI_DEVICE_H__


#include "edvi_forward.h"


/**
 * @file edvi_device.h
 * @defgroup Edvi_Device Edvi Device
 * @brief Functions that manage DVI devices
 * @ingroup Edvi
 *
 * Functions that manage DVI devices
 */


/**
 * Return a newly created Edvi_Device object
 *
 * @param h_dpi The horizontal dpi.
 * @param v_dpi The vertical dpi.
 * @return A pointer to a newly created Edvi_Device
 *
 * Return a newly created Edvi_Device object  The result must be freed with
 * edvi_device_delete().
 *
 * @ingroup Edvi_Device
 */
Edvi_Device *edvi_device_new (int h_dpi,
                              int v_dpi);

/**
 * Delete an Edvi_Device object
 *
 * @param device The device to delete
 *
 * Delete the Edvi_Device @p device that has been created
 * with edvi_device_new()
 *
 * @ingroup Edvi_Device
 */
void edvi_device_delete (Edvi_Device *device);


#endif /* __EDVI_DEVICE_H__ */

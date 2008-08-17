#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#include <Edata.h>
#include <Ecore.h>

#include "Ecore_Li.h"
#include "ecore_li_private.h"
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
#define TEST_BIT(bit, array)    (array[bit/8] & (1<<(bit%8)))

static int _count = 0;
Edata_List *_devices = NULL;
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
int ECORE_LI_EVENT_KEY_DOWN;
int ECORE_LI_EVENT_KEY_UP;
int ECORE_LI_EVENT_BUTTON_UP;
int ECORE_LI_EVENT_BUTTON_DOWN;
int ECORE_LI_EVENT_MOUSE_MOVE;
int ECORE_LI_EVENT_MOUSE_WHEEL;
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int ecore_li_init(void)
{
	if(!_count)
        {
		ECORE_LI_EVENT_KEY_DOWN = ecore_event_type_new();
		ECORE_LI_EVENT_KEY_UP = ecore_event_type_new();
		ECORE_LI_EVENT_BUTTON_UP = ecore_event_type_new();
		ECORE_LI_EVENT_BUTTON_DOWN = ecore_event_type_new();
		ECORE_LI_EVENT_MOUSE_MOVE = ecore_event_type_new();
		ECORE_LI_EVENT_MOUSE_WHEEL = ecore_event_type_new();
	}
	return ++_count;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int ecore_li_shutdown(void)
{
	if (--_count)
		return _count;
	/* TODO free the list */
	return _count;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ecore_Li_Device *ecore_li_device_new(const char *path)
{
	Ecore_Li_Device *device;
	char event_type_bitmask[EV_MAX/8 + 1];
	int event_type;
	int fd;

	if (!path) return NULL;
	device = calloc(1, sizeof(Ecore_Li_Device));
	if (!device) return NULL;

	if (!_devices)
		_devices = edata_list_new();

	if ((fd = open(path, O_RDONLY, O_NONBLOCK)) < 0)
	{
		goto error_open;
	}
	/* query capabilities */
	if (ioctl(fd, EVIOCGBIT(0, EV_MAX), event_type_bitmask) < 0)
	{
		goto error_caps;
	}
	/* query name */
	device->info.name = calloc(256, sizeof(char));
	if (ioctl(fd, EVIOCGNAME(sizeof(char) * 256), device->info.name) < 0)
	{
		strcpy(device->info.name, "Unknown");
	}
	device->fd = fd;
	device->info.dev = strdup(path);
	/* common */
	device->mouse.threshold = CLICK_THRESHOLD;
	device->keyboard.layout = &kbd_layout;

	/* set info */
	for (event_type = 0; event_type < EV_MAX; event_type++)
	{
		if (!TEST_BIT(event_type, event_type_bitmask))
			continue;
		switch (event_type)
		{
			case EV_SYN:
			break;

			case EV_KEY:
			device->info.caps |= ECORE_LI_DEVICE_CAP_KEYS_OR_BUTTONS;
			break;

			case EV_REL:
			device->info.caps |= ECORE_LI_DEVICE_CAP_RELATIVE;
			break;

			case EV_ABS:
			device->info.caps |= ECORE_LI_DEVICE_CAP_ABSOLUTE;
			break;

			case EV_MSC:
			case EV_LED:
			case EV_SND:
			case EV_REP:
			case EV_FF :
			case EV_FF_STATUS:
			case EV_PWR:
			default:
			break;
		}
	}
	edata_list_append(_devices, device);
	return device;
error_caps:
	close(fd);
error_open:
	free(device);
	return NULL;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char *ecore_li_device_name_get(Ecore_Li_Device *dev)
{
	assert(dev);
	return dev->info.name;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ecore_li_device_enable(Ecore_Li_Device *dev, int enable)
{
	/* TODO redo this function */
	assert(dev);
	if (enable == dev->enabled) return;
	if (enable)
	{
		/* if the device already had a handler */
		if (!dev->handler)
			dev->handler = ecore_main_fd_handler_add(dev->fd, ECORE_FD_READ, device_fd_callback, dev, NULL, NULL);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 * 
 * If the device is a relative input device, 
 * we must set a width and height for it. If its
 * absolute set the ioctl correctly, if not, unsupported
 * device
 */
EAPI void
ecore_li_device_axis_size_set(Ecore_Li_Device *dev, int w, int h)
{
	assert(dev);
	
	if(w < 0 || h < 0)
		return;
	/* FIXME 
	 * this code is for a touchscreen device, 
	 * make it configurable (ABSOLUTE | RELATIVE)
	 */
	if(dev->info.caps & ECORE_LI_DEVICE_CAP_ABSOLUTE)
	{
		/* FIXME looks like some kernels dont include this struct */
		struct input_absinfo abs_features;

		ioctl(dev->fd, EVIOCGABS(ABS_X), &abs_features);
		dev->mouse.min_w = abs_features.minimum;
		dev->mouse.rel_w = (double)(abs_features.maximum - abs_features.minimum)/(double)(w);

		ioctl(dev->fd, EVIOCGABS(ABS_Y), &abs_features);
		dev->mouse.min_h = abs_features.minimum;
		dev->mouse.rel_h = (double)(abs_features.maximum - abs_features.minimum)/(double)(h);
	}
	else if(!(dev->info.caps & ECORE_LI_DEVICE_CAP_RELATIVE))
		return;

	/* update the local values */
	if(dev->mouse.x > w - 1)
		dev->mouse.x = w -1;
	if(dev->mouse.y > h - 1)
		dev->mouse.y = h -1;
	dev->mouse.w = w;
	dev->mouse.h = h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
ecore_li_device_keyboard_layout_set(Ecore_Li_Device *dev, Ecore_Li_Keyboard_Layout *l)
{
	assert(dev);

	dev->keyboard.layout = l;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void ecore_li_device_delete(Ecore_Li_Device *d)
{
	assert(d);
	free(d);
}


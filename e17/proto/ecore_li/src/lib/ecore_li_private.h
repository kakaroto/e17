#ifndef _ECORE_LI_PRIVATE_H
#define _ECORE_LI_PRIVATE_H

#include <assert.h>
#include <linux/input.h>

#include "kbd_layout.h" 	// Default keyboard layout
#define CLICK_THRESHOLD 0.25 	// Default double click threshold

struct _Ecore_Li_Device
{
	int fd;
	Ecore_Fd_Handler *handler;
	int enabled;
	struct {
		int caps;
		char *name;
		char *dev;
	} info;
	struct
	{
		/* common mouse */
		int x,y;
		int w,h;
		
		double last;
		double prev;
		double threshold;
		/* absolute axis */
		int min_w, min_h;
		double rel_w, rel_h;

	} mouse;
	struct
	{
		Ecore_Li_Keyboard_Layout *layout;
		int mods;
		int capslock;
	} keyboard;
};

int device_fd_callback(void *data, Ecore_Fd_Handler *fdh);



#endif

#include <unistd.h>
#include <Edata.h>
#include <Ecore.h>
#include "Ecore_Li.h"
#include "ecore_li_private.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static int 
_keyboard_modifiers_unset(int *mods, char *keyname)
{
	if (!strcmp(keyname, "Shift"))
	{
		*mods &= ~(1 << (ECORE_LI_MOD_SHIFT - 1));
	}
	else if (!strcmp(keyname, "AltGr"))
	{
		*mods &= ~(1 << (ECORE_LI_MOD_ALTGR - 1));
	}
	else if (!strcmp(keyname, "Control"))
	{
		*mods &= ~(1 << (ECORE_LI_MOD_CONTROL - 1));
	}
	else if (!strcmp(keyname, "Alt"))
	{
		*mods &= ~(1 << (ECORE_LI_MOD_ALT - 1));
	}
	else if (!strcmp(keyname, "ShiftL"))
	{
		*mods &= ~(1 << (ECORE_LI_MOD_SHIFTL - 1));
	}
	else if (!strcmp(keyname, "ShiftR"))
	{
		*mods &= ~(1 << (ECORE_LI_MOD_SHIFTR - 1));
	}
	else if (!strcmp(keyname, "CtrlL"))
	{
		*mods &= ~(1 << (ECORE_LI_MOD_CONTROLL - 1));
	}
	else if (!strcmp(keyname, "CtrlR"))
	{
		*mods &= ~(1 << (ECORE_LI_MOD_CONTROLR - 1));
	}
	else
		return 0;
	return 1;
}

static int 
_keyboard_modifiers_set(int *mods, char *keyname)
{
	if (!strcmp(keyname, "Shift"))
	{
		*mods |= 1 << (ECORE_LI_MOD_SHIFT - 1);
	}
	else if (!strcmp(keyname, "AltGr"))
	{
		*mods |= 1 << (ECORE_LI_MOD_ALTGR - 1);
	}
	else if (!strcmp(keyname, "Control"))
	{
		*mods |= 1 << (ECORE_LI_MOD_CONTROL - 1);
	}
	else if (!strcmp(keyname, "Alt"))
	{
		*mods |= 1 << (ECORE_LI_MOD_ALT - 1);
	}
	else if (!strcmp(keyname, "ShiftL"))
	{
		*mods |= 1 << (ECORE_LI_MOD_SHIFTL - 1);
	}
	else if (!strcmp(keyname, "ShiftR"))
	{
		*mods |= 1 << (ECORE_LI_MOD_SHIFTR - 1);
	}
	else if (!strcmp(keyname, "CtrlL"))
	{
		*mods |= 1 << (ECORE_LI_MOD_CONTROLL - 1);
	}
	else if (!strcmp(keyname, "CtrlR"))
	{
		*mods |= 1 << (ECORE_LI_MOD_CONTROLR - 1);
	}
	else
		return 0;
	return 1;
}

static int 
_keyboard_locks_toggle(int *locks, char *keyname)
{
	if (!strcmp(keyname, "Caps_Lock"))
	{
		*locks = !*locks;
	}
	else
		return 0;
	return 1;
}
static Ecore_Li_Event_Key_Down *_key_down_get(Ecore_Li_Device *d, int code)
{
	Ecore_Li_Event_Key_Down *ev;
	char *tmp;
	char *keyname;

	if (!d) return NULL;

	ev = calloc(1, sizeof(Ecore_Li_Event_Key_Down));
	if (!ev) return NULL;

	printf("%d\n", code);	
	/* on modifiers/locks we aren't strict, just check on the
	 * "plain" element to see if it's a modifier
	 */
	tmp = d->keyboard.layout->codes[code].mod[ECORE_LI_MOD_PLAIN].name;
	/* check if its a modifier */
	if (tmp && _keyboard_modifiers_set(&d->keyboard.mods, tmp))
		keyname = tmp;
	/* check if its a capslock and toggle it */
	else if (tmp && _keyboard_locks_toggle(&d->keyboard.capslock, tmp))
		keyname = tmp;
	else
	{
		int mods = d->keyboard.mods;
		/* check the capslock, if it set and the 
		 * key supports it we behave like a shift
		 * or ~shift in case shift is already pressed 
		 * if the key doesnt support it we use the
		 * actual keyname
		 */
		if (d->keyboard.layout->codes[code].mod[mods].letter && d->keyboard.capslock)
		{
			if (mods & (1 << (ECORE_LI_MOD_SHIFT - 1)))
				mods &= ~(1 << (ECORE_LI_MOD_SHIFT - 1));
			else
				mods |= (1 << (ECORE_LI_MOD_SHIFT - 1));
		}
		keyname = d->keyboard.layout->codes[code].mod[mods].name;
		if (!keyname)
			keyname = tmp;
	}
	ev->dev = d;
	ev->keyname = strdup(keyname);

	return ev;
}

static Ecore_Li_Event_Key_Up * _key_up_get(Ecore_Li_Device *d, int code)
{
	Ecore_Li_Event_Key_Up *ev;
	char *keyname;
	char *tmp;
	int caps = 0;
	
	if (!d) return NULL;
			
	ev = calloc(1, sizeof(Ecore_Li_Event_Key_Up));
	if (!ev) return NULL;

	tmp = d->keyboard.layout->codes[code].mod[ECORE_LI_MOD_PLAIN].name;
	if (tmp && _keyboard_modifiers_unset(&d->keyboard.mods, tmp))
		keyname = tmp;
	else if (tmp && _keyboard_locks_toggle(&caps, tmp))
		keyname = tmp;
	else
	{
		int mods = d->keyboard.mods;

		if (d->keyboard.layout->codes[code].mod[mods].letter && d->keyboard.capslock)
		{
			if (mods & (1 << (ECORE_LI_MOD_SHIFT - 1)))
				mods &= ~(1 << (ECORE_LI_MOD_SHIFT - 1));
			else
				mods |= (1 << (ECORE_LI_MOD_SHIFT - 1));
		}
		keyname = d->keyboard.layout->codes[code].mod[mods].name;
		if (!keyname)
			keyname = tmp;
	}
	ev->dev = d;
	ev->keyname = strdup(keyname);
	
	return ev;
}

static void _key_free(void *data, void *ev)
{
	Ecore_Li_Event_Key_Up *e = ev;
	if (!e) return;

	free(e->keyname);
	if (e->keysymbol) free(e->keysymbol);
	if (e->key_compose) free(e->key_compose);
	free(e);
}

static inline void
_event_abs(Ecore_Li_Device *dev, struct input_event *iev)
{
	switch (iev->code)
	{
		case ABS_X:
		case ABS_Y:
		{
			Ecore_Li_Event_Mouse_Move *ev;
			if ((iev->code == ABS_X) && (dev->mouse.w != 0))
			{
				int tmp;

				tmp = (int)((double)(iev->value - dev->mouse.min_w) / dev->mouse.rel_w);
				if(tmp < 0)
					dev->mouse.x = 0;
				else if(tmp > dev->mouse.w)
					dev->mouse.x = dev->mouse.w;
				else
					dev->mouse.x = tmp;
			}
			else if((iev->code == ABS_Y) && (dev->mouse.h != 0))
			{
				int tmp;

				tmp = (int)((double)(iev->value - dev->mouse.min_h) / dev->mouse.rel_h);
				if(tmp < 0)
					dev->mouse.y = 0;
				else if(tmp > dev->mouse.h)
					dev->mouse.y = dev->mouse.h;
				else
					dev->mouse.y = tmp;
			}
			ev = calloc(1,sizeof(Ecore_Li_Event_Mouse_Move));
			ev->x = dev->mouse.x;
			ev->y = dev->mouse.y;
			ev->dev = dev;

			ecore_event_add(ECORE_LI_EVENT_MOUSE_MOVE, ev, NULL, NULL);
			break;
		}
		case ABS_PRESSURE:
			/* TODO emulate a button press */
			break;
	}
}

static inline void
_event_rel(Ecore_Li_Device *dev, struct input_event *iev)
{
	/* dispatch the button events if they are queued */
	switch (iev->code)
	{
		case REL_X:
		case REL_Y:
		{
			Ecore_Li_Event_Mouse_Move *ev;
			if (iev->code == REL_X)
			{
				dev->mouse.x += iev->value;
				if(dev->mouse.x > dev->mouse.w - 1)
					dev->mouse.x = dev->mouse.w;
				else if(dev->mouse.x < 0)
					dev->mouse.x = 0;
			}
			else
			{
				dev->mouse.y += iev->value;
				if(dev->mouse.y > dev->mouse.h - 1)
					dev->mouse.y = dev->mouse.h;
				else if(dev->mouse.y < 0)
					dev->mouse.y = 0;
			}
			ev = calloc(1,sizeof(Ecore_Li_Event_Mouse_Move));
			ev->x = dev->mouse.x;
			ev->y = dev->mouse.y;
			ev->dev = dev;

			ecore_event_add(ECORE_LI_EVENT_MOUSE_MOVE, ev, NULL, NULL);
			break;
		}
		case REL_WHEEL:
		case REL_HWHEEL:
		{
			Ecore_Li_Event_Mouse_Wheel *ev;
			ev = calloc(1, sizeof(Ecore_Li_Event_Mouse_Wheel));

			ev->x = dev->mouse.x;
			ev->y = dev->mouse.y;
			if(iev->code == REL_HWHEEL)
				ev->direction = 1;
			ev->wheel = iev->value;
			ev->dev = dev;
			ecore_event_add(ECORE_LI_EVENT_MOUSE_WHEEL, ev, NULL, NULL);
			break;
		}
		default:
			break;
	}

}

static inline void
_event_key(Ecore_Li_Device *dev, struct input_event *iev)
{
	/* check for basic keyboard keys */
	if (iev->code >= KEY_ESC && iev->code <= KEY_COMPOSE)
	{
		/* if we have a value is a down */
		if (iev->value)
		{
			Ecore_Li_Event_Key_Down *ev;

			ev = _key_down_get(dev, iev->code);
			if (!ev) return;
			
			ecore_event_add(ECORE_LI_EVENT_KEY_DOWN, ev, _key_free, NULL);

		}
		/* else is an up */
		else
		{
			Ecore_Li_Event_Key_Up *ev;

			ev = _key_up_get(dev, iev->code);
			if (!ev) return;
			
			ecore_event_add(ECORE_LI_EVENT_KEY_UP, ev, _key_free, NULL);
		}
	}
	/* check for mouse button events */
	else if (iev->code >= BTN_MOUSE && iev->code < BTN_JOYSTICK)
	{
		int button;

		button = ((iev->code & 0x00F) + 1);
		if (iev->value)
		{
			Ecore_Li_Event_Button_Down *ev;
			double current;

			ev = calloc(1, sizeof(Ecore_Li_Event_Button_Down));
			ev->dev = dev;
			ev->button = button;
			ev->x = dev->mouse.x;
			ev->y = dev->mouse.y;

			current = ecore_time_get();
			if((current - dev->mouse.prev) <= dev->mouse.threshold)
			{
				ev->double_click = 1;
			}
			if((current - dev->mouse.last) <= (2 * dev->mouse.threshold))
			{
				ev->triple_click = 1;
				/* reset */
				dev->mouse.prev = 0;
				dev->mouse.last = 0;
				current = 0;
			}
			else
			{
				/* update values */
				dev->mouse.last = dev->mouse.prev;
				dev->mouse.prev = current;
			}
			ecore_event_add(ECORE_LI_EVENT_BUTTON_DOWN, ev, NULL ,NULL);
		}
		else
		{
			Ecore_Li_Event_Button_Up *ev;

			ev = calloc(1,sizeof(Ecore_Li_Event_Button_Up));
			ev->dev = dev;
			ev->button = button;
			ev->x = dev->mouse.x;
			ev->y = dev->mouse.y;

			ecore_event_add(ECORE_LI_EVENT_BUTTON_UP, ev, NULL ,NULL);
		}
	}
}
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
int
device_fd_callback(void *data, Ecore_Fd_Handler *fdh)
{
	Ecore_Li_Device *dev;
	struct input_event ev[64];
	int len;
	int i;

	dev = (Ecore_Li_Device*)data;
	/* read up to 64 events at once */
	len = read(dev->fd, &ev, sizeof(ev));
	// printf("[ecore_li_li_device:fd_callback] received %d data\n", len);
	for (i = 0; i < len/sizeof(ev[0]); i++)
	{
		switch (ev[i].type)
		{
			case EV_ABS:
				_event_abs(dev, &ev[i]);
				break;
			case EV_REL:
				_event_rel(dev, &ev[i]);
				break;
			case EV_KEY:
				_event_key(dev, &ev[i]);
				break;
			default:
				break;
		}
	}
	return 1;
}

#include <ctype.h>

#include "enna.h"
#include "event_key.h"

typedef struct _Input_Module_Item Input_Module_Item;
struct _Input_Module_Item
{
    Enna_Module *module;
    Enna_Class_Input *class;
};

static Eina_List *_input_modules;

static const struct
{
    const char *keyname;
    const char *modifier;
    enna_key_t keycode;
} enna_keymap[] = {
    { "Left",         0,              ENNA_KEY_LEFT          },
    { "Right",        0,              ENNA_KEY_RIGHT         },
    { "Up",           0,              ENNA_KEY_UP            },
    { "KP_Up",        0,              ENNA_KEY_UP            },
    { "Down",         0,              ENNA_KEY_DOWN          },
    { "KP_Down",      0,              ENNA_KEY_DOWN          },
    { "Home",         0,              ENNA_KEY_HOME          },
    { "KP_Home",      0,              ENNA_KEY_HOME          },
    { "End",          0,              ENNA_KEY_END           },
    { "KP_End",       0,              ENNA_KEY_END           },
    { "Prior",        0,              ENNA_KEY_PAGE_UP       },
    { "Next",         0,              ENNA_KEY_PAGE_DOWN     },
    { "Return",       0,              ENNA_KEY_OK            },
    { "KP_Enter",     0,              ENNA_KEY_OK            },
    { "BackSpace",    0,              ENNA_KEY_CANCEL        },
    { "space",        0,              ENNA_KEY_SPACE         },
    { "Escape",       0,              ENNA_KEY_QUIT          },
    { "Super_L",      0,              ENNA_KEY_MENU          },
    { "0",            0,              ENNA_KEY_0             },
    { "KP_0",         0,              ENNA_KEY_0             },
    { "1",            0,              ENNA_KEY_1             },
    { "KP_1",         0,              ENNA_KEY_1             },
    { "2",            0,              ENNA_KEY_2             },
    { "KP_2",         0,              ENNA_KEY_2             },
    { "3",            0,              ENNA_KEY_3             },
    { "KP_3",         0,              ENNA_KEY_3             },
    { "4",            0,              ENNA_KEY_4             },
    { "KP_4",         0,              ENNA_KEY_4             },
    { "5",            0,              ENNA_KEY_5             },
    { "KP_5",         0,              ENNA_KEY_5             },
    { "6",            0,              ENNA_KEY_6             },
    { "KP_6",         0,              ENNA_KEY_6             },
    { "7",            0,              ENNA_KEY_7             },
    { "KP_7",         0,              ENNA_KEY_7             },
    { "8",            0,              ENNA_KEY_8             },
    { "KP_8",         0,              ENNA_KEY_8             },
    { "9",            0,              ENNA_KEY_9             },
    { "KP_9",         0,              ENNA_KEY_9             },
    { "a",            0,              ENNA_KEY_A             },
    { "b",            0,              ENNA_KEY_B             },
    { "c",            0,              ENNA_KEY_C             },
    { "d",            0,              ENNA_KEY_D             },
    { "e",            0,              ENNA_KEY_E             },
    { "f",            "Control",      ENNA_KEY_FULLSCREEN    },
    { "f",            0,              ENNA_KEY_F             },
    { "g",            0,              ENNA_KEY_G             },
    { "h",            0,              ENNA_KEY_H             },
    { "i",            0,              ENNA_KEY_I             },
    { "j",            0,              ENNA_KEY_J             },
    { "k",            0,              ENNA_KEY_K             },
    { "l",            0,              ENNA_KEY_L             },
    { "m",            0,              ENNA_KEY_M             },
    { "n",            0,              ENNA_KEY_N             },
    { "o",            0,              ENNA_KEY_O             },
    { "p",            0,              ENNA_KEY_P             },
    { "q",            0,              ENNA_KEY_Q             },
    { "r",            0,              ENNA_KEY_R             },
    { "s",            0,              ENNA_KEY_S             },
    { "t",            0,              ENNA_KEY_T             },
    { "u",            0,              ENNA_KEY_U             },
    { "v",            0,              ENNA_KEY_V             },
    { "w",            0,              ENNA_KEY_W             },
    { "x",            0,              ENNA_KEY_X             },
    { "y",            0,              ENNA_KEY_Y             },
    { "z",            0,              ENNA_KEY_Z             },
    { NULL,           0,             ENNA_KEY_UNKNOWN       }
};

/* Static functions */

static void _event_cb(void *data, char *event)
{

    if (!event)
        return;

    evas_event_feed_key_down(enna->evas, event, event, event, NULL, ecore_time_get(), data);
    enna_log(ENNA_MSG_EVENT, NULL, "LIRC event : %s", event);


}

/* Public Functions */

enna_key_t
enna_get_key (void *event)
{
    int i;
    Evas_Event_Key_Down *ev;

    ev = event;

    if (!ev)
    return ENNA_KEY_UNKNOWN;

    for (i = 0; enna_keymap[i].keyname; i++)

    {
	/* Test First if modifer is set and is different than "None"*/
	if (enna_keymap[i].modifier
	    && evas_key_modifier_is_set (ev->modifiers, enna_keymap[i].modifier)
	    && !strcmp (enna_keymap[i].keyname, ev->key) )
	{
	    enna_log (ENNA_MSG_EVENT, NULL, "Key pressed : [%s] + %s",
		enna_keymap[i].modifier,
		enna_keymap[i] );
	    return enna_keymap[i].keycode;
	}
	/* Else just test if keyname match */
	else if (!enna_keymap[i].modifier && !strcmp (enna_keymap[i].keyname, ev->key))
	{
	    enna_log (ENNA_MSG_EVENT, NULL, "Key pressed : %s",
		enna_keymap[i] );
	    return enna_keymap[i].keycode;
	}
    }

    return ENNA_KEY_UNKNOWN;
}

int enna_key_is_alpha(enna_key_t key)
{
    return (key >= ENNA_KEY_A && key <= ENNA_KEY_Z);
}

char enna_key_get_alpha(enna_key_t key)
{
    int i;

    for (i = 0; enna_keymap[i].keyname; i++)
        if (enna_keymap[i].keycode == key)
            return enna_keymap[i].keyname[0];

    return ' ';
}

void enna_input_init()
{
#ifdef BUILD_INPUT_LIRC
    Enna_Module *em;
    Input_Module_Item *item;
#endif

    /* Create Input event "Key Down" */
    ENNA_EVENT_INPUT_KEY_DOWN = ecore_event_type_new();

    _input_modules = NULL;

#ifdef BUILD_INPUT_LIRC
    em = enna_module_open("lirc", ENNA_MODULE_INPUT, enna->evas);
    item = calloc(1, sizeof(Input_Module_Item));
    item->module = em;
    _input_modules = eina_list_append(_input_modules, item);
    enna_module_enable(em);
#endif

}

void enna_input_shutdown()
{
    Eina_List *l = NULL;

    for (l = _input_modules; l; l = l->next)
    {
        Input_Module_Item *item = l->data;
        item->class->func.class_shutdown(0);
        enna_module_disable(item->module);
    }
    eina_list_free(_input_modules);
}

int enna_input_class_register(Enna_Module *module, Enna_Class_Input *class)
{
    Eina_List *l = NULL;

    for (l = _input_modules; l; l = l->next)
    {
        Input_Module_Item *item = l->data;
        if (module == item->module)
        {
            item->class = class;
            class->func.class_init(0);
            class->func.class_event_cb_set(_event_cb, item);

            return 0;
        }
    }
    return -1;
}

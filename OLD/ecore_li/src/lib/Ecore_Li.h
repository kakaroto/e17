#ifndef _ECORE_LI_H
#define _ECORE_LI_H

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

/**
 * @file
 * @brief
 *
 */

extern int ECORE_LI_EVENT_KEY_DOWN;
extern int ECORE_LI_EVENT_KEY_UP;
extern int ECORE_LI_EVENT_BUTTON_UP;
extern int ECORE_LI_EVENT_BUTTON_DOWN;
extern int ECORE_LI_EVENT_MOUSE_MOVE;
extern int ECORE_LI_EVENT_MOUSE_WHEEL;

typedef struct _Ecore_Li_Device Ecore_Li_Device;
enum
{
	ECORE_LI_DEVICE_CAP_NONE            = 0x00000000,
	ECORE_LI_DEVICE_CAP_RELATIVE        = 0x00000001,
	ECORE_LI_DEVICE_CAP_ABSOLUTE        = 0x00000002,
	ECORE_LI_DEVICE_CAP_KEYS_OR_BUTTONS = 0x00000004
};
typedef struct _Ecore_Li_Event_Key               Ecore_Li_Event_Key_Down; /**< Key Down event */
typedef struct _Ecore_Li_Event_Key               Ecore_Li_Event_Key_Up; /**< Key Up event */
typedef struct _Ecore_Li_Event_Button_Down Ecore_Li_Event_Button_Down; /**< Mouse Down event */
typedef struct _Ecore_Li_Event_Button_Up   Ecore_Li_Event_Button_Up; /**< Mouse Up event */
typedef struct _Ecore_Li_Event_Mouse_Move        Ecore_Li_Event_Mouse_Move; /**< Mouse Move event */
typedef struct _Ecore_Li_Event_Mouse_Wheel       Ecore_Li_Event_Mouse_Wheel; /**< Mouse Wheel event */

struct _Ecore_Li_Event_Key /** Key event */
{
	Ecore_Li_Device *dev; /**< The device associated with the event */
	char   *keyname; /**< The name of the key that was pressed */
	char   *keysymbol; /**< The logical symbol of the key that was pressed */
	char   *key_compose; /**< The UTF-8 string conversion if any */
};
   
struct _Ecore_Li_Event_Mouse_Wheel /** Mouse Wheel event */
{
	Ecore_Li_Device *dev; /**< The device associated with the event */
	int x; /**< */
	int y; /**< */
	int direction; /**< 0 = vertical, 1 = horizontal */
	int wheel; /**< value 1 (left/up), -1 (right/down) */
};

struct _Ecore_Li_Event_Mouse_Move /** Mouse Move event */
{
	Ecore_Li_Device *dev; /**< The device associated with the event */
	int     x; /**< Mouse co-ordinates where the mouse cursor moved to */
	int     y; /**< Mouse co-ordinates where the mouse cursor moved to */
};

struct _Ecore_Li_Event_Button_Down /** Mouse Down event */
{
	Ecore_Li_Device *dev; /**< The device associated with the event */
	int     button; /**< Mouse button that was pressed (1 - 32) */
	int     x; /**< Mouse co-ordinates when mouse button was pressed */
	int     y; /**< Mouse co-ordinates when mouse button was pressed */
	int     double_click : 1; /**< Set if click was a double click */
	int     triple_click : 1; /**< Set if click was a triple click  */
};
   
struct _Ecore_Li_Event_Button_Up /** Mouse Up event */
{
	Ecore_Li_Device *dev; /**< The device associated with the event */
	int     button; /**< Mouse button that was released (1 - 32) */
	int     x; /**< Mouse co-ordinates when mouse button was raised */
	int     y; /**< Mouse co-ordinates when mouse button was raised */
};

enum
{
	ECORE_LI_MOD_PLAIN,
	ECORE_LI_MOD_SHIFT,
	ECORE_LI_MOD_ALTGR,
	ECORE_LI_MOD_CONTROL,
	ECORE_LI_MOD_ALT,
	ECORE_LI_MOD_SHIFTL,
	ECORE_LI_MOD_SHIFTR,
	ECORE_LI_MOD_CONTROLL,
	ECORE_LI_MOD_CONTROLR,
	ECORE_LI_MODS
};


typedef struct _Ecore_Li_Keyboard_Keycode Ecore_Li_Keyboard_Keycode;
struct _Ecore_Li_Keyboard_Keycode /** Keyboard Keycode */
{
	int code; 	 	/**< the keycode number (i.e 100)  FIXME is it neccessary? */
	struct
	{
		char *name; 	/* the key name (i.e Alt_Gr) */
		int  value;	/* FIXME the key value on utf8 or unicode ?*/
		char letter;	/* is it a letter (i.e caps lock sensitive) */
	} mod[256]; 		/* 8 possible modifiers (2^8) */
};

typedef struct _Ecore_Li_Keyboard_Compose Ecore_Li_Keyboard_Compose;
struct _Ecore_Li_Keyboard_Compose
{
	char sequence[3];
};

typedef struct _Ecore_Li_Keyboard_Layout Ecore_Li_Keyboard_Layout;
struct _Ecore_Li_Keyboard_Layout
{
	Ecore_Li_Keyboard_Compose	*compose; 	/* composed chars, mainly for accents */
	int				compose_count; 	/* compose number of elements */
	Ecore_Li_Keyboard_Keycode 	codes[256];	/* 256 keycodes at most*/
};

EAPI int ecore_li_init(void);
EAPI int ecore_li_shutdown(void);

EAPI Ecore_Li_Device *ecore_li_device_new(const char *path);
EAPI void ecore_li_device_delete(Ecore_Li_Device *d);

EAPI void ecore_li_device_enable(Ecore_Li_Device *dev, int enable);
EAPI const char *ecore_li_device_name_get(Ecore_Li_Device *dev);
EAPI void ecore_li_device_axis_size_set(Ecore_Li_Device *dev, int w, int h);
EAPI void ecore_li_device_keyboard_layout_set(Ecore_Li_Device *dev, Ecore_Li_Keyboard_Layout *l);

#endif

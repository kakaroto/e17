#ifndef __EWL_EVENTS_H__
#define __EWL_EVENTS_H__

/**
 * @defgroup Ewl_Events Events: Lower Level Event Handlers
 * @brief Defines the routines that dispatch the lower level events to EWL.
 *
 * @{
 */

typedef struct Ewl_Event_Window_Expose Ewl_Event_Window_Expose;

struct Ewl_Event_Window_Expose
{
	int x, y, w, h;
};

typedef struct Ewl_Event_Window_Configure Ewl_Event_Window_Configure;

struct Ewl_Event_Window_Configure
{
	int x, y, w, h;
};

typedef struct Ewl_Event_Window_Delete Ewl_Event_Window_Delete;

struct Ewl_Event_Window_Delete
{
};

typedef struct Ewl_Event_Key_Down Ewl_Event_Key_Down;

struct Ewl_Event_Key_Down
{
	unsigned int modifiers;
	char *keyname;
};

typedef struct Ewl_Event_Key_Up Ewl_Event_Key_Up;

struct Ewl_Event_Key_Up
{
	unsigned int modifiers;
	char *keyname;
};

typedef struct Ewl_Event_Mouse_Down Ewl_Event_Mouse_Down;

struct Ewl_Event_Mouse_Down
{
	unsigned int modifiers;
	int button;
	int x, y;
};

typedef struct Ewl_Event_Mouse_Up Ewl_Event_Mouse_Up;

struct Ewl_Event_Mouse_Up
{
	unsigned int modifiers;
	int button;
	int x, y;
};

typedef struct Ewl_Event_Mouse_Move Ewl_Event_Mouse_Move;

struct Ewl_Event_Mouse_Move
{
	unsigned int modifiers;
	int x, y;
};

typedef struct Ewl_Event_Mouse_In Ewl_Event_Mouse_In;

struct Ewl_Event_Mouse_In
{
	unsigned int modifiers;
	int x, y;
};

typedef struct Ewl_Event_Mouse_Out Ewl_Event_Mouse_Out;

struct Ewl_Event_Mouse_Out
{
	unsigned int modifiers;
	int x, y;
};

int ewl_ev_init(void);
unsigned int ewl_ev_get_modifiers();

/**
 * @}
 */

#endif				/* __EWL_EVENTS_H__ */

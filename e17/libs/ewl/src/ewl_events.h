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
};

typedef struct Ewl_Event_Key_Up Ewl_Event_Key_Up;

struct Ewl_Event_Key_Up
{
};

typedef struct Ewl_Event_Mouse_Down Ewl_Event_Mouse_Down;

struct Ewl_Event_Mouse_Down
{
};

typedef struct Ewl_Event_Mouse_Up Ewl_Event_Mouse_Up;

struct Ewl_Event_Mouse_Up
{
};

typedef struct Ewl_Event_Mouse_Move Ewl_Event_Mouse_Move;

struct Ewl_Event_Mouse_Move
{
};

typedef struct Ewl_Event_Mouse_Out Ewl_Event_Mouse_Out;

struct Ewl_Event_Mouse_Out
{
};

int ewl_ev_init(void);

int ewl_ev_window_expose(void *data, int type, void *_ev);
int ewl_ev_window_configure(void *data, int type, void *_ev);
int ewl_ev_window_delete(void *data, int type, void *_ev);

int ewl_ev_key_down(void *data, int type, void *_ev);
int ewl_ev_key_up(void *data, int type, void *_ev);
int ewl_ev_mouse_down(void *data, int type, void *_ev);
int ewl_ev_mouse_up(void *data, int type, void *_ev);
int ewl_ev_mouse_move(void *data, int type, void *_ev);
int ewl_ev_mouse_out(void *data, int type, void *_ev);
int ewl_ev_paste(void *data, int type, void *_ev);

/**
 * @}
 */

#endif				/* __EWL_EVENTS_H__ */

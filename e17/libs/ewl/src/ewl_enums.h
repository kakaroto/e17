
/*\
|*|
|*| Keeps track of most of enums in EWL
|*|
\*/


#ifndef __EWL_ENUMS_H__
#define __EWL_ENUMS_H__

/*
 * This defines the various types of callbacks that can be hooked up for each
 * widget.
 */
typedef enum _ewl_callback_type Ewl_Callback_Type;

enum _ewl_callback_type {
	EWL_CALLBACK_REALIZE,
	EWL_CALLBACK_SHOW,
	EWL_CALLBACK_HIDE,
	EWL_CALLBACK_DESTROY,
	EWL_CALLBACK_DESTROY_RECURSIVE,
	EWL_CALLBACK_DELETE_WINDOW,
	EWL_CALLBACK_CONFIGURE,
	EWL_CALLBACK_REPARENT,
	EWL_CALLBACK_KEY_DOWN,
	EWL_CALLBACK_KEY_UP,
	EWL_CALLBACK_MOUSE_DOWN,
	EWL_CALLBACK_MOUSE_UP,
	EWL_CALLBACK_MOUSE_MOVE,
	EWL_CALLBACK_FOCUS_IN,
	EWL_CALLBACK_FOCUS_OUT,
	EWL_CALLBACK_SELECT,
	EWL_CALLBACK_UNSELECT,
	EWL_CALLBACK_CLICKED,
	EWL_CALLBACK_HILITED,
	EWL_CALLBACK_VALUE_CHANGED,
	EWL_CALLBACK_THEME_UPDATE,
	EWL_CALLBACK_STATE_CHANGED,
	EWL_CALLBACK_MAX
};

/*
 * The orientation enum is used in a few widgets to specify whether the widget
 * should be laid out in a horizontal or vertical fashion.
 */
typedef enum _ewl_orientation Ewl_Orientation;

enum _ewl_orientation {
	EWL_ORIENTATION_HORIZONTAL,
	EWL_ORIENTATION_VERTICAL
};

/*
 * The state enum specifies the current state of a widget, ie. has it been
 * clicked, does it have the keyboard focus, etc.
 */
typedef enum _ewl_state Ewl_State;

enum _ewl_state {
	EWL_STATE_HILITED = 1,
	EWL_STATE_PRESSED = 2,
	EWL_STATE_SELECTED = 4,
	EWL_STATE_DND = 8
};


/*
 * The alignment enumeration allows for specifying how an element is aligned
 * within it's container.
 */
typedef enum _ewl_alignment Ewl_Alignment;

enum _ewl_alignment {
	EWL_ALIGNMENT_CENTER = 0x0,
	EWL_ALIGNMENT_LEFT = 0x1,
	EWL_ALIGNMENT_RIGHT = 0x2,
	EWL_ALIGNMENT_TOP = 0x10,
	EWL_ALIGNMENT_BOTTOM = 0x20
};

typedef enum _ewl_fill_policy Ewl_Fill_Policy;

enum _ewl_fill_policy {
	EWL_FILL_POLICY_NORMAL,
	EWL_FILL_POLICY_FILL
};

typedef enum _ewl_position Ewl_Position;

enum _ewl_position {
	EWL_POSITION_TOP,
	EWL_POSITION_LEFT,
	EWL_POSITION_RIGHT,
	EWL_POSITION_BOTTOM
};

#endif				/* __EWL_ENUMS_H__ */


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

enum _ewl_callback_type
{
	EWL_CALLBACK_EXPOSE,
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
	EWL_CALLBACK_DESELECT,
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

enum _ewl_orientation
{
	EWL_ORIENTATION_HORIZONTAL,
	EWL_ORIENTATION_VERTICAL
};

/*
 * The state enum specifies the current state of a widget, ie. has it been
 * clicked, does it have the keyboard focus, etc.
 */
typedef enum _ewl_state Ewl_State;

enum _ewl_state
{
	EWL_STATE_NORMAL = 1 << 0,
	EWL_STATE_HILITED = 1 << 1,
	EWL_STATE_PRESSED = 1 << 2,
	EWL_STATE_SELECTED = 1 << 3,
	EWL_STATE_DND = 1 << 4
};


/*
 * The alignment enumeration allows for specifying how an element is aligned
 * within it's container.
 */
typedef enum _ewl_alignment Ewl_Alignment;

enum _ewl_alignment
{
	EWL_ALIGNMENT_CENTER = 0x0,
	EWL_ALIGNMENT_LEFT = 0x1,
	EWL_ALIGNMENT_RIGHT = 0x2,
	EWL_ALIGNMENT_TOP = 0x10,
	EWL_ALIGNMENT_BOTTOM = 0x20
};

/*
 * Fill policy identifies to containers whether child widgets should be
 * stretched to fill available space or keep their current size.
 */
typedef enum _ewl_fill_policy Ewl_Fill_Policy;

enum _ewl_fill_policy
{
	EWL_FILL_POLICY_NORMAL,
	EWL_FILL_POLICY_FILL
};

/*
 * Visibility identifies whether a widget is shown and has been realized
 */
typedef enum _ewl_visibility Ewl_Visibility;

enum _ewl_visibility
{
	EWL_VISIBILITY_HIDDEN = 0x0,
	EWL_VISIBILITY_SHOWN = 0x1,
	EWL_VISIBILITY_REALIZED = 0x2
};

#endif /* __EWL_ENUMS_H__ */

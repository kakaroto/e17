

#ifndef __EWL_ENUMS_H__
#define __EWL_ENUMS_H__

typedef enum _ewl_widget_type Ewl_Widget_Type;

enum _ewl_widget_type
{
	EWL_WIDGET_TYPE_UNKNOWN,
	EWL_WIDGET_TYPE_BOX,
	EWL_WIDGET_TYPE_BUTTON,
	EWL_WIDGET_TYPE_CHECKBUTTON,
	EWL_WIDGET_TYPE_CURSOR,
	EWL_WIDGET_TYPE_ENTRY,
	EWL_WIDGET_TYPE_IMAGE,
	EWL_WIDGET_TYPE_LIST,
	EWL_WIDGET_TYPE_NOTEBOOK,
	EWL_WIDGET_TYPE_RADIOBUTTON,
	EWL_WIDGET_TYPE_SCROLLBAR,
	EWL_WIDGET_TYPE_SEEKER,
	EWL_WIDGET_TYPE_SELECTION,
	EWL_WIDGET_TYPE_SEPARATOR,
	EWL_WIDGET_TYPE_SPINNER,
	EWL_WIDGET_TYPE_TABLE,
	EWL_WIDGET_TYPE_TEXT,
	EWL_WIDGET_TYPE_WINDOW,
	EWL_WIDGET_TYPE_MAX
};

/*
 * This defines the various types of callbacks that can be hooked up for each
 * widget.
 */
typedef enum _ewl_callback_type Ewl_Callback_Type;

enum _ewl_callback_type
{
	EWL_CALLBACK_NONE,
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
	EWL_STATE_HILITED = 0x1,
	EWL_STATE_PRESSED = 0x2,
	EWL_STATE_SELECTED = 0x10,
	EWL_STATE_DND = 0x20
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


typedef enum _ewl_position Ewl_Position;

enum _ewl_position
{
	EWL_POSITION_LEFT,
	EWL_POSITION_RIGHT,
	EWL_POSITION_TOP,
	EWL_POSITION_BOTTOM
};

typedef enum _ewl_notebook_flags Ewl_Notebook_Flags;

enum _ewl_notebook_flags
{
	EWL_NOTEBOOK_FLAG_TABS_HIDDEN = 0x1
};

typedef enum _ewl_fx_modifies Ewl_FX_Modifies;

enum _ewl_fx_modifies
{
	EWL_FX_MODIFIES_NONE = (0x1 << 0),
	EWL_FX_MODIFIES_ALPHA_CHANNEL = (0x1 << 1),
	EWL_FX_MODIFIES_RED_CHANNEL = (0x1 << 2),
	EWL_FX_MODIFIES_GREEN_CHANNEL = (0x1 << 3),
	EWL_FX_MODIFIES_BLUE_CHANNEL = (0x1 << 4),
	EWL_FX_MODIFIES_ALL_CHANNELS = (0x1 << 5),
	EWL_FX_MODIFIES_POSITION = (0x1 << 6),
	EWL_FX_MODIFIES_SIZE = (0x1 << 7),
	EWL_FX_MODIFIES_GEOMETRY = (0x1 << 8)
};


#endif /* __EWL_ENUMS_H__ */

#ifndef __EWL_ENUMS_H__
#define __EWL_ENUMS_H__

/**
 * @defgroup Ewl_Enums Various Flags and Enumerations used in EWL
 * Provides bitmasks, flags, and other enumerations for use by widgets in EWL.
 */

/**
 * This defines the various types of callbacks that can be hooked up for each
 * widget.
 */
typedef enum Ewl_Callback_Type Ewl_Callback_Type;

enum Ewl_Callback_Type
{
	EWL_CALLBACK_NONE, /**< A placeholder FIXME: Is this necessary still? */
	EWL_CALLBACK_EXPOSE, /**< Triggered when the window needs redrawing */
	EWL_CALLBACK_REALIZE, /**< Event when a widget is first drawn */
	EWL_CALLBACK_UNREALIZE, /**< When a widget is no longer drawn */
	EWL_CALLBACK_SHOW, /**< A widget has been marked visible */
	EWL_CALLBACK_HIDE, /**< A widget is marked hidden */
	EWL_CALLBACK_DESTROY, /**< The widget is freed */
	EWL_CALLBACK_DELETE_WINDOW, /**< The window is being closed */
	EWL_CALLBACK_CONFIGURE, /**< The object is being resized */
	EWL_CALLBACK_REPARENT, /**< A widget has been placed in a container */
	EWL_CALLBACK_KEY_DOWN, /**< A key was pressed down */
	EWL_CALLBACK_KEY_UP, /**< A key was released */
	EWL_CALLBACK_MOUSE_DOWN, /**< Mouse was pressed down */
	EWL_CALLBACK_MOUSE_UP, /**< Mouse was released */
	EWL_CALLBACK_MOUSE_MOVE, /**< Mouse was moved */
	EWL_CALLBACK_FOCUS_IN, /**< Mouse was placed over the widget */
	EWL_CALLBACK_FOCUS_OUT, /**< Mouse was moved away from the widget */
	EWL_CALLBACK_SELECT, /**< Widget was selected by mouse or key */
	EWL_CALLBACK_DESELECT, /**< Widget was deselected by mouse or key */
	EWL_CALLBACK_CLICKED, /**< Mouse was pressed and released on a widget */
	EWL_CALLBACK_DOUBLE_CLICKED, /**< Mouse was clicked twice quickly */
	EWL_CALLBACK_HILITED, /**< Mouse is over the widget */
	EWL_CALLBACK_VALUE_CHANGED, /**< Value in widget changed */
	EWL_CALLBACK_STATE_CHANGED, /**< Alter the state of the appearance */
	EWL_CALLBACK_APPEARANCE_CHANGED, /**< Theme key of widget changed */
	EWL_CALLBACK_WIDGET_ENABLE, /**< Widget has been re-enabled */
	EWL_CALLBACK_WIDGET_DISABLE, /**< Widget no longer takes input */
	EWL_CALLBACK_MAX /**< Flag to indicate last value */
};

/**
 * Flags for the callbacks to indicate interception or notification of the
 * parent.
 */
typedef enum Ewl_Event_Notify Ewl_Event_Notify;

enum Ewl_Event_Notify
{
	EWL_CALLBACK_NOTIFY_NONE = 0,
	EWL_CALLBACK_NOTIFY_NOTIFY = 1,
	EWL_CALLBACK_NOTIFY_INTERCEPT = 2
};

/**
 * The orientation enum is used in a few widgets to specify whether the widget
 * should be laid out in a horizontal or vertical fashion.
 */
typedef enum Ewl_Orientation Ewl_Orientation;

enum Ewl_Orientation
{
	EWL_ORIENTATION_HORIZONTAL,
	EWL_ORIENTATION_VERTICAL
};

/**
 * The state enum specifies the current state of a widget, ie. has it been
 * clicked, does it have the keyboard focus, etc.
 */
typedef enum Ewl_State Ewl_State;

enum Ewl_State
{
	EWL_STATE_NORMAL = (0x1 << 0),
	EWL_STATE_HILITED = (0x1 << 1),
	EWL_STATE_PRESSED = (0x1 << 2),
	EWL_STATE_SELECTED = (0x1 << 3),
	EWL_STATE_DND = (0x1 << 4),
	EWL_STATE_DISABLED = (0x1 << 5)
};


/**
 * The alignment enumeration allows for specifying how an element is aligned
 * within it's container.
 */
typedef enum Ewl_Alignment Ewl_Alignment;
#define EWL_ALIGNMENT_MASK 0xFF

enum Ewl_Alignment
{
	EWL_ALIGNMENT_CENTER = ETOX_ALIGN_CENTER,
	EWL_ALIGNMENT_LEFT = ETOX_ALIGN_LEFT,
	EWL_ALIGNMENT_RIGHT = ETOX_ALIGN_RIGHT,
	EWL_ALIGNMENT_TOP = ETOX_ALIGN_TOP,
	EWL_ALIGNMENT_BOTTOM = ETOX_ALIGN_BOTTOM
};

/**
 * Fill policy identifies to containers whether child widgets should be
 * stretched to fill available space or keep their current size.
 */
typedef enum Ewl_Fill_Policy Ewl_Fill_Policy;
#define EWL_FILL_POLICY_MASK 0xFF00

enum Ewl_Fill_Policy
{
	EWL_FILL_POLICY_NONE = 0,
	EWL_FILL_POLICY_HSHRINK = 0x1000,
	EWL_FILL_POLICY_VSHRINK = 0x2000,
	EWL_FILL_POLICY_SHRINK =
	    EWL_FILL_POLICY_HSHRINK | EWL_FILL_POLICY_VSHRINK,
	EWL_FILL_POLICY_HFILL = 0x4000,
	EWL_FILL_POLICY_VFILL = 0x8000,
	EWL_FILL_POLICY_FILL = EWL_FILL_POLICY_HFILL | EWL_FILL_POLICY_VFILL,
	EWL_FILL_POLICY_ALL = EWL_FILL_POLICY_FILL | EWL_FILL_POLICY_SHRINK
};

#define EWL_FILL_POLICY_NORMAL (EWL_FILL_POLICY_FILL)

/**
 * Flags identifying whether a widget is shown, has been realized, or is
 * recursive (ie. a container).
 */
typedef enum Ewl_Widget_Flags Ewl_Widget_Flags;

enum Ewl_Widget_Flags
{
	EWL_FLAGS_HIDDEN = 0x0,
	EWL_FLAGS_SHOWN = 0x1,
	EWL_FLAGS_REALIZED = 0x2,
	EWL_FLAGS_OBSCURED = 0x4,
	EWL_FLAGS_RECURSIVE = 0x8,
	EWL_FLAGS_TOPLEVEL = 0x10,
	EWL_FLAGS_CSCHEDULED = 0x20,
	EWL_FLAGS_RSCHEDULED = 0x40,
	EWL_FLAGS_DSCHEDULED = 0x80,
};


typedef enum Ewl_Position Ewl_Position;

enum Ewl_Position
{
	EWL_POSITION_LEFT = 0x1,
	EWL_POSITION_RIGHT = 0x2,
	EWL_POSITION_TOP = 0x4,
	EWL_POSITION_BOTTOM = 0x8
};

#define EWL_POSITION_MASK (0xf)

typedef enum Ewl_Window_Flags Ewl_Window_Flags;

enum Ewl_Window_Flags
{
	EWL_WINDOW_AUTO_SIZE = 1,
	EWL_WINDOW_BORDERLESS = 2
};

typedef enum Ewl_Tree_Node_Flags Ewl_Tree_Node_Flags;

enum Ewl_Tree_Node_Flags
{
	EWL_TREE_NODE_NOEXPAND = 0,
	EWL_TREE_NODE_COLLAPSED = 1,
	EWL_TREE_NODE_EXPANDED = 2
};

typedef enum Ewl_Notebook_Flags Ewl_Notebook_Flags;

enum Ewl_Notebook_Flags
{
	EWL_NOTEBOOK_FLAG_TABS_HIDDEN = 0x10
};

typedef enum Ewl_Scrollbar_Flags Ewl_ScrollBar_Flags;

enum Ewl_Scrollbar_Flags
{
	EWL_SCROLLBAR_FLAG_NONE,
	EWL_SCROLLBAR_FLAG_AUTO_VISIBLE,
	EWL_SCROLLBAR_FLAG_ALWAYS_HIDDEN
};


typedef enum Ewl_Filedialog_Type Ewl_Filedialog_Type;

enum Ewl_Filedialog_Type
{
	EWL_FILEDIALOG_TYPE_OPEN,
	EWL_FILEDIALOG_TYPE_CLOSE
};

#endif				/* __EWL_ENUMS_H__ */

#ifndef __EWL_ENUMS_H__
#define __EWL_ENUMS_H__

/**
 * @file ewl_enums.h
 * @defgroup Ewl_Enums Enums: Various Flags and Enumerations used in EWL
 * Provides bitmasks, flags, and other enumerations for use by widgets in EWL.
 *
 * @{
 */

/**
 * @enum Ewl_Callback_Type
 * This defines the various types of callbacks that can be hooked up for each
 * widget.
 */
enum Ewl_Callback_Type
{
	EWL_CALLBACK_EXPOSE, /**< Triggered when the window needs redrawing */
	EWL_CALLBACK_REALIZE, /**< Event when a widget is first drawn */
	EWL_CALLBACK_UNREALIZE, /**< When a widget is no longer drawn */
	EWL_CALLBACK_SHOW, /**< A widget has been marked visible */
	EWL_CALLBACK_HIDE, /**< A widget is marked hidden */
        EWL_CALLBACK_OBSCURE,        /**< Widget is offscreen */
        EWL_CALLBACK_REVEAL,         /**< Widget returned to screen */
	EWL_CALLBACK_DESTROY, /**< The widget is freed */
	EWL_CALLBACK_DELETE_WINDOW, /**< The window is being closed */
	EWL_CALLBACK_CONFIGURE, /**< The object is being resized */
	EWL_CALLBACK_REPARENT, /**< A widget has been placed in a container */
	EWL_CALLBACK_KEY_DOWN, /**< A key was pressed down */
	EWL_CALLBACK_KEY_UP, /**< A key was released */
	EWL_CALLBACK_MOUSE_DOWN, /**< Mouse was pressed down */
	EWL_CALLBACK_MOUSE_UP, /**< Mouse was released */
	EWL_CALLBACK_MOUSE_MOVE, /**< Mouse was moved */
	EWL_CALLBACK_MOUSE_WHEEL, /**< Mouse wheel scrolled */
	EWL_CALLBACK_FOCUS_IN, /**< Mouse was placed over the widget */
	EWL_CALLBACK_FOCUS_OUT, /**< Mouse was moved away from the widget */
	EWL_CALLBACK_SELECT, /**< Widget was selected by mouse or key */
	EWL_CALLBACK_DESELECT, /**< Widget was deselected by mouse or key */
	EWL_CALLBACK_CLICKED, /**< Mouse was pressed and released on a widget */
	EWL_CALLBACK_HILITED, /**< Mouse is over the widget */
	EWL_CALLBACK_VALUE_CHANGED, /**< Value in widget changed */
	EWL_CALLBACK_STATE_CHANGED, /**< Alter the state of the appearance */
	EWL_CALLBACK_APPEARANCE_CHANGED, /**< Theme key of widget changed */
	EWL_CALLBACK_WIDGET_ENABLE, /**< Widget has been re-enabled */
	EWL_CALLBACK_WIDGET_DISABLE, /**< Widget no longer takes input */
	EWL_CALLBACK_PASTE, /**< Data is ready to be pasted */
	EWL_CALLBACK_MAX /**< Flag to indicate last value */
};

typedef enum Ewl_Callback_Type Ewl_Callback_Type;

/**
 * @enum Ewl_Event_Notify
 * Flags for the callbacks to indicate interception or notification of the
 * parent.
 */
enum Ewl_Event_Notify
{
	EWL_CALLBACK_NOTIFY_NONE = 0,
	EWL_CALLBACK_NOTIFY_NOTIFY = 1,
	EWL_CALLBACK_NOTIFY_INTERCEPT = 2,

	EWL_CALLBACK_TYPE_DIRECT = 4
};

typedef enum Ewl_Event_Notify Ewl_Event_Notify;

/**
 * @enum Ewl_Orientation
 * The orientation enum is used in a few widgets to specify whether the widget
 * should be laid out in a horizontal or vertical fashion.
 */
enum Ewl_Orientation
{
	EWL_ORIENTATION_HORIZONTAL,
	EWL_ORIENTATION_VERTICAL
};

typedef enum Ewl_Orientation Ewl_Orientation;

/**
 * @enum Ewl_Flags
 * A variety of flags that affect layout, visibility, scheduling and
 * properties of objects.
 */
enum Ewl_Flags
{
	/*
	 * The alignment enumeration allows for specifying how an element is
	 * aligned within it's container.
	 */
	EWL_FLAG_ALIGN_CENTER = 0, /**< Center align bit */
	EWL_FLAG_ALIGN_LEFT = 0x1, /**< Left align bit */
	EWL_FLAG_ALIGN_RIGHT = 0x2, /**< Right align bit */
	EWL_FLAG_ALIGN_TOP = 0x4, /**< Top align bit */
	EWL_FLAG_ALIGN_BOTTOM = 0x8, /**< Bottom align bit */

	/*
	 * Fill policy identifies to containers whether child widgets should be
	 * stretched to fill available space or keep their current size.
	 */
	EWL_FLAG_FILL_NONE = 0, /**< Do not fill or shrink in any direction */
	EWL_FLAG_FILL_HSHRINK = 0x10, /**< Horizontally shrink bit */
	EWL_FLAG_FILL_VSHRINK = 0x20, /**< Horizontally shrink bit */
	EWL_FLAG_FILL_SHRINK =
	    EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VSHRINK, /**< Shrink bit */
	EWL_FLAG_FILL_HFILL = 0x40, /**< Horizontal fill bit */
	EWL_FLAG_FILL_VFILL = 0x80, /**< Vertical fill bit */
	EWL_FLAG_FILL_FILL = EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VFILL, /**< Fill bit */
	EWL_FLAG_FILL_ALL = EWL_FLAG_FILL_FILL | EWL_FLAG_FILL_SHRINK, /**< Shrunk and fill bit */

	/*
	 * Flags identifying the visibility status of the widget
	 */
	EWL_FLAG_VISIBLE_HIDDEN = 0,
	EWL_FLAG_VISIBLE_SHOWN = 0x100,
	EWL_FLAG_VISIBLE_REALIZED = 0x200,
	EWL_FLAG_VISIBLE_OBSCURED = 0x400,
	EWL_FLAG_VISIBLE_NOCLIP = 0x800,

	/*
	 * Behavior modifying properties.
	 */
	EWL_FLAG_PROPERTY_RECURSIVE = 0x1000,
	EWL_FLAG_PROPERTY_TOPLEVEL = 0x2000,
	EWL_FLAG_PROPERTY_INTERNAL = 0x4000,
	EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS = 0x8000,

	/*
	 * Flags to indicate queues this object is on.
	 */
	EWL_FLAG_QUEUED_CSCHEDULED = 0x10000,
	EWL_FLAG_QUEUED_RSCHEDULED = 0x20000,
	EWL_FLAG_QUEUED_DSCHEDULED = 0x40000,

	EWL_FLAG_QUEUED_CPROCESS = 0x80000,
	EWL_FLAG_QUEUED_RPROCESS = 0x100000,
	EWL_FLAG_QUEUED_DPROCESS = 0x200000,

	/*
	 * The state enum specifies the current state of a widget, ie. has it
	 * been clicked, does it have the keyboard focus, etc.
	 */
	EWL_FLAG_STATE_NORMAL = 0,
	EWL_FLAG_STATE_HILITED = 0x400000,
	EWL_FLAG_STATE_PRESSED = 0x800000,
	EWL_FLAG_STATE_SELECTED = 0x10000000,
	EWL_FLAG_STATE_DND = 0x2000000,
	EWL_FLAG_STATE_DISABLED = 0x4000000,
};

#define EWL_FLAG_FILL_NORMAL (EWL_FLAG_FILL_FILL)

#define EWL_FLAGS_ALIGN_MASK (EWL_FLAG_ALIGN_CENTER | EWL_FLAG_ALIGN_LEFT | \
		EWL_FLAG_ALIGN_RIGHT | EWL_FLAG_ALIGN_TOP | \
		EWL_FLAG_ALIGN_BOTTOM)

#define EWL_FLAGS_FILL_MASK (EWL_FLAG_FILL_NONE | EWL_FLAG_FILL_SHRINK | \
		EWL_FLAG_FILL_FILL)

#define EWL_FLAGS_VISIBLE_MASK (EWL_FLAG_VISIBLE_HIDDEN | \
		EWL_FLAG_VISIBLE_SHOWN | EWL_FLAG_VISIBLE_REALIZED | \
		EWL_FLAG_VISIBLE_OBSCURED | EWL_FLAG_VISIBLE_NOCLIP)

#define  EWL_FLAGS_PROPERTY_MASK (EWL_FLAG_PROPERTY_RECURSIVE | \
		EWL_FLAG_PROPERTY_TOPLEVEL | EWL_FLAG_PROPERTY_INTERNAL | \
		EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS)

#define  EWL_FLAGS_QUEUED_MASK (EWL_FLAG_QUEUED_CSCHEDULED | \
		EWL_FLAG_QUEUED_RSCHEDULED | EWL_FLAG_QUEUED_DSCHEDULED | \
		EWL_FLAG_QUEUED_CPROCESS | EWL_FLAG_QUEUED_RPROCESS | \
		EWL_FLAG_QUEUED_DPROCESS)

#define  EWL_FLAGS_STATE_MASK (EWL_FLAG_STATE_NORMAL | \
		EWL_FLAG_STATE_HILITED | EWL_FLAG_STATE_PRESSED | \
		EWL_FLAG_STATE_SELECTED | EWL_FLAG_STATE_DND | \
		EWL_FLAG_STATE_DISABLED)

/**
 * @enum Ewl_Position
 */
enum Ewl_Position
{
	EWL_POSITION_LEFT = 0x1,
	EWL_POSITION_RIGHT = 0x2,
	EWL_POSITION_TOP = 0x4,
	EWL_POSITION_BOTTOM = 0x8
};

typedef enum Ewl_Position Ewl_Position;

#define EWL_POSITION_MASK (0xf)

/**
 * @enum Ewl_Window_Flags
 */
enum Ewl_Window_Flags
{
	EWL_WINDOW_BORDERLESS = 1,
	EWL_WINDOW_USER_CONFIGURE = 2
};

typedef enum Ewl_Window_Flags Ewl_Window_Flags;

/**
 * @enum Ewl_Tree_Node_Flags
 */
enum Ewl_Tree_Node_Flags
{
	EWL_TREE_NODE_NOEXPAND = 0,
	EWL_TREE_NODE_COLLAPSED = 1,
	EWL_TREE_NODE_EXPANDED = 2,
};

typedef enum Ewl_Tree_Node_Flags Ewl_Tree_Node_Flags;

/**
 * @enum Ewl_Notebook_Flags
 */
enum Ewl_Notebook_Flags
{
	EWL_NOTEBOOK_FLAG_TABS_HIDDEN = 0x10
};

typedef enum Ewl_Notebook_Flags Ewl_Notebook_Flags;

/**
 * @enum Ewl_ScrollPane_Flags
 */
enum Ewl_ScrollPane_Flags
{
	EWL_SCROLLPANE_FLAG_NONE,
	EWL_SCROLLPANE_FLAG_AUTO_VISIBLE,
	EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN
};

typedef enum Ewl_ScrollPane_Flags Ewl_ScrollPane_Flags;

/**
 * @enum Ewl_Filedialog_Type
 */
enum Ewl_Filedialog_Type
{
	EWL_FILEDIALOG_TYPE_OPEN,
	EWL_FILEDIALOG_TYPE_SAVE
};

typedef enum Ewl_Filedialog_Type Ewl_Filedialog_Type;

enum Ewl_Engine
{
	EWL_ENGINE_SOFTWARE_X11 = 1,
	EWL_ENGINE_GL_X11 = 2,
	EWL_ENGINE_FB = 4,
};

typedef enum Ewl_Engine Ewl_Engine;

#define EWL_ENGINE_X11 (EWL_ENGINE_SOFTWARE_X11 | EWL_ENGINE_GL_X11)
#define EWL_ENGINE_ALL (EWL_ENGINE_SOFTWARE_X11 | EWL_ENGINE_GL_X11 | \
			EWL_ENGINE_FB)

enum Ewl_Key_Modifiers
{
	EWL_KEY_MODIFIER_SHIFT = 0x1,
	EWL_KEY_MODIFIER_CTRL = 0x2,
	EWL_KEY_MODIFIER_ALT = 0x4,
	EWL_KEY_MODIFIER_MOD = 0x8,
	EWL_KEY_MODIFIER_WIN = 0x10,
};

typedef enum Ewl_Key_Modifiers Ewl_Key_Modifiers;

enum Ewl_Stock_Type
{
	EWL_STOCK_APPLY = 0,
	EWL_STOCK_ARROW_DOWN,
	EWL_STOCK_ARROW_LEFT,
	EWL_STOCK_ARROW_RIGHT,
	EWL_STOCK_ARROW_UP,
	EWL_STOCK_CANCEL,
	EWL_STOCK_FASTFORWARD,
	EWL_STOCK_HOME,
	EWL_STOCK_OK,
	EWL_STOCK_OPEN,
	EWL_STOCK_PAUSE,
	EWL_STOCK_PLAY,
	EWL_STOCK_QUIT,
	EWL_STOCK_REWIND,
	EWL_STOCK_SAVE,
	EWL_STOCK_STOP,
	EWL_STOCK_NONE
};
typedef enum Ewl_Stock_Type Ewl_Stock_Type;

/**
 * @enum Ewl_Color_Pick_Mode
 */
enum Ewl_Color_Pick_Mode 
{
	EWL_COLOR_PICK_MODE_RGB,
	EWL_COLOR_PICK_MODE_HSV_HUE,
	EWL_COLOR_PICK_MODE_HSV_SATURATION,
	EWL_COLOR_PICK_MODE_HSV_VALUE
};
typedef enum Ewl_Color_Pick_Mode Ewl_Color_Pick_Mode;

enum Ewl_Text_Style 
{
	EWL_TEXT_STYLE_NONE = 0x00,
	EWL_TEXT_STYLE_UNDERLINE = 0x01,
	EWL_TEXT_STYLE_DOUBLE_UNDERLINE = 0x02,
	EWL_TEXT_STYLE_STRIKETHROUGH = 0x04,
	EWL_TEXT_STYLE_SHADOW = 0x08,
	EWL_TEXT_STYLE_SOFT_SHADOW = 0x10,
	EWL_TEXT_STYLE_FAR_SHADOW = 0x20,
	EWL_TEXT_STYLE_OUTLINE = 0x40,
	EWL_TEXT_STYLE_GLOW = 0x80
};
typedef enum Ewl_Text_Style Ewl_Text_Style;

enum Ewl_Text_Trigger_Type 
{
	EWL_TEXT_TRIGGER_TYPE_NONE,
	EWL_TEXT_TRIGGER_TYPE_SELECTION,
	EWL_TEXT_TRIGGER_TYPE_TRIGGER
};
typedef enum Ewl_Text_Trigger_Type Ewl_Text_Trigger_Type;

enum Ewl_Attach_Type 
{
	EWL_ATTACH_TYPE_TOOLTIP,
	EWL_ATTACH_TYPE_COLOR,
	EWL_ATTACH_TYPE_NAME
};
typedef enum Ewl_Attach_Type Ewl_Attach_Type;

enum Ewl_Attach_Data_Type 
{
	EWL_ATTACH_DATA_TYPE_TEXT,
	EWL_ATTACH_DATA_TYPE_WIDGET,
	EWL_ATTACH_DATA_TYPE_OTHER
};
typedef enum Ewl_Attach_Data_Type Ewl_Attach_Data_Type;

/**
 * @}
 */ 
#endif				/* __EWL_ENUMS_H__ */


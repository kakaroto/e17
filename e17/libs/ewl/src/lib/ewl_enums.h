#ifndef EWL_ENUMS_H
#define EWL_ENUMS_H

/**
 * @addtogroup Ewl_Enums Ewl_Enums: Various Flags and Enumerations used in EWL
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
	EWL_CALLBACK_OBSCURE, /**< Widget is offscreen */
	EWL_CALLBACK_REVEAL, /**< Widget returned to screen */
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
	EWL_CALLBACK_MOUSE_IN, /**< Mouse was placed over the widget */
	EWL_CALLBACK_MOUSE_OUT, /**< Mouse was moved away from the widget */
	EWL_CALLBACK_CLICKED, /**< Mouse was pressed and released on a widget */
	EWL_CALLBACK_FOCUS_IN, /**< Widget was selected by mouse or key */
	EWL_CALLBACK_FOCUS_OUT, /**< Widget was deselected by mouse or key */
	EWL_CALLBACK_VALUE_CHANGED, /**< Value in widget changed */
	EWL_CALLBACK_STATE_CHANGED, /**< Alter the state of the appearance */
	EWL_CALLBACK_APPEARANCE_CHANGED, /**< Theme key of widget changed */
	EWL_CALLBACK_WIDGET_ENABLE, /**< Widget has been re-enabled */
	EWL_CALLBACK_WIDGET_DISABLE, /**< Widget no longer takes input */

	EWL_CALLBACK_DND_POSITION, /** A DND position event **/
	EWL_CALLBACK_DND_ENTER, /** On enter of a widget **/
	EWL_CALLBACK_DND_LEAVE, /** On exit of a widget **/
	EWL_CALLBACK_DND_DROP, /** Drop event **/
	
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
	EWL_CALLBACK_NOTIFY_NONE = 0,		/**< No notification or interception */
	EWL_CALLBACK_NOTIFY_NOTIFY = 1,		/**< Notify of call */
	EWL_CALLBACK_NOTIFY_INTERCEPT = 2,	/**< Intercept call */
	EWL_CALLBACK_TYPE_DIRECT = 4		/**< Direct call */
};

typedef enum Ewl_Event_Notify Ewl_Event_Notify;

/**
 * @enum Ewl_Orientation
 * The orientation enum is used in a few widgets to specify whether the widget
 * should be laid out in a horizontal or vertical fashion.
 */
enum Ewl_Orientation
{
	EWL_ORIENTATION_HORIZONTAL,	/**< Horizontal layout */
	EWL_ORIENTATION_VERTICAL	/**< Vertical layout */
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
	EWL_FLAG_VISIBLE_HIDDEN = 0,		/**< Widget hidden */
	EWL_FLAG_VISIBLE_SHOWN = 0x100,		/**< Widget shown */
	EWL_FLAG_VISIBLE_REALIZED = 0x200,	/**< Widget realized */
	EWL_FLAG_VISIBLE_OBSCURED = 0x400,	/**< Widget obscured */
	EWL_FLAG_VISIBLE_NOCLIP = 0x800,	/**< Widget has no clip */

	/*
	 * Behavior modifying properties.
	 */
	EWL_FLAG_PROPERTY_RECURSIVE = 0x1000,	/**< Widget is recursive */
	EWL_FLAG_PROPERTY_TOPLEVEL = 0x2000,	/**< Widget is a top level widget */
	EWL_FLAG_PROPERTY_INTERNAL = 0x4000,	/**< Widget is internal */
	EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS = 0x8000,	/**< Widget will block tab focus changes */
	EWL_FLAG_PROPERTY_FOCUSABLE = 0x10000,	/**< Widget is focusable */

	/*
	 * Flags to indicate queues this object is on.
	 */
	EWL_FLAG_QUEUED_CSCHEDULED = 0x20000,	/**< Configure scheduled */
	EWL_FLAG_QUEUED_RSCHEDULED = 0x40000,	/**< Reveal scheduled */
	EWL_FLAG_QUEUED_DSCHEDULED = 0x80000,	/**< Delete scheduled */

	EWL_FLAG_QUEUED_CPROCESS = 0x100000,	/**< Configure in progress */
	EWL_FLAG_QUEUED_RPROCESS = 0x200000,	/**< Reveal in progress */
	EWL_FLAG_QUEUED_DPROCESS = 0x400000,	/**< Delete in progress */

	/*
	 * The state enum specifies the current state of a widget, ie. has it
	 * been clicked, does it have the keyboard focus, etc.
	 */
	EWL_FLAG_STATE_NORMAL = 0,		/**< Widget state normal */
	EWL_FLAG_STATE_MOUSE_IN = 0x800000,	/**< Mouse is in the widget */
	EWL_FLAG_STATE_PRESSED = 0x1000000,	/**< Widget is pressed */
	EWL_FLAG_STATE_FOCUSED = 0x20000000,	/**< Widget has focus */
	EWL_FLAG_STATE_DND = 0x4000000,		/**< Widget is engaged in DND */
	EWL_FLAG_STATE_DISABLED = 0x8000000,	/**< Widget is disabled */

	EWL_FLAG_PROPERTY_DND_AWARE	= 0x10000000,	/**< Widget is DND aware */
	EWL_FLAG_PROPERTY_DRAGGABLE	= 0x20000000,	/**< Widget is dragable */
	EWL_FLAG_STATE_DND_WAIT		= 0x40000000,	/**< Widget is in DND wait state */
	
};

/**
 * @def EWL_FLAG_FILL_NORMAL
 * Flag specifing normal fill values 
 */
#define EWL_FLAG_FILL_NORMAL (EWL_FLAG_FILL_FILL)

/**
 * @def EWL_FLAGS_ALIGN_MASK
 * The alignment mask
 */
#define EWL_FLAGS_ALIGN_MASK (EWL_FLAG_ALIGN_CENTER | EWL_FLAG_ALIGN_LEFT | \
		EWL_FLAG_ALIGN_RIGHT | EWL_FLAG_ALIGN_TOP | \
		EWL_FLAG_ALIGN_BOTTOM)

/**
 * @def EWL_FLAGS_FILL_MASK
 * The fill mask
 */
#define EWL_FLAGS_FILL_MASK (EWL_FLAG_FILL_NONE | EWL_FLAG_FILL_SHRINK | \
		EWL_FLAG_FILL_FILL)

/**
 * @def EWL_FLAGS_VISIBLE_MASK
 * The visibility mask
 */
#define EWL_FLAGS_VISIBLE_MASK (EWL_FLAG_VISIBLE_HIDDEN | \
		EWL_FLAG_VISIBLE_SHOWN | EWL_FLAG_VISIBLE_REALIZED | \
		EWL_FLAG_VISIBLE_OBSCURED | EWL_FLAG_VISIBLE_NOCLIP)

/**
 * @def EWL_FLAGS_PROPERTY_MASK
 * The property mask
 */
#define EWL_FLAGS_PROPERTY_MASK (EWL_FLAG_PROPERTY_RECURSIVE | \
		EWL_FLAG_PROPERTY_TOPLEVEL | EWL_FLAG_PROPERTY_INTERNAL | \
		EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS | EWL_FLAG_PROPERTY_FOCUSABLE | \
		EWL_FLAG_PROPERTY_DND_AWARE | EWL_FLAG_PROPERTY_DRAGGABLE )

/**
 * @def EWL_FLAGS_QUEUED_MASK
 * The queue mask
 */
#define EWL_FLAGS_QUEUED_MASK (EWL_FLAG_QUEUED_CSCHEDULED | \
		EWL_FLAG_QUEUED_RSCHEDULED | EWL_FLAG_QUEUED_DSCHEDULED | \
		EWL_FLAG_QUEUED_CPROCESS | EWL_FLAG_QUEUED_RPROCESS | \
		EWL_FLAG_QUEUED_DPROCESS)

/**
 * @def EWL_FLAGS_STATE_MASK
 * The state mask
 */
#define EWL_FLAGS_STATE_MASK (EWL_FLAG_STATE_NORMAL | \
		EWL_FLAG_STATE_MOUSE_IN | EWL_FLAG_STATE_PRESSED | \
		EWL_FLAG_STATE_FOCUSED | EWL_FLAG_STATE_DND | \
		EWL_FLAG_STATE_DISABLED | EWL_FLAG_STATE_DND_WAIT)

/**
 * @enum Ewl_Position
 */
enum Ewl_Position
{
	EWL_POSITION_LEFT = 0x1,	/**< Left position */
	EWL_POSITION_RIGHT = 0x2,	/**< Right position */
	EWL_POSITION_TOP = 0x4,		/**< Top position */
	EWL_POSITION_BOTTOM = 0x8	/**< Bottom position */
};

typedef enum Ewl_Position Ewl_Position;

#define EWL_POSITION_MASK (0xf)

/**
 * @enum Ewl_Window_Flags
 */
enum Ewl_Window_Flags
{
	EWL_WINDOW_BORDERLESS = 0x1,		/**< Window is borderless */
	EWL_WINDOW_USER_CONFIGURE = 0x2,	/**< User configure */
	EWL_WINDOW_GRAB_POINTER = 0x4,		/**< Window grabs pointer */
	EWL_WINDOW_GRAB_KEYBOARD = 0x8,		/**< Window grabs keyboard */
	EWL_WINDOW_OVERRIDE = 0x10		/**< Window override setting */
};

typedef enum Ewl_Window_Flags Ewl_Window_Flags;

/**
 * @enum Ewl_Tree_Node_Flags
 */
enum Ewl_Tree_Node_Flags
{
	EWL_TREE_NODE_NOEXPAND = 0,	/**< Node does not expand */
	EWL_TREE_NODE_COLLAPSED = 1,	/**< Node is collapsed */
	EWL_TREE_NODE_EXPANDED = 2,	/**< Node is expanded */
};

typedef enum Ewl_Tree_Node_Flags Ewl_Tree_Node_Flags;

/**
 * @enum Ewl_Notebook_Flags
 */
enum Ewl_Notebook_Flags
{
	EWL_NOTEBOOK_FLAG_TABS_HIDDEN = 0x10	/**< Tabbar is hidden */
};

typedef enum Ewl_Notebook_Flags Ewl_Notebook_Flags;

/**
 * @enum Ewl_ScrollPane_Flags
 */
enum Ewl_ScrollPane_Flags
{
	EWL_SCROLLPANE_FLAG_NONE,		/**< No flags set */
	EWL_SCROLLPANE_FLAG_AUTO_VISIBLE,	/**< Hide if possible */
	EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN	/**< Always hide */
};

typedef enum Ewl_ScrollPane_Flags Ewl_ScrollPane_Flags;

enum Ewl_Engine
{
	EWL_ENGINE_SOFTWARE_X11 = 1,	/**< The Software X11 engine */
	EWL_ENGINE_GL_X11 = 2,		/**< The GL X11 engine */
	EWL_ENGINE_FB = 4,		/**< The Framebuffer engine */
};

typedef enum Ewl_Engine Ewl_Engine;

/**
 * @def EWL_ENGINE_X11
 * Mask for the X11 based engines 
 */
#define EWL_ENGINE_X11 (EWL_ENGINE_SOFTWARE_X11 | EWL_ENGINE_GL_X11)

/**
 * @def EWL_ENGINE_ALL
 * Mask for all engines
 */
#define EWL_ENGINE_ALL (EWL_ENGINE_SOFTWARE_X11 | EWL_ENGINE_GL_X11 | \
			EWL_ENGINE_FB)

/**
 * @enum Ewl_Key_Modifiers
 */
enum Ewl_Key_Modifiers
{
	EWL_KEY_MODIFIER_SHIFT = 0x1,	/**< Shift key */
	EWL_KEY_MODIFIER_CTRL = 0x2,	/**< Ctrl key */
	EWL_KEY_MODIFIER_ALT = 0x4,	/**< Alt key */
	EWL_KEY_MODIFIER_MOD = 0x8,	/**< Mod key */
	EWL_KEY_MODIFIER_WIN = 0x10,	/**< Win key */
};

typedef enum Ewl_Key_Modifiers Ewl_Key_Modifiers;

/**
 * @enum Ewl_Stock_Type
 */
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
 * @enum Ewl_Color_Mode
 */
enum Ewl_Color_Mode 
{
	EWL_COLOR_MODE_RGB_RED,
	EWL_COLOR_MODE_RGB_GREEN,
	EWL_COLOR_MODE_RGB_BLUE,
	EWL_COLOR_MODE_HSV_HUE,
	EWL_COLOR_MODE_HSV_SATURATION,
	EWL_COLOR_MODE_HSV_VALUE
};
typedef enum Ewl_Color_Mode Ewl_Color_Mode;

/**
 * @enum Ewl_Spectrum_Type
 */
enum Ewl_Spectrum_Type
{
	EWL_SPECTRUM_TYPE_SQUARE,
	EWL_SPECTRUM_TYPE_VERTICAL
};
typedef enum Ewl_Spectrum_Type Ewl_Spectrum_Type;

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
	EWL_ATTACH_TYPE_NAME,
	EWL_ATTACH_TYPE_MOUSE_CURSOR,
	EWL_ATTACH_TYPE_NOTEBOOK_DATA
};
typedef enum Ewl_Attach_Type Ewl_Attach_Type;

enum Ewl_Attach_Data_Type 
{
	EWL_ATTACH_DATA_TYPE_TEXT,
	EWL_ATTACH_DATA_TYPE_WIDGET,
	EWL_ATTACH_DATA_TYPE_OTHER
};
typedef enum Ewl_Attach_Data_Type Ewl_Attach_Data_Type;

enum Ewl_Media_Module_Type
{
	EWL_MEDIA_MODULE_XINE,
	EWL_MEDIA_MODULE_GSTREAMER
};
typedef enum Ewl_Media_Module_Type Ewl_Media_Module_Type;

enum Ewl_Tree_Mode
{
	EWL_TREE_MODE_NONE,
	EWL_TREE_MODE_SINGLE,
	EWL_TREE_MODE_MULTI
};

typedef enum Ewl_Tree_Mode Ewl_Tree_Mode;

enum Ewl_Text_Context_Mask
{
	EWL_TEXT_CONTEXT_MASK_NONE = 0x00,
	EWL_TEXT_CONTEXT_MASK_FONT = 0x01,
	EWL_TEXT_CONTEXT_MASK_SIZE = 0x02,
	EWL_TEXT_CONTEXT_MASK_STYLES = 0x04,
	EWL_TEXT_CONTEXT_MASK_ALIGN = 0x08,
	EWL_TEXT_CONTEXT_MASK_WRAP = 0x10,
	EWL_TEXT_CONTEXT_MASK_COLOR = 0x20,
	EWL_TEXT_CONTEXT_MASK_BG_COLOR = 0x40,
	EWL_TEXT_CONTEXT_MASK_GLOW_COLOR = 0x80,
	EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR = 0x100,
	EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR = 0x200,
	EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR = 0x400,
	EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR = 0x800,
	EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR = 0x1000
};
typedef enum Ewl_Text_Context_Mask Ewl_Text_Context_Mask;

enum Ewl_Icon_Type
{
	EWL_ICON_TYPE_SHORT,
	EWL_ICON_TYPE_LONG
};
typedef enum Ewl_Icon_Type Ewl_Icon_Type;

enum Ewl_Freebox_Layout_Type
{
	EWL_FREEBOX_LAYOUT_MANUAL,
	EWL_FREEBOX_LAYOUT_COMPARATOR,
	EWL_FREEBOX_LAYOUT_AUTO
};
typedef enum Ewl_Freebox_Layout_Type Ewl_Freebox_Layout_Type;

enum Ewl_Filelist_Event_Type
{
	EWL_FILELIST_EVENT_TYPE_DIR_CHANGE,
	EWL_FILELIST_EVENT_TYPE_SELECTION_CHANGE,
	EWL_FILELIST_EVENT_TYPE_FILE_SELECTED
};
typedef enum Ewl_Filelist_Event_Type Ewl_Filelist_Event_Type;

#define EWL_MOUSE_CURSOR_X 0
#define EWL_MOUSE_CURSOR_ARROW 2
#define EWL_MOUSE_CURSOR_BASED_ARROW_DOWN 4
#define EWL_MOUSE_CURSOR_UP 6
#define EWL_MOUSE_CURSOR_BOAT 8
#define EWL_MOUSE_CURSOR_BOTTOM_LEFT_CORNER 12
#define EWL_MOUSE_CURSOR_BOTTOM_RIGHT_CORNER 14
#define EWL_MOUSE_CURSOR_BOTTOM_SIDE 16
#define EWL_MOUSE_CURSOR_BOTTOM_TEE 18
#define EWL_MOUSE_CURSOR_BOX_SPIRAL 20
#define EWL_MOUSE_CURSOR_CENTER_PTR 22
#define EWL_MOUSE_CURSOR_CIRCLE 24
#define EWL_MOUSE_CURSOR_CLOCK 26
#define EWL_MOUSE_CURSOR_COFFEE_MUG 28
#define EWL_MOUSE_CURSOR_CROSS 30
#define EWL_MOUSE_CURSOR_CROSS_REVERSE 32
#define EWL_MOUSE_CURSOR_CROSSHAIR 34
#define EWL_MOUSE_CURSOR_DIAMOND_CROSS 36
#define EWL_MOUSE_CURSOR_DOT 38
#define EWL_MOUSE_CURSOR_DOT_BOX_MASK 40
#define EWL_MOUSE_CURSOR_DOUBLE_ARROW 42
#define EWL_MOUSE_CURSOR_DRAFT_LARGE 44
#define EWL_MOUSE_CURSOR_DRAFT_SMALL 46
#define EWL_MOUSE_CURSOR_DRAPED_BOX 48
#define EWL_MOUSE_CURSOR_EXCHANGE 50
#define EWL_MOUSE_CURSOR_FLEUR 52
#define EWL_MOUSE_CURSOR_GOBBLER 54
#define EWL_MOUSE_CURSOR_GUMBY 56
#define EWL_MOUSE_CURSOR_HAND1 58
#define EWL_MOUSE_CURSOR_HAND2 60
#define EWL_MOUSE_CURSOR_HEART 62
#define EWL_MOUSE_CURSOR_ICON 64
#define EWL_MOUSE_CURSOR_IRON_CROSS 66
#define EWL_MOUSE_CURSOR_LEFT_PTR 68
#define EWL_MOUSE_CURSOR_LEFT_SIDE 70
#define EWL_MOUSE_CURSOR_LEFT_TEE 72
#define EWL_MOUSE_CURSOR_LEFTBUTTON 74
#define EWL_MOUSE_CURSOR_LL_ANGLE 76
#define EWL_MOUSE_CURSOR_LR_ANGLE 78
#define EWL_MOUSE_CURSOR_MAN 80
#define EWL_MOUSE_CURSOR_MIDDLEBUTTON 82
#define EWL_MOUSE_CURSOR_MOUSE 84
#define EWL_MOUSE_CURSOR_PENCIL 86
#define EWL_MOUSE_CURSOR_PIRATE 88
#define EWL_MOUSE_CURSOR_PLUS 90
#define EWL_MOUSE_CURSOR_QUESTION_ARROW 92
#define EWL_MOUSE_CURSOR_RIGHT_PTR 94
#define EWL_MOUSE_CURSOR_RIGHT_SIDE 96
#define EWL_MOUSE_CURSOR_RIGHT_TEE 98
#define EWL_MOUSE_CURSOR_RIGHTBUTTON 100
#define EWL_MOUSE_CURSOR_RTL_LOGO 102
#define EWL_MOUSE_CURSOR_SAILBOAT 104
#define EWL_MOUSE_CURSOR_SB_DOWN_ARROW 106
#define EWL_MOUSE_CURSOR_SB_H_DOUBLE_ARROW 108
#define EWL_MOUSE_CURSOR_SB_LEFT_ARROW 110
#define EWL_MOUSE_CURSOR_SB_RIGHT_ARROW 112
#define EWL_MOUSE_CURSOR_SB_UP_ARROW 114
#define EWL_MOUSE_CURSOR_SB_V_DOUBLE_ARROW 116
#define EWL_MOUSE_CURSOR_SHUTTLE 118
#define EWL_MOUSE_CURSOR_SIZING 120
#define EWL_MOUSE_CURSOR_SPIDER 122
#define EWL_MOUSE_CURSOR_SPRAYCAN 124
#define EWL_MOUSE_CURSOR_STAR 126
#define EWL_MOUSE_CURSOR_TARGET 128
#define EWL_MOUSE_CURSOR_TCROSS 130
#define EWL_MOUSE_CURSOR_TOP_LEFT_ARROW 132
#define EWL_MOUSE_CURSOR_TOP_LEFT_CORNER 134
#define EWL_MOUSE_CURSOR_TOP_RIGHT_CORNER 136
#define EWL_MOUSE_CURSOR_TOP_SIDE 138
#define EWL_MOUSE_CURSOR_TOP_TEE 140
#define EWL_MOUSE_CURSOR_TREK 142
#define EWL_MOUSE_CURSOR_UL_ANGLE 144
#define EWL_MOUSE_CURSOR_UMBRELLA 146
#define EWL_MOUSE_CURSOR_UR_ANGLE 148
#define EWL_MOUSE_CURSOR_WATCH 150
#define EWL_MOUSE_CURSOR_XTERM 152

/**
 * @}
 */ 
#endif				/* __EWL_ENUMS_H__ */


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

	EWL_CALLBACK_DND_POSITION, /**< A DND position event **/
	EWL_CALLBACK_DND_ENTER, /**< On enter of a widget **/
	EWL_CALLBACK_DND_LEAVE, /**< On exit of a widget **/
	EWL_CALLBACK_DND_DROP, /**< Drop event **/
	
	EWL_CALLBACK_MAX /**< Flag to indicate last value */
};

/**
 * The Ewl_Callback_Type 
 */
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

/**
 * The Ewl_Event_Notify type
 */
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

/**
 * The Ewl_Orientation type
 */
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
 * The possible placement positions
 */
enum Ewl_Position
{
	EWL_POSITION_LEFT = 0x1,	/**< Left position */
	EWL_POSITION_RIGHT = 0x2,	/**< Right position */
	EWL_POSITION_TOP = 0x4,		/**< Top position */
	EWL_POSITION_BOTTOM = 0x8	/**< Bottom position */
};

/**
 * The Ewl_Position type
 */
typedef enum Ewl_Position Ewl_Position;

/**
 * @def EWL_POSITION_MASK
 * The mask for the Ewl_Position bitmap
 */
#define EWL_POSITION_MASK (0xf)

/**
 * @enum Ewl_Window_Flags
 * The possible window states
 */
enum Ewl_Window_Flags
{
	EWL_WINDOW_BORDERLESS = 0x1,		/**< Window is borderless */
	EWL_WINDOW_USER_CONFIGURE = 0x2,	/**< User configure */
	EWL_WINDOW_GRAB_POINTER = 0x4,		/**< Window grabs pointer */
	EWL_WINDOW_GRAB_KEYBOARD = 0x8,		/**< Window grabs keyboard */
	EWL_WINDOW_OVERRIDE = 0x10,		/**< Window override setting */
	EWL_WINDOW_FULLSCREEN = 0x20,		/**< Window is fullscreen */
	EWL_WINDOW_DIALOG = 0x40,		/**< Window has no max and min buttons */
	EWL_WINDOW_MODAL = 0x80			/**< Window is modal */
};

/**
 * The Ewl_Window_Flags type
 */
typedef enum Ewl_Window_Flags Ewl_Window_Flags;

/**
 * @enum Ewl_Tree_Node_Flags
 * The available tree node states
 */
enum Ewl_Tree_Node_Flags
{
	EWL_TREE_NODE_NOEXPAND = 0,	/**< Node does not expand */
	EWL_TREE_NODE_COLLAPSED = 1,	/**< Node is collapsed */
	EWL_TREE_NODE_EXPANDED = 2,	/**< Node is expanded */
};

/**
 * The Ewl_Tree_Node_Flags type
 */
typedef enum Ewl_Tree_Node_Flags Ewl_Tree_Node_Flags;

/**
 * @enum Ewl_Notebook_Flags
 * States effecting the notebook
 */
enum Ewl_Notebook_Flags
{
	EWL_NOTEBOOK_FLAG_TABS_HIDDEN = 0x10	/**< Tabbar is hidden */
};

/**
 * The Ewl_Notebook_Flags type
 */
typedef enum Ewl_Notebook_Flags Ewl_Notebook_Flags;

/**
 * @enum Ewl_Scrollpane_Flags
 * The possible scrollpane settings
 */
enum Ewl_Scrollpane_Flags
{
	EWL_SCROLLPANE_FLAG_NONE,		/**< No flags set */
	EWL_SCROLLPANE_FLAG_AUTO_VISIBLE,	/**< Hide if possible */
	EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN	/**< Always hide */
};

/**
 * The Ewl_Scrollpane_Flags type
 */
typedef enum Ewl_Scrollpane_Flags Ewl_Scrollpane_Flags;

/**
 * @enum Ewl_Key_Modifiers
 * The key modifiers
 */
enum Ewl_Key_Modifiers
{
	EWL_KEY_MODIFIER_SHIFT = 0x1,	/**< Shift key */
	EWL_KEY_MODIFIER_CTRL = 0x2,	/**< Ctrl key */
	EWL_KEY_MODIFIER_ALT = 0x4,	/**< Alt key */
	EWL_KEY_MODIFIER_MOD = 0x8,	/**< Mod key */
	EWL_KEY_MODIFIER_WIN = 0x10,	/**< Win key */
};

/**
 * The Ewl_Key_Modifiers type
 */
typedef enum Ewl_Key_Modifiers Ewl_Key_Modifiers;

/**
 * @enum Ewl_State_Type
 * The state type
 */
enum Ewl_State_Type {
	EWL_STATE_TRANSIENT,
	EWL_STATE_PERSISTENT
};

/**
 * The Ewl_State_Type type
 */
typedef enum Ewl_State_Type Ewl_State_Type;

/**
 * @enum Ewl_Stock_Type
 * The different stock types
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

/**
 * The Ewl_Stock_Type type
 */
typedef enum Ewl_Stock_Type Ewl_Stock_Type;

/**
 * @enum Ewl_Color_Mode
 * The different colour picker modes
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

/**
 * The Ewl_Color_Mode type
 */
typedef enum Ewl_Color_Mode Ewl_Color_Mode;

/**
 * @enum Ewl_Spectrum_Type
 * The spectrum types
 */
enum Ewl_Spectrum_Type
{
	EWL_SPECTRUM_TYPE_SQUARE,
	EWL_SPECTRUM_TYPE_VERTICAL
};

/**
 * The Ewl_Spectrum_Type type
 */
typedef enum Ewl_Spectrum_Type Ewl_Spectrum_Type;

/**
 * @enum Ewl_Text_Style
 * The possible styles that can be set into ewl_text widgets
 */
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

/**
 * The Ewl_Text_Style type
 */
typedef enum Ewl_Text_Style Ewl_Text_Style;

/**
 * @enum Ewl_Text_Trigger_Type
 * The types of trigger that can be set
 */
enum Ewl_Text_Trigger_Type 
{
	EWL_TEXT_TRIGGER_TYPE_NONE,
	EWL_TEXT_TRIGGER_TYPE_SELECTION,
	EWL_TEXT_TRIGGER_TYPE_TRIGGER
};

/**
 * The Ewl_Text_Trigger_Type
 */
typedef enum Ewl_Text_Trigger_Type Ewl_Text_Trigger_Type;

/**
 * @enum Ewl_Attach_Type
 * The types that can be attached to a widget
 */
enum Ewl_Attach_Type 
{
	EWL_ATTACH_TYPE_TOOLTIP,
	EWL_ATTACH_TYPE_COLOR,
	EWL_ATTACH_TYPE_NAME,
	EWL_ATTACH_TYPE_MOUSE_CURSOR,
	EWL_ATTACH_TYPE_NOTEBOOK_DATA
};

/**
 * The Ewl_Attach_Type 
 */
typedef enum Ewl_Attach_Type Ewl_Attach_Type;

/**
 * @enum Ewl_Attach_Data_Type
 * The types of data that can be set into a widgets attachment
 */
enum Ewl_Attach_Data_Type 
{
	EWL_ATTACH_DATA_TYPE_TEXT,
	EWL_ATTACH_DATA_TYPE_WIDGET,
	EWL_ATTACH_DATA_TYPE_OTHER
};

/**
 * The Ewl_Attach_Data_Type
 */
typedef enum Ewl_Attach_Data_Type Ewl_Attach_Data_Type;

/**
 * @enum Ewl_Media_Module_Type
 * The possible modules to use with ewl_media
 */
enum Ewl_Media_Module_Type
{
	EWL_MEDIA_MODULE_XINE,
	EWL_MEDIA_MODULE_GSTREAMER
};

/**
 * The Ewl_Media_Module_Type
 */
typedef enum Ewl_Media_Module_Type Ewl_Media_Module_Type;

/**
 * @enum Ewl_Tree_Mode
 * The possible tree mode settings
 */
enum Ewl_Tree_Mode
{
	EWL_TREE_MODE_NONE,
	EWL_TREE_MODE_SINGLE,
	EWL_TREE_MODE_MULTI
};

/**
 * The Ewl_Tree_Mode type
 */
typedef enum Ewl_Tree_Mode Ewl_Tree_Mode;

/**
 * @enum Ewl_Text_Context_Mask
 * The possible context masks for the ewl_text
 */
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

/**
 * The Ewl_Text_Context_Mask type
 */
typedef enum Ewl_Text_Context_Mask Ewl_Text_Context_Mask;

/**
 * @enum Ewl_Icon_Type
 * The possible icon types
 */
enum Ewl_Icon_Type
{
	EWL_ICON_TYPE_SHORT,
	EWL_ICON_TYPE_LONG
};

/**
 * The Ewl_Icon_Type
 */
typedef enum Ewl_Icon_Type Ewl_Icon_Type;

/**
 * @enum Ewl_Freebox_Layout_Type
 * The possibly layout settings for the freebox
 */
enum Ewl_Freebox_Layout_Type
{
	EWL_FREEBOX_LAYOUT_MANUAL,
	EWL_FREEBOX_LAYOUT_COMPARATOR,
	EWL_FREEBOX_LAYOUT_AUTO
};

/**
 * The Ewl_Freebox_Layout_Type
 */
typedef enum Ewl_Freebox_Layout_Type Ewl_Freebox_Layout_Type;

/**
 * @enum Ewl_Filelist_Event_Type
 * The possible events that can happen from the filelist
 */
enum Ewl_Filelist_Event_Type
{
	EWL_FILELIST_EVENT_TYPE_DIR_CHANGE,
	EWL_FILELIST_EVENT_TYPE_SELECTION_CHANGE,
	EWL_FILELIST_EVENT_TYPE_FILE_SELECTED
};

/**
 * The Ewl_Filelist_Event_Type 
 */
typedef enum Ewl_Filelist_Event_Type Ewl_Filelist_Event_Type;

/**
 * @enum Ewl_Mouse_Cursor_Type
 * The possible mouse cursor settings
 */
enum Ewl_Mouse_Cursor_Type
{
	EWL_MOUSE_CURSOR_X = 0,
	EWL_MOUSE_CURSOR_ARROW = 2,
	EWL_MOUSE_CURSOR_BASED_ARROW_DOWN = 4,
	EWL_MOUSE_CURSOR_UP = 6,
	EWL_MOUSE_CURSOR_BOAT = 8,
	EWL_MOUSE_CURSOR_BOTTOM_LEFT_CORNER = 12,
	EWL_MOUSE_CURSOR_BOTTOM_RIGHT_CORNER = 14,
	EWL_MOUSE_CURSOR_BOTTOM_SIDE = 16,
	EWL_MOUSE_CURSOR_BOTTOM_TEE = 18,
	EWL_MOUSE_CURSOR_BOX_SPIRAL = 20,
	EWL_MOUSE_CURSOR_CENTER_PTR = 22,
	EWL_MOUSE_CURSOR_CIRCLE = 24,
	EWL_MOUSE_CURSOR_CLOCK = 26,
	EWL_MOUSE_CURSOR_COFFEE_MUG = 28,
	EWL_MOUSE_CURSOR_CROSS = 30,
	EWL_MOUSE_CURSOR_CROSS_REVERSE = 32,
	EWL_MOUSE_CURSOR_CROSSHAIR = 34,
	EWL_MOUSE_CURSOR_DIAMOND_CROSS = 36,
	EWL_MOUSE_CURSOR_DOT = 38,
	EWL_MOUSE_CURSOR_DOT_BOX_MASK = 40,
	EWL_MOUSE_CURSOR_DOUBLE_ARROW = 42,
	EWL_MOUSE_CURSOR_DRAFT_LARGE = 44,
	EWL_MOUSE_CURSOR_DRAFT_SMALL = 46,
	EWL_MOUSE_CURSOR_DRAPED_BOX = 48,
	EWL_MOUSE_CURSOR_EXCHANGE = 50,
	EWL_MOUSE_CURSOR_FLEUR = 52,
	EWL_MOUSE_CURSOR_GOBBLER = 54,
	EWL_MOUSE_CURSOR_GUMBY = 56,
	EWL_MOUSE_CURSOR_HAND1 = 58,
	EWL_MOUSE_CURSOR_HAND2 = 60,
	EWL_MOUSE_CURSOR_HEART = 62,
	EWL_MOUSE_CURSOR_ICON = 64,
	EWL_MOUSE_CURSOR_IRON_CROSS = 66,
	EWL_MOUSE_CURSOR_LEFT_PTR = 68,
	EWL_MOUSE_CURSOR_LEFT_SIDE = 70,
	EWL_MOUSE_CURSOR_LEFT_TEE = 72,
	EWL_MOUSE_CURSOR_LEFTBUTTON = 74,
	EWL_MOUSE_CURSOR_LL_ANGLE = 76,
	EWL_MOUSE_CURSOR_LR_ANGLE = 78,
	EWL_MOUSE_CURSOR_MAN = 80,
	EWL_MOUSE_CURSOR_MIDDLEBUTTON = 82,
	EWL_MOUSE_CURSOR_MOUSE = 84,
	EWL_MOUSE_CURSOR_PENCIL = 86,
	EWL_MOUSE_CURSOR_PIRATE = 88,
	EWL_MOUSE_CURSOR_PLUS = 90,
	EWL_MOUSE_CURSOR_QUESTION_ARROW = 92,
	EWL_MOUSE_CURSOR_RIGHT_PTR = 94,
	EWL_MOUSE_CURSOR_RIGHT_SIDE = 96,
	EWL_MOUSE_CURSOR_RIGHT_TEE = 98,
	EWL_MOUSE_CURSOR_RIGHTBUTTON = 100,
	EWL_MOUSE_CURSOR_RTL_LOGO = 102,
	EWL_MOUSE_CURSOR_SAILBOAT = 104,
	EWL_MOUSE_CURSOR_SB_DOWN_ARROW = 106,
	EWL_MOUSE_CURSOR_SB_H_DOUBLE_ARROW = 108,
	EWL_MOUSE_CURSOR_SB_LEFT_ARROW = 110,
	EWL_MOUSE_CURSOR_SB_RIGHT_ARROW = 112,
	EWL_MOUSE_CURSOR_SB_UP_ARROW = 114,
	EWL_MOUSE_CURSOR_SB_V_DOUBLE_ARROW = 116,
	EWL_MOUSE_CURSOR_SHUTTLE = 118,
	EWL_MOUSE_CURSOR_SIZING = 120,
	EWL_MOUSE_CURSOR_SPIDER = 122,
	EWL_MOUSE_CURSOR_SPRAYCAN = 124,
	EWL_MOUSE_CURSOR_STAR = 126,
	EWL_MOUSE_CURSOR_TARGET = 128,
	EWL_MOUSE_CURSOR_TCROSS = 130,
	EWL_MOUSE_CURSOR_TOP_LEFT_ARROW = 132,
	EWL_MOUSE_CURSOR_TOP_LEFT_CORNER = 134,
	EWL_MOUSE_CURSOR_TOP_RIGHT_CORNER = 136,
	EWL_MOUSE_CURSOR_TOP_SIDE = 138,
	EWL_MOUSE_CURSOR_TOP_TEE = 140,
	EWL_MOUSE_CURSOR_TREK = 142,
	EWL_MOUSE_CURSOR_UL_ANGLE = 144,
	EWL_MOUSE_CURSOR_UMBRELLA = 146,
	EWL_MOUSE_CURSOR_UR_ANGLE = 148,
	EWL_MOUSE_CURSOR_WATCH = 150,
	EWL_MOUSE_CURSOR_XTERM = 152
};

/**
 * The Ewl_Mouse_Cursor_Type
 */
typedef enum Ewl_Mouse_Cursor_Type Ewl_Mouse_Cursor_Type;

/**
 * @}
 */ 
#endif				/* __EWL_ENUMS_H__ */


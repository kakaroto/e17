

#ifndef __EWL_ENUMS_H__
#define __EWL_ENUMS_H__

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
	EWL_CALLBACK_UNREALIZE,
	EWL_CALLBACK_SHOW,
	EWL_CALLBACK_HIDE,
	EWL_CALLBACK_DESTROY,
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
	EWL_CALLBACK_DOUBLE_CLICKED,
	EWL_CALLBACK_HILITED,
	EWL_CALLBACK_VALUE_CHANGED,
	EWL_CALLBACK_THEME_UPDATE,
	EWL_CALLBACK_STATE_CHANGED,
	EWL_CALLBACK_APPEARANCE_CHANGED,
	EWL_CALLBACK_FX_STARTED,
	EWL_CALLBACK_FX_STOPPED,
	EWL_CALLBACK_WIDGET_ENABLE,
	EWL_CALLBACK_WIDGET_DISABLE,
	EWL_CALLBACK_MAX
};

/*
 * Flags for the callbacks to indicate interception or notification of the
 * parent.
 */
typedef enum _ewl_event_notify Ewl_Event_Notify;

enum _ewl_event_notify
{
	EWL_CALLBACK_NOTIFY_NONE = 0,
	EWL_CALLBACK_NOTIFY_NOTIFY = 1,
	EWL_CALLBACK_NOTIFY_INTERCEPT = 2
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
	EWL_STATE_NORMAL = (0x1 << 0),
	EWL_STATE_HILITED = (0x1 << 1),
	EWL_STATE_PRESSED = (0x1 << 2),
	EWL_STATE_SELECTED = (0x1 << 3),
	EWL_STATE_DND = (0x1 << 4),
	EWL_STATE_DISABLED = (0x1 << 5)
};


/*
 * The alignment enumeration allows for specifying how an element is aligned
 * within it's container.
 */
typedef enum _ewl_alignment Ewl_Alignment;
#define EWL_ALIGNMENT_MASK 0xFF

enum _ewl_alignment
{
	EWL_ALIGNMENT_CENTER = ETOX_ALIGN_CENTER,
	EWL_ALIGNMENT_LEFT = ETOX_ALIGN_LEFT,
	EWL_ALIGNMENT_RIGHT = ETOX_ALIGN_RIGHT,
	EWL_ALIGNMENT_TOP = ETOX_ALIGN_TOP,
	EWL_ALIGNMENT_BOTTOM = ETOX_ALIGN_BOTTOM
};

/*
 * Fill policy identifies to containers whether child widgets should be
 * stretched to fill available space or keep their current size.
 */
typedef enum _ewl_fill_policy Ewl_Fill_Policy;
#define EWL_FILL_POLICY_MASK 0xFF00

enum _ewl_fill_policy
{
	EWL_FILL_POLICY_NONE = 0,
	EWL_FILL_POLICY_HSHRINK = 0x1000,
	EWL_FILL_POLICY_VSHRINK = 0x2000,
	EWL_FILL_POLICY_SHRINK =
	    EWL_FILL_POLICY_HSHRINK | EWL_FILL_POLICY_VSHRINK,
	EWL_FILL_POLICY_HFILL = 0x4000,
	EWL_FILL_POLICY_VFILL = 0x8000,
	EWL_FILL_POLICY_FILL = EWL_FILL_POLICY_HFILL | EWL_FILL_POLICY_VFILL,
	EWL_FILL_POLICY_NORMAL = EWL_FILL_POLICY_FILL | EWL_FILL_POLICY_SHRINK
};

/*
 * Flags identifying whether a widget is shown, has been realized, or is
 * recursive (ie. a container).
 */
typedef enum _ewl_widget_flags Ewl_Widget_Flags;

enum _ewl_widget_flags
{
	EWL_FLAGS_HIDDEN = 0x0,
	EWL_FLAGS_SHOWN = 0x1,
	EWL_FLAGS_REALIZED = 0x2,
	EWL_FLAGS_OBSCURED = 0x4,
	EWL_FLAGS_RECURSIVE = 0x8,
};


typedef enum _ewl_position Ewl_Position;

enum _ewl_position
{
	EWL_POSITION_LEFT,
	EWL_POSITION_RIGHT,
	EWL_POSITION_TOP,
	EWL_POSITION_BOTTOM
};

typedef enum _ewl_window_flags Ewl_Window_Flags;

enum _ewl_window_flags
{
	EWL_WINDOW_AUTO_SIZE = 1,
	EWL_WINDOW_BORDERLESS = 2
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
	EWL_FX_MODIFIES_ALL_CHANNELS = EWL_FX_MODIFIES_ALPHA_CHANNEL |
	    EWL_FX_MODIFIES_RED_CHANNEL | EWL_FX_MODIFIES_GREEN_CHANNEL |
	    EWL_FX_MODIFIES_BLUE_CHANNEL,
	EWL_FX_MODIFIES_POSITION = (0x1 << 6),
	EWL_FX_MODIFIES_SIZE = (0x1 << 7),
	EWL_FX_MODIFIES_GEOMETRY =
	    EWL_FX_MODIFIES_POSITION | EWL_FX_MODIFIES_SIZE
};

typedef enum _ewl_scrollbar_flags Ewl_ScrollBar_Flags;

enum _ewl_scrollbar_flags
{
	EWL_SCROLLBAR_FLAG_NONE,
	EWL_SCROLLBAR_FLAG_AUTO_VISIBLE,
	EWL_SCROLLBAR_FLAG_ALWAYS_HIDDEN
};


typedef enum _ewl_filedialog_type Ewl_Filedialog_Type;

enum _ewl_filedialog_type
{
	EWL_FILEDIALOG_TYPE_OPEN,
	EWL_FILEDIALOG_TYPE_CLOSE
};

#endif				/* __EWL_ENUMS_H__ */

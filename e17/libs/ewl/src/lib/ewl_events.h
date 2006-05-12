#ifndef EWL_EVENTS_H
#define EWL_EVENTS_H

/**
 * @addtogroup Ewl_Events Ewl_Events: Lower Level Event Handlers
 * @brief Defines the routines that dispatch the lower level events to EWL.
 *
 * @{
 */

/**
 * The Ewl_Event_Window_Expose structure
 */
typedef struct Ewl_Event_Window_Expose Ewl_Event_Window_Expose;

/**
 * Describes the evas region that received an expose notification.
 */
struct Ewl_Event_Window_Expose
{
	int x; /**< Beginning X coordinate for the expose rectangle */
	int y; /**< Beginning Y coordinate for the expose rectangle */
	int w; /**< Width of the expose rectangle */
	int h; /**< Height of the expose rectangle */
};

/**
 * The Ewl_Event_Window_Configure type
 */
typedef struct Ewl_Event_Window_Configure Ewl_Event_Window_Configure;

/**
 * Notifies widgets when the enclosing window size has changed.
 */
struct Ewl_Event_Window_Configure
{
	int x; /**< Beginning X coordinate of the windows new position */
	int y; /**< Beginning Y coordinate of the windows new position */
	int w; /**< The new width of the window */
	int h; /**< The new height of the window */
};

/**
 * The Ewl_Event_Window_Delete type
 */
typedef struct Ewl_Event_Window_Delete Ewl_Event_Window_Delete;

/**
 * Notifies of window close requests.
 */
struct Ewl_Event_Window_Delete
{
	int ignore;	/**< Unused. */
};

/**
 *  The Ewl_Event_Window_Key_Down structure 
 */
typedef struct Ewl_Event_Key_Down Ewl_Event_Key_Down;

/**
 * Provides clients with necessary information about the key press event.
 */
struct Ewl_Event_Key_Down
{
	unsigned int modifiers; /**< Modifiers that were pressed */
	char *keyname; /**< Name of the key that was pressed */
};

/**
 * The Ewl_Event_Key_Up structure
 */
typedef struct Ewl_Event_Key_Up Ewl_Event_Key_Up;

/**
 * Provides clients with necessary information about the key release event.
 */
struct Ewl_Event_Key_Up
{
	unsigned int modifiers; /**< Modifiers that were pressed */
	char *keyname; /**< Name of the key that was released */
};

/**
 * The Ewl_Event_Mouse_Down structure
 */
typedef struct Ewl_Event_Mouse_Down Ewl_Event_Mouse_Down;

/**
 * Provides information about the mouse down event.
 */
struct Ewl_Event_Mouse_Down
{
	unsigned int modifiers; /**< Modifiers that were pressed */
	int button; /**< The mouse button that was pressed */
	int clicks; /**< Number of consecutive clicks */
	int x; /**< X coordinate the mouse press occurred at */
	int y; /**< Y coordinate the mouse press occurred at */
};

/**
 * The Ewl_Event_Mouse_Up structure
 */
typedef struct Ewl_Event_Mouse_Up Ewl_Event_Mouse_Up;

/**
 * Provides information about the mouse up event.
 */
struct Ewl_Event_Mouse_Up
{
	unsigned int modifiers; /**< Modifiers that were pressed */
	int button; /**< The mouse button that was released */
	int x; /**< X coordinate the mouse release occurred at */
	int y; /**< Y coordinate the mouse release occurred at */
};

/**
 * The Ewl_Event_Mouse_Move structure
 */
typedef struct Ewl_Event_Mouse_Move Ewl_Event_Mouse_Move;

/**
 * Provides information about mouse movement
 */
struct Ewl_Event_Mouse_Move
{
	unsigned int modifiers; /**< Modifiers that were pressed */
	int x; /**< X coordinate the mouse moved to */
	int y; /**< Y coordinate the mouse moved to */
};

/**
 * The Ewl_Event_Mouse_In structure
 */
typedef struct Ewl_Event_Mouse_In Ewl_Event_Mouse_In;

/**
 * Provides information about the mouse entering
 */
struct Ewl_Event_Mouse_In
{
	unsigned int modifiers; /**< Modifiers that were pressed */
	int x; /**< X coordinate the mouse entered at */
	int y; /**< Y coordinate the mouse entered at */
};

/**
 * The Ewl_Event_Mouse_Out structure
 */
typedef struct Ewl_Event_Mouse_Out Ewl_Event_Mouse_Out;

/**
 * Provides information about the mouse leaving
 */
struct Ewl_Event_Mouse_Out
{
	unsigned int modifiers; /**< Modifiers that were pressed */
	int x; /**< X coordinate the mouse left at */
	int y; /**< Y coordinate the mouse left at */
};

/**
 * The Ewl_Event_Mouse_Wheel structure
 */
typedef struct Ewl_Event_Mouse_Wheel Ewl_Event_Mouse_Wheel;

/**
 * Provides information about the mouse wheel scrolling
 */
struct Ewl_Event_Mouse_Wheel
{
	unsigned int modifiers; /**< Modifiers that were pressed */
	int x; /**< X coordinate the mouse left at */
	int y; /**< Y coordinate the mouse left at */
	int z; /**< Z value of mouse wheel */
	int dir; /**< Direction mouse wheel scrolled */
};

/**
 * The Ewl_Dnd_Types type 
 */
typedef struct Ewl_Dnd_Types Ewl_Dnd_Types;

/**
 * Provides type information about an external DND drag
 */
struct Ewl_Dnd_Types
{
	int num_types;	/**< The number of DND types */
	char** types;	/**< The list of DND types */
	
};

/**
 * Provides information about dnd drops movement
 */
struct Ewl_Event_Dnd_Drop
{
        int x; /**< X coordinate the mouse moved to */
        int y; /**< Y coordinate the mouse moved to */
	void* data; /**< Data from drop source 	    */
};

/**
 * The Ewl_Event_Dnd_Drop type
 */
typedef struct Ewl_Event_Dnd_Drop Ewl_Event_Dnd_Drop;

/**
 * The Ewl_Dialog_Evenet type
 */
typedef struct Ewl_Dialog_Event Ewl_Dialog_Event;

/**
 * Stores the reponse from a dialog
 */
struct Ewl_Dialog_Event
{
	unsigned int response; /**< The response ID from the dialog */
};

/**
 * The Ewl_Filepicker_Event type
 */
typedef struct Ewl_Filepicker_Event Ewl_Filepicker_Event;

/**
 * Contains the filepicker response
 */
struct Ewl_Filepicker_Event
{
	unsigned int response;	/**< The filepicker response */
};

/**
 * The Ewl_Filelist_Event type
 */
typedef struct Ewl_Filelist_Event Ewl_Filelist_Event;

/**
 * Contains the filelist event type
 */
struct Ewl_Filelist_Event
{
	Ewl_Filelist_Event_Type type;	/**< The type of event */
};

int		ewl_ev_init(void);
unsigned int 	ewl_ev_modifiers_get(void);
void 		ewl_ev_modifiers_set(unsigned int modifiers);

/**
 * @}
 */

#endif

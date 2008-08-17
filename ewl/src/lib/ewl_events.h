/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_EVENTS_H
#define EWL_EVENTS_H

/**
 * @addtogroup Ewl_Events Ewl_Events: Lower Level Event Handlers
 * @brief Defines the routines that dispatch the lower level events to EWL.
 *
 * @{
 */

/**
 * @def EWL_EVENT_WINDOW_EXPOSE(e)
 * Typedcasts a pointer to an Ewl_Event_Window_Expose pointer
 */
#define EWL_EVENT_WINDOW_EXPOSE(e) ((Ewl_Event_Window_Expose *)(e))

/**
 * The Ewl_Event_Window_Expose structure
 */
typedef struct Ewl_Event_Window_Expose Ewl_Event_Window_Expose;

/**
 * @brief Describes the canvas region that received an expose notification.
 */
struct Ewl_Event_Window_Expose
{
        int x; /**< Beginning X coordinate for the expose rectangle */
        int y; /**< Beginning Y coordinate for the expose rectangle */
        int w; /**< Width of the expose rectangle */
        int h; /**< Height of the expose rectangle */
};

/**
 * @def EWL_EVENT_KEY(e)
 * Typedcasts a pointer to an Ewl_Event_Key pointer
 */
#define EWL_EVENT_KEY(e) ((Ewl_Event_Key *)(e))


/**
 * The Ewl_Event_Key structure
 */
typedef struct Ewl_Event_Key Ewl_Event_Key;

/**
 * @brief Provides the basic information about key events
 */
struct Ewl_Event_Key
{
        unsigned int modifiers;        /**< Key modifiers that were pressed */
        char *keyname;                /**< Name of the key that was pressed */
};

/**
 * @def EWL_EVENT_KEY_DOWN(e)
 * Typedcasts a pointer to an Ewl_Event_Key_Down pointer
 */
#define EWL_EVENT_KEY_DOWN(e) ((Ewl_Event_Key_Down *)(e))

/**
 *  The Ewl_Event_Window_Key_Down structure
 */
typedef struct Ewl_Event_Key_Down Ewl_Event_Key_Down;

/**
 * @brief Provides clients with necessary information about the key press event.
 */
struct Ewl_Event_Key_Down
{
        Ewl_Event_Key base;        /**< Basic key information */
};

/**
 * @def EWL_EVENT_KEY_UP(e)
 * Typedcasts a pointer to an Ewl_Event_Key_Up pointer
 */
#define EWL_EVENT_KEY_UP(e) ((Ewl_Event_Key_Up *)(e))

/**
 * The Ewl_Event_Key_Up structure
 */
typedef struct Ewl_Event_Key_Up Ewl_Event_Key_Up;

/**
 * @brief Provides clients with necessary information about the key release event.
 */
struct Ewl_Event_Key_Up
{
        Ewl_Event_Key base;        /**< Basic key information */
};

/**
 * @def EWL_EVENT_MOUSE(e)
 * Typedcasts a pointer to an Ewl_Event_Mouse pointer
 */
#define EWL_EVENT_MOUSE(e) ((Ewl_Event_Mouse *)(e))

/**
 * The Ewl_Event_Mouse structure
 */
typedef struct Ewl_Event_Mouse Ewl_Event_Mouse;

/**
 * @brief Provides base mouse information
 */
struct Ewl_Event_Mouse
{
        unsigned int modifiers; /**< Modifiers that were pressed */
        int x;                        /**< X coordinate the mouse release occurred at */
        int y;                        /**< Y coordinate the mouse release occurred at */
};

/**
 * @def EWL_EVENT_MOUSE_DOWN(e)
 * Typedcasts a pointer to an Ewl_Event_Mouse_Down pointer
 */
#define EWL_EVENT_MOUSE_DOWN(e) ((Ewl_Event_Mouse_Down *)(e))

/**
 * The Ewl_Event_Mouse_Down structure
 */
typedef struct Ewl_Event_Mouse_Down Ewl_Event_Mouse_Down;

/**
 * @brief Provides information about the mouse down event.
 */
struct Ewl_Event_Mouse_Down
{
        Ewl_Event_Mouse base;        /**< Base mouse information */
        int button;                /**< The mouse button that was released */
        int clicks;                /**< Number of consecutive clicks */
};

/**
 * @def EWL_EVENT_MOUSE_UP(e)
 * Typedcasts a pointer to an Ewl_Event_Mouse_Up pointer
 */
#define EWL_EVENT_MOUSE_UP(e) ((Ewl_Event_Mouse_Up *)(e))

/**
 * The Ewl_Event_Mouse_Up structure
 */
typedef struct Ewl_Event_Mouse_Up Ewl_Event_Mouse_Up;

/**
 * @brief Provides information about the mouse up event.
 */
struct Ewl_Event_Mouse_Up
{
        Ewl_Event_Mouse base;        /**< Base mouse information */
        int button;                /**< The mouse button that was released */
};

/**
 * @def EWL_EVENT_MOUSE_MOVE(e)
 * Typedcasts a pointer to an Ewl_Event_Mouse_Move pointer
 */
#define EWL_EVENT_MOUSE_MOVE(e) ((Ewl_Event_Mouse_Move *)(e))

/**
 * The Ewl_Event_Mouse_Move structure
 */
typedef struct Ewl_Event_Mouse_Move Ewl_Event_Mouse_Move;

/**
 * @brief Provides information about mouse movement
 */
struct Ewl_Event_Mouse_Move
{
        Ewl_Event_Mouse base;        /**< Base mouse information */
};

/**
 * @def EWL_EVENT_MOUSE_IN(e)
 * Typedcasts a pointer to an Ewl_Event_Mouse_In pointer
 */
#define EWL_EVENT_MOUSE_IN(e) ((Ewl_Event_Mouse_In *)(e))

/**
 * The Ewl_Event_Mouse_In structure
 */
typedef struct Ewl_Event_Mouse_In Ewl_Event_Mouse_In;

/**
 * @brief Provides information about the mouse entering
 */
struct Ewl_Event_Mouse_In
{
        Ewl_Event_Mouse base;        /**< Base mouse information */
};

/**
 * @def EWL_EVENT_MOUSE_OUT(e)
 * Typedcasts a pointer to an Ewl_Event_Mouse_Out pointer
 */
#define EWL_EVENT_MOUSE_OUT(e) ((Ewl_Event_Mouse_Out *)(e))

/**
 * The Ewl_Event_Mouse_Out structure
 */
typedef struct Ewl_Event_Mouse_Out Ewl_Event_Mouse_Out;

/**
 * @brief Provides information about the mouse leaving
 */
struct Ewl_Event_Mouse_Out
{
        Ewl_Event_Mouse base; /**< Base mouse information */
};

/**
 * @def EWL_EVENT_MOUSE_WHEEL(e)
 * Typedcasts a pointer to an Ewl_Event_Mouse_Wheel pointer
 */
#define EWL_EVENT_MOUSE_WHEEL(e) ((Ewl_Event_Mouse_Wheel *)(e))

/**
 * The Ewl_Event_Mouse_Wheel structure
 */
typedef struct Ewl_Event_Mouse_Wheel Ewl_Event_Mouse_Wheel;

/**
 * @brief Provides information about the mouse wheel scrolling
 */
struct Ewl_Event_Mouse_Wheel
{
        Ewl_Event_Mouse base;        /**< Base mouse information */
        int z;                        /**< Z value of mouse wheel */
        int dir;                /**< Direction mouse wheel scrolled */
};

/**
 * @def EWL_DND_TYPES(t)
 * Typedcasts a pointer to an Ewl_Dnd_Types pointer
 */
#define EWL_DND_TYPES(t) ((Ewl_Dnd_Types *)(t))

/**
 * The Ewl_Dnd_Types type
 */
typedef struct Ewl_Dnd_Types Ewl_Dnd_Types;

/**
 * @brief Provides type information about an external DND drag
 */
struct Ewl_Dnd_Types
{
        int num_types;        /**< The number of DND types */
        char** types;        /**< The list of DND types */
};

/**
 * @def EWL_EVENT_DND_POSITION(e)
 * Typedcasts a pointer to an Ewl_Event_Dnd_Position pointer
 */
#define EWL_EVENT_DND_POSITION(e) ((Ewl_Event_Dnd_Position *)(e))

/**
 * The Ewl_Event_Dnd_Position type
 */
typedef struct Ewl_Event_Dnd_Position Ewl_Event_Dnd_Position;

/**
 * @brief Provides information about dnd drops movement
 */
struct Ewl_Event_Dnd_Position
{
        int x; /**< X coordinate the mouse moved to */
        int y; /**< Y coordinate the mouse moved to */
};

/**
 * @def EWL_EVENT_DND_DROP(e)
 * Typedcasts a pointer to an Ewl_Event_Dnd_Drop pointer
 */
#define EWL_EVENT_DND_DROP(e) ((Ewl_Event_Dnd_Drop *)(e))

/**
 * The Ewl_Event_Dnd_Drop type
 */
typedef struct Ewl_Event_Dnd_Drop Ewl_Event_Dnd_Drop;

/**
 * @brief Provides information about dnd drops movement
 */
struct Ewl_Event_Dnd_Drop
{
        int x; /**< X coordinate the mouse moved to */
        int y; /**< Y coordinate the mouse moved to */
        void* data; /**< Data from drop source             */
};

/**
 * @def EWL_EVENT_DND_DATA_RECEIVED(e)
 * Typedcasts a pointer to an Ewl_Event_Dnd_Data_Received pointer
 */
#define EWL_EVENT_DND_DATA_RECEIVED(e) ((Ewl_Event_Dnd_Data_Received *)(e))

/**
 * The Ewl_Event_Dnd_Data_Received type
 */
typedef struct Ewl_Event_Dnd_Data_Received Ewl_Event_Dnd_Data_Received;

/**
 * @brief Provides information about dnd drop data
 */
struct Ewl_Event_Dnd_Data_Received
{
        char *type; /**< Type of data from drop source */
        void *data; /**< Data from drop source */
        unsigned int len; /**< Length of received data */
        unsigned int format; /**< Bit format of received data */
};

/**
 * @def EWL_EVENT_DND_DATA_REQUEST(e)
 * Typedcasts a pointer to an Ewl_Event_Dnd_Data_Request pointer
 */
#define EWL_EVENT_DND_DATA_REQUEST(e) ((Ewl_Event_Dnd_Data_Request *)(e))

/**
 * The Ewl_Event_Dnd_Data_Requested type
 */
typedef struct Ewl_Event_Dnd_Data_Request Ewl_Event_Dnd_Data_Request;

/**
 * @brief Provides information about dnd data requests
 */
struct Ewl_Event_Dnd_Data_Request
{
        void *handle; /**< Engine specific handle for responding to request */
        char *type;   /**< Type of data requested */
};

/**
 * @def EWL_EVENT_ACTION_RESPONSE(e)
 * Typedcasts a pointer to an Ewl_Event_Action_Response pointer
 */
#define EWL_EVENT_ACTION_RESPONSE(e) ((Ewl_Event_Action_Response*)(e))

/**
 * The Ewl_Event_Action_Response type
 */
typedef struct Ewl_Event_Action_Response Ewl_Event_Action_Response;

/**
 * @brief Stores the reponse from a user action
 */
struct Ewl_Event_Action_Response
{
        unsigned int response; /**< The response ID */
};

/**
 * @def EWL_EVENT_STATE_CHANGE(e)
 * Typedefs a pointer to an Ewl_Event_State_Change pointer
 */
#define EWL_EVENT_STATE_CHANGE(e) ((Ewl_Event_State_Change*)(e))

/**
 * The Ewl_Event_State_Change type
 */
typedef struct Ewl_Event_State_Change Ewl_Event_State_Change;

/**
 * @brief Provides information about the changed state
 */
struct Ewl_Event_State_Change
{
        const char *state;
        Ewl_State_Type flag;
};

unsigned int         ewl_ev_modifiers_get(void);
void                 ewl_ev_modifiers_set(unsigned int modifiers);

/**
 * @}
 */

#endif

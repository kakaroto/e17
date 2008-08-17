/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_WIDGET_H
#define EWL_WIDGET_H

/**
 * @addtogroup Ewl_Widget Ewl_Widget: The Parent Widget Class Common to All Widgets
 * @brief Defines the Ewl_Widget class and it's accessor/modifier functions.
 *
 * The Ewl_Widget extends the Ewl_Object to provide the basic facilities
 * necessary for widgets to interact with the end user. This includes basic
 * callbacks for input events, window information changes, and drawing to the
 * display.
 *
 * @remarks Inherits from Ewl_Object.
 * @if HAVE_IMAGES
 * @image html Ewl_Widget_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_WIDGET_TYPE
 * The type name for the Ewl_Widget widget
 */
#define EWL_WIDGET_TYPE "widget"

/**
 * @def EWL_WIDGET_IS(w)
 * Returns TRUE if the widget is an Ewl_Widget, FALSE otherwise
 */
#define EWL_WIDGET_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_WIDGET_TYPE))

/**
 * The attachment list
 */
typedef struct Ewl_Attach_List Ewl_Attach_List;

/**
 * @brief A list of things attached to a widget
 */
struct Ewl_Attach_List
{
        void **list;                /**< The attachment list */
        unsigned int direct:1;      /**< Is this a direct pointer to an attached item */
        unsigned int len:31;        /**< Number of things in the attachment list */
};

/**
 * A key/value pair set
 */
typedef struct Ewl_Pair Ewl_Pair;

/**
 * @def EWL_PAIR(p)
 * Typedcasts a pointer to an Ewl_Pair pointer
 */
#define EWL_PAIR(p) ((Ewl_Pair *)p)

/**
 * @brief Contains a key and a value pair
 */
struct Ewl_Pair
{
        const char *key;        /**< The key */
        char *value;            /**< The value */
};

/**
 * A list of key value pairs
 */
typedef struct Ewl_Pair_List Ewl_Pair_List;

/**
 * @brief Contains a list of key value pairs
 */
struct Ewl_Pair_List
{
        Ewl_Pair **list;        /**< The key/value list */
        unsigned int direct:1;  /**< Does this list point directly to a pair */
        unsigned int len:31;    /**< Length of the list */
};

/**
 * Callback chain contains a list and bitmask of chain properties.
 */
typedef struct Ewl_Callback_Chain Ewl_Callback_Chain;

/**
 * @brief The callback chain contains the length, mask and information on the list
 */
struct Ewl_Callback_Chain
{
        void **list;            /**< The callback list */
        unsigned char mask;     /**< The callback mask */
        unsigned char len;      /**< Length of the list */
        unsigned char index;    /**< Current list index */
};

/**
 * A set of colours
 */
typedef struct Ewl_Color_Set Ewl_Color_Set;

/**
 * @brief Contains an RGBA set of colours
 */
struct Ewl_Color_Set
{
        unsigned char r,        /**< The red value */
                      g,        /**< The green value */
                      b,        /**< The blue value */
                      a;        /**< The alpha value */
};

/**
 * Function pointer for the Ewl widget drag
 */
typedef void *(*Ewl_Widget_Drag)(void);

/**
 * The class that all widgets should inherit. Provides reference to a parent
 * widget/container, callbacks, and appearance information.
 */
typedef struct Ewl_Widget Ewl_Widget;

/**
 * @def EWL_WIDGET(widget)
 * @brief Typecast a pointer to an Ewl_Widget pointer.
 */
#define EWL_WIDGET(widget) ((Ewl_Widget *) widget)

/**
 * @brief Inherits from Ewl_Object and extends to provide appearance, parent, and
 * callback capabilities.
 */
struct Ewl_Widget
{
        Ewl_Object object; /**< Inherit the base Object class */
        Ewl_Widget *parent; /**< The parent widget, actually a container */

        Ewl_Callback_Chain callbacks[EWL_CALLBACK_MAX + 1];
                                                /**< Callback chain array */
        Ewl_Attach_List *attach;       /**< List of attachments on the widget */

        void *smart_object; /**< Smart Object for the layer stuff */
        void *fx_clip_box;  /**< Clipping rectangle of widget */

        void *theme_object; /**< Appearance shown on canvas */
        const char *theme_path;     /**< Path to the file for loading */
        const char *theme_group;    /**< Group in theme to use */
        const char *theme_state;    /**< State of the appearance */
        const char *appearance;   /**< Key to lookup appearance in theme */
        const char *inheritance;  /**< Inheritance of path widget */
        int layer;                /**< the layer relative to the parent */

        Ecore_Hash *theme;          /**< Overriding theme settings */
        Ewl_Pair_List theme_text;   /**< Overriding text in theme */
        unsigned int flags;         /**< the widget flags */
};

Ewl_Widget      *ewl_widget_new(void);
int              ewl_widget_init(Ewl_Widget *w);
void             ewl_widget_free(Ewl_Widget *w);

void             ewl_widget_name_set(Ewl_Widget *w, const char *name);
const char      *ewl_widget_name_get(Ewl_Widget *w);
Ewl_Widget      *ewl_widget_name_find(const char *name);

void             ewl_widget_reparent(Ewl_Widget *widget);
void             ewl_widget_realize(Ewl_Widget *widget);
void             ewl_widget_unrealize(Ewl_Widget *w);
void             ewl_widget_reveal(Ewl_Widget *w);
void             ewl_widget_obscure(Ewl_Widget *w);
void             ewl_widget_show(Ewl_Widget *widget);
void             ewl_widget_hide(Ewl_Widget *widget);
void             ewl_widget_destroy(Ewl_Widget *widget);
void             ewl_widget_configure(Ewl_Widget *widget);

void             ewl_widget_data_set(Ewl_Widget *w, void *k, void *v);
void            *ewl_widget_data_del(Ewl_Widget *w, void *k);
void            *ewl_widget_data_get(Ewl_Widget *w, void *k);

void             ewl_widget_state_set(Ewl_Widget *w, const char *state,
                                                Ewl_State_Type flag);

void             ewl_widget_appearance_set(Ewl_Widget *w, const char *appearance);
const char      *ewl_widget_appearance_get(Ewl_Widget *w);
char            *ewl_widget_appearance_path_get(Ewl_Widget *w);
int              ewl_widget_appearance_path_size_get(Ewl_Widget *w);
int              ewl_widget_appearance_path_copy(Ewl_Widget *w, char *buf,
                                                int size);
void             ewl_widget_appearance_part_text_set(Ewl_Widget *w, 
                                                     const char *part,
                                                     const char *text);
const char      *ewl_widget_appearance_part_text_get(Ewl_Widget *w, const char *part);
void             ewl_widget_appearance_text_set(Ewl_Widget *w, const char *text);
const char      *ewl_widget_appearance_text_get(Ewl_Widget *w);

void             ewl_widget_inherit(Ewl_Widget *widget, const char *type);

unsigned int     ewl_widget_type_is(Ewl_Widget *widget, const char *type);
unsigned int     ewl_widget_onscreen_is(Ewl_Widget *widget);

void             ewl_widget_parent_set(Ewl_Widget *w, Ewl_Widget *p);
Ewl_Widget      *ewl_widget_parent_get(Ewl_Widget *w);

void             ewl_widget_enable(Ewl_Widget *w);
void             ewl_widget_disable(Ewl_Widget *w);

void             ewl_widget_print(Ewl_Widget *w);
void             ewl_widget_print_verbose(Ewl_Widget *w);
void             ewl_widget_tree_print(Ewl_Widget *w);

void             ewl_widget_layer_top_set(Ewl_Widget *w, int top);
int              ewl_widget_layer_top_get(Ewl_Widget *w);
void             ewl_widget_layer_priority_set(Ewl_Widget *w, int layer);
int              ewl_widget_layer_priority_get(Ewl_Widget *w);

void             ewl_widget_internal_set(Ewl_Widget *w, unsigned int val);
unsigned int     ewl_widget_internal_is(Ewl_Widget *w);

void             ewl_widget_unmanaged_set(Ewl_Widget *w, unsigned int val);
unsigned int     ewl_widget_unmanaged_is(Ewl_Widget *w);

void             ewl_widget_clipped_set(Ewl_Widget *w, unsigned int val);
unsigned int     ewl_widget_clipped_is(Ewl_Widget *w);

void             ewl_widget_focus_send(Ewl_Widget *w);
Ewl_Widget      *ewl_widget_focused_get(void);

void             ewl_widget_tab_order_append(Ewl_Widget *w);
void             ewl_widget_tab_order_prepend(Ewl_Widget *w);
void             ewl_widget_tab_order_insert(Ewl_Widget *w, unsigned int idx);
void             ewl_widget_tab_order_insert_before(Ewl_Widget *w, Ewl_Widget *before);
void             ewl_widget_tab_order_insert_after(Ewl_Widget *w, Ewl_Widget *after);
void             ewl_widget_tab_order_remove(Ewl_Widget *w);

void             ewl_widget_focusable_set(Ewl_Widget *w, unsigned int val);
unsigned int     ewl_widget_focusable_get(Ewl_Widget *w);

void             ewl_widget_ignore_focus_change_set(Ewl_Widget *w, unsigned int val);
unsigned int     ewl_widget_ignore_focus_change_get(Ewl_Widget *w);

void             ewl_widget_color_set(Ewl_Widget *w, unsigned int r, unsigned int g,
                                                        unsigned int b, unsigned int a);
void             ewl_widget_color_get(Ewl_Widget *w, unsigned int *r, unsigned int *g,
                                                        unsigned int *b, unsigned int *a);

int              ewl_widget_parent_of(Ewl_Widget *c, Ewl_Widget *w);

void             ewl_widget_flags_add(Ewl_Widget *o, unsigned int flags,
                                      unsigned int mask);
void             ewl_widget_flags_remove(Ewl_Widget *o, unsigned int flags,
                                         unsigned int mask);

/**
 * @param o: the parameter to retrieve the current value of widget flags
 * @param mask: get only the flags specified in mask
 * @return Returns the current setting of the widget flags for @a o.
 * @brief Retrieves the current setting of the widget flags for @a o.
 */
#define ewl_widget_flags_get(o, mask) \
        (EWL_WIDGET(o)->flags & mask)

/**
 * @param o: the widget to check for a specified flags
 * @param check_flags: the bitmask of flags to check on the widget
 * @param mask: get only the flags specified in mask
 * @return Returns TRUE if any of the specified flags are set, FALSE otherwise.
 * @brief Determines if widget has the requested @a flags set.
 */
#define ewl_widget_flags_has(o, check_flags, mask) \
        (!!(EWL_WIDGET(o)->flags & ((check_flags) & mask)))

/**
 * @param o: the widget to check for a specified flags
 * @param check_flags: the bitmask of flags to check on the widget
 * @param mask: get only the flags specified in mask
 * @return Returns TRUE if the specified flags are set, FALSE otherwise.
 * @brief Determines if widget has all of the requested @a flags set.
 */
#define ewl_widget_flags_has_all(o, check_flags, mask) \
        ((EWL_WIDGET(o)->flags & ((check_flags) & mask)) == ((check_flags) & mask))


/**
 * @def ewl_widget_recursive_set(o)
 * @param o: the widget to change the recursive flag
 * @param val: a boolean indicating the value of the recursive flag
 * @return Returns no value.
 * @brief Changes the recursive flag value to match @a val.
 */
#define ewl_widget_recursive_set(o, val) \
        (val ? ewl_widget_flags_add(o, EWL_FLAG_PROPERTY_RECURSIVE, \
                                    EWL_FLAGS_PROPERTY_MASK) : \
         ewl_widget_flags_remove(o, EWL_FLAG_PROPERTY_RECURSIVE, \
                                    EWL_FLAGS_PROPERTY_MASK));

/**
 * @def ewl_widget_recursive_get(o)
 * @param o: the parameter to retrieve the current value of recursive flag
 * @return Returns the current setting of the recursive flag for @a o.
 * @brief Retrieves the current setting of the recursive flag for @a o.
 */
#define ewl_widget_recursive_get(o) \
        (ewl_widget_flags_get(o, EWL_FLAG_PROPERTY_RECURSIVE))

/**
 * @def ewl_widget_toplevel_set(o, val)
 * @param o: the widget to change the top level flag
 * @param val: a boolean indicating the value of the top level flag
 * @return Returns no value.
 * @brief Changes the top level flag value to match @a val.
 */
#define ewl_widget_toplevel_set(o, val) \
        (val ? ewl_widget_flags_add(o, EWL_FLAG_PROPERTY_TOPLEVEL, \
                                    EWL_FLAGS_PROPERTY_MASK) : \
         ewl_widget_flags_remove(o, EWL_FLAG_PROPERTY_TOPLEVEL, \
                                    EWL_FLAGS_PROPERTY_MASK));

/**
 * @def ewl_widget_toplevel_get(o)
 * @param o: the parameter to retrieve the current value of top level flag
 * @return Returns the current setting of the top level flag for @a o.
 * @brief Retrieves the current setting of the top level flag for @a o.
 */
#define ewl_widget_toplevel_get(o) \
        (ewl_widget_flags_get(o, EWL_FLAG_PROPERTY_TOPLEVEL))

/**
 * @def ewl_widget_state_add(o, state)
 * @param o: The widget to work with
 * @param state: The state to set into the widget
 * Adds the given state @a state to the widget @a o
 */
#define ewl_widget_state_add(o, state) \
        ewl_widget_flags_add(o, state, EWL_FLAGS_STATE_MASK)

/**
 * @def ewl_widget_state_remove(o, state)
 * @param o: The widget to work with
 * @param state: The state to remove
 * Removes the given state from the given @a o widget
 */
#define ewl_widget_state_remove(o, state) \
        ewl_widget_flags_remove(o, state, EWL_FLAGS_STATE_MASK)

/**
 * @def ewl_widget_state_has(o, state)
 * @param o: The widget to check
 * @param state: The state to check
 * Checks if the given state @a state is set on the given widget @a o
 */
#define ewl_widget_state_has(o, state) \
        ewl_widget_flags_has(o, state, EWL_FLAGS_STATE_MASK)

/**
 * @def ewl_widget_state_get(o, state)
 * @param o: The widget to work with
 * @param state: The state to get
 * Retrives the given state @a state from the widget @a o
 */
#define ewl_widget_state_get(o, state) \
        ewl_widget_flags_get(o, state, EWL_FLAGS_STATE_MASK)

/**
 * @def ewl_widget_queued_add(o, queued)
 * @param o: The widget to work with
 * @param queued: Add the given queue flag to the widget
 * Adds the given queue flag @a queued to the widget @a o
 */
#define ewl_widget_queued_add(o, queued) \
        ewl_widget_flags_add(o, queued, EWL_FLAGS_QUEUED_MASK)

/**
 * @def ewl_widget_queued_remove(o, queued)
 * Remove the @a queued flag from the @a o widget
 */
#define ewl_widget_queued_remove(o, queued) \
        ewl_widget_flags_remove(o, queued, EWL_FLAGS_QUEUED_MASK)

/**
 * @def ewl_widget_queued_has(o, queued)
 * Determine if the @a o widget has the @a queued flag set
 */
#define ewl_widget_queued_has(o, queued) \
        ewl_widget_flags_has(o, queued, EWL_FLAGS_QUEUED_MASK)

/**
 * @def ewl_widget_queued_get(o, queued)
 * Retrieve the value for the @a queued queue flag
 */
#define ewl_widget_queued_get(o, queued) \
        ewl_widget_flags_get(o, queued, EWL_FLAGS_QUEUED_MASK)

/**
 * @def ewl_widget_visible_add(o, visible)
 * Add the @a visible flag to the widget @a o
 */
#define ewl_widget_visible_add(o, visible) \
        ewl_widget_flags_add(o, visible, EWL_FLAGS_VISIBLE_MASK)

/**
 * @def ewl_widget_visible_remove(o, visible)
 * Remove the @a visible flag from the widget @a o
 */
#define ewl_widget_visible_remove(o, visible) \
        ewl_widget_flags_remove(o, visible, EWL_FLAGS_VISIBLE_MASK)

/**
 * @def ewl_widget_visible_has(o, visible)
 * Check if the @a visible flag is set in the widget @a o
 */
#define ewl_widget_visible_has(o, visible) \
        ewl_widget_flags_has(o, visible, EWL_FLAGS_VISIBLE_MASK)

/**
 * @def ewl_widget_visible_get(o, visible)
 * Retrieves the @a visble flag from the widget @a o
 */
#define ewl_widget_visible_get(o, visible) \
        ewl_widget_flags_get(o, visible, EWL_FLAGS_VISIBLE_MASK)

/**
 * @def RECURSIVE(o)
 * Used to test if a widget is recursive, aka. an Ewl_Container
 */
#define RECURSIVE(o) (EWL_WIDGET(o)->flags & EWL_FLAG_PROPERTY_RECURSIVE)

/**
 * @def REALIZED(o)
 * Used to test if a widget has been realized.
 */
#define REALIZED(o) (EWL_WIDGET(o)->flags & EWL_FLAG_VISIBLE_REALIZED)

/**
 * @def VISIBLE(o)
 * Used to test if a widget is visible.
 */
#define VISIBLE(o) (EWL_WIDGET(o)->flags & EWL_FLAG_VISIBLE_SHOWN)

/**
 * @def REVEALED(o)
 * Used to determine if a widget is marked as revealed.
 */
#define REVEALED(o) (EWL_WIDGET(o)->flags & EWL_FLAG_VISIBLE_REVEALED)

/**
 * @def HIDDEN(o)
 * Used to determine if a widget is hidden.
 */
#define HIDDEN(o) (!(EWL_WIDGET(o)->flags & EWL_FLAG_VISIBLE_SHOWN))

/**
 * @def DESTROYED(o)
 * Used to determine if a widget has been destroyed
 */
#define DESTROYED(o) (ewl_widget_queued_has(EWL_WIDGET(o), \
                                        EWL_FLAG_QUEUED_SCHEDULED_DESTROY) \
                        || ewl_widget_queued_has(EWL_WIDGET(o), \
                                        EWL_FLAG_QUEUED_PROCESS_DESTROY))

/**
 * @def CONFIGURED(o)
 * Used to determine if a widget is scheduled for configure
 */
#define CONFIGURED(o) (ewl_widget_queued_has(EWL_WIDGET(o), \
                                        EWL_FLAG_QUEUED_SCHEDULED_CONFIGURE) \
                        || ewl_widget_queued_has(EWL_WIDGET(o), \
                                        EWL_FLAG_QUEUED_PROCESS_CONFIGURE))

/**
 * @def DISABLED(o)
 * Used to determine if a widget is disabled
 */
#define DISABLED(o) (ewl_widget_state_has(EWL_WIDGET(o), EWL_FLAG_STATE_DISABLED))

/**
 * @def ewl_widget_in_tab_list_get(o)
 * @param o: the parameter to retrieve the current value of the in tab list flag
 * @return Returns the current setting of the in tab list flag for @a o.
 * @brief Retrieves the current setting of the in tab list flag for @a o.
 */
#define ewl_widget_in_tab_list_get(o) \
        (ewl_widget_flags_get(o, EWL_FLAG_PROPERTY_IN_TAB_LIST))

/**
 * @def ewl_widget_in_tab_list_set(o, val)
 * @param o: the widget to change the in tab list
 * @param val: a boolean indicating the value of the tab list flag
 * @return Returns no value.
 * @brief Changes the tab list flag value to match @a val.
 */
#define ewl_widget_in_tab_list_set(o, val) \
        (val ? ewl_widget_flags_add(o, EWL_FLAG_PROPERTY_IN_TAB_LIST, \
                                    EWL_FLAGS_PROPERTY_MASK) : \
         ewl_widget_flags_remove(o, EWL_FLAG_PROPERTY_IN_TAB_LIST, \
                                    EWL_FLAGS_PROPERTY_MASK));



#define UNMANAGED(w) (EWL_WIDGET(w)->flags & EWL_FLAG_PROPERTY_UNMANAGED)
#define TOPLAYERED(w) (EWL_WIDGET(w)->flags & EWL_FLAG_PROPERTY_TOPLAYERED)

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_widget_cb_show(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_hide(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_reveal(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_obscure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_realize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_unrealize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_reparent(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_enable(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_disable(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_focus_in(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_focus_out(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_mouse_in(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_mouse_out(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_mouse_down(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_mouse_up(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_cb_mouse_move(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif

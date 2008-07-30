/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_text.h"
#include "ewl_text_trigger.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

/**
 * Typdef for the Ewl_Text_Trigger_Area struct
 */
typedef struct Ewl_Text_Trigger_Area Ewl_Text_Trigger_Area;

/**
 * @def EWL_TEXT_TRIGGER_AREA_TYPE
 * The type name for the Ewl_Text_Area_Trigger widget
 */
#define EWL_TEXT_TRIGGER_AREA_TYPE "trigger_area"

/**
 * @def EWL_TEXT_TRIGGER_AREA_IS(w)
 * Returns TRUE if the widget is an Ewl_Text_Area_Trigger, FALSE otherwise
 */
#define EWL_TEXT_TRIGGER_AREA_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TEXT_TRIGGER_AREA_TYPE))


/**
 * @def EWL_TEXT_TRIGGER_AREA(area)
 * Typecasts a pointer to an Ewl_Text_Trigger_Area pointer
 */
#define EWL_TEXT_TRIGGER_AREA(area) ((Ewl_Text_Trigger_Area *) area)

/**
 * @brief Inherits from Ewl_Widget and extends to provide a trigger area
 */
struct Ewl_Text_Trigger_Area
{
        Ewl_Widget widget;                /**< Inherits from Ewl_Widget */
        Ewl_Text_Trigger *trigger;        /**< The parent trigger */
};

static Ewl_Widget *ewl_text_trigger_area_new();
static int ewl_text_trigger_area_init(Ewl_Text_Trigger_Area *area);
static Ewl_Widget *ewl_text_trigger_area_cached_get(Ewl_Text *t);
static void ewl_text_trigger_area_cache(Ewl_Text *t, Ewl_Text_Trigger_Area *a);
static void ewl_text_trigger_area_type_set(Ewl_Text_Trigger_Area *area,
                                        Ewl_Text_Trigger_Type type);
static void ewl_text_trigger_area_cb_mouse_in(Ewl_Widget *w, void *ev,
                                                void *data);
static void ewl_text_trigger_area_cb_mouse_out(Ewl_Widget *w, void *ev,
                                                void *data);
static void ewl_text_trigger_area_cb_mouse_up(Ewl_Widget *w, void *ev,
                                                void *data);
static void ewl_text_trigger_area_cb_mouse_down(Ewl_Widget *w, void *ev,
                                                void *data);

/**
 * @param type: The type of trigger to create
 * @return Returns a new ewl_text_trigger widget
 * @brief Creates a new trigger for the text object
 */
Ewl_Widget *
ewl_text_trigger_new(Ewl_Text_Trigger_Type type)
{
        Ewl_Text_Trigger *trigger;

        DENTER_FUNCTION(DLEVEL_STABLE);

        trigger = NEW(Ewl_Text_Trigger, 1);
        if (!trigger)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_text_trigger_init(trigger, type))
        {
                ewl_widget_destroy(EWL_WIDGET(trigger));
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(trigger, DLEVEL_STABLE);
}

/**
 * @param trigger: The trigger to initialize
 * @param type: The type of the triger
 * @return Returns TRUE if successful of FALSE otherwise
 * @brief Initializes a trigger to default values
 */
int
ewl_text_trigger_init(Ewl_Text_Trigger *trigger, Ewl_Text_Trigger_Type type)
{
        const char *type_str;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(trigger, FALSE);

        if (type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
                type_str = EWL_TEXT_TRIGGER_TYPE;
        else if (type == EWL_TEXT_TRIGGER_TYPE_SELECTION)
                type_str = EWL_TEXT_SELECTION_TYPE;
        else
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        if (!ewl_widget_init(EWL_WIDGET(trigger)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(trigger), type_str);
        ewl_widget_inherit(EWL_WIDGET(trigger), EWL_TEXT_TRIGGER_TYPE);

        ewl_callback_prepend(EWL_WIDGET(trigger), EWL_CALLBACK_DESTROY,
                                        ewl_text_trigger_cb_destroy, NULL);
        ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_SHOW,
                                        ewl_text_trigger_cb_show, NULL);
        ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_HIDE,
                                        ewl_text_trigger_cb_hide, NULL);
        ewl_callback_append(EWL_WIDGET(trigger), EWL_CALLBACK_UNREALIZE,
                                        ewl_text_trigger_cb_unrealize, NULL);

        trigger->areas = ecore_list_new();
        trigger->type = type;

        ewl_widget_focusable_set(EWL_WIDGET(trigger), FALSE);
        ewl_widget_internal_set(EWL_WIDGET(trigger), TRUE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The trigger destroy callback
 */
void
ewl_text_trigger_cb_destroy(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Text_Trigger *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TRIGGER_TYPE);

        t = EWL_TEXT_TRIGGER(w);

        /* note we don't clean up the the list of areas, because it is
         * up to the text widget to remove them on the child remove callback */
        IF_FREE_LIST(t->areas);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @return Returns the type of the trigger
 * @brief Retrieves the type of the trigger
 */
Ewl_Text_Trigger_Type
ewl_text_trigger_type_get(Ewl_Text_Trigger *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, EWL_TEXT_TRIGGER_TYPE_NONE);
        DCHECK_TYPE_RET(t, EWL_TEXT_TRIGGER_TYPE, EWL_TEXT_TRIGGER_TYPE_NONE);

        DRETURN_INT(t->type, DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @param char_pos: The position to set
 * @return Returns no value
 * @brief Sets the start position of the trigger @a t to position @a pos
 */
void
ewl_text_trigger_start_pos_set(Ewl_Text_Trigger *t, unsigned int char_pos)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TRIGGER_TYPE);

        t->char_pos = char_pos;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @return Returns the current start position of the trigger
 * @brief Retrieves the start position of the trigger
 */
unsigned int
ewl_text_trigger_start_pos_get(Ewl_Text_Trigger *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TRIGGER_TYPE, 0);

        DRETURN_INT(t->char_pos, DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @param char_len: The length to set on the cursor
 * @return Returns no value
 * @brief Sets the length @a len on the trigger @a t
 */
void
ewl_text_trigger_length_set(Ewl_Text_Trigger *t, unsigned int char_len)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TRIGGER_TYPE);

        t->char_len = char_len;

        /* if the length is set to 0 remove the areas */
        if (char_len == 0)
                ewl_text_trigger_areas_cleanup(t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @return Returns the length of the trigger
 * @brief Retrieves the length from the cursor @a t
 */
unsigned int
ewl_text_trigger_length_get(Ewl_Text_Trigger *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TRIGGER_TYPE, 0);

        DRETURN_INT(t->char_len, DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @param char_pos: The position to set as the base for the cursor
 * @return Returns no value
 * @brief Sets the given position @a pos as the base for the trigger @a t
 */
void
ewl_text_trigger_base_set(Ewl_Text_Trigger *t, unsigned int char_pos)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TRIGGER_TYPE);

        t->char_base = char_pos;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The trigger to work with
 * @return Returns the current base position of the cursor
 * @brief Retrieves the current base position of the cursor
 */
unsigned int
ewl_text_trigger_base_get(Ewl_Text_Trigger *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TRIGGER_TYPE, 0);

        DRETURN_INT(t->char_base, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text parent
 * @param cur: The trigger to work with
 * @param x: The x position
 * @param y: The y position
 * @param w: The width
 * @param h: The height
 * @return Returns no value
 * @brief Creates a trigger area at the given co-ordinates
 */
void
ewl_text_trigger_area_add(Ewl_Text *t, Ewl_Text_Trigger *cur,
                                        int x, int y, int w, int h)
{
        Ewl_Widget *area;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_PARAM_PTR(cur);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);
        DCHECK_TYPE(cur, EWL_TEXT_TRIGGER_TYPE);

        area = ewl_text_trigger_area_cached_get(t);
        ewl_text_trigger_area_type_set(EWL_TEXT_TRIGGER_AREA(area), cur->type);
        ewl_object_geometry_request(EWL_OBJECT(area), x, y, w, h);
        EWL_TEXT_TRIGGER_AREA(area)->trigger = cur;

        ewl_widget_show(area);

        if (!cur->areas) cur->areas = ecore_list_new();
        ecore_list_append(cur->areas, area);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param trig: The trigger to work with
 * @return Returns no value
 * @brief Cleans up the trigger areas for @a trig
 */
void
ewl_text_trigger_areas_cleanup(Ewl_Text_Trigger *trig)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(trig);
        DCHECK_TYPE(trig, EWL_TEXT_TRIGGER_TYPE);

        /* cleanup any old areas */
        if (trig->areas)
        {
                Ewl_Text_Trigger_Area *area;

                while ((area = ecore_list_first_remove(trig->areas)))
                        ewl_text_trigger_area_cache(trig->text_parent, area);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The trigger to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Hides the trigger
 */
void
ewl_text_trigger_cb_hide(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *area;
        Ewl_Text_Trigger *trig;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TRIGGER_TYPE);

        trig = EWL_TEXT_TRIGGER(w);
        if (!trig->areas) DRETURN(DLEVEL_STABLE);;

        ecore_list_first_goto(trig->areas);
        while ((area = ecore_list_next(trig->areas)))
                ewl_widget_hide(area);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The trigger to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Shows the trigger
 */
void
ewl_text_trigger_cb_show(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *area;
        Ewl_Text_Trigger *trig;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TRIGGER_TYPE);

        trig = EWL_TEXT_TRIGGER(w);
        if (!trig->areas) DRETURN(DLEVEL_STABLE);

        ecore_list_first_goto(trig->areas);
        while ((area = ecore_list_next(trig->areas)))
                ewl_widget_show(area);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The trigger to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Unrealizes the trigger
 */
void
ewl_text_trigger_cb_unrealize(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TRIGGER_TYPE);

        ewl_text_trigger_areas_cleanup(EWL_TEXT_TRIGGER(w));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Text_Trigger_Area stuff
 */

/**
 * @internal
 * @param type: The trigger area type to create
 * @return Returns a new trigger area of the given type
 * @brief Creates and returns a new trigger_area of the given type
 */
static Ewl_Widget *
ewl_text_trigger_area_new(void)
{
        Ewl_Text_Trigger_Area *area;

        DENTER_FUNCTION(DLEVEL_STABLE);

        area = NEW(Ewl_Text_Trigger_Area, 1);
        if (!area)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_text_trigger_area_init(area))
        {
                ewl_widget_destroy(EWL_WIDGET(area));
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(area, DLEVEL_STABLE);
}

/**
 * @internal
 * @param area: The trigger area to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes a triggger area to default values
 */
static int
ewl_text_trigger_area_init(Ewl_Text_Trigger_Area *area)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(area, FALSE);

        w = EWL_WIDGET(area);

        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(w, EWL_TEXT_TRIGGER_AREA_TYPE);

        ewl_widget_focusable_set(w, FALSE);
        ewl_widget_internal_set(w, TRUE);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_IN,
                        ewl_text_trigger_area_cb_mouse_in, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_OUT,
                        ewl_text_trigger_area_cb_mouse_out, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
                        ewl_text_trigger_area_cb_mouse_down, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
                        ewl_text_trigger_area_cb_mouse_up, NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param area: The trigger area to set the type for
 * @param type: The type of the trigger area
 * @return Returns nothing
 * @brief Set the trigger type of the area
 */
static void
ewl_text_trigger_area_type_set(Ewl_Text_Trigger_Area *area,
                                Ewl_Text_Trigger_Type type)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(area);
        DCHECK_TYPE(area, EWL_TEXT_TRIGGER_AREA_TYPE);

        ewl_widget_appearance_set(EWL_WIDGET(area),
                        ((type == EWL_TEXT_TRIGGER_TYPE_SELECTION) ?
                         "selection_area" : "trigger_area"));

        if (type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
                ewl_widget_color_set(EWL_WIDGET(area), 0, 0, 0, 0);
        else
                ewl_widget_color_set(EWL_WIDGET(area), 255, 255, 255, 255);

        DRETURN(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The Ewl_Text parent
 * @return Returns no value
 */
static Ewl_Widget *
ewl_text_trigger_area_cached_get(Ewl_Text *t)
{
        Ewl_Widget *area;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        if (t->areas_cache && !ecore_list_empty_is(t->areas_cache))
                area = ecore_list_first_remove(t->areas_cache);
        else {
                area = ewl_text_trigger_area_new();
                ewl_container_child_append(EWL_CONTAINER(t), area);
        }

        DRETURN_PTR(area, DLEVEL_STABLE);
}

static void
ewl_text_trigger_area_cache(Ewl_Text *t, Ewl_Text_Trigger_Area *area)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_PARAM_PTR(area);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);
        DCHECK_TYPE(area, EWL_TEXT_TRIGGER_AREA_TYPE);

        area->trigger = NULL;
        ewl_widget_hide(EWL_WIDGET(area));

        if (DESTROYED(t) || DESTROYED(area))
                DRETURN(DLEVEL_STABLE);

        if (!t->areas_cache)
                t->areas_cache = ecore_list_new();

        ecore_list_append(t->areas_cache, area);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The trigger area
 * @param ev: The event data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The trigger area mouse in callback
 */
void
ewl_text_trigger_area_cb_mouse_in(Ewl_Widget *w, void *ev,
                                        void *data __UNUSED__)
{
        Ewl_Text_Trigger_Area *a;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TRIGGER_AREA_TYPE);

        a = EWL_TEXT_TRIGGER_AREA(w);
        if (a->trigger)
                ewl_callback_call_with_event_data(EWL_WIDGET(a->trigger),
                                                EWL_CALLBACK_MOUSE_IN, ev);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The trigger area
 * @param ev: The event data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The trigger area mouse out callback
 */
void
ewl_text_trigger_area_cb_mouse_out(Ewl_Widget *w, void *ev,
                                                void *data __UNUSED__)
{
        Ewl_Text_Trigger_Area *a;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TRIGGER_AREA_TYPE);

        a = EWL_TEXT_TRIGGER_AREA(w);
        if (a->trigger)
                ewl_callback_call_with_event_data(EWL_WIDGET(a->trigger),
                                                EWL_CALLBACK_MOUSE_OUT, ev);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the trigger area
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger area mouse up callback
 */
void
ewl_text_trigger_area_cb_mouse_up(Ewl_Widget *w, void *ev,
                                        void *data __UNUSED__)
{
        Ewl_Text_Trigger_Area *a;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TRIGGER_AREA_TYPE);

        a = EWL_TEXT_TRIGGER_AREA(w);
        if (a->trigger)
                ewl_callback_call_with_event_data(EWL_WIDGET(a->trigger),
                                                EWL_CALLBACK_MOUSE_UP, ev);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the area
 * @param ev: The event data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The trigger mouse down callback
 */
void
ewl_text_trigger_area_cb_mouse_down(Ewl_Widget *w, void *ev,
                                        void *data __UNUSED__)
{
        Ewl_Text_Trigger_Area *a;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TRIGGER_AREA_TYPE);

        a = EWL_TEXT_TRIGGER_AREA(w);
        if (a->trigger)
                ewl_callback_call_with_event_data(EWL_WIDGET(a->trigger),
                                                EWL_CALLBACK_MOUSE_DOWN, ev);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


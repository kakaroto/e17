/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
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
 * @def EWL_TEXT_TRIGGER_AREA(area)
 * Typecasts a pointer to an Ewl_Text_Trigger_Area pointer
 */
#define EWL_TEXT_TRIGGER_AREA(area) ((Ewl_Text_Trigger_Area *) area)

/**
 * @brief Inherits from Ewl_Widget and extends to provide a trigger area
 */
struct Ewl_Text_Trigger_Area
{
	Ewl_Widget widget;		/**< Inherits from Ewl_Widget */
	unsigned int deleted;	/**< Is this area deleted */
};

static Ewl_Widget *ewl_text_trigger_area_new(Ewl_Text_Trigger_Type type);
static int ewl_text_trigger_area_init(Ewl_Text_Trigger_Area *area,
					Ewl_Text_Trigger_Type type);

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
	char *type_str;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("trigger", trigger, FALSE);

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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_TEXT_TRIGGER_TYPE);

	t = EWL_TEXT_TRIGGER(w);

	if (t->areas)
	{
		Ewl_Text_Trigger_Area *area;

		while ((area = ecore_list_remove_first(t->areas)))
			ewl_widget_destroy(EWL_WIDGET(area));

		IF_FREE_LIST(t->areas);
	}

	/* remove ourself from the parents trigger list, if needed */
	if ((t->text_parent) && (t->text_parent->triggers)
			&& (ecore_list_goto(t->text_parent->triggers, t)))
		ecore_list_remove(t->text_parent->triggers);

	t->text_parent = NULL;
	t->areas = NULL;

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
	DCHECK_PARAM_PTR_RET("t", t, EWL_TEXT_TRIGGER_TYPE_NONE);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, EWL_TEXT_TRIGGER_TYPE_NONE);

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
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

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
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

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
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

	t->char_len = char_len;

	/* if the length is set to 0 remove the areas */
	if (char_len == 0)
	{
		if (t->areas)
		{
			Ewl_Text_Trigger_Area *area;
			while ((area = ecore_list_remove_first(t->areas)))
				ewl_widget_destroy(EWL_WIDGET(area));
		}
	}

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
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

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
	DCHECK_PARAM_PTR("t", t);
	DCHECK_TYPE("t", t, EWL_TEXT_TRIGGER_TYPE);

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
	DCHECK_PARAM_PTR_RET("t", t, 0);
	DCHECK_TYPE_RET("t", t, EWL_TEXT_TRIGGER_TYPE, 0);

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
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("cur", cur);
	DCHECK_TYPE("t", t, EWL_TEXT_TYPE);
	DCHECK_TYPE("cur", cur, EWL_TEXT_TRIGGER_TYPE);

	area = ewl_text_trigger_area_new(cur->type);
	ewl_container_child_append(EWL_CONTAINER(t), area);
	ewl_widget_internal_set(area, TRUE);
	ewl_object_geometry_request(EWL_OBJECT(area), x, y, w, h);

	ewl_callback_append(area, EWL_CALLBACK_MOUSE_IN, 
			ewl_text_trigger_cb_mouse_in, cur);
	ewl_callback_append(area, EWL_CALLBACK_MOUSE_OUT,
			ewl_text_trigger_cb_mouse_out, cur);
	ewl_callback_append(area, EWL_CALLBACK_MOUSE_DOWN,
			ewl_text_trigger_cb_mouse_down, cur);
	ewl_callback_append(area, EWL_CALLBACK_MOUSE_UP,
			ewl_text_trigger_cb_mouse_up, cur);
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
	DCHECK_PARAM_PTR("trig", trig);
	DCHECK_TYPE("trig", trig, EWL_TEXT_TRIGGER_TYPE);

	/* cleanup any old areas */
	if (trig->areas)
	{
		Ewl_Text_Trigger_Area *area;

		while ((area = ecore_list_remove_first(trig->areas)))
			ewl_widget_destroy(EWL_WIDGET(area));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param trig:
 * @return Returns no value
 * @brief Configures the areas associated with @a trig
 */
void 
ewl_text_trigger_areas_configure(Ewl_Text_Trigger *trig)
{
	Ewl_Text_Trigger_Area *area;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("trig", trig);
	DCHECK_TYPE("trig", trig, EWL_TEXT_TRIGGER_TYPE);

	if (!trig->areas) DRETURN(DLEVEL_STABLE);

	ecore_list_goto_first(trig->areas);
	while ((area = ecore_list_next(trig->areas)))
		ewl_widget_configure(EWL_WIDGET(area));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger mouse in callback
 */
void
ewl_text_trigger_cb_mouse_in(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("data", data, EWL_TEXT_TRIGGER_TYPE);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
						EWL_CALLBACK_MOUSE_IN, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger mouse out callback
 */
void
ewl_text_trigger_cb_mouse_out(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("data", data, EWL_TEXT_TRIGGER_TYPE);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
						EWL_CALLBACK_MOUSE_OUT, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger mouse up callback
 */
void
ewl_text_trigger_cb_mouse_up(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("data", data, EWL_TEXT_TRIGGER_TYPE);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger),
						EWL_CALLBACK_MOUSE_UP, ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: The event data
 * @param data: The Ewl_Text_Trigger
 * @return Returns no value
 * @brief The trigger mouse down callback
 */
void
ewl_text_trigger_cb_mouse_down(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
	Ewl_Text_Trigger *trigger;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("data", data, EWL_TEXT_TRIGGER_TYPE);

	trigger = data;
	ewl_callback_call_with_event_data(EWL_WIDGET(trigger), 
						EWL_CALLBACK_MOUSE_DOWN, ev);

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
Ewl_Widget *
ewl_text_trigger_area_new(Ewl_Text_Trigger_Type type)
{
	Ewl_Text_Trigger_Area *area;

	DENTER_FUNCTION(DLEVEL_STABLE);

	area = NEW(Ewl_Text_Trigger_Area, 1);
	if (!area)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_text_trigger_area_init(area, type))
	{
		ewl_widget_destroy(EWL_WIDGET(area));
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(area, DLEVEL_STABLE);
}

/**
 * @internal
 * @param area: The trigger area to initialize
 * @param type: The type of the trigger area
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes a triggger area to default values
 */
int
ewl_text_trigger_area_init(Ewl_Text_Trigger_Area *area, 
				Ewl_Text_Trigger_Type type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("area", area, FALSE);

	if (!ewl_widget_init(EWL_WIDGET(area)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(area),
			((type == EWL_TEXT_TRIGGER_TYPE_SELECTION) ?
			 "selection_area" : "trigger_area"));
	ewl_widget_inherit(EWL_WIDGET(area), "trigger_area");

	if (type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
		ewl_widget_color_set(EWL_WIDGET(area), 0, 0, 0, 0);

	ewl_widget_focusable_set(EWL_WIDGET(area), FALSE);
	ewl_widget_internal_set(EWL_WIDGET(area), TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


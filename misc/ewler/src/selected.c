/**
 * selected widget - automatically sets up any widget passed to it as a visual
 * selection
 */
#include <Ewl.h>

#include "ewler.h"
#include "form.h"
#include "selected.h"

#define SELECTED_MIN_WIDTH	15
#define SELECTED_MIN_HEIGHT	15

/**
 * @param w: the child widget to be selected
 * @return Returns NULL on failure, or a newly allocated selected on success.
 * @brief Allocate and initialize a new selected with given child
 */
Ewl_Widget *
ewler_selected_new(Ewl_Widget *w)
{
	Ewler_Selected *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewler_Selected, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewler_selected_init(s, w)) {
		ewl_widget_destroy(EWL_WIDGET(s));
		s = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the selected to initialize
 * @param w: the child widget to be selected
 * @return Returns 0 on failure, or non-zero on success
 * @brief initialize a new selected with given child
 */
int
ewler_selected_init(Ewler_Selected *s, Ewl_Widget *w)
{
	Ewl_Widget *sw, *cw;
	Ewl_Container *parent;
	int index;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, FALSE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	sw = EWL_WIDGET(s);
	parent = EWL_CONTAINER(w->parent);

	ecore_list_goto(parent->children, w);
	index = ecore_list_index(parent->children);

	if (!ewl_box_init(EWL_BOX(s), EWL_ORIENTATION_VERTICAL))
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	ewl_widget_set_appearance(sw, "selected");
	ewl_theme_data_set_str(sw, "/selected/file",
												 PACKAGE_DATA_DIR"/themes/ewler.eet");
	ewl_theme_data_set_str(sw, "/selected/group", "selected");
	ewl_widget_set_layer( sw, 0 );

	ewl_container_insert_child(parent, sw, index);
	ewl_object_request_geometry(EWL_OBJECT(s),
															CURRENT_X(w), CURRENT_Y(w),
															CURRENT_W(w), CURRENT_H(w));
	ewl_object_set_fill_policy(EWL_OBJECT(s),
														 ewl_object_get_fill_policy(EWL_OBJECT(w)));

	ewl_callback_append(sw, EWL_CALLBACK_CONFIGURE,
											ewler_selected_configure_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
											ewler_selected_realize_cb, s);
	ewl_callback_append(sw, EWL_CALLBACK_REALIZE,
											ewler_selected_selector_realize_cb, NULL);
	ewl_callback_append(sw, EWL_CALLBACK_SELECT,
											ewler_selected_select_cb, NULL);
	ewl_callback_append(sw, EWL_CALLBACK_DESELECT,
											ewler_selected_deselect_cb, NULL);
	ewl_callback_append(sw, EWL_CALLBACK_MOUSE_MOVE,
											ewler_selected_mouse_move_cb, NULL);
	ewl_callback_append(sw, EWL_CALLBACK_MOUSE_DOWN,
											ewler_selected_mouse_down_cb, NULL);
	ewl_callback_append(sw, EWL_CALLBACK_MOUSE_UP,
											ewler_selected_mouse_up_cb, NULL);

	if( ewl_object_preferred_w_sum_get(EWL_OBJECT(w)) < SELECTED_MIN_WIDTH &&
			ewl_object_preferred_w_sum_get(EWL_OBJECT(w)) < SELECTED_MIN_HEIGHT )
		ewl_object_set_preferred_size(EWL_OBJECT(w),
																	SELECTED_MIN_WIDTH, SELECTED_MIN_HEIGHT);

	s->selected = w;
	s->dragging = NULL;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: the selected item to obtain the widget of
 * @return Returns the currently selected widget
 * @brief Get the widget this selected item refers to
 */
Ewl_Widget *
ewler_selected_get(Ewler_Selected *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", s, NULL);

	DRETURN_PTR(s->selected, DLEVEL_STABLE);
}

void
ewler_selected_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewler_Selected *s;
	int x, y, width, height;

	ewl_widget_set_state( w, w->bit_state );

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWLER_SELECTED(w);
	
	/* the width comes in from the selected, the position is set by the parent */
	ewl_object_preferred_size_sum_get(EWL_OBJECT(s->selected), &width, &height);
	x = CURRENT_X(s);
	y = CURRENT_Y(s);

	if( x != CURRENT_X(s->selected) || y != CURRENT_Y(s->selected) )
		ewl_object_request_position(EWL_OBJECT(s->selected), x, y);
	if( width != CURRENT_W(s) || height != CURRENT_H(s) )
		ewl_object_set_preferred_size(EWL_OBJECT(s), width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewler_selected_selector_realize_cb(Ewl_Widget *w, void *ev_data,
																	 void *user_data)
{
	Ewler_Selected *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWLER_SELECTED(w);

	/* THIS IS A HACK!!! THE WIDGET IS ALWAYS IN THE DEFAULT STATE, BUT WE
	 * HIDE IT IF THE BITSTATE DOESN'T SAY DEFAULT */
	if( !strcmp( w->bit_state, "default" ) )
		evas_object_layer_set(w->theme_object, 1000);
	else {
		ewl_widget_set_state( w, "none" );
		ewl_widget_set_state( w, "deselect" );
	}

	ewl_container_append_child(EWL_CONTAINER(s), s->selected);

	if( ewl_widget_get_data( s->selected, "unsizable" ) )
		return;

	edje_object_signal_callback_add(w->theme_object,
																	"down", "top_left",
																	ewler_selected_part_down, w);
	edje_object_signal_callback_add(w->theme_object,
																	"up", "top_left",
																	ewler_selected_part_up, w);
	edje_object_signal_callback_add(w->theme_object,
																	"down", "top_middle",
																	ewler_selected_part_down, w);
	edje_object_signal_callback_add(w->theme_object,
																	"up", "top_middle",
																	ewler_selected_part_up, w);
	edje_object_signal_callback_add(w->theme_object,
																	"down", "top_right",
																	ewler_selected_part_down, w);
	edje_object_signal_callback_add(w->theme_object,
																	"up", "top_right",
																	ewler_selected_part_up, w);
	edje_object_signal_callback_add(w->theme_object,
																	"down", "middle_right",
																	ewler_selected_part_down, w);
	edje_object_signal_callback_add(w->theme_object,
																	"up", "middle_right",
																	ewler_selected_part_up, w);
	edje_object_signal_callback_add(w->theme_object,
																	"down", "bottom_right",
																	ewler_selected_part_down, w);
	edje_object_signal_callback_add(w->theme_object,
																	"up", "bottom_right",
																	ewler_selected_part_up, w);
	edje_object_signal_callback_add(w->theme_object,
																	"down", "bottom_middle",
																	ewler_selected_part_down, w);
	edje_object_signal_callback_add(w->theme_object,
																	"up", "bottom_middle",
																	ewler_selected_part_up, w);
	edje_object_signal_callback_add(w->theme_object,
																	"down", "bottom_left",
																	ewler_selected_part_down, w);
	edje_object_signal_callback_add(w->theme_object,
																	"up", "bottom_left",
																	ewler_selected_part_up, w);
	edje_object_signal_callback_add(w->theme_object,
																	"down", "middle_left",
																	ewler_selected_part_down, w);
	edje_object_signal_callback_add(w->theme_object,
																	"up", "middle_left",
																	ewler_selected_part_up, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewler_selected_realize_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewler_Selected *s;
	int width = 0, height = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWLER_SELECTED(user_data);

	ewl_object_preferred_size_sum_get(EWL_OBJECT(s->selected), &width, &height);

	ewl_object_request_size(EWL_OBJECT(s), width, height);
	ewl_object_set_preferred_size(EWL_OBJECT(s), width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewler_selected_deselect_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewler_Selected *s;
	int x, y, width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWLER_SELECTED(w);

	ewl_container_nointercept_callback(EWL_CONTAINER(s), EWL_CALLBACK_MOUSE_DOWN);
	ewl_object_get_current_geometry(EWL_OBJECT(s), &x, &y, &width, &height);
	ewl_object_request_geometry(EWL_OBJECT(s->selected),
															x, y, width, height);

	ewl_widget_set_state( w, "deselect" );
	evas_object_layer_set(w->theme_object, -990);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewler_selected_select_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_widget_set_state( w, "default" );
	evas_object_layer_set(w->theme_object, 1000);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
											ewler_selected_mouse_down_cb, NULL);
	ewl_container_intercept_callback(EWL_CONTAINER(w), EWL_CALLBACK_MOUSE_DOWN);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewler_selected_part_down(void *data, Evas_Object *o,
												 const char *emission, const char *source)
{
	Ewler_Selected *s = data;
	Ewl_Widget *sw = data;

	if( s->dragging )
		FREE(s->dragging);

	s->dragging = strdup( source );
	s->corners.x = CURRENT_X(sw);
	s->corners.y = CURRENT_Y(sw);
	s->corners.u = CURRENT_X(sw) + CURRENT_W(sw);
	s->corners.v = CURRENT_Y(sw) + CURRENT_H(sw);

	form_clear_widget_dragging();
}

void
ewler_selected_part_up(void *data, Evas_Object *o,
											 const char *emission, const char *source)
{
	Ewler_Selected *s = data;

	if( s->dragging )
		FREE(s->dragging);

	s->dragging = NULL;
}

void
ewler_selected_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Move *ev = ev_data;
	Ewler_Selected *s = EWLER_SELECTED(w);

	embed = ewl_embed_find_by_widget(w);

	evas_event_feed_mouse_move(embed->evas, ev->x, ev->y);

	if( s->dragging ) {
		if( strstr( s->dragging, "left" ) )
			s->corners.x += ev->x - s->last_pos.x;
		if( strstr( s->dragging, "right" ) )
			s->corners.u += ev->x - s->last_pos.x;
		if( strstr( s->dragging, "top" ) )
			s->corners.y += ev->y - s->last_pos.y;
		if( strstr( s->dragging, "bottom" ) )
			s->corners.v += ev->y - s->last_pos.y;

		if( s->corners.u < s->corners.x )
			s->corners.u = s->corners.x;
		if( s->corners.v < s->corners.y )
			s->corners.v = s->corners.y;

		ewl_object_request_position(EWL_OBJECT(s), s->corners.x, s->corners.y);
		ewl_object_set_preferred_size(EWL_OBJECT(s),
																	s->corners.u - s->corners.x,
																	s->corners.v - s->corners.y);
		ewl_object_set_maximum_size(EWL_OBJECT(s->selected), 100000, 100000);
		ewl_object_set_preferred_size(EWL_OBJECT(s->selected),
																	s->corners.u - s->corners.x,
																	s->corners.v - s->corners.y);


	}

	s->last_pos.x = ev->x;
	s->last_pos.y = ev->y;
}

void
ewler_selected_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Down *ev = ev_data;
	Ewler_Selected *s = EWLER_SELECTED(w);
	static Ecore_List *w_stack = NULL;
	int x, y;

	x = ev->x;
	y = ev->y;

	if( !w_stack )
		w_stack = ecore_list_new();

	if( ecore_list_goto_first( w_stack ) == s )
		return;

	if( ev->button == 1 ) {
		form_set_widget_dragging( s->selected, ev );
		embed = ewl_embed_find_by_widget(w);

		evas_event_feed_mouse_down(embed->evas, ev->button);

		if( !form_widget_created() ) {
			ewl_container_nointercept_callback(EWL_CONTAINER(s),
																				 EWL_CALLBACK_MOUSE_DOWN);
			form_clear_widget_selected();

			ecore_list_prepend( w_stack, s );
			ewl_embed_feed_mouse_down(embed, ev->button, ev->clicks,
																ev->x, ev->y, ev->modifiers);
			ecore_list_remove_first( w_stack );

			ewl_callback_del_type(w, EWL_CALLBACK_MOUSE_DOWN);
			ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
													ewler_selected_mouse_down_cb, NULL);
			ewl_container_intercept_callback(EWL_CONTAINER(s),
																			 EWL_CALLBACK_MOUSE_DOWN);

			form_set_widget_selected();
		} else if( form_widget_created() ) {
			form_clear_widget_created();
			form_clear_widget_dragging();
		}
	}
}

void
ewler_selected_mouse_up_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Up *ev = ev_data;

	embed = ewl_embed_find_by_widget(w);

	evas_event_feed_mouse_move(embed->evas, ev->x, ev->y);
	evas_event_feed_mouse_up(embed->evas, ev->button);
}

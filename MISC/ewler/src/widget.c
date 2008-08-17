/**
 * Widget manager for ewler.  Takes care of all of the ui routines for widgets.
 */
#include <Ewl.h>

#include "ewler.h"
#include "config.h"

bool widget_under_cursor;

static void *
elem_at_index(Ecore_Hash *elems, int i)
{
	Ecore_List *keys;
	Ewler_Widget_Elem *elem;
	char *name;

	keys = ecore_hash_keys(elems);
	while( (name = ecore_list_next(keys)) ) {
		elem = ecore_hash_get(elems, name);

		if( elem->spec->index == i ) {
			ecore_list_destroy(keys);
			return &elem->info.ivalue;
		}
	}

	ecore_list_destroy(keys);
	return NULL;
}

static EWL_CALLBACK_DEFN(realize)
{
	Ewler_Widget *ewler_w = user_data;
	Ecore_List *elems;
	Ewler_Widget_Elem *elem;
	char *name;
	void *args[4];
	int i, n;

	if( ewler_w->selected )
		widget_select(ewler_w);

	elems = ecore_hash_keys(ewler_w->elems);

	while( (name = ecore_list_next(elems)) ) {
		elem = ecore_hash_get(ewler_w->elems, name);

		if( elem->spec->get_func )
			switch( elem->spec->type ) {
				case EWLER_SPEC_ELEM_INT:
					elem->info.ivalue = (int) elem->spec->get_func(ewler_w->w);

					if( elem->entry ) {
						ewl_text_text_set(EWL_TEXT(elem->entry), "");
						ewl_text_text_append(EWL_TEXT(elem->entry), elem_to_s(elem));
					}
					break;
				case EWLER_SPEC_ELEM_STRING:
					IF_FREE(elem->info.svalue);
					elem->info.svalue = (char *) elem->spec->get_func(ewler_w->w);

					if( elem->entry )
						ewl_text_text_set(EWL_TEXT(elem->entry), elem_to_s(elem));
					break;
				case EWLER_SPEC_ELEM_STRUCT:
					n = elem->spec->info.children->nodes;
					for( i=0;i<n;i++ )
						args[i] = elem_at_index(elem->info.children, i);

					switch( n ) {
						case 1: elem->spec->get_func(ewler_w->w, args[0]); break;
						case 2: elem->spec->get_func(ewler_w->w, args[0], args[1]); break;
						case 3:
							elem->spec->get_func(ewler_w->w, args[0], args[1], args[2]);
							break;
						case 4:
							elem->spec->get_func(ewler_w->w,
																	 args[0], args[1], args[2], args[3]);
							break;
						/* haven't seen anything higher just yet */
					}
					break;
				case EWLER_SPEC_ELEM_ENUM:
					elem->info.evalue = (int) elem->spec->get_func(ewler_w->w);

					if( elem->entry ) {
						Ewl_Widget *item;

						item = ecore_hash_get(elem->items, (void *) elem->info.evalue);
						ewl_combo_selected_set(EWL_COMBO(elem->entry), item);
					}
					break;
				default: break;
			}
		else if(elem->spec->type == EWLER_SPEC_ELEM_STRUCT)
			/* iterate through children, since we don't have a mass get */;
	}

	ecore_list_destroy(elems);
}

static EWL_CALLBACK_DEFN(configure)
{
	Ewler_Widget *ewler_w = user_data;
	Ewler_Widget_Elem *preferred, *current, *elem;
	int x, y, height, width;
	int pref_w, pref_h;

	if( !REALIZED(ewler_w->w) )
		return;

	if( ewler_w->selected ) {
		ewl_widget_layer_set(ewler_w->fg, 1000);
		ewl_widget_layer_set(ewler_w->bg, -1);
		ewl_widget_layer_set(ewler_w->w, 1);
	} else if( ewler_w->parent ) {
		ewl_widget_layer_set(ewler_w->bg, -1);
		ewl_widget_layer_set(ewler_w->w, 1);
	}

	ewl_object_current_geometry_get(EWL_OBJECT(ewler_w->w),
																	&x, &y, &width, &height);
	ewl_object_preferred_inner_size_get(EWL_OBJECT(ewler_w->w), &pref_w, &pref_h);

	current = ecore_hash_get(ewler_w->elems, "current");

	elem = ecore_hash_get(current->info.children, "x");
	if( elem->info.ivalue != x ) {
		if( elem->text && !elem->changed ) {
			int len;

			len = ewl_text_length_get(EWL_TEXT(elem->text));
			ewl_text_cursor_position_set(EWL_TEXT(elem->text), 0);
			ewl_text_color_apply(EWL_TEXT(elem->text), 255, 0, 0, 255, len);
		}

		elem->info.ivalue = x;

		elem->changed = true;
		if( elem->entry )
			ewl_text_text_set(EWL_TEXT(elem->entry), elem_to_s(elem));
	}

	elem = ecore_hash_get(current->info.children, "y");
	if( elem->info.ivalue != y ) {
		if( elem->text && !elem->changed ) {
			int len;

			len = ewl_text_length_get(EWL_TEXT(elem->text));
			ewl_text_cursor_position_set(EWL_TEXT(elem->text), 0);
			ewl_text_color_apply(EWL_TEXT(elem->text), 255, 0, 0, 255, len);
		}

		elem->info.ivalue = y;

		elem->changed = true;
		if( elem->entry )
			ewl_text_text_set(EWL_TEXT(elem->entry), elem_to_s(elem));
	}

	elem = ecore_hash_get(current->info.children, "w");
	if( elem->info.ivalue != width ) {
		if( elem->text && !elem->changed ) {
			int len;

			len = ewl_text_length_get(EWL_TEXT(elem->text));
			ewl_text_cursor_position_set(EWL_TEXT(elem->text), 0);
			ewl_text_color_apply(EWL_TEXT(elem->text), 255, 0, 0, 255, len);
		}

		elem->info.ivalue = width;

		elem->changed = true;
		if( elem->entry )
			ewl_text_text_set(EWL_TEXT(elem->entry), elem_to_s(elem));
	}

	elem->info.ivalue = width;

	elem = ecore_hash_get(current->info.children, "h");
	if( elem->info.ivalue != height ) {
		if( elem->text && !elem->changed ) {
			int len;

			len = ewl_text_length_get(EWL_TEXT(elem->text));
			ewl_text_cursor_position_set(EWL_TEXT(elem->text), 0);
			ewl_text_color_apply(EWL_TEXT(elem->text), 255, 0, 0, 255, len);
		}

		elem->info.ivalue = height;

		elem->changed = true;
		if( elem->entry )
			ewl_text_text_set(EWL_TEXT(elem->entry), elem_to_s(elem));
	}

	preferred = ecore_hash_get(ewler_w->elems, "preferred");

	elem = ecore_hash_get(preferred->info.children, "w");
	if( elem->info.ivalue != pref_w ) {
		if( elem->text && !elem->changed ) {
			int len;

			len = ewl_text_length_get(EWL_TEXT(elem->text));
			ewl_text_cursor_position_set(EWL_TEXT(elem->text), 0);
			ewl_text_color_apply(EWL_TEXT(elem->text), 255, 0, 0, 255, len);
		}

		elem->info.ivalue = pref_w;

		elem->changed = true;
		if( elem->entry )
			ewl_text_text_set(EWL_TEXT(elem->entry), elem_to_s(elem));
	}

	elem = ecore_hash_get(preferred->info.children, "h");
	if( elem->info.ivalue != pref_h ) {
		if( elem->text && !elem->changed ) {
			int len;

			len = ewl_text_length_get(EWL_TEXT(elem->text));
			ewl_text_cursor_position_set(EWL_TEXT(elem->text), 0);
			ewl_text_color_apply(EWL_TEXT(elem->text), 255, 0, 0, 255, len);
		}

		elem->info.ivalue = pref_h;

		elem->changed = true;
		if( elem->entry )
			ewl_text_text_set(EWL_TEXT(elem->entry), elem_to_s(elem));
	}

	if( !ewler_w->configured ) {
		ewl_object_geometry_request(EWL_OBJECT(ewler_w->fg), x, y, width, height);
		ewl_object_geometry_request(EWL_OBJECT(ewler_w->bg), x, y, width, height);
	}

	ewler_w->configured = false;

	elem->info.ivalue = height;
}

static EWL_CALLBACK_DEFN(fg_configure)
{
	Ewler_Widget *ewler_w = user_data;

	ewler_w->configured = true;
}

void
widget_select( Ewler_Widget *w )
{
	w->selected = true;

	ewl_widget_state_set(w->fg, "default");
	ewl_widget_configure(w->fg);
}

void
widget_clear_ui_hooks( Ewler_Widget *w )
{
	Ecore_Hash *elems;
	Ecore_List *elems_stack;
	Ecore_List *names, *names_stack;
	char *name;

	names = ecore_hash_keys(w->elems);

	elems = w->elems;

	elems_stack = ecore_list_new();
	names_stack = ecore_list_new();

	while( (name = ecore_list_next(names)) ) {
		Ewler_Widget_Elem *elem;

		elem = ecore_hash_get(elems, name);

		elem->entry = NULL;
		elem->text = NULL;
		if( elem->items )
			ecore_hash_destroy(elem->items);
		elem->items = NULL;

		if( elem->spec->type == EWLER_SPEC_ELEM_STRUCT ) {
			ecore_list_prepend(elems_stack, elems);
			ecore_list_prepend(names_stack, names);

			elems = elem->info.children;
			names = ecore_hash_keys(elems);
		}

		while( !ecore_list_current(names) && elems != w->elems ) {
			ecore_list_destroy(names);
			elems = ecore_list_first_remove(elems_stack);
			names = ecore_list_first_remove(names_stack);
		}
	}

	ecore_list_destroy(names);
	ecore_list_destroy(elems_stack);
	ecore_list_destroy(names_stack);
}

void
widget_deselect( Ewler_Widget *w )
{
	w->selected = false;

	ewl_widget_state_set(w->fg, "deselect");
	ewl_widget_configure(w->fg);

	widget_clear_ui_hooks(w);
}

void
widget_toggle( Ewler_Widget *w )
{
	if( w->selected )
		widget_deselect(w);
	else
		widget_select(w);
}

void
fg_part_down( void *data, Evas_Object *o,
							const char *emission, const char *source )
{
	Ewler_Widget *w = data;

	IF_FREE(w->source);

	w->source = strdup(source);
}

void
fg_part_up( void *data, Evas_Object *o,
						const char *emission, const char *source )
{
	Ewler_Widget *w = data;

	IF_FREE(w->source);
}

static EWL_CALLBACK_DEFN(fg_realize)
{
	Ewler_Widget *ewler_w = user_data;

	if( ewler_w->selected )
		widget_select(ewler_w);
	else
		widget_deselect(ewler_w);

	edje_object_signal_callback_add(w->theme_object, "down", "top_left",
																	fg_part_down, user_data);
	edje_object_signal_callback_add(w->theme_object, "up", "top_left",
																	fg_part_up, user_data);
	edje_object_signal_callback_add(w->theme_object, "down", "top_middle",
																	fg_part_down, user_data);
	edje_object_signal_callback_add(w->theme_object, "up", "top_middle",
																	fg_part_up, user_data);
	edje_object_signal_callback_add(w->theme_object, "down", "top_right",
																	fg_part_down, user_data);
	edje_object_signal_callback_add(w->theme_object, "up", "top_right",
																	fg_part_up, user_data);

	edje_object_signal_callback_add(w->theme_object, "down", "middle_right",
																	fg_part_down, user_data);
	edje_object_signal_callback_add(w->theme_object, "up", "middle_right",
																	fg_part_up, user_data);
	
	edje_object_signal_callback_add(w->theme_object, "down", "bottom_right",
																	fg_part_down, user_data);
	edje_object_signal_callback_add(w->theme_object, "up", "bottom_right",
																	fg_part_up, user_data);
	edje_object_signal_callback_add(w->theme_object, "down", "bottom_middle",
																	fg_part_down, user_data);
	edje_object_signal_callback_add(w->theme_object, "up", "bottom_middle",
																	fg_part_up, user_data);
	edje_object_signal_callback_add(w->theme_object, "down", "bottom_left",
																	fg_part_down, user_data);
	edje_object_signal_callback_add(w->theme_object, "up", "bottom_left",
																	fg_part_up, user_data);

	edje_object_signal_callback_add(w->theme_object, "down", "middle_left",
																	fg_part_down, user_data);
	edje_object_signal_callback_add(w->theme_object, "up", "middle_left",
																	fg_part_up, user_data);
}

static EWL_CALLBACK_DEFN(fg_mouse_move)
{
	Ewler_Widget *ewler_w = user_data;
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Move *ev = ev_data;

	if( ewler_w->mouse.state & EWLER_WIDGET_MOUSE_DOWN &&
			ewler_w->mouse.button == 1 ) {
		int	dx = ev->x - ewler_w->mouse.state_x;
		int	dy = ev->y - ewler_w->mouse.state_y;
		
		if( ewler_w->source ) {
			int *x, *y, *u, *v;

			x = &ewler_w->corners.x;
			y = &ewler_w->corners.y;
			u = &ewler_w->corners.u;
			v = &ewler_w->corners.v;

			if( strstr( ewler_w->source, "left" ) )
				*x += dx;
			if( strstr( ewler_w->source, "right" ) )
				*u += dx;
			if( strstr( ewler_w->source, "top" ) ) {
				if( ev->modifiers & EWL_KEY_MODIFIER_CTRL )
					*y = *v - *u + *x;
				else
					*y += dy;
			}
			if( strstr( ewler_w->source, "bottom" ) ) {
				if( ev->modifiers & EWL_KEY_MODIFIER_CTRL )
					*v = *y + *u - *x;
				else
					*v += dy;
			}

			if( *u < *x )
				*u = *x;
			if( *v < *y )
				*v = *y;

			ewl_object_preferred_inner_size_set(EWL_OBJECT(ewler_w->fg),
																					*u - *x, *v - *y);
			ewl_object_minimum_size_set(EWL_OBJECT(ewler_w->fg), *u - *x, *v - *y);
			ewl_object_geometry_request(EWL_OBJECT(ewler_w->fg),
																	*x, *y, *u - *x, *v - *y);
			ewl_object_preferred_inner_size_set(EWL_OBJECT(ewler_w->bg),
																					*u - *x, *v - *y);
			ewl_object_minimum_size_set(EWL_OBJECT(ewler_w->fg), *u - *x, *v - *y);
			ewl_object_geometry_request(EWL_OBJECT(ewler_w->fg),
																	*x, *y, *u - *x, *v - *y);
			ewl_object_preferred_inner_size_set(EWL_OBJECT(ewler_w->w),
																					*u - *x, *v - *y);
			ewl_object_minimum_size_set(EWL_OBJECT(ewler_w->w), *u - *x, *v - *y);
			ewl_object_geometry_request(EWL_OBJECT(ewler_w->w),
																	*x, *y, *u - *x, *v - *y);

			ewler_w->mouse.state_x = ev->x;
			ewler_w->mouse.state_y = ev->y;
		} else {
			Ecore_List *selected;

			selected = form_selected();

			if( selected ) {
				ecore_list_first_goto(selected);

				while( (ewler_w = ecore_list_next(selected)) ) {
					ewl_object_position_request(EWL_OBJECT(ewler_w->bg),
																			CURRENT_X(ewler_w->bg) + dx,
																			CURRENT_Y(ewler_w->bg) + dy);
					ewl_object_position_request(EWL_OBJECT(ewler_w->fg),
																			CURRENT_X(ewler_w->fg) + dx,
																			CURRENT_Y(ewler_w->fg) + dy);
					ewl_object_position_request(EWL_OBJECT(ewler_w->w),
																			CURRENT_X(ewler_w->w) + dx,
																			CURRENT_Y(ewler_w->w) + dy);

					ewler_w->mouse.state_x = ev->x;
					ewler_w->mouse.state_y = ev->y;
				}
			}
		}
	}

	embed = ewl_embed_widget_find(w);
	if( embed )
		evas_event_feed_mouse_move(embed->evas, ev->x, ev->y, 0, NULL);
}

static EWL_CALLBACK_DEFN(fg_mouse_in)
{
	Ewler_Widget *ewler_w = user_data;
	Ewl_Embed *embed;

	ewler_w->mouse.state |= EWLER_WIDGET_MOUSE_IN;

	embed = ewl_embed_widget_find(w);
	evas_event_feed_mouse_in(embed->evas, 0, NULL);
}

static EWL_CALLBACK_DEFN(fg_mouse_out)
{
	Ewler_Widget *ewler_w = user_data;
	Ewl_Embed *embed;

	if( !(ewler_w->mouse.state & EWLER_WIDGET_MOUSE_DOWN) )
		ewler_w->mouse.state = 0;

	embed = ewl_embed_widget_find(w);
	evas_event_feed_mouse_out(embed->evas, 0, NULL);
}

static EWL_CALLBACK_DEFN(fg_mouse_down)
{
	Ewler_Widget *ewler_w = user_data;
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Down *ev = ev_data;
	bool multi;

	if( !ewler_w->selectable ||
			widget_under_cursor )
		return;

	widget_under_cursor = true;

	ewler_w->mouse.state |= EWLER_WIDGET_MOUSE_DOWN;
	ewler_w->mouse.button = ev->button;
	ewler_w->mouse.state_x = ev->x;
	ewler_w->mouse.state_y = ev->y;

	ewler_w->corners.x = CURRENT_X(w);
	ewler_w->corners.y = CURRENT_Y(w);
	ewler_w->corners.u = CURRENT_X(w) + CURRENT_W(w);
	ewler_w->corners.v = CURRENT_Y(w) + CURRENT_H(w);

	if( ev->modifiers & (EWL_KEY_MODIFIER_CTRL | EWL_KEY_MODIFIER_SHIFT) )
		multi = true;
	else
		multi = false;

	if( ev->button == 1 && multi ) {
		form_toggle(ewler_w);
	} else if( ev->button == 1 ||
						 (!multi && !ecore_list_count(form_selected())) ) {
		Ecore_List *selected;

		selected = form_selected();
		if( selected &&
				(ecore_list_count(selected) != 1 ||
				 ecore_list_first_goto(selected) != ewler_w) ) {
			form_deselect_all();
			form_select(ewler_w);
		}
	}

	IF_FREE(ewler_w->source);

	embed = ewl_embed_widget_find(w);
	evas_event_feed_mouse_down(embed->evas, ev->button, EVAS_BUTTON_NONE,
														 0, NULL);
}

static EWL_CALLBACK_DEFN(fg_mouse_up)
{
	Ewler_Widget *ewler_w = user_data;
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Up *ev = ev_data;

	ewler_w->mouse.state &= ~EWLER_WIDGET_MOUSE_DOWN;
	ewler_w->mouse.button = ev->button;
	ewler_w->mouse.state_x = ev->x;
	ewler_w->mouse.state_y = ev->y;

	IF_FREE(ewler_w->source);

	embed = ewl_embed_widget_find(w);
	evas_event_feed_mouse_up(embed->evas, ev->button, EVAS_BUTTON_NONE, 0, NULL);
}

static EWL_CALLBACK_DEFN(widget_destroy)
{
	Ewler_Widget *ewler_w;
	int i;

	ewler_w = user_data;
	if( !ewler_w )
		return;

	if( ewler_w->form )
		form_remove(ewler_w->form, ewler_w);

	IF_FREE(ewler_w->source);
	IF_FREE(ewler_w->parent);

	ecore_hash_destroy(ewler_w->elems);

	for( i=0;i<EWL_CALLBACK_MAX;i++ )
		if( ewler_w->callbacks[i] )
			ecore_list_destroy(ewler_w->callbacks[i]);

	FREE(ewler_w);
}

Ewl_Widget *
widget_new( char *name )
{
	Ewler_Widget *w;

	if( !name )
		return NULL;

	w = spec_new( name );
	if( !w || !w->w )
		return NULL;

	w->selected = false;
	
	ewl_callback_append(w->w, EWL_CALLBACK_REALIZE, realize, w);
	ewl_callback_append(w->w, EWL_CALLBACK_CONFIGURE, configure, w);
	ewl_widget_show(w->w);

	w->bg = ewl_vbox_new();
	w->bg->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ewl_widget_appearance_set(w->bg, "select_bg");
	ewl_theme_data_str_set(w->bg, "/select_bg/file", EWLER_EET);
	ewl_theme_data_str_set(w->bg, "/select_bg/group", "select_bg");
	ewl_object_fill_policy_set(EWL_OBJECT(w->bg), EWL_FLAG_FILL_NONE);
	ewl_widget_data_set(w->bg, "EWLER_WIDGET", w);
	ewl_container_child_append(EWL_CONTAINER(w->bg), w->w);
	ewl_widget_show(w->bg);

	w->fg = ewl_vbox_new();
	w->fg->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ewl_widget_appearance_set(w->fg, "select_fg");
	ewl_theme_data_str_set(w->fg, "/select_fg/file", EWLER_EET);
	ewl_theme_data_str_set(w->fg, "/select_fg/group", "select_fg");
	ewl_callback_append(w->fg, EWL_CALLBACK_CONFIGURE, fg_configure, w);
	ewl_callback_append(w->fg, EWL_CALLBACK_REALIZE, fg_realize, w);
	ewl_callback_append(w->fg, EWL_CALLBACK_FOCUS_IN, fg_mouse_in, w);
	ewl_callback_append(w->fg, EWL_CALLBACK_FOCUS_OUT, fg_mouse_out, w);
	ewl_callback_append(w->fg, EWL_CALLBACK_MOUSE_DOWN, fg_mouse_down, w);
	ewl_callback_append(w->fg, EWL_CALLBACK_MOUSE_UP, fg_mouse_up, w);
	ewl_callback_append(w->fg, EWL_CALLBACK_MOUSE_MOVE, fg_mouse_move, w);
	ewl_callback_append(w->fg, EWL_CALLBACK_DESTROY, widget_destroy, w);
	ewl_object_fill_policy_set(EWL_OBJECT(w->fg), EWL_FLAG_FILL_NONE);
	ewl_widget_data_set(w->fg, "EWLER_WIDGET", w);
	ewl_container_child_append(EWL_CONTAINER(w->fg), w->bg);
	ewl_widget_show(w->fg);

	w->selectable = true;
	w->configured = false;

	return w->fg;
}

#include "ewler.h"

static EWL_CALLBACK_DEFN(_ewler_widget_realize_cb);
static EWL_CALLBACK_DEFN(_ewler_widget_configure_cb);

static EWL_CALLBACK_DEFN(_select_fg_realize_cb);
static EWL_CALLBACK_DEFN(_select_fg_mouse_in_cb);
static EWL_CALLBACK_DEFN(_select_fg_mouse_out_cb);
static EWL_CALLBACK_DEFN(_select_fg_mouse_move_cb);
static EWL_CALLBACK_DEFN(_select_fg_mouse_down_cb);
static EWL_CALLBACK_DEFN(_select_fg_mouse_up_cb);

static EWL_CALLBACK_DEFN(_select_bg_realize_cb);

static void _select_fg_part_down( void *data, Evas_Object *o,
																	const char *emission, const char *source );
static void _select_fg_part_up( void *data, Evas_Object *o,
																const char *emission, const char *source );

static int widget_selected = 0;
static Ewl_Widget *dragging = NULL;

Ewl_Widget *
ewler_widget_dragging_get( void )
{
	return dragging;
}

static void
_populate_attrs( void *value, void *user_data )
{
	Ecore_Hash_Node *node;
	Ewler_Spec_Elem *spec;
	Ewler_Elem *elem;
	Ecore_Hash *attrs;

	attrs = user_data;

	node = value;
	spec = node->value;

	elem = NEW(Ewler_Elem, 1);
	if( !elem )
		return;

	elem->spec = spec;
	elem->changed = 0;

	switch( spec->type ) {
		case EWLER_SPEC_ELEM_INT_TYPE:
			if( spec->type_info.int_type.has_default )
				elem->v.int_val = spec->type_info.int_type.default_value;
			else
				elem->v.int_val = 0;
			break;
		case EWLER_SPEC_ELEM_STRING_TYPE: elem->v.string_val = NULL; break;
		case EWLER_SPEC_ELEM_POINTER_TYPE: elem->v.pointer_val = NULL; break;
		case EWLER_SPEC_ELEM_STRUCT_TYPE:
			elem->v.struct_val = ecore_hash_new( ecore_str_hash, ecore_str_compare );
			ecore_hash_for_each_node( spec->type_info.struct_child_elems,
																_populate_attrs, elem->v.struct_val );
			break;
		case EWLER_SPEC_ELEM_ENUM_TYPE:
			if( spec->type_info.enum_type.has_default )
				elem->v.enum_val = spec->type_info.enum_type.default_value;
			else
				elem->v.enum_val = 0;
			break;
	}

	ecore_hash_set( attrs, node->key, elem );
}

Ewler_Widget *
ewler_widget_new( char *widget_name, Ewler_Project_File *form )
{
	Ewler_Widget *w;
	Ewler_Spec_Widget *spec;
	static char buf[256];

	w = NEW(Ewler_Widget, 1);

	w->realized = 0;
	w->dragging = 0;

	w->bg = ewl_vbox_new();
	ewl_widget_appearance_set( w->bg, "select_bg" );
	ewl_theme_data_str_set( w->bg, "/select_bg/file",
													PACKAGE_DATA_DIR"/themes/ewler.eet" );
	ewl_theme_data_str_set( w->bg, "/select_bg/group", "select_bg" );
	ewl_callback_append( w->bg, EWL_CALLBACK_REALIZE,
											 _select_bg_realize_cb, w );
	ewl_widget_layer_set( w->bg, 0 );
	ewl_widget_data_set( w->bg, "Ewler_Widget", w );
	ewl_object_fill_policy_set( EWL_OBJECT(w->bg), EWL_FLAG_FILL_NONE );

	w->fg = ewl_vbox_new();
	ewl_widget_appearance_set( w->fg, "select_fg" );
	ewl_theme_data_str_set( w->fg, "/select_fg/file",
													PACKAGE_DATA_DIR"/themes/ewler.eet" );
	ewl_theme_data_str_set( w->fg, "/select_fg/group", "select_fg" );
	ewl_callback_append( w->fg, EWL_CALLBACK_REALIZE,
											 _select_fg_realize_cb, w );
	ewl_callback_append( w->fg, EWL_CALLBACK_FOCUS_IN,
											 _select_fg_mouse_in_cb, w );
	ewl_callback_append( w->fg, EWL_CALLBACK_FOCUS_OUT,
											 _select_fg_mouse_out_cb, w );
	ewl_callback_append( w->fg, EWL_CALLBACK_MOUSE_MOVE,
											 _select_fg_mouse_move_cb, w );
	ewl_callback_append( w->fg, EWL_CALLBACK_MOUSE_DOWN,
											 _select_fg_mouse_down_cb, w );
	ewl_callback_append( w->fg, EWL_CALLBACK_MOUSE_UP,
											 _select_fg_mouse_up_cb, w );
	ewl_widget_layer_set( w->fg, 0 );
	ewl_object_fill_policy_set( EWL_OBJECT(w->fg), EWL_FLAG_FILL_NONE );

	ewl_container_child_append( EWL_CONTAINER(w->bg), w->fg );
	ewl_widget_show( w->fg );

	w->spec = ewler_spec_get( widget_name );
	w->ewl_w = ewler_spec_call_ctor( widget_name );
	w->attrs = ecore_hash_new( ecore_str_hash, ecore_str_compare );

	spec = w->spec;

	while( spec ) {
		ecore_hash_for_each_node( spec->elem_specs, _populate_attrs, w->attrs );

		spec = ewler_spec_get( spec->super );
	}

	w->form = form;

	if( w->spec->count )
		sprintf( buf, "%s%d", widget_name, w->spec->count );
	else
		sprintf( buf, "%s", widget_name );

	w->name = strdup( buf );

	ewl_callback_append( w->ewl_w, EWL_CALLBACK_REALIZE,
											 _ewler_widget_realize_cb, w );
	ewl_callback_append( w->ewl_w, EWL_CALLBACK_CONFIGURE,
											 _ewler_widget_configure_cb, w );

	ewl_object_fill_policy_set( EWL_OBJECT(w->ewl_w), EWL_FLAG_FILL_NONE );
	ewl_container_child_append( EWL_CONTAINER(w->fg), w->ewl_w );
	ewl_widget_show( w->ewl_w );

	return w;
}

Ewler_Widget *
ewler_widget_from_bg( Ewl_Widget *bg )
{
	return ewl_widget_data_get( bg, "Ewler_Widget" );
}

void
ewler_widget_select( Ewler_Widget *w )
{
	w->selected = 1;

	ewl_widget_state_set( w->fg, "default" );
	ewler_inspector_widget_set( w );
}

void
ewler_widget_deselect( Ewler_Widget *w )
{
	w->selected = 0;

	ewl_widget_state_set( w->fg, "deselect" );
}

void
ewler_widget_toggle( Ewler_Widget *w )
{
	w->selected ? ewler_widget_deselect(w) : ewler_widget_select(w);
}

void
ewler_widget_attr_set_from_xml( Ewler_Widget *w, Ecore_Hash *attrs, EXML *xml )
{
	Ewler_Elem *elem;
	char *name, *value, *endptr;
	int int_val;

	name = exml_attribute_get( xml, "name" );
	value = exml_value_get( xml );

	elem = ecore_hash_get( attrs, name );

	if( !elem )
		return;

	switch( elem->spec->type ) {
		case EWLER_SPEC_ELEM_INT_TYPE:
			int_val = strtol( value, &endptr, 0 );
			if( endptr )
				return;
			elem->v.int_val = int_val;
			if( elem->spec->set_func )
				elem->spec->set_func( w->ewl_w, elem->v.int_val );
			break;
		case EWLER_SPEC_ELEM_STRING_TYPE:
			elem->v.string_val = value;
			if( elem->v.string_val )
				elem->v.string_val = strdup( elem->v.string_val );
			if( elem->spec->set_func )
				elem->spec->set_func( w->ewl_w, elem->v.string_val );
			break;
		case EWLER_SPEC_ELEM_POINTER_TYPE: break;
		case EWLER_SPEC_ELEM_STRUCT_TYPE:
			exml_down( xml );
			do {
				ewler_widget_attr_set_from_xml( w, elem->v.struct_val, xml );
			} while( exml_next_nomove( xml ) );
			exml_up( xml );
			break;
		case EWLER_SPEC_ELEM_ENUM_TYPE: break;
	}
}

char *
ewler_widget_elem_to_s( Ewler_Elem *elem )
{
	static char buf[256];

	switch( elem->spec->type ) {
		case EWLER_SPEC_ELEM_INT_TYPE: sprintf( buf, "%d", elem->v.int_val ); break;
		case EWLER_SPEC_ELEM_STRING_TYPE:
			strcpy( buf, elem->v.string_val ? elem->v.string_val : "" ); break;
		case EWLER_SPEC_ELEM_POINTER_TYPE:
			strcpy( buf, elem->v.pointer_val ? elem->v.pointer_val : "" );
			break;
		case EWLER_SPEC_ELEM_ENUM_TYPE:
#if 0
			{
				char *val = ecore_hash_get( elem->spec->type_info.enum_type.map_rev,
																		(void *) elem->v.enum_val );
				printf( "%s\n", val );
				if( val )
					strcpy( buf, val );
			}
#endif
			strcpy( buf, "" );
			break;
	}

	return buf;
}	

static void
_attr_call_get( void *value, void *user_data )
{
	Ecore_Hash_Node *node;
	Ewler_Widget *ewler_w;
	Ewler_Elem *elem;
	Ewl_Widget *w;
	int int_val;
	char *string_val;

	ewler_w = user_data;
	w = ewler_w->ewl_w;

	node = value;
	elem = node->value;

	if( !elem->spec->get_func &&
			elem->spec->type != EWLER_SPEC_ELEM_STRUCT_TYPE )
		return;

	switch( elem->spec->type ) {
		case EWLER_SPEC_ELEM_INT_TYPE:
			int_val = elem->spec->get_func( w );

			if( ewler_w->realized && int_val != elem->v.int_val )
				elem->changed = 1;

			elem->v.int_val = int_val;
			break;
		case EWLER_SPEC_ELEM_STRING_TYPE:
			string_val = (char *) elem->spec->get_func( w );

			if( ewler_w->realized && string_val != elem->v.string_val )
				elem->changed = 1;

			if( string_val && elem->v.string_val &&
					!strcmp( string_val, elem->v.string_val ) )
				elem->changed = 0;

			if( elem->v.string_val )
				free( elem->v.string_val );

			elem->v.string_val = string_val;
			break;
		case EWLER_SPEC_ELEM_POINTER_TYPE:
			if( elem->spec->get_func )
				elem->v.pointer_val = (void *) elem->spec->get_func( w );
			break;
		case EWLER_SPEC_ELEM_STRUCT_TYPE:
			ecore_hash_for_each_node( elem->v.struct_val, _attr_call_get, ewler_w );
			break;
		case EWLER_SPEC_ELEM_ENUM_TYPE:
			if( elem->spec->get_func )
				elem->v.enum_val = elem->spec->get_func( w );
			break;
	}
}

static EWL_CALLBACK_DEFN(_ewler_widget_realize_cb)
{
	Ewler_Widget *ewler_w;

	ewl_callback_del( w, EWL_CALLBACK_REALIZE, _ewler_widget_realize_cb );

	ewler_w = user_data;

	ecore_hash_for_each_node( ewler_w->attrs, _attr_call_get, ewler_w );
	ewler_inspector_update();

	ewler_w->realized = 1;
}

static EWL_CALLBACK_DEFN(_ewler_widget_configure_cb)
{
	Ewler_Widget *ewler_w;

	ewler_w = user_data;

	ecore_hash_for_each_node( ewler_w->attrs, _attr_call_get, ewler_w );
	ewler_inspector_update();
}

static EWL_CALLBACK_DEFN(_select_bg_realize_cb)
{
	evas_object_layer_set( w->theme_object, -990 );
}

static EWL_CALLBACK_DEFN(_select_fg_realize_cb)
{
	ewl_widget_state_set( w, "deselect" );
	evas_object_layer_set( w->theme_object, 1000 );

	edje_object_signal_callback_add( w->theme_object, "down", "top_left",
																	 _select_fg_part_down, user_data);
	edje_object_signal_callback_add( w->theme_object, "up", "top_left",
																	 _select_fg_part_up, user_data);
	edje_object_signal_callback_add( w->theme_object, "down", "top_middle",
																	 _select_fg_part_down, user_data);
	edje_object_signal_callback_add( w->theme_object, "up", "top_middle",
																	 _select_fg_part_up, user_data);
	edje_object_signal_callback_add( w->theme_object, "down", "top_right",
																	 _select_fg_part_down, user_data);
	edje_object_signal_callback_add( w->theme_object, "up", "top_right",
																	 _select_fg_part_up, user_data);
	edje_object_signal_callback_add( w->theme_object, "down", "middle_right",
																	 _select_fg_part_down, user_data);
	edje_object_signal_callback_add( w->theme_object, "up", "middle_right",
																	 _select_fg_part_up, user_data);
	edje_object_signal_callback_add( w->theme_object, "down", "bottom_right",
																	 _select_fg_part_down, user_data);
	edje_object_signal_callback_add( w->theme_object, "up", "bottom_right",
																	 _select_fg_part_up, user_data);
	edje_object_signal_callback_add( w->theme_object, "down", "bottom_middle",
																	 _select_fg_part_down, user_data);
	edje_object_signal_callback_add( w->theme_object, "up", "bottom_middle",
																	 _select_fg_part_up, user_data);
	edje_object_signal_callback_add( w->theme_object, "down", "bottom_left",
																	 _select_fg_part_down, user_data);
	edje_object_signal_callback_add( w->theme_object, "up", "bottom_left",
																	 _select_fg_part_up, user_data);
	edje_object_signal_callback_add( w->theme_object, "down", "middle_left",
																	 _select_fg_part_down, user_data);
	edje_object_signal_callback_add( w->theme_object, "up", "middle_left",
																	 _select_fg_part_up, user_data);
}

static EWL_CALLBACK_DEFN(_select_fg_mouse_in_cb)
{
	Ewl_Embed *embed;

	embed = ewl_embed_widget_find(w);

	evas_event_feed_mouse_in( embed->evas, NULL );
}

static EWL_CALLBACK_DEFN(_select_fg_mouse_out_cb)
{
	Ewl_Embed *embed;

	embed = ewl_embed_widget_find(w);

	evas_event_feed_mouse_out( embed->evas, NULL );
}

static EWL_CALLBACK_DEFN(_select_fg_mouse_move_cb)
{
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Move *move_ev = ev;
	Ewler_Widget *ewler_w = user_data;
	static int lx = 0, ly = 0;
	int dx, dy;

	dx = move_ev->x - lx;
	dy = move_ev->y - ly;

	if( move_ev->modifiers & EWL_KEY_MODIFIER_CTRL )
		dy = dx;

	embed = ewl_embed_widget_find(w);

	evas_event_feed_mouse_move( embed->evas, move_ev->x, move_ev->y, NULL );

	if( ewler_w->dragging ) {
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
				if( move_ev->modifiers & EWL_KEY_MODIFIER_CTRL )
					*y = *v - *u + *x;
				else
					*y += dy;
			}
			if( strstr( ewler_w->source, "bottom" ) ) {
				if( move_ev->modifiers & EWL_KEY_MODIFIER_CTRL )
					*v = *y + *u - *x;
				else
					*v += dy;
			}

			if( *u < *x )
				*u = *x;
			if( *v < *y )
				*v = *y;

			ewl_object_preferred_inner_size_set( EWL_OBJECT(ewler_w->ewl_w),
																					 *u - *x, *v - *y );
			ewl_object_geometry_request( EWL_OBJECT(ewler_w->ewl_w), *x, *y,
																	 *u - *x, *v - *y );
			ewl_object_preferred_inner_size_set( EWL_OBJECT(ewler_w->fg),
																					 *u - *x, *v - *y );
			ewl_object_geometry_request( EWL_OBJECT(ewler_w->fg), *x, *y,
																	 *u - *x, *v - *y );
			ewl_object_preferred_inner_size_set( EWL_OBJECT(ewler_w->bg),
																					 *u - *x, *v - *y );
			ewl_object_geometry_request( EWL_OBJECT(ewler_w->bg), *x, *y,
																	 *u - *x, *v - *y );
		} else {
			Ecore_List *l;

			l = ewler_form_select_get();

			while( (ewler_w = ecore_list_next(l)) ) {
				ewl_object_position_request( EWL_OBJECT(ewler_w->bg),
																		 CURRENT_X(ewler_w->bg) + dx,
																		 CURRENT_Y(ewler_w->bg) + dy );
				ewl_object_position_request( EWL_OBJECT(ewler_w->fg),
																		 CURRENT_X(ewler_w->fg) + dx,
																		 CURRENT_Y(ewler_w->fg) + dy );
				ewl_object_position_request( EWL_OBJECT(ewler_w->ewl_w),
																		 CURRENT_X(ewler_w->ewl_w) + dx,
																		 CURRENT_Y(ewler_w->ewl_w) + dy );
			}
		}
	}

	lx = move_ev->x;
	ly = move_ev->y;
}

static EWL_CALLBACK_DEFN(_select_fg_mouse_down_cb)
{
	Ewler_Widget *ewler_w = user_data;
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Down *down_ev = ev;

	embed = ewl_embed_widget_find( w );

	if( widget_selected )
		return;

	widget_selected = 1;

	if( !(down_ev->modifiers &
				(EWL_KEY_MODIFIER_CTRL | EWL_KEY_MODIFIER_SHIFT)) ) {
		ewler_form_select_clear();
		ewler_widget_select( ewler_w );
	} else
		ewler_widget_toggle( ewler_w );

	ewler_w->dragging = 1;
	dragging = ewler_w->fg;

	if( ewler_w->source )
		free( ewler_w->source );
	ewler_w->source = NULL;

	ewler_w->corners.x = CURRENT_X(w);
	ewler_w->corners.y = CURRENT_Y(w);
	ewler_w->corners.u = CURRENT_X(w) + CURRENT_W(w);
	ewler_w->corners.v = CURRENT_Y(w) + CURRENT_H(w);

	evas_event_feed_mouse_down( embed->evas, down_ev->button,
														  EVAS_BUTTON_NONE, NULL );

}

static EWL_CALLBACK_DEFN(_select_fg_mouse_up_cb)
{
	Ewler_Widget *ewler_w = user_data;
	Ewl_Embed *embed;
	Ewl_Event_Mouse_Up *up_ev = ev;

	widget_selected = 0;

	embed = ewl_embed_widget_find( w );

	ewler_w->dragging = 0;
	dragging = NULL;

	if( ewler_w->source )
		free( ewler_w->source );
	ewler_w->source = NULL;

	evas_event_feed_mouse_up( embed->evas, up_ev->button,
														EVAS_BUTTON_NONE, NULL );
}

static void
_select_fg_part_down( void *data, Evas_Object *o,
											const char *emission, const char *source )
{
	Ewler_Widget *w = data;

	if( w->source )
		free( w->source );

	w->source = strdup( source );
}

static void
_select_fg_part_up( void *data, Evas_Object *o,
										const char *emission, const char *source )
{
	Ewler_Widget *w = data;

	if( w->source )
		free( w->source );
	w->source = NULL;
}

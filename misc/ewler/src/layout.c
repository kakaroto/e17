#include <Ewl.h>

#include "ewler.h"
#include "form.h"
#include "selected.h"
#include "widgets.h"

static int count = 0;

static void layout_widgets( Ewler_Form *form, Ewl_Orientation orientation );

void
layout_horizontal_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = user_data;

	layout_widgets( form, EWL_ORIENTATION_HORIZONTAL );
}

void
layout_vertical_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ewler_Form *form = user_data;

	layout_widgets( form, EWL_ORIENTATION_VERTICAL );
}

static void
__layout_realize_cb( Ewl_Widget *w, void *ev_data, void *user_data )
{
	Ecore_List *cl = user_data;
	Ewl_Widget *cw;

	ecore_list_goto_first( cl );

	while( (cw = ecore_list_remove( cl )) )
		ewl_container_append_child( EWL_CONTAINER(w), cw );

	ecore_list_destroy( cl );

	ewl_callback_del( w, EWL_CALLBACK_REALIZE, __layout_realize_cb );
}

static void
layout_widgets( Ewler_Form *form, Ewl_Orientation orientation )
{
	Ewl_Widget *c_w, *s = ecore_list_goto_first( form->selected );
	Ewl_Widget *box, *parent;
	static char name[64];
	char *widget_name;

	if( s == form->overlay ) {
		if( form->layout ) {
			ewl_box_set_orientation( EWL_BOX(form->layout), orientation );
		} else {
			Ecore_List *cl = ecore_list_new();
			Ecore_List *pl = EWL_CONTAINER(form->overlay)->children;

			ecore_list_goto_first( pl );

			while( (s = ecore_list_next( pl )) ) {
				if( s == form->popup )
					continue;

				ecore_list_append( cl, s );
			}

			sprintf( name, "Layout%d", count++ );
			widget_name = strdup( name );

			form->layout = ewl_hbox_new();
			ewl_box_set_orientation( EWL_BOX(form->layout), orientation );
			widget_create_info( form->layout, "Ewl_Box", widget_name );

			ewl_object_request_position( EWL_OBJECT(form->layout), 0, 0 );
			ewl_object_set_fill_policy( EWL_OBJECT(form->layout),
																	EWL_FLAG_FILL_FILL );
			ewl_object_set_minimum_size( EWL_OBJECT(form->layout), 1, 1 );
			ewl_object_set_maximum_size( EWL_OBJECT(form->layout), 100000, 100000 );
			ewl_callback_append( form->layout, EWL_CALLBACK_REALIZE,
													 __layout_realize_cb, cl );

			ewl_container_append_child( EWL_CONTAINER(form->overlay), form->layout );
			form_add_widget( form, widget_name, form->layout );
			ewl_widget_show( form->layout );

			s = ewler_selected_new( form->layout );
			ewl_object_set_fill_policy( EWL_OBJECT(s), EWL_FLAG_FILL_FILL );
			ewl_widget_show( s );

			widget_changed( form->layout );

			form_selected_append( form, s );
		}
	} else if( ecore_list_nodes( form->selected ) == 1 &&
						 (c_w = ewler_selected_get(EWLER_SELECTED(s))) &&
						 widget_is_type( c_w, "Ewl_Box" ) ) {
		ewl_box_set_orientation( EWL_BOX(c_w), orientation );
	} else {
		Ecore_List *cl = ecore_list_new();

		parent = s->parent;

		while( (s = ecore_list_next( form->selected )) ) {
			if( s == form->overlay || parent != s->parent ) {
				/* can't group widgets that do not have a common parent */
				ecore_list_destroy( cl );
				return;
			}
			ecore_list_append( cl, s );
		}

		sprintf( name, "Layout%d", count++ );
		widget_name = strdup( name );

		box = ewl_hbox_new();
		ewl_box_set_orientation( EWL_BOX(box), orientation );
		widget_create_info( box, "Ewl_Box", widget_name );

		s = ecore_list_goto_first( form->selected );

		ewl_object_request_position( EWL_OBJECT(box), CURRENT_X(s), CURRENT_Y(s) );
		ewl_object_set_fill_policy( EWL_OBJECT(box), EWL_FLAG_FILL_NONE );
		ewl_object_set_minimum_size( EWL_OBJECT(box), 1, 1 );
		ewl_object_set_maximum_size( EWL_OBJECT(box), 100000, 100000 );
		ewl_callback_append( box, EWL_CALLBACK_REALIZE, __layout_realize_cb, cl );

		ewl_container_append_child( EWL_CONTAINER(parent), box );
		form_add_widget( form, widget_name, box );
		ewl_widget_show( box );

		s = ewler_selected_new( box );
		ewl_widget_show( s );

		form_selected_append( form, s );
	}
}

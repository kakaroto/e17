#include "ewler.h"

static EWL_CALLBACK_DEFN(_ewler_tool_clicked_cb);
static int _tool_create_in_selected( Ewler_Widget *w, Ewler_Tool *tool );

Ewl_Widget *
ewler_tool_new( char *icon_file, char *name, char *cursor,
								Ewler_Spec_Widget *widget )
{
	Ewler_Tool *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = NEW(Ewler_Tool, 1);
	if (!t)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewler_tool_init(t, icon_file, name, cursor, widget);

	DRETURN_PTR(EWL_WIDGET(t), DLEVEL_STABLE);
}

int
ewler_tool_init( Ewler_Tool *t, char *icon_file, char *name, char *cursor,
								 Ewler_Spec_Widget *widget )
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);

	w = EWL_WIDGET(t);

	if( !ewl_button_init( EWL_BUTTON(t), name ) )
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	t->name = strdup( name );
	t->widget = ewler_spec_get( name );

	ewl_callback_append( w, EWL_CALLBACK_CLICKED, _ewler_tool_clicked_cb, NULL );

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static
EWL_CALLBACK_DEFN(_ewler_tool_clicked_cb)
{
	Ewler_Project_File *form;

	form = ewler_form_active_get();

	if( form )
		_tool_create_in_selected(FORM(form).top, EWLER_TOOL(w));
}

static int
_tool_create_in_selected( Ewler_Widget *w, Ewler_Tool *tool )
{
	Ewler_Project_File *form;
	Ewler_Widget *ewler_w;

	form = ewler_form_active_get();

	if( !w )
		return 1;

	if( ewler_spec_is( w->spec, "Ewl_Container" ) ) {
		if( w->selected ) {
			ewler_w = ewler_widget_new( tool->widget->name, form );
			printf( "appending to %s\n", w->name );
			
			ewl_container_child_append( EWL_CONTAINER(w->ewl_w), ewler_w->bg );
			ewl_widget_show( ewler_w->bg );

			return 1;
		} else {
			Ewl_Widget *child_bg;

			ewl_container_child_iterate_begin( EWL_CONTAINER(w->ewl_w) );

			while( (child_bg = ewl_container_child_next( EWL_CONTAINER(w->ewl_w) )) )
				if( _tool_create_in_selected( ewler_widget_from_bg(child_bg), tool ) )
					return 1;
		}
	} else {
		return 0;
	}

	if( w == FORM(form).top ) {
		ewler_w = ewler_widget_new( tool->widget->name, form );
		ewl_container_child_append( EWL_CONTAINER(w->ewl_w), ewler_w->bg );
		ewl_widget_show( ewler_w->bg );
	}

	return 0;
}

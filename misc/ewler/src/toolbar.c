#include "ewler.h"

static Ewl_Widget *toolbar;

static EWL_CALLBACK_DEFN(_ewler_toolbar_toggle);

void
ewler_toolbar_init( void )
{
	Ewl_Widget *vbox;
	Ewl_Widget *tool;
	Ewler_Spec_Widget *spec;
	Ecore_List *specs;
	
	toolbar = ewl_window_new();

	ewl_window_title_set( EWL_WINDOW(toolbar), "Toolbar" );
	ewl_object_preferred_inner_size_set( EWL_OBJECT(toolbar), 300, 400 );
	ewl_callback_append( toolbar, EWL_CALLBACK_DELETE_WINDOW,
											 _ewler_toolbar_toggle, NULL );

	vbox = ewl_vbox_new();
	ewl_container_child_append( EWL_CONTAINER(toolbar), vbox );
	ewl_widget_show( vbox );

	specs = ewler_specs_get();

	ecore_list_goto_first( specs );

	while( (spec = ecore_list_next( specs )) ) {
		tool = ewler_tool_new( PACKAGE_IMAGE_DIR, spec->name, NULL, NULL );
		ewl_container_child_append( EWL_CONTAINER(vbox), tool );
		ewl_object_fill_policy_set( EWL_OBJECT(tool), EWL_FLAG_FILL_NONE );
		ewl_widget_show( tool );
	}

	ewl_widget_show( toolbar );
}

void
ewler_toolbar_toggle( void )
{
	if( VISIBLE(toolbar) )
		ewl_widget_hide( toolbar );
	else
		ewl_widget_show( toolbar );
}

static EWL_CALLBACK_DEFN(_ewler_toolbar_toggle)
{
	ewler_toolbar_toggle();
}

#include <stdio.h>

#include <Ewl.h>
#include "widgets.h"
#include "callback.h"

static FILE *fout = NULL;

const char *init_str = "\n"
"#include <Ewl.h>\n"
"\n"
"Ewl_Widget *\n"
"%s_new( void )\n"
"{\n";

void
gen_decls( Ecore_List *w )
{
	Widget_Data_Elem *data;
	Ecore_List *c, *children;

	data = widget_lookup_data( w, "ewler_widget_name" );

	if( data->w_str.value )
		fprintf( fout, "\tEwl_Widget *%s;\n", data->w_str.value );

	children = widget_get_children( w );

	if( children ) {
		ecore_list_goto_first( children );

		while( (c = ecore_list_next( children )) )
			gen_decls( c );
	}
}

void
gen_sets( Ecore_List *w, char *name, char *parent )
{
	Widget_Data_Elem *data;
	int args, i;

	ecore_list_goto_first( w );

	while( (data = ecore_list_next( w )) ) {
		if( data->type->w.set ) {
			switch( data->type->w.w_type ) {
				case WIDGET_INTEGER_TYPE:
					fprintf( fout, "\t%s( %s(%s), %d );\n",
									 data->type->w.set, data->type->w.cast,
									 name, data->w_int.value );
					break;
				case WIDGET_STRING_TYPE:
					if( data->w_str.value ) {
						fprintf( fout, "\t%s( %s(%s), \"%s\" );\n", 
										 data->type->w.set, data->type->w.cast,
										 name, data->w_str.value );
					}
					break;
				case WIDGET_STRUCT_TYPE:
					fprintf( fout, "\t%s( %s(%s), ",
									 data->type->w.set, data->type->w.cast, name );
					args = ecore_list_nodes( data->w_struct.members );

					for( i=0;i<args;i++ ) {
						Widget_Data_Elem *m;

						if( i )
							fprintf( fout, ", " );

						ecore_list_goto_first( data->w_struct.members );

						while( (m = ecore_list_next( data->w_struct.members )) )
							if( m->type->w.index == i )
								switch( m->type->w.w_type ) {
									case WIDGET_INTEGER_TYPE:
										fprintf( fout, "%d", m->w_int.value );
										break;
									case WIDGET_STRING_TYPE:
										if( m->w_str.value )
											fprintf( fout, "\"%s\"", m->w_str.value );
										else
											fprintf( fout, "NULL" );
										break;
									case WIDGET_ENUM_TYPE:
										{
											char *enum_name;

											enum_name = ecore_hash_get( data->type->w_enum.map_rev,
																									(void *) data->w_enum.value );

											if( enum_name )
												fprintf( fout, "%s", enum_name );
											else
												fprintf( fout, "0" );
										}
										break;
									case WIDGET_STRUCT_TYPE:
										/* well, we're fucked here */
										break;
								}
					}

					fprintf( fout, " );\n" );
					break;
				case WIDGET_ENUM_TYPE:
					{
						char *enum_name;

						enum_name = ecore_hash_get( data->type->w_enum.map_rev,
																				(void *) data->w_enum.value );

						if( enum_name )
							fprintf( fout, "\t%s( %s(%s), %s );\n",
											 data->type->w.set, data->type->w.cast,
											 name, enum_name );
					}
					break;
			}
		} else if( data->type->w.w_type == WIDGET_STRUCT_TYPE )
			gen_sets( data->w_struct.members, name, NULL );
	}
	
	/**
	 * callbacks to enter here
	 */
	if( parent ) {
		fprintf( fout, "\tewl_container_child_append( EWL_CONTAINER(%s), %s );\n",
						 parent, name );
		fprintf( fout, "\tewl_widget_show( %s );\n\n", name );
	} else {
		fprintf( fout, "\n" );
	}
}

void
gen_callbacks( Ecore_List *w, char *name, char *top_widget )
{
  Ecore_List *cb_list = ewler_callbacks_get( w );
	Ewler_Callback *cb;

	ecore_list_goto_first( cb_list );
	while( (cb = ecore_list_next( cb_list )) )
		fprintf( fout, "\tewl_callback_append( %s, %s, %s, %s );\n",
						 name, ewler_callback_string( cb->callback ), cb->handler,
						 top_widget );

	fprintf( fout, "\n" );
}

void
gen_inits( Ecore_List *w, char *parent )
{
  static char *top_widget;
	Widget_Data_Elem *data;
	Ecore_List *c, *children;
	char *class, *name;
	int args, i;

	class = widget_get_class( w );

	data = widget_lookup_data( w, "ewler_widget_name" );

	name = data->w_str.value;

	if( parent == NULL )
	  top_widget = name;

	ecore_list_goto_first( w );
	fprintf( fout, "\t%s = %s( ", name, widget_get_ctor( class ) );
	args = widget_get_ctor_nargs( class );

	for( i=0;i<args;i++ ) {
		Widget_Data_Elem *ctor_arg;

		if( i )
			fprintf( fout, ", " );
	
		ctor_arg = widget_lookup_ctor_arg( w, i );

		if( ctor_arg ) {
			switch( ctor_arg->type->w.w_type ) {
				case WIDGET_INTEGER_TYPE:
					fprintf( fout, "%d", ctor_arg->w_int.value );
					break;
				case WIDGET_STRING_TYPE:
					if( ctor_arg->w_str.value )
						fprintf( fout, "\"%s\"", ctor_arg->w_str.value );
					else
						fprintf( fout, "NULL" );
					break;
				case WIDGET_STRUCT_TYPE:
				case WIDGET_POINTER_TYPE: fprintf( fout, "NULL" ); break;
				case WIDGET_ENUM_TYPE:
					{
						char *enum_name;

						enum_name = ecore_hash_get( ctor_arg->type->w_enum.map_rev,
																				(void *) ctor_arg->w_enum.value );
						if( enum_name )
							fprintf( fout, "%s", enum_name );
						else
							fprintf( fout, "0" );
					}
					break;
			}
		} else
			fprintf( fout, "NULL" );
	}

	fprintf( fout, " );\n\n" );

	gen_sets( w, name, parent );

	gen_callbacks( w, name, top_widget );

	children = widget_get_children( w );

	if( children ) {
		ecore_list_goto_first( children );

		while( (c = ecore_list_next( children )) ) {
			gen_inits( c, name );
		}
	}
}

void
gen_ret( Ecore_List *w )
{
	Widget_Data_Elem *data;

	data = widget_lookup_data( w, "ewler_widget_name" );

	fprintf( fout, "\treturn %s;\n", data->w_str.value );
}

void
generate_output( Ecore_List *info, char *prefix, int fd )
{
	fout = fdopen( fd, "w" );
	Ecore_List *children, *child;

	fprintf( fout, init_str, prefix );

	gen_decls( info );

	fprintf( fout, "\n" );

	gen_inits( info, NULL );

	gen_ret( info );

	fprintf( fout, "}\n" );

	fclose( fout );
}

#include <dlfcn.h>

#include "ewler.h"

static Ecore_List *specs;
static void *dl_handle;

void
ewler_spec_init( void )
{
	specs = ecore_list_new();
}

Ecore_List *
ewler_specs_get( void )
{
	return specs;
}

static void _create_elem( EXML *xml, Ecore_Hash *container )
{
	char *type;
	Ewler_Spec_Elem *elem;

	elem = NEW(Ewler_Spec_Elem, 1);

	elem->name = exml_value_get( xml );
	if( elem->name )
		elem->name = strdup( strip(elem->name) );

	type = exml_attribute_get( xml, "type" );

	elem->set_name = exml_attribute_get( xml, "set" );
	if( elem->set_name ) {
		elem->set_name = strdup( elem->set_name );
		elem->set_func = dlsym( dl_handle, elem->set_name );
	}

	elem->get_name = exml_attribute_get( xml, "get" );
	if( elem->get_name ) {
		elem->get_name = strdup( elem->get_name );
		elem->get_func = dlsym( dl_handle, elem->get_name );
	}

	if( !strcmp( type, "int" ) ) {
		char *default_value;

		default_value = exml_attribute_get( xml, "default" );
		if( default_value ) {
			elem->type_info.int_type.has_default = 1;
			elem->type_info.int_type.default_value =
				strtol( default_value, NULL, 0 );
		}

		elem->type = EWLER_SPEC_ELEM_INT_TYPE;
	} else if( !strcmp( type, "string" ) ) {
		elem->type = EWLER_SPEC_ELEM_STRING_TYPE;
	} else if( !strcmp( type, "struct" ) ) {
		Ecore_Hash *child_elems;

		child_elems = ecore_hash_new( ecore_str_hash, ecore_str_compare );

		if( exml_down( xml ) ) {

			do {
				_create_elem( xml, child_elems );
			} while( exml_next_nomove( xml ) );

			exml_up( xml );
		}

		elem->type_info.struct_child_elems = child_elems;

		elem->type = EWLER_SPEC_ELEM_STRUCT_TYPE;
	} else if( !strcmp( type, "enum" ) ) {
		Ecore_Hash *map, *map_rev;
		char *default_value;

		default_value = exml_attribute_get( xml, "default" );
		if( default_value ) {
			elem->type_info.enum_type.has_default = 1;
			elem->type_info.enum_type.default_value =
				strtol( default_value, NULL, 0 );
		}

		map = ecore_hash_new( ecore_str_hash, ecore_str_compare );
		map_rev = ecore_hash_new( ecore_direct_hash, ecore_direct_compare );

		if( exml_down( xml ) ) {
			do {
				char *value, *id;
				int int_val;

				value = exml_attribute_get( xml, "value" );
				if( value )
					int_val = strtol( value, NULL, 0 );

				id = exml_attribute_get( xml, "id" );
				if( id )
					id = strdup( id );

				ecore_hash_set( map, id, value );
				ecore_hash_set( map_rev, value, id );

				exml_next_nomove( xml );
			} while( exml_next_nomove( xml ) );

			exml_up( xml );
		}

		elem->type_info.enum_type.map = map;
		elem->type_info.enum_type.map_rev = map_rev;

		elem->type = EWLER_SPEC_ELEM_ENUM_TYPE;
	} else {
		elem->type = EWLER_SPEC_ELEM_POINTER_TYPE;
	}

	ecore_hash_set( container, elem->name, elem );
}

int
ewler_spec_read( char *filename )
{
	EXML *xml;
	Ewler_Spec_Widget *spec;

	xml = exml_new();
	if( !xml )
		return -1;

	if( exml_file_read( xml, filename ) < 0 ) {
		exml_destroy( xml );
		return -1;
	}

	exml_goto_top( xml );
	if( exml_down( xml ) == NULL )
		goto done;

	do {
		char *visible, *nargs, *soname;

		spec = NEW(Ewler_Spec_Widget,1);

		spec->name = exml_attribute_get( xml, "name" );
		if( spec->name )
			spec->name = strdup( spec->name );

		spec->super = exml_attribute_get( xml, "super" );
		if( spec->super )
			spec->super = strdup( spec->super );

		soname = exml_attribute_get( xml, "soname" );
		if( soname )
			dl_handle = dlopen( soname, RTLD_NOW );
		else
			dl_handle = dlopen( NULL, RTLD_NOW );
		
		spec->ctor_name = exml_attribute_get( xml, "ctor" );
		if( spec->ctor_name ) {
			spec->ctor_name = strdup( spec->ctor_name );
			spec->ctor_func = dlsym( dl_handle, spec->ctor_name );
		}

		nargs = exml_attribute_get( xml, "nargs" );
		if( nargs )
			spec->ctor_nargs = strtol( nargs, NULL, 0 );
		else
			spec->ctor_nargs = 0;

		visible = exml_attribute_get( xml, "visible" );

		if( visible && !strcmp( visible, "no" ) )
			spec->visible = 0;
		else
			spec->visible = 1;

		if( spec->ctor_name )
			spec->ctor_func = dlsym( dl_handle, spec->ctor_name );

		spec->elem_specs = ecore_hash_new( ecore_str_hash, ecore_str_compare );

		if( exml_down( xml ) ) {
			do {
				_create_elem( xml, spec->elem_specs );
			} while( exml_next_nomove( xml ) );

			exml_up( xml );
		}

		ecore_list_append( specs, spec );

		dlclose( dl_handle );
	} while( exml_next_nomove( xml ) );

done:
	exml_destroy( xml );
	return 0;
}

Ewl_Widget *
ewler_spec_call_ctor( char *widget_name )
{
	Ewler_Spec_Widget *spec;
	Ewl_Widget *w = NULL;

	ecore_list_goto_first( specs );

	while( (spec = ecore_list_next( specs )) )
		if( !strcmp( spec->name, widget_name ) && spec->ctor_func ) {
			switch( spec->ctor_nargs ) {
				case 2: w = spec->ctor_func( widget_name, widget_name ); break;
				case 1: w = spec->ctor_func( widget_name ); break;
				case 0:
				default:
					w = spec->ctor_func();
			}
		}

	return w;
}

Ewler_Spec_Widget *
ewler_spec_get( char *widget_name )
{
	Ewler_Spec_Widget *spec;

	ecore_list_goto_first( specs );

	while( (spec = ecore_list_next( specs )) )
		if( widget_name && !strcmp( spec->name, widget_name ) )
			return spec;

	return NULL;
}

int
ewler_spec_is( Ewler_Spec_Widget *spec, char *type )
{
	while( spec != NULL ) {
		if( !strcmp( spec->name, type ) )
			return true;
		spec = ewler_spec_get( spec->super );
	}

	return false;
}

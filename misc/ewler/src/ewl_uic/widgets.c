/**
 * widgets.c - handles loading, organizing, and managing the widgets available
 * 	to users for plopping
 *
 * George Gensure
 */

#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include <string.h>
#include <Ewl.h>

#include "ewler.h"
#include "widgets.h"

static void process( xmlTextReaderPtr reader, void *dl_handle );
static int lookup_name( const xmlChar *name );

Ecore_Hash *widgets = NULL;
Ecore_Hash *widgets_info = NULL;
Ecore_Hash *widget_parents = NULL;
Ecore_Hash *widget_ctors = NULL;
Ecore_Hash *widget_ctor_args = NULL;
Ecore_Hash *widget_tree = NULL;

#define WIDGET_CLASS_TOKEN	1
#define WIDGET_ELEM_TOKEN		2
#define WIDGET_TEXT_TOKEN		3
#define WIDGET_MAX_TOKEN		WIDGET_TEXT_TOKEN

typedef struct Widget_Name Widget_Name;

static struct Widget_Name {
	const char *name;
	int token;
} widget_names[] = {
	{ "class", WIDGET_CLASS_TOKEN },
	{ "elem", WIDGET_ELEM_TOKEN },
	{ "#text", WIDGET_TEXT_TOKEN },
	{ NULL, -1 }
};

static int widget_error = 0;

int widget_get_name( Ewl_Object *o );

static Widget_Type_Elem widget_name_type = {
	{
		WIDGET_STRING_TYPE, "ewler_widget_name", 0,
		NULL, NULL, NULL
	}
};

char *
strudup( char *s )
{
	char *upper = strdup( s );

	s = upper;

	while( *s ) {
		*s = toupper( *s );
		*s++;
	}

	return upper;
}

int
widgets_load( const char *filename )
{
	xmlTextReaderPtr reader;
	void *dl_handle;
	int ret;
	
	dl_handle = dlopen( NULL, RTLD_NOW );
	reader = xmlNewTextReaderFilename( filename );

	if( !widgets ) {
		widgets = ecore_hash_new( ecore_str_hash, ecore_str_compare );
		widgets_info = ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
		widget_parents = ecore_hash_new( ecore_str_hash, ecore_str_compare );
		widget_ctors = ecore_hash_new( ecore_str_hash, ecore_str_compare );
		widget_ctor_args = ecore_hash_new( ecore_str_hash, ecore_str_compare );
		widget_tree = ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
	}

	if( !reader ) {
		fprintf( stderr, "Unable to open %s", filename );
		return -1;
	}

	while( (ret = xmlTextReaderRead( reader )) == 1 )
		process( reader, dl_handle );

	xmlFreeTextReader( reader );

	if( ret )
		fprintf( stderr, "Parse failed in file %s", filename );

	dlclose( dl_handle );
	return ret;
}

static Ecore_List *t_list = NULL;
static const char *t_parent = NULL;

static void
__add_to_t_list( void *val )
{
	Ecore_Hash_Node *n = ECORE_HASH_NODE(val);
	if( t_list && t_parent && n->value && !strcmp( n->value, t_parent ) )
		ecore_list_append( t_list, n->key );
	else if( t_parent == n->value )
		ecore_list_append( t_list, n->key );
}

/**
 * This needs to be synched!
 */
Ecore_List *
widgets_get_children( const char *parent )
{
	Ecore_List *children = ecore_list_new();
	t_list = children;
	t_parent = parent;

	ecore_hash_for_each_node( widget_parents, __add_to_t_list );
	t_list = NULL;
	t_parent = NULL;

	return children;
}

void
widget_set_error( int err )
{
	widget_error = err;
}

int
widget_get_error( void ) {
	int err = widget_error;

	widget_error = 0;

	return err;
}

static Ecore_List *w_info = NULL;

static void
__copy_w_info( void *val )
{
	Ecore_Hash_Node *n = ECORE_HASH_NODE(val);
	static Ecore_List *c_info = NULL;
	static int lvl = 0;
	Ecore_List *t_info = NULL;
	Widget_Data_Elem *elem;

	if( c_info == NULL )
		c_info = w_info;

	elem = ALLOC(Widget_Data_Elem);

	elem->type = n->value;

	switch( elem->type->w.w_type ) {
		case WIDGET_INTEGER_TYPE:
			elem->w_int.value = elem->type->w_int.default_value;
			break;
		case WIDGET_STRING_TYPE:
			if( elem->type->w_str.default_value )
				elem->w_str.value = strdup( elem->type->w_str.default_value );
			else
				elem->w_str.value = NULL;
			break;
		case WIDGET_POINTER_TYPE:
			elem->w_ptr.value = NULL;
			break;
		case WIDGET_STRUCT_TYPE:
			elem->w_struct.members = ecore_list_new();
			t_info = c_info;
			c_info = elem->w_struct.members;
			lvl++;

			ecore_hash_for_each_node( elem->type->w_struct.members, __copy_w_info );

			lvl--;
			c_info = t_info;
			break;
	}

	ecore_list_prepend( c_info, elem );

	if( !lvl )
		c_info = NULL;
}

Ecore_List *
widget_create_info( const char *name )
{
	const char *c_name = name;
	Ecore_Hash *class;
	Widget_Data_Elem *widget_name_elem;

	w_info = ecore_list_new();

	widget_name_elem = ALLOC(Widget_Data_Elem);
	widget_name_elem->type = &widget_name_type;
	widget_name_elem->w_str.value = NULL;

	while( c_name != NULL ) {
		class = ecore_hash_get( widgets, (char *) c_name );

		ecore_hash_for_each_node( class, __copy_w_info );

		c_name = ecore_hash_get( widget_parents, (char *) c_name );
	}
	ecore_list_prepend( w_info, widget_name_elem );

	ecore_hash_set( widgets_info, w_info, strdup( name ) );

	return w_info;
}

/**
 * Not to be confused with widgets_get_children, this returns all the children
 * of an instantiated widget, not the children in the hierarchy */
Ecore_List *
widget_get_children( Ecore_List *parent )
{
	return ECORE_LIST(ecore_hash_get( widget_tree, parent ));
}

void
widget_set_parent( Ecore_List *child, Ecore_List *parent )
{
	Ecore_List *children;

	if( !(children = ecore_hash_get( widget_tree, parent )) ) {
		children = ecore_list_new();

		ecore_hash_set( widget_tree, parent, children );
	}

	ecore_list_append( children, child );
}

char *
widget_get_class( Ecore_List *info )
{
	return (char *) ecore_hash_get( widgets_info, info );
}

char *
widget_get_ctor( char *class )
{
	return (char *) ecore_hash_get( widget_ctors, class );
}

int
widget_get_ctor_nargs( char *class ) {
	return (int) ecore_hash_get( widget_ctor_args, class );
}

void
__destroy_w_info( void *val )
{
	Widget_Data_Elem *data = val;

	switch( data->type->w.w_type ) {
		case WIDGET_INTEGER_TYPE: break;
		case WIDGET_STRING_TYPE:
			if( data->w_str.value ) {
				FREE(data->w_str.value);
			}
			break;
		case WIDGET_POINTER_TYPE:
#if 0
			if( data->w_ptr.value ) {
				FREE(data->w_ptr.value);
			}
#endif
			break;
		case WIDGET_STRUCT_TYPE:
			ecore_list_set_free_cb( data->w_struct.members, __destroy_w_info );
			ecore_list_destroy( data->w_struct.members );
			break;
	}

	FREE(data);
}

void
widget_destroy_info( Ecore_List *info )
{
	char *class;

	class = ecore_hash_remove( widgets_info, info );

	FREE(class);

	ecore_list_set_free_cb( info, __destroy_w_info );
	ecore_list_destroy( info );
}

Widget_Data_Elem *
widget_lookup_data( Ecore_List *info, char *elem )
{
	Widget_Data_Elem *data;

	ecore_list_goto_first( info );

	while( (data = ecore_list_next( info )) )
		if( !strcmp( data->type->w.name, elem ) )
			return data;

	return NULL;
}

void
widget_strset_info( Ecore_List *info, char *elem, char *value )
{
	Widget_Data_Elem *data;

	if( !value )
		return;

	ecore_list_goto_first( info );

	while( (data = ecore_list_next( info )) )
		if( !strcmp( data->type->w.name, elem ) ) {
			switch( data->type->w.w_type ) {
				case WIDGET_INTEGER_TYPE:
					data->w_int.value = strtol( value, NULL, 0 );
					return;
				case WIDGET_STRING_TYPE:
					if( value )
						data->w_str.value = strdup( value );
					else
						data->w_str.value = NULL;
					return;
			}
		}
}

static int
lookup_name( const xmlChar *name )
{
	int i = 0, token = -1;

	while( widget_names[i].name ) {
		if( !strcmp( name, widget_names[i].name ) )
			token = widget_names[i].token;
		i++;
	}

	return token;
}

Widget_Type_Elem *
elem_new( const char *type, xmlTextReaderPtr reader )
{
	const char *ptr;
	const xmlChar *xml_attr;
	Widget_Type_Elem *elem;
	int len;

	if( !type ) {
		fprintf( stderr, "elem_new: No type specified" );
		return NULL;
	}

	len = strlen(type);
	elem = ALLOC(Widget_Type_Elem);

	if( !elem ) {
		fprintf( stderr, "elem_new: Out of memory" );
		return NULL;
	}

	if( !strcmp( type, "int" ) ||
			((ptr = strrchr( type, ' ' )) && !strcmp( ptr+1, "int" ))) {
		elem->w_int.w_type = WIDGET_INTEGER_TYPE;
		xml_attr = xmlTextReaderGetAttribute( reader, "default" );
		if( xml_attr )
			elem->w_int.default_value = strtol( xml_attr, NULL, 0 );
		else
			elem->w_int.default_value = 0;
	} else if( !strcmp( type, "string" ) ) {
		elem->w_str.w_type = WIDGET_STRING_TYPE;
		xml_attr = xmlTextReaderGetAttribute( reader, "default" );
		if( xml_attr )
			elem->w_str.default_value = strdup( xml_attr );
		else
			elem->w_str.default_value = NULL;
	} else if( !strcmp( type, "struct" ) ) {
		elem->w_struct.w_type = WIDGET_STRUCT_TYPE;
		elem->w_struct.members =
			ecore_hash_new( ecore_str_hash, ecore_str_compare );
	} else if( type[len-1] == '*' && strpbrk( type, " \t" ) ) {
		elem->w_ptr.w_type = WIDGET_POINTER_TYPE;

		len = strcspn( type, " \t" ) + 1;
		elem->w_ptr.ptr_type = CALLOC(char, len);
		memcpy( elem->w_ptr.ptr_type, type, len );
	} else {
		fprintf( stderr, "elem_new: unsupported type: %s", type );
		FREE(elem);
	}

	return elem;
}

static void
process( xmlTextReaderPtr reader, void *dl_handle )
{
	const xmlChar *name, *value;
	const xmlChar *xml_attr;
	static char *class_name, *super_name;
	static Ecore_Hash *class, *elem_hash;
	static Widget_Type_Elem *elem, *last_elem = NULL;
	static void *ctor;
	static int nargs;
	int token;

	name = xmlTextReaderConstName( reader );
	value = xmlTextReaderConstValue( reader );

	token = lookup_name( name );

	switch( token ) {
		case WIDGET_CLASS_TOKEN:
			switch( xmlTextReaderNodeType( reader ) ) {
				case XML_READER_TYPE_ELEMENT:
					elem = NULL;

					xml_attr = xmlTextReaderGetAttribute( reader, "name" );
					class_name = strdup( xml_attr );

					xml_attr = xmlTextReaderGetAttribute( reader, "super" );
					if( xml_attr )
						super_name = strdup( xml_attr );
					else
						super_name = NULL;

					xml_attr = xmlTextReaderGetAttribute( reader, "ctor" );
					if( xml_attr )
						ctor = strdup( xml_attr );
					else
						ctor = NULL;

					xml_attr = xmlTextReaderGetAttribute( reader, "nargs" );
					if( xml_attr )
						nargs = strtol( xml_attr, NULL, 0 );
					else
						nargs = 0;

					class = ecore_hash_new( ecore_str_hash, ecore_str_compare );
					elem_hash = class;
					break;
				case XML_READER_TYPE_END_ELEMENT:
					ecore_hash_set( widgets, class_name, class );
					ecore_hash_set( widget_parents, class_name, super_name );
					if( ctor ) {
						ecore_hash_set( widget_ctors, class_name, ctor );
						ecore_hash_set( widget_ctor_args, class_name, (void *) nargs );
					}
					class = NULL;
					elem_hash = NULL;
					break;
			}
			break;
		case WIDGET_ELEM_TOKEN:
			switch( xmlTextReaderNodeType( reader ) ) {
				case XML_READER_TYPE_ELEMENT:
					xml_attr = xmlTextReaderGetAttribute( reader, "type" );

					elem = elem_new( xml_attr, reader );
					elem->w.parent = last_elem;
					last_elem = elem;

					elem->w.w_flags = 0;

					xml_attr = xmlTextReaderGetAttribute( reader, "mod" );
					if( xml_attr && !strcmp( xml_attr, "no" ) )
						elem->w.w_flags |= ELEM_NO_MODIFY;

					xml_attr = xmlTextReaderGetAttribute( reader, "get" );
					if( xml_attr )
						elem->w.get = strdup( xml_attr );
					else
						elem->w.get = NULL;

					xml_attr = xmlTextReaderGetAttribute( reader, "set" );
					if( xml_attr )
						elem->w.set = strdup( xml_attr );
					else
						elem->w.set = NULL;

					xml_attr = xmlTextReaderGetAttribute( reader, "index" );
					if( xml_attr )
						elem->w.index = strtol( xml_attr, NULL, 0 );
					else
						elem->w.index = 0;

					elem->w.cast = strudup( class_name );

					if( elem->w.w_type == WIDGET_STRUCT_TYPE )
						elem_hash = elem->w_struct.members;
					break;
				case XML_READER_TYPE_END_ELEMENT:
					last_elem = elem->w.parent;
					if( elem->w.w_type == WIDGET_STRUCT_TYPE && last_elem )
						elem_hash = last_elem->w_struct.members;
					else if( elem->w.w_type == WIDGET_STRUCT_TYPE )
						elem_hash = class;

					ecore_hash_set( elem_hash, elem->w.name, elem );

					elem = last_elem;
					break;
			}
			break;
		case WIDGET_TEXT_TOKEN:
			if( strpbrk( value, "\n\t" ) != (char *) value ) {
				int len = strcspn( value, "\t\n" );
				elem->w.name = CALLOC(char, len + 1);
				memcpy( elem->w.name, value, len );
				elem->w.name[len] = '\0';
			}
			break;
	}
}

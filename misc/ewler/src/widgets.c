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
#include "form.h"
#include "widgets.h"
#include "inspector.h"

static void process( xmlTextReaderPtr reader, void *dl_handle );
static int lookup_name( const xmlChar *name );

Ecore_Hash *widgets = NULL;
Ecore_Hash *widget_parents = NULL;
Ecore_Hash *widget_ctors = NULL;
Ecore_Hash *widgets_info = NULL;
Ecore_Hash *widgets_type_rev = NULL;
Ecore_Hash *widgets_visibility = NULL;

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
void widget_set_name( Ewl_Object *o, char *name );

static Widget_Type_Elem widget_name_type = {
	{
		WIDGET_STRING_TYPE, "ewler_widget_name", 0,
		widget_get_name, (Widget_Set_Func) widget_set_name, NULL
	}
};

int
widget_get_name( Ewl_Object *o )
{
	Ecore_List *info = widget_get_info( EWL_WIDGET(o) );
	Widget_Data_Elem *data;

	ecore_list_goto_first( info );

	while( (data = ecore_list_next( info )) )
		if( !strcmp( data->type->w.name, "ewler_widget_name" ) )
			return (int) data->w_str.value;

	return 0;
}

void
widget_set_name( Ewl_Object *o, char *name )
{
	Ewler_Form *form;
	Ewl_Widget *w;

	form = inspector_get_form();

	if( form && (w = form_get_element_by_name( form, name )) ) {
		if( w == EWL_WIDGET(o) ) {
			widget_set_error( 0 );
			return;
		}
		widget_set_error( 1 );
		ewler_error_dialog( "The name of a widget must be unique.\n"
												"'%s' is already used in form '%s',\n"
												"so the name has been reverted to '%s'.",
												name,
												(char *) widget_get_name( EWL_OBJECT(form->window) ),
												(char *) widget_get_name( o ) );
		return;
	}

	widget_set_error( 0 );
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
		widget_parents = ecore_hash_new( ecore_str_hash, ecore_str_compare );
		widget_ctors = ecore_hash_new( ecore_str_hash, ecore_str_compare );
		widgets_info = ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
		widgets_type_rev =
			ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
		widgets_visibility = ecore_hash_new( ecore_str_hash, ecore_str_compare );
	}

	if( !reader ) {
		ewler_error_dialog( "Unable to open %s", filename );
		return -1;
	}

	while( (ret = xmlTextReaderRead( reader )) == 1 )
		process( reader, dl_handle );

	xmlFreeTextReader( reader );

	if( ret )
		ewler_error_dialog( "Parse failed in file %s", filename );

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

int
widget_is_type( Ewl_Widget *w, char *type )
{
	char *class = ecore_hash_get( widgets_type_rev, w );

	while( class != NULL ) {
		if( !strcmp( class, type ) )
			return 1;
		class = ecore_hash_get( widget_parents, class );
	}

	return 0;
}

char *
widget_get_type( Ewl_Widget *w )
{
	return (char *) ecore_hash_get( widgets_type_rev, w );
}

Ewler_Ctor
widget_get_ctor( const char *name )
{
	Ewler_Ctor ctor;

	ctor = ecore_hash_get( widget_ctors, (char *) name );

	return ctor;
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
static Ewl_Widget *c_widget = NULL;

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
			if( elem->type->w.get )
				elem->w_int.value = elem->type->w.get( EWL_OBJECT(c_widget) );
			else
				elem->w_int.value = elem->type->w_int.default_value;
			break;
		case WIDGET_STRING_TYPE:
			if( elem->type->w_str.default_value ) {
				elem->w_str.value = strdup( elem->type->w_str.default_value );
				if( elem->type->w.set )
					elem->type->w.set( EWL_OBJECT(c_widget), elem->w_str.value );
			} else if( elem->type->w.get ) {
				elem->w_str.value = (char *) elem->type->w.get( EWL_OBJECT(c_widget) );
				if( elem->w_str.value )
					elem->w_str.value = strdup( elem->w_str.value );
			} else
				elem->w_str.value = NULL;
			break;
		case WIDGET_POINTER_TYPE:
			if( elem->type->w.get )
				elem->w_ptr.value = (void *) elem->type->w.get( EWL_OBJECT(c_widget) );
			else
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
		case WIDGET_ENUM_TYPE:
			if( elem->type->w.get )
				elem->w_enum.value = elem->type->w.get( EWL_OBJECT(c_widget) );
			else
				elem->w_enum.value = elem->type->w_enum.default_value;
			break;
	}

	ecore_list_prepend( c_info, elem );

	if( !lvl )
		c_info = NULL;
}

static void
__update_elem_from_widget( void *val )
{
	Widget_Data_Elem *data = val;

	switch( data->type->w.w_type ) {
		case WIDGET_INTEGER_TYPE:
			if( data->type->w.get )
				data->w_int.value = data->type->w.get( EWL_OBJECT(c_widget) );
			break;
		case WIDGET_STRING_TYPE:
			if( data->type->w.get ) {
				data->w_str.value = (char *) data->type->w.get( EWL_OBJECT(c_widget) );
				if( data->w_str.value )
					data->w_str.value = strdup( data->w_str.value );
			}
			break;
		case WIDGET_POINTER_TYPE:
			/* add lookup of widget to name */
			break;
		case WIDGET_STRUCT_TYPE:
			ecore_list_for_each( data->w_struct.members, __update_elem_from_widget );
			break;
		case WIDGET_ENUM_TYPE:
			if( data->type->w.get )
				data->w_enum.value = data->type->w.get( EWL_OBJECT(c_widget) );
			break;
	}
}

void
widget_changed( Ewl_Widget *w )
{
	Ecore_List *w_info = widget_get_info( w );
	c_widget = w;

	ecore_list_for_each( w_info, __update_elem_from_widget );

	inspector_update();
}

static void
__widget_changed( Ewl_Widget *w, void *ev_data, void *user_data )
{
	widget_changed( w );
}

void
widget_create_info( Ewl_Widget *w, const char *name, char *widget_name )
{
	const char *c_name = name;
	Ecore_Hash *class;
	Widget_Data_Elem *widget_name_elem;

	c_widget = w;
	w_info = ecore_list_new();

	widget_name_elem = ALLOC(Widget_Data_Elem);
	widget_name_elem->type = &widget_name_type;
	widget_name_elem->w_str.value = widget_name;

	while( c_name != NULL ) {
		class = ecore_hash_get( widgets, (char *) c_name );

		ecore_hash_for_each_node( class, __copy_w_info );

		c_name = ecore_hash_get( widget_parents, (char *) c_name );
	}
	ecore_list_prepend( w_info, widget_name_elem );

	ecore_hash_set( widgets_info, w, w_info );
	ecore_hash_set( widgets_type_rev, w, strdup( name ) );

	ewl_callback_append( w, EWL_CALLBACK_REALIZE,
											 __widget_changed, NULL );
	ewl_callback_append( w, EWL_CALLBACK_CONFIGURE,
											 __widget_changed, NULL );
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
widget_destroy_info( Ewl_Widget *w )
{
	Ecore_List *info = widget_get_info( w );
	char *name;

	ecore_hash_remove( widgets_info, w );

	name = ecore_hash_remove( widgets_type_rev, w );
	FREE(name);

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
widget_strset_info( Ewl_Object *o, Ecore_List *info, char *elem, char *value )
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
					if( data->type->w.set )
						data->type->w.set( o, data->w_int.value );
					return;
				case WIDGET_STRING_TYPE:
					data->w_str.value = strdup( value );
					if( data->type->w.set )
						data->type->w.set( o, (int) data->w_str.value );
					return;
				case WIDGET_ENUM_TYPE:
					data->w_enum.value =
						(int) ecore_hash_get( data->type->w_enum.map, value );
					if( data->type->w.set )
						data->type->w.set( o, data->w_enum.value );
					return;
			}
		}
}

Ecore_List *
widget_get_info( Ewl_Widget *w )
{
	return ECORE_LIST(ecore_hash_get( widgets_info, w ));
}

int
widget_is_visible( char *class )
{
	return (int) ecore_hash_get( widgets_visibility, class );
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
		ewler_error_dialog( "elem_new: No type specified" );
		return NULL;
	}

	len = strlen(type);
	elem = ALLOC(Widget_Type_Elem);

	if( !elem ) {
		ewler_error_dialog( "elem_new: Out of memory" );
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
	} else if( !strcmp( type, "enum" ) ) {
		elem->w_enum.w_type = WIDGET_ENUM_TYPE;
		elem->w_enum.map = ecore_hash_new( ecore_str_hash, ecore_str_compare );
		elem->w_enum.map_rev =
			ecore_hash_new( ecore_direct_hash, ecore_direct_compare );
	} else if( !strcmp( type, "enum_val" ) ) {
		FREE(elem);
	} else if( type[len-1] == '*' && strpbrk( type, " \t" ) ) {
		elem->w_ptr.w_type = WIDGET_POINTER_TYPE;

		len = strcspn( type, " \t" ) + 1;
		elem->w_ptr.ptr_type = CALLOC(char, len);
		memcpy( elem->w_ptr.ptr_type, type, len );
	} else {
		ewler_error_dialog( "elem_new: unsupported type: %s", type );
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
	static int class_visible;
	static Ecore_Hash *class, *elem_hash;
	static Widget_Type_Elem *elem, *last_elem = NULL;
	Widget_Type_Elem *e;
	static void *ctor;
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

					xml_attr = xmlTextReaderGetAttribute( reader, "visible" );
					if( xml_attr && !strcmp( xml_attr, "no" ) )
						class_visible = 0;
					else
						class_visible = 1;

					xml_attr = xmlTextReaderGetAttribute( reader, "ctor" );
					if( xml_attr ) {
						ctor = dlsym( dl_handle, xml_attr );
						if( ctor == NULL && dlerror() ) {
							fprintf( stderr, "%s: error processing ctor: %s\n",
											 class_name, dlerror() );
							exit( EXIT_FAILURE );
						}
					} else
						ctor = NULL;

					class = ecore_hash_new( ecore_str_hash, ecore_str_compare );
					elem_hash = class;
					break;
				case XML_READER_TYPE_END_ELEMENT:
					ecore_hash_set( widgets, class_name, class );
					ecore_hash_set( widgets_visibility,
													class_name, (void *) class_visible );
					ecore_hash_set( widget_parents, class_name, super_name );
					if( ctor )
						ecore_hash_set( widget_ctors, class_name, ctor );
					class = NULL;
					elem_hash = NULL;
					break;
			}
			break;
		case WIDGET_ELEM_TOKEN:
			switch( xmlTextReaderNodeType( reader ) ) {
				case XML_READER_TYPE_ELEMENT:
					xml_attr = xmlTextReaderGetAttribute( reader, "type" );

					e = elem_new( xml_attr, reader );
					if( !e ) {
						if( !strcmp( xml_attr, "enum_val" ) ) {
							int value;
							char *id;

							xml_attr = xmlTextReaderGetAttribute( reader, "value" );
							value = strtol( xml_attr, NULL, 0 );

							xml_attr = xmlTextReaderGetAttribute( reader, "id" );
							id = strdup( xml_attr );

							ecore_hash_set( elem->w_enum.map, id, (void *) value );
							ecore_hash_set( elem->w_enum.map_rev, (void *) value, id );
						}
						last_elem = elem;
						elem = NULL;
						break;
					}

					elem = e;

					elem->w.parent = last_elem;
					last_elem = elem;

					elem->w.w_flags = 0;

					xml_attr = xmlTextReaderGetAttribute( reader, "mod" );
					if( xml_attr && !strcmp( xml_attr, "no" ) )
						elem->w.w_flags |= ELEM_NO_MODIFY;

					xml_attr = xmlTextReaderGetAttribute( reader, "get" );
					if( xml_attr )
						elem->w.get = dlsym( dl_handle, xml_attr );
					else
						elem->w.get = NULL;

					xml_attr = xmlTextReaderGetAttribute( reader, "set" );
					if( xml_attr )
						elem->w.set = dlsym( dl_handle, xml_attr );
					else
						elem->w.set = NULL;

					xml_attr = xmlTextReaderGetAttribute( reader, "index" );
					if( xml_attr )
						elem->w.index = strtol( xml_attr, NULL, 0 );
					else
						elem->w.index = 0;

					if( elem->w.w_type == WIDGET_STRUCT_TYPE )
						elem_hash = elem->w_struct.members;
					break;
				case XML_READER_TYPE_END_ELEMENT:
					if( elem ) {
						last_elem = elem->w.parent;
						if( elem->w.w_type == WIDGET_STRUCT_TYPE && last_elem )
							elem_hash = last_elem->w_struct.members;
						else if( elem->w.w_type == WIDGET_STRUCT_TYPE )
							elem_hash = class;

						ecore_hash_set( elem_hash, elem->w.name, elem );
					}

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

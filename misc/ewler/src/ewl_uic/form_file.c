#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include <string.h>
#include <Ewl.h>

#include "ewler.h"
#include "form.h"
#include "widgets.h"

#define ENCODING "ISO-8859-1"

#define WIDGET_TOKEN	1
#define ELEM_TOKEN		2
#define TEXT_TOKEN		3
#define MAX_TOKEN			TEXT_TOKEN

typedef struct Widget_Name Widget_Name;

static struct Widget_Name {
	const char *name;
	int token;
} widget_names[] = {
	{ "widget", WIDGET_TOKEN },
	{ "elem", ELEM_TOKEN },
	{ "#text", TEXT_TOKEN },
	{ NULL, -1 }
};

static xmlTextReaderPtr reader;

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

static Ecore_List *
process_read( void )
{
	const xmlChar *name, *value;
	const xmlChar *xml_attr;
	static char *elem_name;
	static Ecore_List *cur = NULL, *parent, *cur_info;
	Widget_Data_Elem *data;
	static Ecore_List *info_stack = NULL;
	static Ecore_List *data_stack = NULL;
	static int token, done = 0;

	if( !info_stack && !data_stack ) {
		info_stack = ecore_list_new();
		data_stack = ecore_list_new();
	}

	name = xmlTextReaderConstName( reader );
	value = xmlTextReaderConstValue( reader );

	token = lookup_name( name );

	switch( token ) {
		case WIDGET_TOKEN:
			switch( xmlTextReaderNodeType( reader ) ) {
				case XML_READER_TYPE_ELEMENT:
					xml_attr = xmlTextReaderGetAttribute( reader, "type" );
					parent = cur;

					cur = widget_create_info( xml_attr );
					widget_set_parent( cur, parent );
					ecore_list_prepend( info_stack, cur );
					break;
				case XML_READER_TYPE_END_ELEMENT:
					cur = ecore_list_remove_first( info_stack );
					if( !cur )
						done = 1;
					break;
			}
			break;
		case ELEM_TOKEN:
			switch( xmlTextReaderNodeType( reader ) ) {
				case XML_READER_TYPE_ELEMENT:
					xml_attr = xmlTextReaderGetAttribute( reader, "name" );
					cur_info = ecore_list_goto_first( info_stack );

					elem_name = (char *) xml_attr;
					data = widget_lookup_data( cur_info, elem_name );
					if( data->type->w.w_type == WIDGET_STRUCT_TYPE ) {
						ecore_list_prepend( info_stack, data->w_struct.members );
					}
					ecore_list_prepend( data_stack, data );
					break;
				case XML_READER_TYPE_END_ELEMENT:
					data = ecore_list_remove_first( data_stack );
					if( data->type->w.w_type == WIDGET_STRUCT_TYPE )
						ecore_list_remove_first( info_stack );
					break;
			}
			break;
		case TEXT_TOKEN:
			if( strpbrk( value, "\n\t" ) != (char *) value ) {
				Ecore_List *cur_info;

				cur_info = ecore_list_goto_first( info_stack );

				widget_strset_info( cur_info, elem_name, (char *) value );
			}
			break;
	}

	if( done ) {
		Ecore_List *c = cur;

		ecore_list_destroy( info_stack );
		ecore_list_destroy( data_stack );
		info_stack = NULL;
		data_stack = NULL;
		cur = NULL;
		done = 0;
		return c;
	}

	return cur;
}

static Ecore_List *
read_widget( void )
{
	Ecore_List *top = NULL;
	int ret;

	while( (ret = xmlTextReaderRead( reader )) == 1 )
		top = process_read();

	return top;
}

/* bulk read of all widgets from a file */
Ecore_List *
form_file_read( int fd )
{
	Ecore_List *w_info;

	reader = xmlReaderForFd( fd, NULL, ENCODING, 0 );

	w_info = read_widget();

	xmlFreeTextReader( reader );

	return w_info;
}

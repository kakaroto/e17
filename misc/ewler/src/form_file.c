#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <string.h>
#include <Ewl.h>

#include "ewler.h"
#include "form.h"
#include "widgets.h"
#include "selected.h"

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

static xmlTextWriterPtr writer;
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

static void
__write_element( void *val )
{
	Widget_Data_Elem *data = val;
	static char buf[32];

	xmlTextWriterStartElement( writer, "elem" );
	xmlTextWriterWriteAttribute( writer, "name", data->type->w.name );

	switch( data->type->w.w_type ) {
		case WIDGET_INTEGER_TYPE:
			snprintf( buf, 31, "%ld", data->w_int.value );
			xmlTextWriterWriteString( writer, buf );
			xmlTextWriterEndElement( writer );
			break;
		case WIDGET_STRING_TYPE:
			if( data->w_str.value ) {
				xmlTextWriterWriteString( writer, data->w_str.value );
				xmlTextWriterEndElement( writer );
			} else {
				xmlTextWriterFullEndElement( writer );
				xmlTextWriterWriteString( writer, "\n" );
			}
			break;
		case WIDGET_POINTER_TYPE:
			xmlTextWriterFullEndElement( writer );
			xmlTextWriterWriteString( writer, "\n" );
			break;
		case WIDGET_STRUCT_TYPE:
			ecore_list_for_each( data->w_struct.members, __write_element );
			xmlTextWriterEndElement( writer );
			break;
		case WIDGET_ENUM_TYPE:
			{
				char *enum_name;

				enum_name = ecore_hash_get( data->type->w_enum.map_rev,
																		(void *) data->w_enum.value );

				if( enum_name ) {
					xmlTextWriterWriteString( writer, enum_name );
					xmlTextWriterEndElement( writer );
				} else {
					xmlTextWriterFullEndElement( writer );
					xmlTextWriterWriteString( writer, "\n" );
				}
			}
			break;
	}
}

static void
write_widget( Ewl_Widget *w )
{
	Ewler_Selected *cs;
	Ecore_List *info = widget_get_info( w );

	if( !info )
		return;

	xmlTextWriterStartElement( writer, "widget" );
	xmlTextWriterWriteAttribute( writer, "type", widget_get_type( w ) );

	ecore_list_for_each( info, __write_element );

	if( widget_is_type( w, "Ewl_Container" ) ) {
		ecore_list_goto_first( EWL_CONTAINER(w)->children );

		while( (cs = ecore_list_next( EWL_CONTAINER(w)->children )) )
			write_widget( ewler_selected_get( cs ) );
	}

	/* write callbacks for widget */
	xmlTextWriterEndElement( writer );
}

int
form_file_write( Ewler_Form *form )
{
	writer = xmlNewTextWriterFilename( form->filename, 0 );

	xmlTextWriterSetIndent( writer, 1 );
	xmlTextWriterSetIndentString( writer, "\t" );

	xmlTextWriterStartDocument( writer, NULL, ENCODING, NULL );

	write_widget( form->overlay );

	xmlTextWriterEndDocument( writer );

	xmlFreeTextWriter( writer );

	return 0;
}

static Ewl_Widget *
process_read( Ewler_Form *form )
{
	const xmlChar *name, *value;
	const xmlChar *xml_attr;
	static char *elem_name;
	static Ewl_Widget *cur = NULL, *parent;
	Widget_Data_Elem *data;
	Ewler_Ctor ctor;
	Ecore_List *cur_info;
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
					ctor = widget_get_ctor( xml_attr );	
					parent = cur;

					if( ctor ) {
						cur = ctor( xml_attr, xml_attr, xml_attr, xml_attr );
					} else {
						ewler_error_dialog( "Unknown constructor for type '%s'", xml_attr );
						/* probably a little drastic... we need a way to ditch the load */
						printf( "unknown constructor for type %s\n", xml_attr );
						exit( 1 );
					}
					if( parent ) {
						ewl_container_append_child( EWL_CONTAINER(parent), cur );
					}
					
					if( !cur ) {
						ewler_error_dialog( "Could not create widget of type '%s'",
																xml_attr );
						printf( "could not create widget of type %s\n", xml_attr );
						exit( 1 );
					}

					/* creating the widget's data with no name to start... the name will
					 * get loaded in as a normal attribute */
					widget_create_info( cur, xml_attr, NULL );
					ecore_list_prepend( info_stack, widget_get_info( cur ) );
					break;
				case XML_READER_TYPE_END_ELEMENT:
					/* sets up the elements/elements_rev entries and adds necessary
					 * callbacks */
					form_add_widget( form,(char *) widget_get_name( EWL_OBJECT(cur) ),
													 cur );

					ecore_list_remove_first( info_stack );
					ewl_widget_show( cur );

					if( cur->parent ) {
						Ewl_Widget *s = ewler_selected_new( cur );
						ewl_callback_call( s, EWL_CALLBACK_DESELECT );
						ewl_widget_show( s );

						cur = cur->parent;
					} else {
						done = 1;
					}
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
				cur_info = ecore_list_goto_first( info_stack );

				widget_strset_info( EWL_OBJECT(cur), cur_info,
														elem_name, (char *) value );
			}
			break;
	}

	if( done ) {
		Ewl_Widget *c = cur;

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

static Ewl_Widget *
read_widget( Ewler_Form *form )
{
	Ewl_Widget *top = NULL;
	int ret;

	while( (ret = xmlTextReaderRead( reader )) == 1 )
		top = process_read( form );

	return top;
}

/* bulk read of all widgets from a file */
int
form_file_read( Ewler_Form *form )
{
	reader = xmlNewTextReaderFilename( form->filename );

	form->overlay = read_widget( form );

	xmlFreeTextReader( reader );

	return 0;
}

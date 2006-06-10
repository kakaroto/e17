#include <string.h>

#include <EXML.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

static int _exml_read(EXML *xml, xmlTextReader *reader);
static int _exml_write(EXML *xml, xmlTextWriter *writer);

inline void exml_print_warning(const char *function, const char *sparam)
{
	fprintf(stderr, "***** Developer Warning ***** :\n"
                  "\tThis program is calling:\n\n"
                  "\t%s();\n\n"
                  "\tWith the parameter:\n\n"
                  "\t%s\n\n"
                  "\tbeing NULL. Please fix your program.\n", function, sparam);
	fflush(stderr);
}

/**
 * Create and initialize a new xml document.
 * @return	A new initialized xml document on success, @c NULL on failure.
 * @ingroup EXML_Creation_Group
 */
EXML *exml_new()
{
	EXML *xml;

	xml = (EXML *) malloc(sizeof(EXML));
	if (!xml)
		return NULL;

	if (!exml_init(xml)) {
		FREE(xml);
		return NULL;
	}

	return xml;
}

/**
 * Initialize an xml document structure to some sane starting values.
 * @param   xml The xml to initialize.
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Creation_Group
 */
int exml_init(EXML *xml)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	memset(xml, 0, sizeof(EXML));

	xml->buffers = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	return TRUE;
}

/**
 * Create a new child tag for the current node level, and sets it to current
 * @param   xml The xml document to add the tag to
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Create_Element_Group
 */
int exml_start(EXML *xml)
{
	EXML_Node *node;

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	if (xml->current == NULL && xml->top)
		return FALSE;

	node = exml_node_new();
	if (!node)
		return FALSE;

	node->parent = xml->current;

	/* no sanity check on xml->current pointer */

	if (xml->top == NULL)
		xml->current = xml->top = node;
	else {
		ecore_list_append( xml->current->children, node );
		xml->current = node;
	}

	return TRUE;
}

/**
 * End the current element - just a call to exml_up
 * @param   xml The xml document to add the tag to
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Create_Element_Group
 */
int exml_end(EXML *xml)
{
	return exml_up(xml) != NULL || xml->current != xml->top;
}

/**
 * Set the current tag name
 * @param   xml The xml document to modify
 * @param   tag The new tag name
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Modfiy_Element_Group
 */
int exml_tag_set(EXML *xml, char *tag)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);
	CHECK_PARAM_POINTER_RETURN("tag", tag, FALSE);

	IF_FREE(xml->current->tag);

	xml->current->tag = strdup( tag );
	if (!xml->current->tag)
		return FALSE;

	return TRUE;
}

/**
 * Set an attribute for this tag
 * @param   xml The xml document to modify
 * @param   attr The attribute to set
 * @param   value The value to set this attribute to
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Modfiy_Element_Group
 */
int exml_attribute_set(EXML *xml, char *attr, char *value)
{
	char *oldvalue;

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);
	CHECK_PARAM_POINTER_RETURN("attr", attr, FALSE);

	oldvalue = ecore_hash_get( xml->current->attributes, attr );
	IF_FREE(oldvalue);

	if (value)
		value = strdup(value);

	ecore_hash_set( xml->current->attributes, strdup( attr ), value );

	return TRUE;
}

/**
 * Set the current tag value
 * @param   xml The xml document to modify
 * @param   value The new value name
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Modfiy_Element_Group
 */
int exml_value_set(EXML *xml, char *value)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);
	CHECK_PARAM_POINTER_RETURN("value", value, FALSE);

	IF_FREE(xml->current->value);

	xml->current->value = strdup( value );
	if (!xml->current->value)
		return FALSE;

	return TRUE;
}

static void _exml_node_destroy( void *data )
{
	EXML_Node *node = data;

	if (node) {
		ecore_hash_destroy(node->attributes);
		IF_FREE(node->tag);
		IF_FREE(node->value);
		ecore_list_destroy(node->children);

		FREE(node);
	}
}

/**
 * Remove the current node from the document
 * @param   xml The xml document to modify
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Modfiy_Element_Group
 */
int exml_tag_remove(EXML *xml)
{
	EXML_Node *n_cur;

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	n_cur = xml->current;
	if (!n_cur)
		return FALSE;

	n_cur = n_cur->parent;

	if (n_cur) {
		EXML_Node *c_parent = n_cur;
		Ecore_List *c_list = c_parent->children;

		ecore_list_goto( c_list, xml->current );
		ecore_list_remove_destroy( c_list );
		if ((n_cur = ecore_list_current( c_list )) == NULL)
			if ((n_cur = ecore_list_goto_last( c_list )) == NULL)
				n_cur = c_parent;
	} else {
		/* we're removing the top level node */
		xml->top = NULL;
		_exml_node_destroy( xml->current );
	}

	xml->current = n_cur;
	return TRUE;
}

/**
 * Get the current node
 * @param   xml The xml document
 * @return  The current xml node
 * @ingroup EXML_Retrieve_Element_Group
 */
EXML_Node *exml_get(EXML *xml)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	return xml->current;
}

/**
 * Get the current tag name
 * @param   xml The xml document
 * @return  The current xml tag name
 * @ingroup EXML_Retrieve_Element_Group
 */
char *exml_tag_get(EXML *xml)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	return xml->current ? xml->current->tag : NULL;
}

/**
 * Get a current node attribute
 * @param   xml The xml document
 * @param   attr The attribute name to retrieve
 * @return  The value of an attribute of the current xml node
 * @ingroup EXML_Retrieve_Element_Group
 */
char *exml_attribute_get(EXML *xml, char *attr)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	return xml->current ? ecore_hash_get(xml->current->attributes, attr) : NULL;
}

/**
 * Get the current tag value
 * @param   xml The xml document
 * @return  The current xml tag value
 * @ingroup EXML_Retrieve_Element_Group
 */
char *exml_value_get(EXML *xml)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	return xml->current ? xml->current->value : NULL;
}

/**
 * Move the current xml document pointer to the toplevel
 * @param   xml The xml document
 * @return  The current xml tag name
 * @ingroup EXML_Traversal_Group
 */
char *exml_goto_top(EXML *xml)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	xml->current = xml->top;

	return xml->current ? xml->current->tag : NULL;
}

/**
 * Move the current xml document pointer to the indicated node
 * @param   xml The xml document
 * @param   node The position within the document to move to
 * @return  The current xml tag name
 * @ingroup EXML_Traversal_Group
 */
char *exml_goto_node(EXML *xml, EXML_Node *node)
{
	Ecore_List *stack;
	EXML_Node *n, *l;

	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	stack = ecore_list_new();
	n = node;

	while( n->parent != NULL ) {
		ecore_list_prepend(stack, n);
		n = n->parent;
	}

	l = xml->top;

	if (n != l)
		return NULL;

	while( (n = ecore_list_remove_first(stack)) ) {
		l = ecore_list_goto(l->children, n);

		if (!l)
			return NULL;
	}

	xml->current = node;

	return xml->current ? xml->current->tag : NULL;
}

/**
 * Move the current xml document pointer to the tag indicated by @c tag
 * and @c value
 * @param   xml The xml document
 * @param   tag The xml tag to look for
 * @param   value The xml value to look for
 * @return  The current xml tag name
 * @ingroup EXML_Traversal_Group
 */
char *exml_goto(EXML *xml, char *tag, char *value)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	exml_goto_top(xml);

	while( exml_get(xml) ) {
		if (!strcmp(tag, exml_tag_get(xml)) &&
				!strcmp(value, exml_value_get(xml))) {
			return exml_tag_get( xml );
		}
		exml_next(xml);
	}

	return NULL;
}

/**
 * Move the current xml to its next sibling, return NULL and move to the parent
 * when there are no more children
 * @param   xml The xml document
 * @return  The current xml tag name
 * @ingroup EXML_Traversal_Group
 */
char *exml_next(EXML *xml)
{
	Ecore_List *p_list;
	EXML_Node *parent;

	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	if (xml->current) {
		parent = xml->current->parent;

		if (parent) {
			p_list = parent->children;

			ecore_list_goto( p_list, xml->current );
			ecore_list_next( p_list );
			if ((xml->current = ecore_list_current(p_list)) == NULL) {
				xml->current = parent;
				return exml_next(xml);
			}
		} else
			xml->current = NULL;
	}

	return xml->current ? xml->current->tag : NULL;
}

/**
 * Move the current xml to its next sibling, return NULL and move to parent
 * at the end of the list
 * @param   xml The xml document
 * @return  The current xml tag name or NULL
 * @ingroup EXML_Traversal_Group
 */
char *exml_next_nomove(EXML *xml)
{
	Ecore_List *p_list;
	EXML_Node *parent, *cur;

	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	if (xml->current) {
		cur = xml->current;
		parent = cur->parent;

		if (parent) {
			p_list = parent->children;

			ecore_list_goto( p_list, xml->current );
			ecore_list_next( p_list );
			if ((xml->current = ecore_list_current(p_list)) == NULL) {
				xml->current = cur;
				return NULL;
			}
		} else
			xml->current = NULL;
	}

	return xml->current ? xml->current->tag : NULL;
}

/**
 * Move the current xml to its first child if there are children
 * @param   xml The xml document
 * @return  The current xml tag name
 * @ingroup EXML_Traversal_Group
 */
char *exml_down(EXML *xml)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	if (exml_has_children(xml))
		xml->current = ecore_list_goto_first( xml->current->children );
	else
		return NULL;

	return xml->current ? xml->current->tag : NULL;
}

/**
 * Move the current xml to its parent
 * @param   xml The xml document
 * @return  The current xml tag name
 * @ingroup EXML_Traversal_Group
 */
char *exml_up(EXML *xml)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, NULL);

	if (xml->current)
		xml->current = xml->current->parent;

	return xml->current ? xml->current->tag : NULL;
}

/**
 * Check for children in the current node
 * @param   xml The xml document
 * @return  @c TRUE if children are present, @c FALSE if not.
 */
int exml_has_children(EXML *xml)
{
	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);
	
	if (xml->current && xml->current->children)
		return !ecore_list_is_empty(xml->current->children);

	return FALSE;
}

/**
 * Clear the entire xml document
 * @param   xml The xml document
 * @return  nothing
 */
void exml_clear(EXML *xml)
{
	CHECK_PARAM_POINTER("xml", xml);

	exml_goto_top(xml);
	exml_tag_remove(xml);
}

/**
 * Destroys this xml document
 * @param   xml The xml document
 * @return  nothing
 */
void exml_destroy(EXML *xml)
{
	CHECK_PARAM_POINTER("xml", xml);

	exml_clear(xml);

	ecore_hash_destroy(xml->buffers);

	FREE(xml);
}

/**
 * Read the xml document in from a file
 * @param   xml The xml document
 * @param   filename The source xml input filename
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Read_Group
 */
int exml_file_read(EXML *xml, char *filename)
{
	xmlTextReaderPtr reader;

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	reader = xmlReaderForFile( filename, NULL, XML_PARSE_RECOVER );

	return _exml_read(xml, reader);
}

/**
 * Read the xml document in from a file descriptor.
 * @param   xml The xml document
 * @param   fd The source xml input descriptor
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Read_Group
 */
int exml_fd_read(EXML *xml, int fd)
{
	xmlTextReaderPtr reader;

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	reader = xmlReaderForFd( fd, "", NULL, XML_PARSE_RECOVER );

  return _exml_read(xml, reader);
}

/**
 * Read the xml document in from a memory location.
 * @param   xml The xml document
 * @param   ptr The source xml input location
 * @param   len The size of the memory buffer
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Read_Group
 */
int exml_mem_read(EXML *xml, void *s_mem, size_t len)
{
	xmlTextReaderPtr reader;

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	reader = xmlReaderForMemory( s_mem, len, "", NULL, XML_PARSE_RECOVER );

	return _exml_read(xml, reader);
}

static int _exml_read(EXML *xml, xmlTextReader *reader)
{
	int ret, empty;
	xmlChar *name, *value;

	if (!reader)
		return -1;

	exml_clear( xml );

	while( (ret = xmlTextReaderRead( reader )) == 1 ) {
		name = xmlTextReaderName(reader);
		value = xmlTextReaderValue(reader);
		empty = xmlTextReaderIsEmptyElement(reader);

		switch( xmlTextReaderNodeType(reader) ) {
			case XML_READER_TYPE_ELEMENT:
				exml_start(xml);
				exml_tag_set(xml, (char *) name);
		
				if (xmlTextReaderHasAttributes(reader)) {
					xmlTextReaderMoveToFirstAttribute(reader);
					do {
						xmlChar *attr_name, *attr_value;

						attr_name = xmlTextReaderName(reader);
						attr_value = xmlTextReaderValue(reader);

						exml_attribute_set(xml, (char *) attr_name, (char *) attr_value);

						xmlFree(attr_name);
						xmlFree(attr_value);
					} while( xmlTextReaderMoveToNextAttribute(reader) == 1 );
				}

				if (!empty)
					break;
			case XML_READER_TYPE_END_ELEMENT:
				exml_end(xml);
				break;
			case XML_READER_TYPE_WHITESPACE:
				break;
			case XML_READER_TYPE_TEXT:
				exml_value_set(xml, (char *) value);
				break;
		}
		xmlFree(name);
		xmlFree(value);
	}

	xmlTextReaderClose(reader);
	xmlFreeTextReader(reader);

	exml_goto_top( xml );

	return TRUE;
}

/**
 * Write the xml document out to a file
 * @param   xml The xml document
 * @param   filename The source xml input filename
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Write_Group
 */
int exml_file_write(EXML *xml, char *filename)
{
	xmlTextWriter *writer;

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	writer = xmlNewTextWriterFilename(filename, 0);

	return _exml_write(xml, writer);
}

/**
 * Write the xml document out to a file descriptor.
 * @param   xml The xml document
 * @param   fd The source xml input descriptor
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Write_Group
 */
int exml_fd_write(EXML *xml, int fd)
{
	xmlTextWriter *writer;
	xmlOutputBuffer *out;

	out = xmlOutputBufferCreateFd(fd, NULL);
	if (out == NULL) {
		xmlGenericError(xmlGenericErrorContext,
										"xmlNewTextWriterFd : out of memory!\n");
		return FALSE;
	}

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	writer = xmlNewTextWriter( out );

	return _exml_write(xml, writer);
}

/**
 * Write the xml document out to a memory location.
 * @param   xml The xml document
 * @param   ptr The source xml input location
 * @param   len The size of the memory buffer
 * @return  a pointer to the memory location, or NULL if 
 * @ingroup EXML_Write_Group
 */
void *exml_mem_write(EXML *xml, size_t *len)
{
	xmlTextWriter *writer;
	xmlBuffer *buf;

	CHECK_PARAM_POINTER_RETURN("xml", xml, FALSE);

	buf = xmlBufferCreate();
	writer = xmlNewTextWriterMemory( buf, 0 );

	if (_exml_write(xml, writer)) {
		ecore_hash_set( xml->buffers, (void *) xmlBufferContent( buf ), buf );

		*len = xmlBufferLength( buf );
		return (void *) xmlBufferContent( buf );
	} else {
		*len = 0;
		xmlBufferFree( buf );
		return NULL;
	}
}

/**
 * Free memory allocated by a call to exml_mem_write
 * @param   xml The xml document
 * @param   ptr The xml buffer content
 * @return  nothing
 */
void exml_mem_free(EXML *xml, void *ptr)
{
	xmlBuffer *buf;

	CHECK_PARAM_POINTER("xml", xml);

	if ((buf = ecore_hash_get(xml->buffers, ptr)))
		xmlBufferFree( buf );
}

static void _exml_write_element(EXML_Node *node,
																xmlTextWriter *writer)
{
	EXML_Node *child;
	Ecore_List *keys;
	xmlChar *name;

	xmlTextWriterStartElement( writer, (xmlChar *) node->tag );

	keys = ecore_hash_keys( node->attributes );
	ecore_list_goto_first( keys );

	while( (name = ecore_list_next( keys )) ) {
		xmlChar *value = ecore_hash_get( node->attributes, name );
		xmlTextWriterWriteAttribute( writer, name, value );
	}

	ecore_list_destroy( keys );

	if (node->value)
		xmlTextWriterWriteString( writer, (xmlChar *) node->value );

	ecore_list_goto_first( node->children );
	
	while( (child = ecore_list_next( node->children )) )
		_exml_write_element( child, writer );

	xmlTextWriterEndElement( writer );
}

static int _exml_write(EXML *xml, xmlTextWriter *writer)
{
	xmlTextWriterSetIndent( writer, 1 );
	xmlTextWriterSetIndentString( writer, (xmlChar *) "\t" );
	xmlTextWriterStartDocument( writer, NULL, NULL, NULL );
	/* as of now, we do not write a DTD.  This will be in effect with a new
	 * set of functions designed to manipulate the DTD, as well as added
	 * capability for reading DTDs from orthogonal read sources */

	_exml_write_element(xml->top, writer);

	xmlTextWriterEndDocument( writer );
	xmlFreeTextWriter( writer );

	return TRUE;
}

/**
 * Allocate and initialize an EXML_Node struct
 * @return   allocated XML_Node struct, or NULL if an error occurred
 * @ingroup  EXML_
 */
EXML_Node *exml_node_new( void )
{
	EXML_Node *node;

	node = (EXML_Node *)malloc(sizeof(EXML_Node));
	if (!node)
		return NULL;

	if (!exml_node_init(node)) {
		FREE(node);
		return NULL;
	}

	return node;
}

/**
 * Initialize an XML_Node to starting values
 * @param   node The node to initialize
 * @return  @c TRUE if successful, @c FALSE if an error occurs.
 * @ingroup EXML_Creation_Group
 */
int exml_node_init(EXML_Node *node)
{
	CHECK_PARAM_POINTER_RETURN("node", node, FALSE);

	memset(node, 0, sizeof(EXML_Node));

	node->attributes = ecore_hash_new( ecore_str_hash, ecore_str_compare );
	ecore_hash_set_free_value( node->attributes, free );
	ecore_hash_set_free_key( node->attributes, free );
	node->children = ecore_list_new();
	ecore_list_set_free_cb( node->children, _exml_node_destroy );

	return TRUE;
}

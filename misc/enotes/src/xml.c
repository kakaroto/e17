
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#include "xml.h"

/* Freeing */

/**
 * @param p: The XmlEntry to free.
 * @brief: Free's an XmlEntry structure (and its contents
 *         in some cases).
 */
void
free_xmlentry(XmlEntry * p)
{
	if (p->name != NULL)
		free(p->name);
	if (p->value != NULL)
		free(p->value);
	free(p);
	return;
}

/* Reading */

/**
 * @param fn: The filename of the xml file to open for reading.
 * @return: The XmlReadHandle to be used for viewing and ending the file.
 * @brief: Opens an xml file for reading, returns a handle.
 */
XmlReadHandle  *
xml_read(char *fn)
{
	XmlReadHandle  *p = malloc(sizeof(XmlReadHandle));
	char           *unabletoopen = malloc(PATH_MAX);
	char           *unabletoparse = malloc(PATH_MAX);

	snprintf(unabletoopen, PATH_MAX, "Unable to open \"%s\"", fn);
	snprintf(unabletoparse, PATH_MAX, "Unable to parse \"%s\"", fn);

	p->doc = xmlParseFile(fn);
	if (p->doc == NULL) {
		dml(unabletoopen, 1);
		free(p);
		p = NULL;
	} else {
		p->cur = xmlDocGetRootElement(p->doc);
		if (p->cur == NULL) {
			dml(unabletoparse, 1);
			free(p);
			p = NULL;
		}
		p->cur = p->cur->xmlChildrenNode;
	}

	free(unabletoopen);
	free(unabletoparse);
	return (p);
}

/**
 * @param h: The handle to end and free.
 * @brief: Closes the xml file and free'd the handle supplied (h).
 */
void
xml_read_end(XmlReadHandle * h)
{
	xmlFreeDoc(h->doc);
	xmlCleanupParser();
	free(h);
	return;
}


/**
 * @param h: The handle to the cycle which wants to be taken forwards.
 * @brief: Moves the xml cycle forwards onto the next entry.
 */
void
xml_read_next_entry(XmlReadHandle * h)
{
	h->cur = h->cur->next;
	return;
}

/**
 * @param h: The handle to the cycle which wants to be taken backwards.
 * @brief: Moves the xml cycle backwards onto the previous entry.
 */
void
xml_read_prev_entry(XmlReadHandle * h)
{
	h->cur = h->cur->prev;
	return;
}


/**
 * @return: The XmlEntry containing the current entry according to h.
 * @param h: The handle of the cycle we want to get the information from.
 * @brief: Pulls out the current xml entry.
 */
XmlEntry       *
xml_read_entry_get_entry(XmlReadHandle * h)
{
	XmlEntry       *p = malloc(sizeof(XmlEntry));

	p->value = xmlNodeListGetString(h->doc, h->cur->xmlChildrenNode, 1);
	p->name = strdup(h->cur->name);

	return (p);
}

/* Writing */

/**
 * @return: The write handle for the cycle.
 * @param fn: The filename and location of the xml file to write to.
 * @brief: Opens an xml file for writing.
 */
XmlWriteHandle *
xml_write(char *fn)
{
	XmlWriteHandle *p = malloc(sizeof(XmlWriteHandle));
	char           *unabletoopen = malloc(PATH_MAX);

	snprintf(unabletoopen, PATH_MAX, "Unable to open (begin) \"%s\"", fn);

	p->doc = xmlNewDoc(NULL);
	if (p->doc == NULL) {
		dml(unabletoopen, 1);
		free(p);
		p = NULL;
	} else {
		p->cur = xmlNewDocNode(p->doc, NULL, BAD_CAST "enotes", NULL);
		xmlDocSetRootElement(p->doc, p->cur);
		p->writer = xmlNewTextWriterFilename(fn, 0);
		p->rc = xmlTextWriterStartDocument(p->writer, NULL, MY_ENCODING,
						   NULL);
		xmlTextWriterStartElement(p->writer, BAD_CAST "enotes");
	}

	free(unabletoopen);
	return (p);
}

/**
 * @param h: The write handle to free.
 * @brief: Closes the xml file and free's the handle.
 */
void
xml_write_end(XmlWriteHandle * h)
{
	xmlTextWriterEndElement(h->writer);
	xmlTextWriterEndDocument(h->writer);
	xmlFreeTextWriter(h->writer);
	xmlCleanupParser();
	free(h);
	return;
}


/**
 * @param h: The write handle to add an entry too.
 * @param name: The xml tag name you want to append to the handle file.
 * @param value: The value to put inside these tags.
 * @brief: Appends an xml entry into the opened xml file pointed
 *         to by h.
 */
void
xml_write_append_entry(XmlWriteHandle * h, char *name, char *value)
{
	xmlTextWriterWriteFormatElement(h->writer, BAD_CAST name, "%s", value);
	return;
}

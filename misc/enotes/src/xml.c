
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

void
xml_read_end(XmlReadHandle * h)
{
	xmlFreeDoc(h->doc);
	xmlCleanupParser();
	free(h);
	return;
}


void
xml_read_next_entry(XmlReadHandle * h)
{
	h->cur = h->cur->next;
	return;
}

void
xml_read_prev_entry(XmlReadHandle * h)
{
	h->cur = h->cur->prev;
	return;
}


XmlEntry       *
xml_read_entry_get_entry(XmlReadHandle * h)
{
	XmlEntry       *p = malloc(sizeof(XmlEntry));

	p->value = xmlNodeListGetString(h->doc, h->cur->xmlChildrenNode, 1);
	p->name = strdup(h->cur->name);

	return (p);
}

/* Writing */
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

void
xml_write_end(XmlWriteHandle * h)
{
	xmlTextWriterEndElement(h->writer);
	xmlTextWriterEndDocument(h->writer);
	xmlFreeTextWriter(h->writer);
	free(h);
	return;
}


void
xml_write_append_entry(XmlWriteHandle * h, char *name, char *value)
{
	xmlTextWriterWriteFormatElement(h->writer, BAD_CAST name, "%s", value);
	return;
}

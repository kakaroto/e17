/**************************************************/
/**               E  -  N O T E S                **/
/**                                              **/
/**  The contents of this file are released to   **/
/**  the public under the General Public Licence **/
/**  Version 2.                                  **/
/**                                              **/
/**  By  Thomas Fletcher (www.fletch.vze.com)    **/
/**                                              **/
/**************************************************/


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

	p->doc = xmlParseFile(fn);
	if (p->doc == NULL) {
		fprintf(stderr, "E-Notes: Unable to open, \"%s\".\n", fn);
		free(p);
		p = NULL;
	} else {
		p->cur = xmlDocGetRootElement(p->doc);
		if (p->cur == NULL) {
			fprintf(stderr,
				"E-Notes: Failed to parse file. \"%s\".\n", fn);
			free(p);
			p = NULL;
		}
		p->cur = p->cur->xmlChildrenNode;
	}

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

	p->doc = xmlNewDoc(NULL);
	if (p->doc == NULL) {
		fprintf(stderr, "E-Notes: Unable to open, \"%s\".\n", fn);
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

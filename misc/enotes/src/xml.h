
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


#ifndef XML_H
#define XML_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <libxml/xmlreader.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "debug.h"

#define MY_ENCODING NULL

typedef struct {
	xmlDocPtr       doc;
	xmlNodePtr      cur;
} XmlReadHandle;

typedef struct {
	xmlDocPtr       doc;
	xmlNodePtr      cur;
	xmlTextWriterPtr writer;
	int             rc;
} XmlWriteHandle;

#ifndef XMLENTRY_DEF
#define XMLENTRY_DEF 1
typedef struct {
	char           *name;
	char           *value;
} XmlEntry;
#endif

/* Freeing */
void            free_xmlentry(XmlEntry * p);

/* Reading */
XmlReadHandle  *xml_read(char *fn);
void            xml_read_end(XmlReadHandle * h);

void            xml_read_next_entry(XmlReadHandle * h);
void            xml_read_prev_entry(XmlReadHandle * h);

XmlEntry       *xml_read_entry_get_entry(XmlReadHandle * h);

/* Writing */
XmlWriteHandle *xml_write(char *fn);
void            xml_write_end(XmlWriteHandle * h);

void            xml_write_append_entry(XmlWriteHandle * h, char *name,
				       char *value);

#endif

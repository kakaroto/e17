
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


#ifndef STORAGE_H
#define STORAGE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <Evas.h>		// For the list structures.

#include "xml.h"
#include "debug.h"

#define DEF_STORAGE_LOC "%s/.e/notes/notes.xml"
#define DEF_VALUE_SEPERATION "|"
#define MAX_VALUE 2000

typedef struct {
	char           *title;
	char           *content;
	int             width;
	int             height;
} NoteStor;

/* Freeing */
void            free_note_stor(NoteStor * p);

/* One Shot Functions. :-) */
void            append_note_stor(NoteStor * p);
void            remove_note_stor(NoteStor * p);

/* Cycle Functions */
XmlReadHandle  *stor_cycle_begin(void);
void            stor_cycle_end(XmlReadHandle * p);

void            stor_cycle_next(XmlReadHandle * p);
void            stor_cycle_prev(XmlReadHandle * p);

NoteStor       *stor_cycle_get_notestor(XmlReadHandle * p);

/* Internal Functions */
char           *make_storage_fn(void);
NoteStor       *get_notestor_from_value(char *e);
char           *get_value_from_notestor(NoteStor * p);

#endif

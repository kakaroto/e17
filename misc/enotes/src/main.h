
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


#ifndef MAIN_H
#define MAIN_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Ipc.h>
#include <Edje.h>

#include "config.h"
#include "note.h"
#include "storage.h"
#include "usage.h"
#include "debug.h"


#define MAX_TEMPLATE_SIZE 999
#define TEMPLATE_LOC "%s/.e/notes/defnote"

#define INTRO_TITLE "Welcome to E-Notes"
#define INTRO_CONTENT "Welcome to E-Notes\nBy Thomas Fletcher.\n\nReport bugs to:\nshadyfletch@yahoo.co.uk\n\nEnjoy!"

/* The Main Function */
int             main(int argc, char *argv[]);

/* External Variables */
extern int      dispusage;
MainConfig     *main_config;

#endif

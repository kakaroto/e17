
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
#include <Ecore_Config.h>
#include <Edje.h>

#include "config.h"
#include "note.h"
#include "storage.h"
#include "debug.h"
#include "welcome.h"
#include "ipc.h"


#define MAX_TEMPLATE_SIZE 999
#define TEMPLATE_LOC "%s/.e/notes/defnote"

extern char    *remotecmd;

void            update_autosave(void);

/* The Main Function */
int             main(int argc, char *argv[]);

/* External Variables */
extern int      dispusage;
extern MainConfig *main_config;

#endif

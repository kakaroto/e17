
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
#include "usage.h"
#include "debug.h"


#define INTRO_TITLE "Welcome to E-Notes"
#define INTRO_CONTENT "\
Welcome to E-Notes, by\n\
Thomas Fletcher.\n\n\
Please E-Mail any bugs you\n\
encounter to:\n\
shadyfletch@yahoo.co.uk\n\n\
ENJOY! :-)\n\
"

/* The Main Function */
int             main(int argc, char *argv[]);

/* External Variables */
extern int      dispusage;
MainConfig     *main_config;

#endif

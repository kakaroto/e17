
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


#ifndef CONFIG_H
#define CONFIG_H 1

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "debug.h"
#include "../config.h"
#include "xml.h"


#define DEF_CONFIG_LOC "%s/.e/notes/config.xml"
#define COPY_COMMAND "cp"

typedef struct {
	char           *render_method;
	char           *theme;
	int             controlcentre;
	int             intro;
	int             debug;
	int             autosave;
} MainConfig;

#ifndef XMLENTRY_DEF
#define XMLENTRY_DEF 1
typedef struct {
	char           *name;
	char           *value;
} XmlEntry;
#endif

MainConfig     *mainconfig_new(void);
void            mainconfig_free(MainConfig * p);

void            read_global_configuration(MainConfig * p);
void            read_local_configuration(MainConfig * p);
void            read_configuration(MainConfig * p, char *fn);
void            check_local_configuration(void);

void            processopt(XmlEntry * info, MainConfig * p);

#endif


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


#ifndef USAGE_H
#define USAGE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "debug.h"
#include "config.h"
#include "../config.h"
#include "ipc.h"

#define USAGE "\
\
E-Notes\n\
By Thomas Fletcher\n\n\
Command Line Arguments:\n\
\n\
POSIX     GNU               TYPE    DESCRITION\n\
-?        --help            N/A     Display the Usage.\n\
-v        --version         N/A     Display the Version.\n\
\n\
-R        --remote          STR     Send a remote message to\n\
                                    a running E-Notes.\n\
\n\
-c        --config-file     STR     Configuration File.\n\
\n\
-r        --render-method   STR     Render Method\n\
-t        --theme           STR     Theme\n\
-C        --control-centre  INT     Enable/Disable the Control\n\
                                    Centre.\n\
-i        --intro           INT     Display the Intro.\n\
\
\n"

#define USAGE_VERSION "E-Notes Version:\n%s\n"

#define OPTSTR "v?hc:r:t:i:R:d:"

static struct option long_options[] = {
	{"help", 0, 0, '?'},
	{"version", 0, 0, 'v'},
	{"remote", 1, 0, 'R'},
	{"config-file", 1, 0, 'c'},
	{"render-method", 1, 0, 'r'},
	{"theme", 1, 0, 't'},
	{"control-centre", 1, 0, 'C'},
	{"intro", 1, 0, 'i'},
	{"debug", 1, 0, 'd'},
	{NULL, 0, 0, 0}
};

extern char    *optarg;

/* Reading the Usage */
void            read_usage_configuration(MainConfig * p, int argc,
					 char *argv[]);
char           *read_usage_for_configuration_fn(int argc, char *argv[]);

/* Printing the Usage */
void            print_usage(void);

/* External Variables */
extern int      dispusage;

#endif

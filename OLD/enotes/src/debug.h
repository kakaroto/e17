
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


#ifndef DEBUG_H
#define DEBUG_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"


#define DEBUG_MSG "E-Notes: %s.\n"
#define DEBUG_ERROR_MSG "E-Notes: ERROR - %s.\n"
#define DEBUG_WARNING_MSG "E-Notes: WARNING - %s.\n"
#define DEBUG_NOTICE_MSG "E-Notes: NOTICE - %s.\n"

#define debug_func_in(foo) dfi(foo)
#define debug_func_out(foo) dfo(foo)

#define debug_msg(foo) dm(foo)
#define debug_msg_lvl(foo,lvl) dml(foo,lvl)

#define error_msg(foo) debug_msg_lvl(foo,3)
#define warning_msg(foo) debug_msg_lvl(foo,4)
#define notice_msg(foo) debug_msg_lvl(foo,5)

typedef struct {
	char           *name;
	int             level;
} DebugFuncLst;

extern DebugFuncLst func_list[];


void            debug_msg_lvl(char *msg, int level);
void            debug_msg(char *msg);

#endif

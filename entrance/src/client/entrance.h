#ifndef _ENTRANCE_H_
#define _ENTRANCE_H_

#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <syslog.h>

#include "util.h"
#include "entrance_session.h"
#include <locale.h>
#define _(string) gettext(string)

#define X_TESTING 0
#define EXITCODE 42

extern int ENTRANCE_DESKTOP_TYPE_XSESSION;
#endif

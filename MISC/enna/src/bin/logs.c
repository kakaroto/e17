#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "enna.h"

#define DEFAULT_MODULE_NAME "Enna"

#define NORMAL   "\033[0m"
#define COLOR(x) "\033[" #x ";1m"
#define BOLD     COLOR(1)
#define F_RED    COLOR(31)
#define F_GREEN  COLOR(32)
#define F_YELLOW COLOR(33)
#define F_BLUE   COLOR(34)
#define B_RED    COLOR(41)

void elog(int level, const char *module, char *file, int line,
        const char *format, ...)
{
    static const char const *c[] =
    { [ENNA_MSG_EVENT] = F_BLUE, [ENNA_MSG_INFO] = F_GREEN,
            [ENNA_MSG_WARNING] = F_YELLOW, [ENNA_MSG_ERROR] = F_RED,
            [ENNA_MSG_CRITICAL] = B_RED, 
};

static const char const *l[] =
{
    [ENNA_MSG_EVENT] = "Event",
    [ENNA_MSG_INFO] = "Info",
    [ENNA_MSG_WARNING] = "Warn",
    [ENNA_MSG_ERROR] = "Err",
    [ENNA_MSG_CRITICAL] = "Crit",
};

va_list va;
int verbosity;

if (!enna || !format)
return;

verbosity = enna->lvl;

/* do we really want loging ? */
if (verbosity == ENNA_MSG_NONE)
return;

if (level < verbosity)
return;

va_start (va, format);

if (!module)
module = DEFAULT_MODULE_NAME;

fprintf (stderr, "[" BOLD "%s" NORMAL "] [%s:%d] %s%s" NORMAL ": ",
        module, file, line, c[level], l[level]);

vfprintf (stderr, format, va);
fprintf (stderr, "\n");
va_end (va);
}

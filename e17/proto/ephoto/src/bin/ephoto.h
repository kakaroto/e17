#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include "config.h"
#include <Ewl.h>
#include <stdio.h>
#include <stdlib.h>

/* Main gui callbacks */
void init_gui(void);

/* NLS */
#ifdef ENABLE_NLS
# include <libintl.h>
# include <locale.h>
# define _(str) gettext(str)
# define gettext_noop(str) str
# define N_(str) gettext_noop(str)
#else
# define _(str) (str)
# define gettext_noop(str) str
# define N_(str) gettext_noop(str)
# define gettext(str) ((char*) (str))
#endif

/* NLS callbacks */
char *sgettext(const char *msgid);
#define S_(str) sgettext(str)

#endif

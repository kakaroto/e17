#ifndef INTL_H
#define INTL_H
/*NLS */
#include <string.h>

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

inline char * sgettext(const char *msgid)
{
	char *msgval = gettext (msgid);
	if (msgval == msgid)
		msgval = strrchr (msgid, '|') + 1;
	return msgval;
}

#endif

#ifndef EWL_INTL_H
#define EWL_INTL_H

#include <string.h>

#ifdef ENABLE_NLS
# include <libintl.h>
# include <locale.h>
#else
# define dgettext(domain, str) ((char*) (str))
#endif

#define D_(str) dgettext(PACKAGE, str)
#define gettext_noop(str) str
#define N_(str) gettext_noop(str)
#define SD_(str) sdgettext(PACKAGE, str)

static inline char *
sdgettext(const char *domain, const char *msgid)
{
        char *msgval = dgettext(domain, msgid);
        if (msgval == msgid)
                msgval = strrchr(msgid, '|') + 1;
        return msgval;
}

#endif

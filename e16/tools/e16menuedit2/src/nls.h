#ifndef _NLS_H
#define _NLS_H

#ifdef ENABLE_NLS

#include <libintl.h>

#define _(String) gettext(String)
#define N_(String) (String)

#else /* don't support NLS */

#define _(String) (String)
#define N_(String) (String)

#endif

#endif /* _NLS_H */

#ifndef _MESS_HEADER_H
#define _MESS_HEADER_H

#include <Evas.h>

#ifndef _CONFIG_H
# define _CONFIG_H
# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif
#endif

char *_load_plain(const char *file);
char *_load_file(const char *file);
Eina_Bool _save_markup_utf8(const char *file, const char *text);
Eina_Bool _save_plain_utf8(const char *file, const char *text);

void editor_font_choose(Evas_Object *ent, const char *font, int size);

#ifdef HAVE___ATTRIBUTE__
# define __UNUSED__ __attribute__((unused))
#else
# define __UNUSED__
#endif

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(x) gettext(x)
#else
# define _(x) (x)
#endif

#endif

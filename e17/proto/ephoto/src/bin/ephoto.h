#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include "config.h"
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Ewl.h>
#include <fnmatch.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Main gui callbacks */
void init_gui(void);

/* Ephoto Browsing */
Ecore_List *get_directories(char *directory);
Ecore_List *get_images(char *directory);

/* Ephoto Imaging */
unsigned int *rotate_left(Ewl_Widget *image);
unsigned int *rotate_right(Ewl_Widget *image);
void update_image(Ewl_Widget *image, int w, int h, unsigned int *data);

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

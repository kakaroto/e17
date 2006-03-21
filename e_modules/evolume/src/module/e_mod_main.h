
#ifndef E_MOD_MAIN_H_INCLUDED
#define E_MOD_MAIN_H_INCLUDED

#include <e.h>


#define DEBUG
#ifdef DEBUG
# ifndef DBG
#  define DBG fprintf
# endif
#else
# ifndef DBG
#  define DBG
# endif
#endif


static char * D_(char *str)
{
    fprintf(stderr,"dgettext(%s, %s)\n", PACKAGE, str);
    return dgettext(PACKAGE, str);
}

extern char* module_root;
extern char* module_theme;

#endif //E_MOD_MAIN_H_INCLUDED

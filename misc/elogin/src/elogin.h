#ifndef ELOGIN_H
#define ELOGIN_H	1

#include "ewl.h"

void	elogin_init (void);
char	cb_test_option(int argc, char *argv[]);
char	cb_mouse(EwlWidget *w, EwlEvent *ev, EwlData *d);
char	cb_keydown(EwlWidget *w, EwlEvent *ev, EwlData *d);


#endif

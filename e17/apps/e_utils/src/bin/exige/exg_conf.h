#ifndef EXG_CONF_H
#define EXG_CONF_H

#include "Exige.h"
#include "exg_gui.h"

enum
{
    EXG_CONF_TAG_ON_MOUSE,
    EXG_CONF_TAG_TERM
};


int exg_conf_init(Exige *exg);

void exg_conf_default_set();

char *exg_conf_theme_get();
void exg_conf_theme_set(char *theme);

char *exg_conf_term_get();
void exg_conf_term_set(char *term);

int exg_conf_on_mouse_get();
void exg_conf_on_mouse_set(int on_mouse);

#endif

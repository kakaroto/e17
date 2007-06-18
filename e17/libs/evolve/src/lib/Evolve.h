#ifndef _EVOLVE_H
#define _EVOLVE_H

#include <Etk.h>
#include <Evas.h>

typedef struct _Evolve Evolve;

int evolve_init();
int evolve_shutdown();

void evolve_print(Evolve *evolve);
  

Evolve     *evolve_etk_load(char *file);

Evolve     *evolve_eet_load(char *file);
int         evolve_eet_save(Evolve *evolve, char *file);

void        evolve_render(Evolve *evolve);
Etk_Widget *evolve_etk_widget_find(Evolve *evolve, char *name);

void        evolve_signal_connect(Evolve *evolve, char *emission, void (*callback)(char *emission, void *data), void *data);

char       *evolve_code_get(Evolve *evolve);

void        evolve_defines_set(Evas_List *defines);

#endif

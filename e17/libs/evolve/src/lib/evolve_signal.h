#ifndef _EVOLVE_SIGNAL_H
#define _EVOLVE_SIGNAL_H

Evolve_Signal *evolve_signal_new(char *name, char *emission, Evolve *evolve);
void evolve_signal_emit_cb(void *data, Etk_Object *object);
void evolve_signal_connect(Evolve *evolve, char *emission, void (*callback)(char *emission, void *data), void *data);

#endif

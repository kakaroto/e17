#ifdef E_TYPEDEFS
#else
#ifndef E_MOD_KEYBIND_H
#define E_MOD_KEYBIND_H

#include "e_mod_main.h"

#define MIXER_DECREASE_ACTION   "decrease_volume"
#define MIXER_INCREASE_ACTION   "increase_volume"
#define MIXER_MUTE_ACTION       "mute"

int mixer_register_module_actions();
int mixer_unregister_module_actions();

int mixer_register_module_keybindings();
int mixer_unregister_module_keybindings();

#endif
#endif

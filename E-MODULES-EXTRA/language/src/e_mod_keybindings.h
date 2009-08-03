#ifdef E_TYPEDEFS
#else
#ifndef E_MOD_KEYBIND_H
#define E_MOD_KEYBIND_H

#include "e_mod_main.h"

#define LANG_NEXT_ACTION   "switch_next_language"
#define LANG_PREV_ACTION   "switch_prev_language"

int lang_register_module_actions();
int lang_unregister_module_actions();

int lang_register_module_keybindings();
int lang_unregister_module_keybindings();

#endif
#endif

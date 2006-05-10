#ifdef E_TYPEDEFS
#else
#ifndef E_MOD_KEYBIND_H
#define E_MOD_KEYBIND_H

#include "e_mod_main.h"

int _lang_register_module_actions();
int _lang_unregister_module_actions();

int _lang_register_module_keybindings(Lang *l);
int _lang_unregister_module_keybindings(Lang *l);

#endif
#endif

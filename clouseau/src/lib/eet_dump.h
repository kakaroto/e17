#ifndef _EET_DUMP_H
#define _EET_DUMP_H
#include "libclouseau.h"
Tree_Item * eet_dump_tree_load(const char *filename);
Eina_Bool eet_dump_tree_save(const Tree_Item *treeit, const char *filename);
#endif

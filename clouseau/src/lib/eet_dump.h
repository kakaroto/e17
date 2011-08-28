#ifndef _EET_DUMP_H
#define _EET_DUMP_H

void eet_dump_tree_item_descriptor_init(void);
void eet_dump_tree_item_descriptor_shutdown(void);
Tree_Item *eet_dump_tree_load(const char *filename);

Eina_Bool eet_dump_tree_save(const Tree_Item *treeit, const char *filename);

#endif

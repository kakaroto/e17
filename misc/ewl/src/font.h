#ifndef _FONT_H_
#define _FONT_H_

#include "includes.h"
#include "list.h"

EwlList *ewl_get_font_path_list();
void     ewl_add_font_path(char *path);
void     ewl_remove_font_path(char *path);

#endif /* _FONT_H_ */

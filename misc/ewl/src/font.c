#include "font.h"

EwlList *ewl_get_font_path_list()
{
	static EwlList *path_list = NULL;
	if (!path_list)	{
		path_list = ewl_list_new();
	}
	return path_list;
}

void     ewl_add_font_path(char *path)
{
	ewl_list_insert(ewl_get_font_path_list(),ewl_list_node_new(path));
	return;
}

void     ewl_remove_font_path(char *path)
{
	EwlList     *list = ewl_get_font_path_list();
	EwlListNode *node = ewl_list_find_by_value(list,path);
	if (node)	ewl_list_remove(list,node);
	return;
}


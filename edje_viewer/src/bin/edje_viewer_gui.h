#ifndef _EG_H
#define _EG_H

Gui *main_window_show(const char *file);
int gui_part_col_sort_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1,
      Etk_Tree_Row *row2, void *data);

#endif

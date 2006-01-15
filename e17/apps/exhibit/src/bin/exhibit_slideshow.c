#include "exhibit.h"

void
_ex_slideshow_stop(Exhibit *e)
{
   if(e->slideshow.active)
     {
	e->slideshow.active = ETK_FALSE;
	ecore_timer_del(e->slideshow.timer);
     }
}

void
_ex_slideshow_start(Exhibit *e)
{
   if(!e->slideshow.active)
     {
	e->slideshow.timer = ecore_timer_add(e->slideshow.interval, _ex_slideshow_next, e);
	e->slideshow.active = ETK_TRUE;
     }
}

int
_ex_slideshow_next(void *data)
{
   Exhibit *e;
   Etk_Tree_Row *row, *first_row, *last_row;;
   
   e = data;
   row = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
   last_row = etk_tree_last_row_get(ETK_TREE(e->cur_tab->itree), ETK_FALSE, ETK_FALSE);
   first_row = etk_tree_first_row_get(ETK_TREE(e->cur_tab->itree));   
   
   if(!row || row == last_row)
     row = etk_tree_first_row_get(ETK_TREE(e->cur_tab->itree));
   else
     row = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
   
   etk_tree_row_select(row);
   etk_tree_row_scroll_to(row, ETK_FALSE);
   
   return 1;   
}

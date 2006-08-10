#include "exhibit.h"

void
_ex_slideshow_stop(Exhibit *e)
{
   if(e->slideshow.active)
     {
	etk_statusbar_push(ETK_STATUSBAR(e->statusbar[3]), "", 0);
	e->slideshow.active = ETK_FALSE;
	ecore_timer_del(e->slideshow.timer);
     }
}

void
_ex_slideshow_start(Exhibit *e)
{
	if (e->options->slide_interval)
		e->slideshow.interval = e->options->slide_interval;

   if(!e->slideshow.active)
     {
	etk_statusbar_push(ETK_STATUSBAR(e->statusbar[3]), "Slideshow running", 0);
	e->slideshow.timer = ecore_timer_add(e->slideshow.interval, _ex_slideshow_next, e);
	e->slideshow.active = ETK_TRUE;
     }
}

int
_ex_slideshow_next(void *data)
{
   Exhibit *e;
   Etk_Tree_Row *row, *first_row, *last_row, *count;
	 int i;

   e = data;
   row = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
   last_row = etk_tree_last_row_get(ETK_TREE(e->cur_tab->itree), ETK_FALSE, ETK_FALSE);
   first_row = etk_tree_first_row_get(ETK_TREE(e->cur_tab->itree));

	 /* FIXME
		* Need an etk function for this, this isnt even working properly.
		* Would be nice to display slideshow image x of maximages in sbar.
		*/
	 for (count = etk_tree_first_row_get(ETK_TREE(e->cur_tab->itree)), i = 0; 
			 count && count != row; i++)
		 count = etk_tree_next_row_get(count, ETK_FALSE, ETK_FALSE);
	 
	 D(("Rownumber: %d\n", i));

   if(!row || row == last_row)
     row = etk_tree_first_row_get(ETK_TREE(e->cur_tab->itree));
   else
     row = etk_tree_next_row_get(row, ETK_FALSE, ETK_FALSE);
   
   etk_tree_row_select(row);
   etk_tree_row_scroll_to(row, ETK_FALSE);
   
   return 1;   
}

/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"

void
_ex_slideshow_stop(void)
{
   if(e->slideshow.active)
     {
	etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[3]), "", 0);
	e->slideshow.active = ETK_FALSE;
	ecore_timer_del(e->slideshow.timer);
     }
}

void
_ex_slideshow_start(void)
{
	if (e->options->slide_interval)
		e->slideshow.interval = e->options->slide_interval;

   if(!e->slideshow.active)
     {
	etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[3]), "Slideshow running", 0);
	e->slideshow.timer = ecore_timer_add(e->slideshow.interval, 
	      _ex_slideshow_next, NULL);
	e->slideshow.active = ETK_TRUE;
     }
}

int
_ex_slideshow_next(void *data)
{
   Etk_Tree_Row *row, *last_row;
   int i = 0;
   int n = 0;
   char string[80];

   row = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
   last_row = etk_tree_last_row_get(ETK_TREE(e->cur_tab->itree));

   if(!row || row == last_row)
     row = etk_tree_first_row_get(ETK_TREE(e->cur_tab->itree));
   else
     row = etk_tree_row_next_get(row);
   
   etk_tree_row_select(row);
   etk_tree_row_scroll_to(row, ETK_FALSE);

   i = etk_tree_num_rows_get(ETK_TREE(e->cur_tab->itree));
   n = 1 + etk_tree_row_num_get(ETK_TREE(e->cur_tab->itree), row);
   snprintf(string, sizeof(string), "Slideshow picture %d of %d", n, i);
   etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[3]), string, 0);

   return 1; 
}

int
_ex_slideshow_prev(void *data)
{
   Etk_Tree_Row *row, *first_row;
   int i = 0;
   int n = 0;
   char string[80];

   row = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
   first_row = etk_tree_first_row_get(ETK_TREE(e->cur_tab->itree));

   if(!row || row == first_row)
     row = etk_tree_last_row_get(ETK_TREE(e->cur_tab->itree));
   else
     row = etk_tree_row_prev_get(row);
   
   etk_tree_row_select(row);
   etk_tree_row_scroll_to(row, ETK_FALSE);

   i = etk_tree_num_rows_get(ETK_TREE(e->cur_tab->itree));
   n = 1 + etk_tree_row_num_get(ETK_TREE(e->cur_tab->itree), row);
   snprintf(string, sizeof(string), "Slideshow picture %d of %d", n, i);
   etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[3]), string, 0);

   return 1; 
}

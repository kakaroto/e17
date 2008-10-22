/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"
#include "exhibit_file.h"

int
_ex_thumb_complete(void *data, int type, void *event)
{   
   Epsilon_Request *ev = event;
   Ex_Thumb *thumb;
   char *ext;

   thumb = ev->data;
   if (!thumb)
     return 1;

   ext = strrchr(thumb->name, '.');

   if (ext)
     {
	Etk_Tree_Row *row;
	
	thumb->image = (char*)ev->dest;
	row = _ex_image_find_row_from_file(thumb->tab, thumb->name);
	etk_tree_row_fields_set(row,
				ETK_FALSE,
				thumb->tab->icol, 
				thumb->image, NULL,
				thumb->name, NULL);
	
	if(thumb->selected)
	  {
	     etk_tree_row_select(row);
	     etk_tree_row_scroll_to(row, ETK_TRUE);
	  }
	E_FREE(thumb->name);	
	E_FREE(thumb);
     }

   return 1;
}

void
_ex_thumb_abort(void)
{
   /* TODO: reimplement */
#if 0
   Eina_List *l;
   Ex_Thumb *thumb;

   pid = -1;

   EINA_LIST_FOREACH(thum_list, l, thumb)
   for(l = thumb_list; l; l = l->next)
     {
	if (thumb->tab == e->cur_tab)
	  {
	     E_FREE(thumb->image);
	     E_FREE(thumb->name);
	     if (thumb->ep)
	       epsilon_free(thumb->ep);
	     E_FREE(thumb);
	     thumb_list = eina_list_remove_list(thumb_list, l);
	  }
     }
#endif
}


void
_ex_thumb_abort_all(void)
{
   /* TODO: reimplement */
#if 0   
   pid = -1;
   
   while(thumb_list)
     {
	Ex_Thumb *thumb;	
	thumb = thumb_list->data;
	E_FREE(thumb->image);
	E_FREE(thumb->name);
	if (thumb->ep)
	  epsilon_free(thumb->ep);
	E_FREE(thumb);
	thumb_list = eina_list_remove_list(thumb_list, thumb_list);
     }
   eina_list_free(thumb_list);
#endif   
}

void
_ex_thumb_update_at_row(Etk_Tree_Row *row)
{   
   Etk_Tree *tree;
   Epsilon *ep = NULL;
   char file[PATH_MAX];
   char *icol_string;
   char *old_thumb;   
   Ex_Tab *tab;
   Ex_Thumb *thumb;
   
   if (!row || !(tree = etk_tree_row_tree_get(row)))
     return;       
      
   etk_tree_row_fields_get(
			   row,
			   etk_tree_nth_col_get(ETK_TREE(tree), 0),
			   &old_thumb,
			   NULL,
			   &icol_string,
			   NULL);
   
   tab = _ex_tab_find_by_itree(tree);
   
   snprintf(file, sizeof(file), "%s%s",
	    tab->dir, icol_string);
   
   ecore_file_unlink(old_thumb);
   
   thumb = calloc(1, sizeof(Ex_Thumb));
   thumb->ep = ep;
   thumb->e = e;
   thumb->name = strdup(basename(icol_string));
   thumb->tab = e->cur_tab;
   thumb->selected = ETK_FALSE;
   etk_tree_model_cache_remove(thumb->tab->imodel, old_thumb, NULL);
   epsilon_request_add(file, EPSILON_THUMB_NORMAL, thumb);
}

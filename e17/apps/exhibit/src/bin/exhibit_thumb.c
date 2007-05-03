/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"
#include "exhibit_file.h"

pid_t pid = -1;
Evas_List *thumb_list;

int
_ex_thumb_exe_exit(void *data, int type, void *event)
{
   Ecore_Exe_Event_Del  *ev;
   Ex_Thumb             *thumb;
   char                 *ext;

   ev = event;
   if (ev->pid != pid) return 1;
   if (!thumb_list) return 1;

   thumb = thumb_list->data;
   thumb_list = evas_list_remove_list(thumb_list, thumb_list);

   ext = strrchr(thumb->name, '.');

   if (ext)
     {
	Etk_Tree_Row *row;
	
	thumb->image = (char*)epsilon_thumb_file_get(thumb->ep);
	if (thumb->is_update)
	  {
	     row = _ex_image_find_row_from_file(thumb->tab, thumb->name);
	     etk_tree_row_fields_set(row,
				     ETK_FALSE,
				     thumb->tab->icol, 
				     thumb->image, NULL,
				     thumb->name, NULL);
	  }
	else
	  {
	     row = etk_tree_row_append(ETK_TREE(thumb->tab->itree),
				       NULL,
				       thumb->tab->icol, 
				       thumb->image, NULL,
				       thumb->name, NULL);
	  }
	if(thumb->selected)
	  {
	     etk_tree_row_select(row);
	     etk_tree_row_scroll_to(row, ETK_TRUE);
	  }
	E_FREE(thumb->image);
	E_FREE(thumb->name);
	epsilon_free(thumb->ep);
	E_FREE(thumb);
     }

   pid = -1;
   _ex_thumb_generate();
   return 1;
}

void
_ex_thumb_generate()
{
   Ex_Thumb *thumb;

   if ((!thumb_list) || (pid != -1)) return;

   pid = fork();
   if (pid == 0)
     {
	/* reset signal handlers for the child */
	signal(SIGSEGV, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGBUS, SIG_DFL);
		
	thumb = thumb_list->data;
	if(_ex_file_is_ebg(thumb->name))
	  epsilon_key_set(thumb->ep, "desktop/background");
	if(epsilon_generate(thumb->ep))
	  {
	     thumb->image = (char*)epsilon_thumb_file_get(thumb->ep);
	  }
	exit(0);
     }
}

void
_ex_thumb_abort()
{
   Evas_List *l;
   pid = -1;
   
   for(l = thumb_list; l; l = l->next)
     {
	Ex_Thumb *thumb;
	
	thumb = l->data;
	if (thumb->tab == e->cur_tab)
	  {
	     E_FREE(thumb->image);
	     E_FREE(thumb->name);
	     if (thumb->ep)
	       epsilon_free(thumb->ep);
	     E_FREE(thumb);
	     thumb_list = evas_list_remove_list(thumb_list, l);
	  }
     }
}


void
_ex_thumb_abort_all()
{
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
	thumb_list = evas_list_remove_list(thumb_list, thumb_list);
     }
   evas_list_free(thumb_list);
}

void
_ex_thumb_update_at_row(Etk_Tree_Row *row)
{   
   Etk_Tree *tree;
   Epsilon *ep;
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
	    tab->cur_path, icol_string);
   
   ep = epsilon_new(file);

   /* For some reason, if we dont call this, something goes wrong
    * Need to look into epsilon to see what this function is doing that makes
    * everything work well.
    */
   epsilon_exists(ep);
   ecore_file_unlink(old_thumb);
   
   thumb = calloc(1, sizeof(Ex_Thumb));
   thumb->ep = ep;
   thumb->e = e;
   thumb->name = strdup(basename(icol_string));
   thumb->tab = e->cur_tab;
   thumb_list = evas_list_append(thumb_list, thumb);
   thumb->selected = ETK_FALSE;
   thumb->is_update = ETK_TRUE;
   etk_tree_model_cache_remove(thumb->tab->imodel, old_thumb, NULL);
   if(pid == -1) _ex_thumb_generate();   
}

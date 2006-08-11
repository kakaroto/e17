#include "exhibit.h"

static Ecore_Evas *ee_buf;
static Evas *evas_buf;

static int
_ex_sort_itree_name_compare_cb(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data)
{
   char *f1, *f2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   etk_tree_row_fields_get(row1, col, NULL, &f1, NULL,NULL);
   etk_tree_row_fields_get(row2, col, NULL, &f2, NULL,NULL);
   
   return strcasecmp(f1, f2);
}


static int
_ex_sort_itree_size_compare_cb(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data)
{
   char *f1, *f2;
   struct stat s1, s2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   etk_tree_row_fields_get(row1, col, NULL, &f1, NULL,NULL);
   etk_tree_row_fields_get(row2, col, NULL, &f2, NULL,NULL);
   
   stat(f1, &s1);
   stat(f2, &s2);
   
   if(s1.st_size > s2.st_size)
     return 1;
   else
     return -1;
}

static int
_ex_sort_itree_date_compare_cb(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data)
{
   char *f1, *f2;
   struct stat s1, s2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   etk_tree_row_fields_get(row1, col, NULL, &f1, NULL,NULL);
   etk_tree_row_fields_get(row2, col, NULL, &f2, NULL,NULL);
   
   stat(f1, &s1);
   stat(f2, &s2);
   
   if(s1.st_mtime > s2.st_mtime)
     return 1;
   else
     return -1;
}

static int
_ex_sort_itree_resol_compare_cb(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data)
{
   char *f1, *f2;
   Evas_Object *i1, *i2;
   int w1, h1, w2, h2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   etk_tree_row_fields_get(row1, col, NULL, &f1, NULL,NULL);
   etk_tree_row_fields_get(row2, col, NULL, &f2, NULL,NULL);

   if(!ee_buf)
     {
	ee_buf = ecore_evas_buffer_new(0, 0);
	evas_buf = ecore_evas_get(ee_buf);
     }   
   
   if(_ex_file_is_ebg(f1))
     {
	w1 = 800;
	h1 = 600;
     }
   else
     {
	i1 = evas_object_image_add(evas_buf);
	evas_object_image_file_set(i1, f1, NULL);
	evas_object_image_size_get(i1, &w1, &h1);
	evas_object_del(i1);
     }
   
   if(_ex_file_is_ebg(f2))
     {
	w2 = 800;
	h2 = 600;
     }
   else
     {
	i2 = evas_object_image_add(evas_buf);
	evas_object_image_file_set(i2, f2, NULL);
	evas_object_image_size_get(i2, &w2, &h2);
	evas_object_del(i2);	
     }      
   
   if(w1 * h1 > w2 * h2)
     return 1;
   else
     return -1;   
}

void 
_ex_sort_name_cb(Etk_Object *obj, void *data)
{    
   etk_tree_sort(ETK_TREE(e->cur_tab->itree), _ex_sort_itree_name_compare_cb, 
			 ETK_TRUE, e->cur_tab->icol, NULL);
}

void 
_ex_sort_size_cb(Etk_Object *obj, void *data)
{    
   etk_tree_sort(ETK_TREE(e->cur_tab->itree), _ex_sort_itree_size_compare_cb, ETK_TRUE, e->cur_tab->icol, NULL);
}

void 
_ex_sort_resol_cb(Etk_Object *obj, void *data)
{    
   etk_tree_sort(ETK_TREE(e->cur_tab->itree), _ex_sort_itree_resol_compare_cb, ETK_TRUE, e->cur_tab->icol, NULL);
}

void 
_ex_sort_date_cb(Etk_Object *obj, void *data)
{    
   etk_tree_sort(ETK_TREE(e->cur_tab->itree), _ex_sort_itree_date_compare_cb, ETK_TRUE, e->cur_tab->icol, NULL);
}


void
_ex_sort_label_mouse_down_cb(Etk_Object *object, void *event_info, void *data)
{
   etk_menu_popup(ETK_MENU(data));   
}


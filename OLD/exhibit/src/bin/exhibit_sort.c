/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"

static Ecore_Evas *ee_buf;
static Evas *evas_buf;

static int _ex_sort_resol(char *f1, char *f2)
{
   Evas_Object *i1, *i2;
   int w1, h1, w2, h2;

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

static int
_ex_sort_itree_name_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data)
{
   char *f1, *f2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   etk_tree_row_fields_get(row1, col, NULL, NULL, &f1, NULL);
   etk_tree_row_fields_get(row2, col, NULL, NULL, &f2, NULL);
      
   return strcasecmp(f1, f2);
}


static int
_ex_sort_itree_size_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data)
{
   char *f1, *f2;
   struct stat s1, s2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   f1 = (char *)etk_tree_row_data_get(row1);
   f2 = (char *)etk_tree_row_data_get(row2);
   
   stat(f1, &s1);
   stat(f2, &s2);
   
   if(s1.st_size > s2.st_size)
     return 1;
   else
     return -1;
}

static int
_ex_sort_itree_date_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data)
{
   char *f1, *f2;
   struct stat s1, s2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   f1 = (char *)etk_tree_row_data_get(row1);
   f2 = (char *)etk_tree_row_data_get(row2);
   
   stat(f1, &s1);
   stat(f2, &s2);
   
   if(s1.st_mtime > s2.st_mtime)
     return 1;
   else
     return -1;
}

static int
_ex_sort_itree_resol_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data)
{
   char *f1, *f2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   f1 = (char *)etk_tree_row_data_get(row1);
   f2 = (char *)etk_tree_row_data_get(row2);

   return _ex_sort_resol(f1, f2);
}
   
int _ex_sort_cmp_name(const void *p1, const void *p2)
{
   /* The actual arguments to this function are "pointers to
      pointers to char", but strcmp() arguments are "pointers
      to char", hence the following cast plus dereference 
      */
   
   return strcasecmp(* (char * const *) p1, * (char * const *) p2);
}

int _ex_sort_cmp_size(const void *p1, const void *p2)
{
   struct stat s1, s2;

   stat(* (char * const *) p1, &s1);
   stat(* (char * const *) p2, &s2);

   if(s1.st_size > s2.st_size)
     return 1;
   else
     return -1;
}

int _ex_sort_cmp_date(const void *p1, const void *p2)
{
   struct stat s1, s2;
   
   stat(* (char * const *) p1, &s1);
   stat(* (char * const *) p2, &s2);  
   
   if(s1.st_mtime > s2.st_mtime)
     return 1;
   else
     return -1;
}

int _ex_sort_cmp_resol(const void *p1, const void *p2)
{
   return _ex_sort_resol(* (char * const *) p1, * (char * const *) p2);
}

Etk_Bool
_ex_sort_name_cb(Etk_Object *obj, void *data)
{    
   etk_tree_col_sort_full(etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 0), _ex_sort_itree_name_compare_cb,
			  NULL, ETK_TRUE);
   return ETK_TRUE;
}

Etk_Bool
_ex_sort_size_cb(Etk_Object *obj, void *data)
{    
   etk_tree_col_sort_full(e->cur_tab->icol, _ex_sort_itree_size_compare_cb,
			  NULL, ETK_TRUE);
   return ETK_TRUE;
}

Etk_Bool
_ex_sort_resol_cb(Etk_Object *obj, void *data)
{    
   etk_tree_col_sort_full(e->cur_tab->icol, _ex_sort_itree_resol_compare_cb,
			  NULL, ETK_TRUE);
   return ETK_TRUE;
}

Etk_Bool
_ex_sort_date_cb(Etk_Object *obj, void *data)
{    
   etk_tree_col_sort_full(e->cur_tab->icol, _ex_sort_itree_date_compare_cb,
			  NULL, ETK_TRUE);
   return ETK_TRUE;
}

void
_ex_sort_label_mouse_down_cb(Etk_Object *object, void *event_info, void *data)
{
   etk_menu_popup(ETK_MENU(data));
}


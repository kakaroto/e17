#include <string.h>
#include <Etk.h>
#include "Entrance_Widgets.h"

#include "_ew_list.h"

static void _ew_tree_cb_row_clicked(Etk_Object *, Etk_Tree_Row *, Etk_Event_Mouse_Down *, void *);

Entrance_List
ew_edjelist_new(const char *title, int w, int h, int r_h, int c_w)
{
   Entrance_List ew = _ew_list_new(title, w, h, r_h);
   if(!ew) {
	   return NULL;
   }

   ew->col = etk_tree_col_new(ETK_TREE(ew->owner), NULL, c_w, 0.0);
   etk_tree_col_model_add(ew->col, etk_tree_model_image_new());
   etk_tree_col_model_add(ew->col, etk_tree_model_text_new());
 
   return _ew_list_buildtree(ew);
}

void
ew_edjelist_add(Entrance_List ew, const char *label, const char *edje, const char *group, void *data,  size_t size, void (*func) (void*), void* funcdata)
{
   Etk_Tree_Row *row;
   etk_tree_freeze(ETK_TREE(ew->owner));

   row = etk_tree_row_append(ETK_TREE(ew->owner), NULL, etk_tree_nth_col_get(ETK_TREE(ew->owner), 0), edje, group, label, NULL);

   Entrance_List_Data ewld = ew_listdata_new();
   if(ewld)
   {
	   ewld->func = func;
	   if(data)
	   {
		   memcpy(ewld->data, data, size);
	   }
	   ewld->funcdata = funcdata;
   }
   
   etk_tree_row_data_set(row, ewld);

   etk_tree_thaw(ETK_TREE(ew->owner));
}

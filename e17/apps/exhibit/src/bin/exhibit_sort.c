#include "exhibit.h"

void 
_ex_sort_name_cb(Etk_Object *obj, void *data)
{    
   Exhibit *e;
   
   e = data;
   etk_tree_sort(ETK_TREE(e->itree), _ex_main_itree_name_compare_cb, TRUE, e->icol, NULL);
}

void 
_ex_sort_size_cb(Etk_Object *obj, void *data)
{    
   Exhibit *e;
   
   e = data;
   etk_tree_sort(ETK_TREE(e->itree), _ex_main_itree_size_compare_cb, TRUE, e->icol, NULL);
}

void 
_ex_sort_resol_cb(Etk_Object *obj, void *data)
{    
   Exhibit *e;
   
   e = data;
   etk_tree_sort(ETK_TREE(e->itree), _ex_main_itree_resol_compare_cb, TRUE, e->icol, NULL);
}

void 
_ex_sort_date_cb(Etk_Object *obj, void *data)
{    
   Exhibit *e;
   
   e = data;
   etk_tree_sort(ETK_TREE(e->itree), _ex_main_itree_date_compare_cb, TRUE, e->icol, NULL);
}


void
_ex_sort_label_mouse_down_cb(Etk_Object *object, void *event_info, void *data)
{
   etk_menu_popup(ETK_MENU(data));   
}


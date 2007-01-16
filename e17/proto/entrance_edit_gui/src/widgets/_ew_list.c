#include <Etk.h>
#include "Entrance_Widgets.h"

static void _ew_list_cb_row_clicked(Etk_Object *, Etk_Tree2_Row *, Etk_Event_Mouse_Down *, void *);

Entrance_List
__ew_list_new(void)
{
	Entrance_List el = calloc(1, sizeof(*el));
	if(el) 
	{
		el->owner = NULL;
		el->box = NULL;
		el->col = NULL;
	}

	return el;
}

Entrance_List
_ew_list_new(const char *title, int w, int h, int r_h)
{
   Entrance_List ew = __ew_list_new();
   if(!ew) {
	   return NULL;
   }

   ew->owner = etk_tree2_new();
   etk_signal_connect("row_clicked", ETK_OBJECT(ew->owner), ETK_CALLBACK(_ew_list_cb_row_clicked), NULL);
   etk_widget_size_request_set(ew->owner, w, h);
   etk_tree2_mode_set(ETK_TREE2(ew->owner), ETK_TREE2_MODE_LIST);
   etk_tree2_multiple_select_set(ETK_TREE2(ew->owner), ETK_FALSE);
   etk_tree2_headers_visible_set(ETK_TREE2(ew->owner), ETK_FALSE);
   etk_tree2_rows_height_set(ETK_TREE2(ew->owner), r_h);
   
  ew->col = NULL;

   return ew;

}

Entrance_List
_ew_list_buildtree(Entrance_List ew)
{
	etk_tree2_build(ETK_TREE2(ew->owner));

	/*TODO:should trees have their own labels? being we're putting stuff into a group with a label already.*/
	/*Etk_Widget *hbox;

	Etk_Widget *label = etk_label_new(title);
		 {
			
			hbox = etk_hbox_new(0, 0);
			etk_box_append(ETK_BOX(hbox), label, ETK_TRUE, ETK_FALSE, 0);
			etk_box_append(ETK_BOX(ew->hbox), label, ETK_BOX_START, ETK_BOX_EXPAND, 0);
		 }   */
	   
	   ew->box = etk_vbox_new(0,10);
	   /*etk_box_append(ETK_BOX(ew->box), hbox, ETK_BOX_START, ETK_BOX_EXPAND, 0);*/
	   etk_box_append(ETK_BOX(ew->box), ew->owner, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	   return ew;
}

Entrance_List_Data 
ew_listdata_new(void)
{
	Entrance_List_Data ewld = calloc(1, sizeof(*ewld));
	if(ewld) 
	{
		ewld->func = NULL;
		ewld->funcdata = NULL;
		ewld->data = calloc(255, sizeof(char));
	}

	return ewld;
}

void
ew_list_first_row_select(Entrance_List el)
{
	Etk_Tree2_Row *row = etk_tree2_first_row_get(ETK_TREE2(el->owner));
	etk_tree2_row_select(row);
}

void* 
ew_list_selected_data_get(Entrance_List el)
{
	Etk_Tree2_Row *row = etk_tree2_selected_row_get(ETK_TREE2(el->owner));
	if(!row) {
		return NULL;
	}

	Entrance_List_Data ewld = etk_tree2_row_data_get(row);
	return ewld->data;
}

/*privates*/
static void 
_ew_list_cb_row_clicked(Etk_Object *object, Etk_Tree2_Row *row, Etk_Event_Mouse_Down *event, void *data)
{  
   Entrance_List_Data ewld =  etk_tree2_row_data_get(row);

   if(ewld->func)
     ewld->func(ewld->funcdata);
}



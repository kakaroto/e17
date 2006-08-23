#ifndef _EW_LIST_H
#define _EW_LIST_H

typedef struct _Entrance_List_Data {
	void (*func)(void);
	void *data;
} *Entrance_List_Data;

#define EW_LIST_DATA_FREE(ewld) if(1) \
{ \
	if(ewld) \
	{ \
		if(ewld->data) \
		{ \
			free(ewld->data); \
		} \
		free(ewld); \
	} \
} \
else \


Entrance_Widget _ew_list_new(const char*, int, int, int);
Entrance_Widget _ew_list_buildtree(Entrance_Widget);
Entrance_List_Data ew_listdata_new(void);
void* ew_list_selected_data_get(Entrance_Widget);
void ew_list_first_row_select(Entrance_Widget);

#endif

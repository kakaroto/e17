#ifndef _EW_LIST_H
#define _EW_LIST_H

struct __Entrance_List_Data {
	void (*func)(void);
	void *data;
};

typedef struct __Entrance_List_Data _Entrance_List_Data;
typedef struct __Entrance_List_Data *Entrance_List_Data;

struct __Entrance_List {
	Etk_Widget *owner;
	Etk_Widget *box;
	Etk_Tree_Col *col;
};

typedef struct __Entrance_List _Entrance_List;
typedef struct __Entrance_List *Entrance_List;

#define EW_LIST_FREE(ew) if(1) \
{ \
	if(ew) \
	{ \
		if(ew->owner) \
		{ \
			free(ew->owner); \
		} \
		if(ew->box) \
		{ \
			free(ew->box); \
		} \
		if(ew->col) \
		{ \
			free(ew->col); \
		} \
		free(ew); \
	} \
} \
else \

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


Entrance_List _ew_list_new(const char*, int, int, int);
Entrance_List _ew_list_buildtree(Entrance_List);
Entrance_List_Data ew_listdata_new(void);
void* ew_list_selected_data_get(Entrance_List);

#endif

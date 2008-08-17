#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "../ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>
#include <Ecore.h>
#include <Ecore_Data.h>

#include "EtkTypes.h"
#include "EtkSignals.h"

MODULE = Etk::Combobox::Entry		PACKAGE = Etk::Combobox::Entry		PREFIX = etk_combobox_entry_

Etk_Combobox_Entry *
new(class)
	SV	*class
	CODE:
	RETVAL = ETK_COMBOBOX_ENTRY(etk_combobox_entry_new());
	OUTPUT:
	RETVAL

Etk_Combobox_Entry *
new_default(class)
	SV	*class
	ALIAS:
	NewDefault=1
	CODE:
	RETVAL = ETK_COMBOBOX_ENTRY(etk_combobox_entry_new_default());
	OUTPUT:
	RETVAL

# Convert to entry
Etk_Widget *
etk_combobox_entry_entry_get(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	EntryGet=1

void
etk_combobox_entry_items_height_set(combobox_entry, items_height)
	Etk_Combobox_Entry *combobox_entry
	int items_height
	ALIAS:
	ItemsHeightSet=1

int
etk_combobox_entry_items_height_get(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	ItemsHeightGet=1
	
void
etk_combobox_entry_column_add(combobox_entry, col_type, width, fill_policy, align)
	Etk_Combobox_Entry *combobox_entry
	Etk_Combobox_Entry_Column_Type col_type
	int width
	Etk_Combobox_Entry_Fill_Policy fill_policy
	float align
	ALIAS:
	ColumnAdd=1

void
etk_combobox_entry_build(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	Build=1

void
etk_combobox_entry_clear(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	Clear=1

Etk_Combobox_Entry_Item *
etk_combobox_entry_item_append_empty(combobox)
	Etk_Combobox_Entry * combobox
	ALIAS:
	ItemAppendEmpty=1

Etk_Combobox_Entry_Item *
etk_combobox_entry_item_prepend_empty(combobox)
	Etk_Combobox_Entry * combobox
	ALIAS:
	ItemPrependEmpty=1

Etk_Combobox_Entry_Item *
etk_combobox_entry_item_insert_empty(combobox, after)
	Etk_Combobox_Entry * combobox
	Etk_Combobox_Entry_Item * after
	ALIAS:
	ItemInsertEmpty=1

void
etk_combobox_entry_active_item_set(combobox_entry, item)
	Etk_Combobox_Entry *combobox_entry
	Etk_Combobox_Entry_Item *item
	ALIAS:
	ActiveItemSet=1

Etk_Combobox_Entry_Item *
etk_combobox_entry_active_item_get(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	ActiveItemGet=1

int
etk_combobox_entry_active_item_num_get(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	ActiveItemNumGet=1

Etk_Combobox_Entry_Item * 
etk_combobox_entry_first_item_get(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	FirstItemGet=1

Etk_Combobox_Entry_Item *
etk_combobox_entry_last_item_get(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	LastItemGet=1

Etk_Combobox_Entry_Item *
etk_combobox_entry_nth_item_get(combobox_entry, index)
	Etk_Combobox_Entry *combobox_entry
	int index
	ALIAS:
	NthItemGet=1

Etk_Bool
etk_combobox_entry_is_popped_up(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	IsPoppedUp=1

void
etk_combobox_entry_pop_up(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	PopUp=1

void
etk_combobox_entry_pop_down(combobox_entry)
	Etk_Combobox_Entry *combobox_entry
	ALIAS:
	PopDown=1

void
etk_combobox_entry_popup_feed(combobox_entry, window)
	Etk_Combobox_Entry *combobox_entry
	Etk_Window *window
	ALIAS:
	PopupFeed=1

MODULE = Etk::Combobox::Entry::Item	PACKAGE = Etk::Combobox::Entry::Item	PREFIX = etk_combobox_entry_item_

void
etk_combobox_entry_item_fields_set(item, ...)
	Etk_Combobox_Entry_Item * item
	ALIAS:
	FieldsSet=1
	CODE:
        int i;
        void **ptr = NULL;

        ptr = calloc(items, sizeof(void *));
        memset(ptr, 0, items * sizeof(void *));
	 for(i = 0; i < items - 1; i++)
           {
	      if(SvPOK(ST(i + 1)))
		   ptr[i] = SvPV_nolen(ST(i + 1));
	      else 
		   ptr[i] = SvObj(ST(i + 1), getClass("Etk_Widget"));
	   }
        switch(items)
        {	   
	   case 2:
	   etk_combobox_entry_item_fields_set(item, ptr[0]);
	   break;
	   case 3:
	   etk_combobox_entry_item_fields_set(item, ptr[0], ptr[1]);
	   break;
	   case 4:
	   etk_combobox_entry_item_fields_set(item, ptr[0], ptr[1], ptr[2]);
	   break;
	   case 5:
	   etk_combobox_entry_item_fields_set(item, ptr[0], ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   etk_combobox_entry_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4]);
	   break;
	   case 7:
	   etk_combobox_entry_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5]);
	   break;
	   case 8:
	   etk_combobox_entry_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4],
					     ptr[5], ptr[6]);
	   break;
	   case 9:
	   etk_combobox_entry_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7]);
	   break;
	   case 10:
	   etk_combobox_entry_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8]);
	   break;
	   case 11:
	   etk_combobox_entry_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8],
					     ptr[9]);
	   break;
	}
        if(ptr)
          free(ptr);
 

void
etk_combobox_entry_item_remove(item)
	Etk_Combobox_Entry_Item *item
	ALIAS:
	Remove=1

void
etk_combobox_entry_item_field_set(item, col, data)
	Etk_Combobox_Entry_Item * item
	int col
	SV * data
      ALIAS:
	FieldSet=1
	CODE:
 	if (SvPOK(data))
		etk_combobox_entry_item_field_set(item, col, (void *)SvPV_nolen(data));
 	else
		etk_combobox_entry_item_field_set(item, col, (void *)SvObj(data, "Etk::Widget"));

SV *
etk_combobox_entry_item_field_get(item, col, type=0)
	Etk_Combobox_Entry_Item * item
	int col
	int type
      ALIAS:
	FieldGet=1
	CODE:
	void * data;
	data = etk_combobox_entry_item_field_get(item, col);
 	if (type == 0)
		RETVAL = sv_2mortal(newSVpv((char *)data, 0));
 	else
 		RETVAL = sv_2mortal(newSVObject((Etk_Widget *)data));
	OUTPUT:
	RETVAL

void
etk_combobox_entry_item_data_set(item, data)
	Etk_Combobox_Entry_Item *item
	SV * data
	ALIAS:
	DataSet=1
	CODE:
	etk_combobox_entry_item_data_set(item, newSVsv(data));
	
SV *
etk_combobox_entry_item_data_get(item)
	Etk_Combobox_Entry_Item *item
	ALIAS:
	DataGet=1
	CODE:
	RETVAL = newSVsv((SV*)etk_combobox_entry_item_data_get(item));
	OUTPUT:
	RETVAL

Etk_Combobox_Entry_Item *
etk_combobox_entry_item_prev_get(item)
	Etk_Combobox_Entry_Item *item
	ALIAS:
	PrevGet=1

Etk_Combobox_Entry_Item *
etk_combobox_entry_item_next_get(item)
	Etk_Combobox_Entry_Item *item
	ALIAS:
	NextGet=1

Etk_Combobox_Entry *
etk_combobox_entry_item_combobox_entry_get(item)
	Etk_Combobox_Entry_Item *item
	ALIAS:
	ComboboxEntryGet=1
	
MODULE = Etk::Combobox::Item		PACKAGE = Etk::Combobox::Item		PREFIX = etk_combobox_item_

void
etk_combobox_item_fields_set(item, ...)
	Etk_Combobox_Item * item
	ALIAS:
	FieldsSet=1
	CODE:
        int i;
        void **ptr = NULL;

        ptr = calloc(items, sizeof(void *));
        memset(ptr, 0, items * sizeof(void *));
	 for(i = 0; i < items - 1; i++)
           {
	      if(SvPOK(ST(i + 1)))
		   ptr[i] = SvPV_nolen(ST(i + 1));
	      else 
		   ptr[i] = SvObj(ST(i + 1), getClass("Etk_Widget"));
	   }
        switch(items)
        {	   
	   case 2:
	   etk_combobox_item_fields_set(item, ptr[0]);
	   break;
	   case 3:
	   etk_combobox_item_fields_set(item, ptr[0], ptr[1]);
	   break;
	   case 4:
	   etk_combobox_item_fields_set(item, ptr[0], ptr[1], ptr[2]);
	   break;
	   case 5:
	   etk_combobox_item_fields_set(item, ptr[0], ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   etk_combobox_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4]);
	   break;
	   case 7:
	   etk_combobox_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5]);
	   break;
	   case 8:
	   etk_combobox_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4],
					     ptr[5], ptr[6]);
	   break;
	   case 9:
	   etk_combobox_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7]);
	   break;
	   case 10:
	   etk_combobox_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8]);
	   break;
	   case 11:
	   etk_combobox_item_fields_set(item, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8],
					     ptr[9]);
	   break;
	}
        if(ptr)
          free(ptr);
 


SV *
etk_combobox_item_data_get(item)
	Etk_Combobox_Item *	item
      ALIAS:
	DataGet=1
	CODE:
	RETVAL = newSVsv((SV*)etk_combobox_item_data_get(item));
	OUTPUT:
	RETVAL

Etk_Combobox *
etk_combobox_item_combobox_get(item)
	Etk_Combobox_Item *     item
	ALIAS:
	ComboboxGet=1

void
etk_combobox_item_data_set(item, data)
	Etk_Combobox_Item *	item
	SV *	data
      ALIAS:
	DataSet=1
      CODE:
        etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item), newSVsv(data));

void
etk_combobox_item_remove(item)
	Etk_Combobox_Item *	item
      ALIAS:
	Remove=1

void
etk_combobox_item_field_set(item, col, data)
	Etk_Combobox_Item * item
	int col
	SV * data
      ALIAS:
	FieldSet=1
	CODE:
 	if (SvPOK(data))
		etk_combobox_item_field_set(item, col, (void *)SvPV_nolen(data));
 	else
		etk_combobox_item_field_set(item, col, (void *)SvObj(data, "Etk::Widget"));

SV *
etk_combobox_item_field_get(item, col, type=0)
	Etk_Combobox_Item * item
	int col
	int type
      ALIAS:
	FieldGet=1
	CODE:
	void * data;
	data = etk_combobox_item_field_get(item, col);
 	if (type == 0)
		RETVAL = sv_2mortal(newSVpv((char *)data, 0));
 	else
 		RETVAL = sv_2mortal(newSVObject((Etk_Widget *)data));
	OUTPUT:
	RETVAL


MODULE = Etk::Combobox		PACKAGE = Etk::Combobox		PREFIX = etk_combobox_
	

Etk_Combobox *
new(class)
	SV	*class
	CODE:
	RETVAL = ETK_COMBOBOX(etk_combobox_new());
	OUTPUT:
	RETVAL

Etk_Combobox *
new_default()
      ALIAS:
	NewDefault=1
	CODE:
	RETVAL = ETK_COMBOBOX(etk_combobox_new_default());
	OUTPUT:
	RETVAL

void
etk_combobox_build(combobox)
	Etk_Combobox *	combobox
      ALIAS:
	Build=1

void
etk_combobox_clear(combobox)
	Etk_Combobox *	combobox
      ALIAS:
	Clear=1

void
etk_combobox_column_add(combobox, col_type, width, fill_policy, align)
	Etk_Combobox *	combobox
	Etk_Combobox_Column_Type	col_type
	int	width
	Etk_Combobox_Fill_Policy	fill_policy
	float	align
      ALIAS:
	ColumnAdd=1

void
etk_combobox_items_height_set(combobox, items_height)
	Etk_Combobox *	combobox
	int	items_height
      ALIAS:
	ItemsHeightSet=1

int
etk_combobox_items_height_get(combobox)
	Etk_Combobox *	combobox
      ALIAS:
	ItemsHeightGet=1

Etk_Combobox_Item *
etk_combobox_item_append_empty(combobox)
	Etk_Combobox * combobox
	ALIAS:
	ItemAppendEmpty=1

Etk_Combobox_Item *
etk_combobox_item_prepend_empty(combobox)
	Etk_Combobox * combobox
	ALIAS:
	ItemPrependEmpty=1

Etk_Combobox_Item *
etk_combobox_item_insert_empty(combobox, after)
	Etk_Combobox * combobox
	Etk_Combobox_Item * after
	ALIAS:
	ItemInsertEmpty=1

void
etk_combobox_active_item_set(combobox, item)
	Etk_Combobox * combobox
	Etk_Combobox_Item *     item
	ALIAS:
	ActiveItemSet=1

Etk_Combobox_Item *
etk_combobox_active_item_get(combobox)
       Etk_Combobox *  combobox
      ALIAS:
       ActiveItemGet=1

void
etk_combobox_active_item_num_get(combobox)
	Etk_Combobox * combobox
	ALIAS:
	ActiveItemNumGet=1

Etk_Combobox_Item *
etk_combobox_first_item_get(combobox)
	Etk_Combobox * combobox
	ALIAS:
	FirstItemGet=1

Etk_Combobox_Item *
etk_combobox_last_item_get(combobox)
	Etk_Combobox * combobox
	ALIAS:
	LastItemGet=1

Etk_Combobox_Item *
etk_combobox_item_prev_get(item)
	Etk_Combobox_Item *     item
	ALIAS:
	ItemPrevGet=1

Etk_Combobox_Item *
etk_combobox_item_next_get(item)
	Etk_Combobox_Item *     item
	ALIAS:
	ItemNextGet=1

Etk_Combobox_Item *
etk_combobox_nth_item_get(combobox, index)
	Etk_Combobox * combobox
	int	index
	ALIAS:
	NthItemGet=1



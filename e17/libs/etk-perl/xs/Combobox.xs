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


MODULE = Etk::Combobox::Item		PACKAGE = Etk::Combobox::Item		PREFIX = etk_combobox_item_

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

# void
# etk_combobox_item_col_set(item, col, data)
#	Etk_Combobox_Item * item
#	int col
#	SV * data
#      ALIAS:
#	ColSet=1
#	CODE:
# /	if (SvPOK(data))
#		etk_combobox_item_col_set(item, col, SvPV_nolen(data));
# /	else
#		etk_combobox_item_col_set(item, col, SvEtkWidgetPtr(data));
#
# SV *
# etk_combobox_item_col_get(item, col, type=0)
#	Etk_Combobox_Item * item
#	int col
#	int type
#      ALIAS:
#	ColGet=1
#	CODE:
#	void * data;
#	data = etk_combobox_item_col_get(item, col);
# /	if (type == 0)
#		RETVAL = sv_2mortal(newSVpv((char *)data, 0));
# /	else
# 		RETVAL = sv_2mortal(newSVEtkWidgetPtr((Etk_Widget *)data));
#	OUTPUT:
#	RETVAL




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
etk_combobox_item_append(combobox, ...)
        Etk_Combobox * combobox
      ALIAS:
	ItemAppend=1
    CODE:
        int i;
        void **ptr = NULL;

        ptr = calloc(items, sizeof(void *));
        memset(ptr, 0, items * sizeof(void *));
        /* the idea here is that we either have a max limit on how many items
	 * we can have in a combo, or we create "models" like the tree. lets
	 * see how well this will work.
	 */
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
	   RETVAL = etk_combobox_item_append(combobox, ptr[0]);
	   break;
	   case 3:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0], ptr[1]);
	   break;
	   case 4:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0], ptr[1], ptr[2]);
	   break;
	   case 5:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0], ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4]);
	   break;
	   case 7:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5]);
	   break;
	   case 8:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4],
					     ptr[5], ptr[6]);
	   break;
	   case 9:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7]);
	   break;
	   case 10:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8]);
	   break;
	   case 11:
	   RETVAL = etk_combobox_item_append(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8],
					     ptr[9]);
	   break;
	}
        if(ptr)
          free(ptr);
    OUTPUT:
        RETVAL
  
Etk_Combobox_Item *
etk_combobox_item_prepend(combobox, ...)
        Etk_Combobox * combobox
      ALIAS:
	ItemPrepend=1
    CODE:
        int i;
        void **ptr = NULL;

        ptr = calloc(items, sizeof(void *));
        memset(ptr, 0, items * sizeof(void *));
        /* the idea here is that we either have a max limit on how many items
	 * we can have in a combo, or we create "models" like the tree. lets
	 * see how well this will work.
	 */
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
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0]);
	   break;
	   case 3:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1]);
	   break;
	   case 4:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1], ptr[2]);
	   break;
	   case 5:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4]);
	   break;
	   case 7:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5]);
	   break;
	   case 8:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4],
					     ptr[5], ptr[6]);
	   break;
	   case 9:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7]);
	   break;
	   case 10:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8]);
	   break;
	   case 11:
	   RETVAL = etk_combobox_item_prepend(combobox, ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8],
					     ptr[9]);
	   break;
	}
        if(ptr)
          free(ptr);
    OUTPUT:
        RETVAL
  
Etk_Combobox_Item *
etk_combobox_item_insert(combobox, after, ...)
        Etk_Combobox * combobox
        Etk_Combobox_Item * after
      ALIAS:
	ItemInsert=1
    CODE:
        int i;
        void **ptr = NULL;

        ptr = calloc(items, sizeof(void *));
        memset(ptr, 0, items * sizeof(void *));
        /* the idea here is that we either have a max limit on how many items
	 * we can have in a combo, or we create "models" like the tree. lets
	 * see how well this will work.
	 */
	 for(i = 0; i < items - 2; i++)
           {
	      if(SvPOK(ST(i + 1)))
		   ptr[i] = SvPV_nolen(ST(i + 1));
	      else 
		   ptr[i] = SvObj(ST(i + 1), getClass("Etk_Widget"));
	   }
        switch(items)
        {	   
	   case 2:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0]);
	   break;
	   case 3:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1]);
	   break;
	   case 4:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1], ptr[2]);
	   break;
	   case 5:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1], ptr[2], ptr[3],
						       ptr[4]);
	   break;
	   case 7:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5]);
	   break;
	   case 8:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6]);
	   break;
	   case 9:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6],
						       ptr[7]);
	   break;
	   case 10:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6],
						       ptr[7], ptr[8]);
	   break;
	   case 11:
	   RETVAL = etk_combobox_item_insert(combobox, 
						       after, ptr[0], 
						       ptr[1], ptr[2], ptr[3], 
						       ptr[4], ptr[5], ptr[6], 
						       ptr[7], ptr[8], ptr[9]);
	   break;
	}
        if(ptr)
          free(ptr);
    OUTPUT:
        RETVAL
  
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



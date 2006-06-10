#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <Etk.h>
#include <Ecore.h>
#include <Ecore_Data.h>

#include "const-c.inc"

typedef struct _Callback_Signal_Data Callback_Signal_Data;
typedef struct _Callback_Tree_Compare_Data Callback_Tree_Compare_Data;
typedef struct _Callback_Timer_Data Callback_Timer_Data;

/* Carries info for normal signals */
struct _Callback_Signal_Data
{
   char       *signal_name;   /* etk signal name */
   Etk_Object *object;        /* object signal is connected to */
   SV         *perl_callback; /* perl callback to be called */
   void       *perl_data;     /* perl data to pass to the perl callback */
};

/* Carries info for tree (column) compare callback */
struct _Callback_Tree_Compare_Data
{
   Etk_Object *object;          /* the column */
   SV         *perl_callback;   /* perl function that does the comparison */
   SV         *perl_data;       /* data to be passed to the above function */
};

/* Carries info when we have a timer callback */
struct _Callback_Timer_Data
{
   SV *perl_callback; /* the perl callback to call per timer tick */
   SV *perl_data;     /* data to pass to the perl callback */
};

static void
callback_VOID__VOID(Etk_Object *object, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;
   
   PUSHMARK(SP);
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__INT(Etk_Object *object, int value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSViv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__DOUBLE(Etk_Object *object, double value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVnv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;

   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__POINTER(Etk_Object *object, void *value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   //XPUSHs(sv_2mortal(newSViv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__POINTER_POINTER(Etk_Object *object, void *val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   //XPUSHs(sv_2mortal(newSViv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__INT_POINTER(Etk_Object *object, int val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSViv(val1)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__VOID(Etk_Object *object, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__DOUBLE(Etk_Object *object, double value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVnv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__POINTER_POINTER(Etk_Object *object, void *val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   //XPUSHs(sv_2mortal(newSViv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

int
callback_timer(void *data)
{
   dSP;
   SV* cb;
   Callback_Timer_Data *cbd;
   int count;
   int ret = 0;
   
   cbd = data;   
   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));      
   PUTBACK ;  
   
   count = call_sv(cbd->perl_callback, G_SCALAR);

   SPAGAIN;

   /* if the return value is incorrect, return 0 to end timer */
   if(count != 1)
      croak("Improper return value from timer callback!\n");
   
   ret = POPi;
   
   PUTBACK;
      
   return ret;
}

int
tree_compare_cb( Etk_Tree * tree, Etk_Tree_Row * row1, Etk_Tree_Row *row2,
Etk_Tree_Col * col, void * data )
{
   dSP;
   Callback_Tree_Compare_Data *cbd;
   HV *row1_hv;
   HV *row2_hv;
   HV *col_hv;
   HV *tree_hv;
   SV *data_sv;
   SV *row1_rv;
   SV *row2_rv;
   SV *col_rv;
   SV *tree_rv;
   int count;
   int ret;   

   ENTER ;
   SAVETMPS;   
   
   cbd = data;
   
   tree_hv = (HV*)sv_2mortal((SV*)newHV());
   row1_hv = (HV*)sv_2mortal((SV*)newHV());
   row2_hv = (HV*)sv_2mortal((SV*)newHV());
   col_hv = (HV*)sv_2mortal((SV*)newHV());
          
   tree_rv = newRV(newSViv(0));
   sv_setref_iv(tree_rv, "Etk_WidgetPtr", (IV) tree);
   tree_hv = newHV();
   hv_store(tree_hv, "WIDGET", strlen("WIDGET"), tree_rv, 0);
   row1_rv = newRV(tree_hv);
   sv_bless(tree_rv, gv_stashpv("Etk::Tree", FALSE));   

   row1_rv = newRV(newSViv(0));
   sv_setref_iv(row1_rv, "Etk_Tree_RowPtr", (IV) row1);
   row1_hv = newHV();
   hv_store(row1_hv, "WIDGET", strlen("WIDGET"), row1_rv, 0);
   row1_rv = newRV(row1_hv);
   sv_bless(row1_rv, gv_stashpv("Etk::Tree::Row", FALSE));
   
   row2_rv = newRV(newSViv(0));
   sv_setref_iv(row2_rv, "Etk_Tree_RowPtr", (IV) row2);
   row2_hv = newHV();
   hv_store(row2_hv, "WIDGET", strlen("WIDGET"), row2_rv, 0);
   row2_rv = newRV(row2_hv);
   sv_bless(row2_rv, gv_stashpv("Etk::Tree::Row", FALSE));
   
   col_rv = newRV(newSViv(0));
   sv_setref_iv(col_rv, "Etk_Tree_ColPtr", (IV) col);
   col_hv = newHV();
   hv_store(col_hv, "WIDGET", strlen("WIDGET"), col_rv, 0);
   col_rv = newRV(col_hv);
   sv_bless(col_rv, gv_stashpv("Etk::Tree::Col", FALSE));   

   data_sv = newSVsv(cbd->perl_data);   
   
   PUSHMARK(SP);  	  
   XPUSHs(sv_2mortal(tree_rv));
   XPUSHs(sv_2mortal(row1_rv));
   XPUSHs(sv_2mortal(row2_rv));	
   XPUSHs(sv_2mortal(col_rv));
   XPUSHs(sv_2mortal(data_sv));   
   PUTBACK;

   count = call_sv(cbd->perl_callback, G_SCALAR);
   
   SPAGAIN;

   /* if the return value is incorrect, return 0 to end timer */
   if(count != 1)
       croak("Improper return value from timer callback!\n");

   ret = POPi;

   PUTBACK;
   FREETMPS ;
   LEAVE ; 
   
   return ret;
}


MODULE = Etk		PACKAGE = Etk		

INCLUDE: const-xs.inc

void
etk_accumulator_bool_and(return_value, value_to_accum, data)
	void *	return_value
	 void *	value_to_accum
	void *	data

void
etk_accumulator_bool_or(return_value, value_to_accum, data)
	void *	return_value
	 void *	value_to_accum
	void *	data

void
etk_accumulator_stopping_bool_and(return_value, value_to_accum, data)
	void *	return_value
	 void *	value_to_accum
	void *	data

void
etk_accumulator_stopping_bool_or(return_value, value_to_accum, data)
	void *	return_value
	 void *	value_to_accum
	void *	data

void
etk_alignment_get(alignment)
	Etk_Alignment *	alignment
      PPCODE:
	float xalign;
	float yalign;
	float xscale;
	float yscale;

	etk_alignment_get(ETK_ALIGNMENT(alignment), &xalign, &yalign,
	                  &xscale, &yscale);
        EXTEND(SP, 4);
        PUSHs(sv_2mortal(newSVnv(xalign)));
        PUSHs(sv_2mortal(newSVnv(yalign)));
        PUSHs(sv_2mortal(newSVnv(xscale)));
        PUSHs(sv_2mortal(newSVnv(yscale)));	

Etk_Widget *
etk_alignment_new(xalign, yalign, xscale, yscale)
	float	xalign
	float	yalign
	float	xscale
	float	yscale

void
etk_alignment_set(alignment, xalign, yalign, xscale, yscale)
	Etk_Alignment *	alignment
	float	xalign
	float	yalign
	float	xscale
	float	yscale

Etk_Type *
etk_alignment_type_get()

Evas_List *
etk_argument_extra_find(key)
	char *	key

void
etk_argument_help_show(args)
	Etk_Argument *	args

Etk_Bool
etk_argument_is_set(args, long_name, short_name)
	Etk_Argument *	args
	char *	long_name
	char	short_name

int
etk_arguments_parse(args, argc, argv)
	Etk_Argument *	args
	int	argc
	char **	argv

Etk_Widget *
etk_bin_child_get(bin)
	Etk_Bin *	bin

void
etk_bin_child_set(bin, child)
	Etk_Bin *	bin
	Etk_Widget *	child

Etk_Type *
etk_bin_type_get()
       
void
etk_box_child_packing_get(box, child)
	Etk_Widget *	box
	Etk_Widget *	child
     PPCODE:
       int 	        padding;
       Etk_Bool   	expand;
       Etk_Bool   	fill;
       Etk_Bool   	pack_end;
       
       etk_box_child_packing_get(box, child, &padding, &expand, &fill,
                                 &pack_end);
       EXTEND(SP, 4);
       PUSHs(sv_2mortal(newSViv(padding)));
       PUSHs(sv_2mortal(newSViv(expand)));
       PUSHs(sv_2mortal(newSViv(fill)));
       PUSHs(sv_2mortal(newSViv(pack_end)));

void
etk_box_child_packing_set(box, child, padding, expand, fill, pack_end)
	Etk_Widget *	box
	Etk_Widget *	child
	int	padding
	Etk_Bool	expand
	Etk_Bool	fill
	Etk_Bool	pack_end

Etk_Bool
etk_box_homogeneous_get(box)
	Etk_Widget *	box

void
etk_box_homogeneous_set(box, homogeneous)
	Etk_Widget *	box
	Etk_Bool	homogeneous

void
etk_box_pack_end(box, child, expand, fill, padding)
	Etk_Widget *	box
	Etk_Widget *	child
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding

void
etk_box_pack_start(box, child, expand, fill, padding)
	Etk_Widget *	box
	Etk_Widget *	child
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding

int
etk_box_spacing_get(box)
	Etk_Widget *	box

void
etk_box_spacing_set(box, spacing)
	Etk_Widget *	box
	int	spacing

Etk_Type *
etk_box_type_get()

void
etk_button_alignment_get(button)
	Etk_Widget *	button
      PPCODE:	
       float xalign;
       float yalign;
       
       etk_button_alignment_get(ETK_BUTTON(button), &xalign, &yalign);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSVnv(xalign)));
       PUSHs(sv_2mortal(newSVnv(yalign)));

void
etk_button_alignment_set(button, xalign, yalign)
	Etk_Widget *	button
	float	xalign
	float	yalign

void
etk_button_click(button)
	Etk_Widget *	button

Etk_Image *
etk_button_image_get(button)
	Etk_Widget *	button

void
etk_button_image_set(button, image)
	Etk_Widget *	button
	Etk_Widget *	image

char *
etk_button_label_get(button)
	Etk_Widget *	button

void
etk_button_label_set(button, label)
	Etk_Widget *	button
	char *	label

Etk_Widget *
etk_button_new()

Etk_Widget *
etk_button_new_from_stock(stock_id)
	Etk_Stock_Id	stock_id

Etk_Widget *
etk_button_new_with_label(label)
	char *	label

void
etk_button_press(button)
	Etk_Widget *	button

void
etk_button_release(button)
	Etk_Widget *	button

void
etk_button_set_from_stock(button, stock_id)
	Etk_Widget *	button
	Etk_Stock_Id	stock_id

Etk_Type *
etk_button_type_get()

void
etk_cache_edje_object_add(edje_object)
	Evas_Object *	edje_object

void
etk_cache_edje_object_add_with_state(edje_object, state)
	Evas_Object *	edje_object
	int	state

Evas_Object *
etk_cache_edje_object_find(evas, filename, group)
	Evas *	evas
	char *	filename
	char *	group

Evas_Object *
etk_cache_edje_object_find_with_state(evas, filename, group, state)
	Evas *	evas
	char *	filename
	char *	group
	int	state

void
etk_cache_edje_object_remove(edje_object)
	Evas_Object *	edje_object

int
etk_cache_edje_object_size_get(evas)
	Evas *	evas

void
etk_cache_edje_object_size_set(evas, size)
	Evas *	evas
	int	size

void
etk_cache_empty(evas)
	Evas *	evas

void
etk_cache_image_object_add(image_object)
	Evas_Object *	image_object

Evas_Object *
etk_cache_image_object_find(evas, filename)
	Evas *	evas
	char *	filename

void
etk_cache_image_object_remove(image_object)
	Evas_Object *	image_object

int
etk_cache_image_object_size_get(evas)
	Evas *	evas

void
etk_cache_image_object_size_set(evas, size)
	Evas *	evas
	int	size

void
etk_cache_shutdown()

Etk_Widget *
etk_canvas_new()

Etk_Bool
etk_canvas_object_add(canvas, object)
	Etk_Widget *	canvas
	Evas_Object *	object

void
etk_canvas_object_remove(canvas, object)
	Etk_Widget *	canvas
	Evas_Object *	object

Etk_Type *
etk_canvas_type_get()

Etk_Widget *
etk_check_button_new()

Etk_Widget *
etk_check_button_new_with_label(label)
	char *	label

Etk_Type *
etk_check_button_type_get()

void
etk_clipboard_text_request(widget)
	Etk_Widget *	widget

void
etk_clipboard_text_set(widget, data, length)
	Etk_Widget *	widget
	char *	data
	int	length

Etk_Color
etk_colorpicker_current_color_get(cp)
	Etk_Widget *	cp

void
etk_colorpicker_current_color_set(cp, color)
	Etk_Widget *	cp
	Etk_Color	color

Etk_Colorpicker_Mode
etk_colorpicker_mode_get(cp)
	Etk_Widget *	cp

void
etk_colorpicker_mode_set(cp, mode)
	Etk_Widget *	cp
	Etk_Colorpicker_Mode	mode

Etk_Widget *
etk_colorpicker_new()

Etk_Type *
etk_colorpicker_type_get()

Etk_Combobox_Item *
etk_combobox_active_item_get(combobox)
	Etk_Widget *	combobox

void
etk_combobox_active_item_set(combobox, item)
	Etk_Widget *	combobox
	Etk_Combobox_Item *	item

void
etk_combobox_build(combobox)
	Etk_Widget *	combobox

void
etk_combobox_clear(combobox)
	Etk_Widget *	combobox

void
etk_combobox_column_add(combobox, col_type, size, expand, hfill, vfill, xalign, yalign)
	Etk_Widget *	combobox
	Etk_Combobox_Column_Type	col_type
	int	size
	Etk_Bool	expand
	Etk_Bool	hfill
	Etk_Bool	vfill
	float	xalign
	float	yalign

void
etk_combobox_item_activate(item)
	Etk_Combobox_Item *	item

Etk_Combobox_Item *
etk_combobox_item_append(combobox, ...)
	Etk_Widget *	combobox

Etk_Combobox_Item *
etk_combobox_item_append_relative(combobox, relative, ...)
	Etk_Widget *	combobox
	Etk_Combobox_Item *	relative

void *
etk_combobox_item_data_get(item)
	Etk_Combobox_Item *	item

void
etk_combobox_item_data_set(item, data)
	Etk_Combobox_Item *	item
	void *	data

void
etk_combobox_item_data_set_full(item, data, free_cb)
	Etk_Combobox_Item *	item
	void *	data
	void ( * ) ( void * data ) free_cb

int
etk_combobox_item_height_get(combobox)
	Etk_Widget *	combobox

void
etk_combobox_item_height_set(combobox, item_height)
	Etk_Widget *	combobox
	int	item_height

Etk_Combobox_Item *
etk_combobox_item_prepend(combobox, ...)
	Etk_Widget *	combobox

Etk_Combobox_Item *
etk_combobox_item_prepend_relative(combobox, relative, ...)
	Etk_Widget *	combobox
	Etk_Combobox_Item *	relative

void
etk_combobox_item_remove(combobox, item)
	Etk_Widget *	combobox
	Etk_Combobox_Item *	item

Etk_Type *
etk_combobox_item_type_get()

Etk_Widget *
etk_combobox_new()

Etk_Widget *
etk_combobox_new_default()

Etk_Combobox_Item *
etk_combobox_nth_item_get(combobox, index)
	Etk_Widget *	combobox
	int	index

Etk_Type *
etk_combobox_type_get()

void
etk_container_add(container, widget)
	Etk_Widget *	container
	Etk_Widget *	widget

int
etk_container_border_width_get(container)
	Etk_Widget *	container

void
etk_container_border_width_set(container, border_width)
	Etk_Widget *	container
	int	border_width

void
etk_container_child_space_fill(child, child_space, hfill, vfill, xalign, yalign)
	Etk_Widget *	child
	Etk_Geometry *	child_space
	Etk_Bool	hfill
	Etk_Bool	vfill
	float	xalign
	float	yalign

Evas_List *
etk_container_children_get(container)
	Etk_Widget *	container

void
etk_container_for_each(container, for_each_cb)
	Etk_Widget *	container
	void ( * ) ( Etk_Widget * child ) for_each_cb

void
etk_container_for_each_data(container, for_each_cb, data)
	Etk_Widget *	container
	void ( * ) ( Etk_Widget * child, void * data ) for_each_cb
	void *	data

Etk_Bool
etk_container_is_child(container, widget)
	Etk_Widget *	container
	Etk_Widget *	widget

void
etk_container_remove(container, widget)
	Etk_Widget *	container
	Etk_Widget *	widget

Etk_Type *
etk_container_type_get()

Etk_Widget *
etk_dialog_button_add(dialog, label, response_id)
	Etk_Dialog *	dialog
	char *	label
	int	response_id

Etk_Widget *
etk_dialog_button_add_from_stock(dialog, stock_id, response_id)
	Etk_Dialog *	dialog
	int	stock_id
	int	response_id

Etk_Bool
etk_dialog_has_separator_get(dialog)
	Etk_Dialog *	dialog

void
etk_dialog_has_separator_set(dialog, has_separator)
	Etk_Dialog *	dialog
	Etk_Bool	has_separator

Etk_Widget *
etk_dialog_new()

void
etk_dialog_pack_button_in_action_area(dialog, button, response_id, expand, fill, padding, pack_at_end)
	Etk_Dialog *	dialog
	Etk_Widget *	button
	int	response_id
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding
	Etk_Bool	pack_at_end

void
etk_dialog_pack_in_main_area(dialog, widget, expand, fill, padding, pack_at_end)
	Etk_Dialog *	dialog
	Etk_Widget *	widget
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding
	Etk_Bool	pack_at_end

void
etk_dialog_pack_widget_in_action_area(dialog, widget, expand, fill, padding, pack_at_end)
	Etk_Dialog *	dialog
	Etk_Widget *	widget
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding
	Etk_Bool	pack_at_end

Etk_Type *
etk_dialog_type_get()

Etk_Bool
etk_dnd_init()

void
etk_dnd_shutdown()

void
etk_drag_begin(drag)
	Etk_Drag *	drag

void
etk_drag_data_set(drag, data, size)
	Etk_Drag *	drag
	void *	data
	int	size

Etk_Widget *
etk_drag_new(widget)
	Etk_Widget *	widget

Etk_Widget *
etk_drag_parent_widget_get(drag)
	Etk_Drag *	drag

void
etk_drag_parent_widget_set(drag, widget)
	Etk_Drag *	drag
	Etk_Widget *	widget

Etk_Type *
etk_drag_type_get()

void
etk_drag_types_set(drag, types, num_types)
	Etk_Drag *	drag
	char **	types
	unsigned int	num_types

Evas_Object *
etk_editable_text_object_add(evas)
	Evas *	evas

void
etk_editable_text_object_cursor_hide(object)
	Evas_Object *	object

void
etk_editable_text_object_cursor_move_at_end(object)
	Evas_Object *	object

void
etk_editable_text_object_cursor_move_at_start(object)
	Evas_Object *	object

void
etk_editable_text_object_cursor_move_left(object)
	Evas_Object *	object

void
etk_editable_text_object_cursor_move_right(object)
	Evas_Object *	object

void
etk_editable_text_object_cursor_show(object)
	Evas_Object *	object

Etk_Bool
etk_editable_text_object_delete_char_after(object)
	Evas_Object *	object

Etk_Bool
etk_editable_text_object_delete_char_before(object)
	Evas_Object *	object

Etk_Bool
etk_editable_text_object_insert(object, text)
	Evas_Object *	object
	char *	text

char *
etk_editable_text_object_text_get(object)
	Evas_Object *	object

void
etk_editable_text_object_text_set(object, text)
	Evas_Object *	object
	char *	text

Etk_Widget *
etk_entry_new()

Etk_Bool
etk_entry_password_get(entry)
	Etk_Widget *	entry

void
etk_entry_password_set(entry, on)
	Etk_Widget *	entry
	Etk_Bool	on

char *
etk_entry_text_get(entry)
	Etk_Widget *	entry

void
etk_entry_text_set(entry, text)
	Etk_Widget *	entry
	char *	text

Etk_Type *
etk_entry_type_get()

char *
etk_filechooser_widget_current_folder_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget

void
etk_filechooser_widget_current_folder_set(filechooser_widget, folder)
	Etk_Filechooser_Widget *	filechooser_widget
	char *	folder

Etk_Widget *
etk_filechooser_widget_new()

Etk_Bool
etk_filechooser_widget_select_multiple_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget

void
etk_filechooser_widget_select_multiple_set(filechooser_widget, select_multiple)
	Etk_Filechooser_Widget *	filechooser_widget
	Etk_Bool	select_multiple

char *
etk_filechooser_widget_selected_file_get(widget)
	Etk_Filechooser_Widget *	widget

Evas_List *
etk_filechooser_widget_selected_files_get(widget)
	Etk_Filechooser_Widget *	widget

Etk_Bool
etk_filechooser_widget_show_hidden_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget

void
etk_filechooser_widget_show_hidden_set(filechooser_widget, show_hidden)
	Etk_Filechooser_Widget *	filechooser_widget
	Etk_Bool	show_hidden

Etk_Type *
etk_filechooser_widget_type_get()

char *
etk_frame_label_get(frame)
	Etk_Widget *	frame

void
etk_frame_label_set(frame, label)
	Etk_Widget *	frame
	char *	label

Etk_Widget *
etk_frame_new(label)
	char *	label

Etk_Type *
etk_frame_type_get()

Etk_Widget *
etk_hbox_new(homogeneous, spacing)
	Etk_Bool	homogeneous
	int	spacing

Etk_Type *
etk_hbox_type_get()

Etk_Widget *
etk_hpaned_new()

Etk_Type *
etk_hpaned_type_get()

Etk_Widget *
etk_hscrollbar_new(lower, upper, value, step_increment, page_increment, page_size)
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	double	page_size

Etk_Type *
etk_hscrollbar_type_get()

Etk_Widget *
etk_hseparator_new()

Etk_Type *
etk_hseparator_type_get()

Etk_Widget *
etk_hslider_new(lower, upper, value, step_increment, page_increment)
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment

Etk_Type *
etk_hslider_type_get()

Etk_Iconbox_Icon *
etk_iconbox_append(iconbox, filename, edje_group, label)
	Etk_Iconbox *	iconbox
	char *	filename
	char *	edje_group
	char *	label

void
etk_iconbox_clear(iconbox)
	Etk_Iconbox *	iconbox

Etk_Iconbox_Model *
etk_iconbox_current_model_get(iconbox)
	Etk_Iconbox *	iconbox

void
etk_iconbox_current_model_set(iconbox, model)
	Etk_Iconbox *	iconbox
	Etk_Iconbox_Model *	model

void
etk_iconbox_freeze(iconbox)
	Etk_Iconbox *	iconbox

void *
etk_iconbox_icon_data_get(icon)
	Etk_Iconbox_Icon *	icon

void
etk_iconbox_icon_data_set(icon, data)
	Etk_Iconbox_Icon *	icon
	void *	data

void
etk_iconbox_icon_data_set_full(icon, data, free_cb)
	Etk_Iconbox_Icon *	icon
	void *	data
	void ( * ) ( void * data ) free_cb

void
etk_iconbox_icon_del(icon)
	Etk_Iconbox_Icon *	icon

void
etk_iconbox_icon_file_get(icon)
	Etk_Iconbox_Icon *	icon
	
      PPCODE:
       char *filename;
       char *edje_group;

       etk_iconbox_icon_file_get(icon, &filename, &edje_group);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSVpv(filename, strlen(filename))));
       PUSHs(sv_2mortal(newSVpv(edje_group, strlen(edje_group))));

void
etk_iconbox_icon_file_set(icon, filename, edje_group)
	Etk_Iconbox_Icon *	icon
	char *	filename
	char *	edje_group

Etk_Iconbox_Icon *
etk_iconbox_icon_get_at_xy(iconbox, x, y, over_cell, over_icon, over_label)
	Etk_Iconbox *	iconbox
	int	x
	int	y
	Etk_Bool	over_cell
	Etk_Bool	over_icon
	Etk_Bool	over_label

char *
etk_iconbox_icon_label_get(icon)
	Etk_Iconbox_Icon *	icon

void
etk_iconbox_icon_label_set(icon, label)
	Etk_Iconbox_Icon *	icon
	char *	label

void
etk_iconbox_icon_select(icon)
	Etk_Iconbox_Icon *	icon

void
etk_iconbox_icon_unselect(icon)
	Etk_Iconbox_Icon *	icon

Etk_Bool
etk_iconbox_is_selected(icon)
	Etk_Iconbox_Icon *	icon

void
etk_iconbox_model_free(model)
	Etk_Iconbox_Model *	model

void
etk_iconbox_model_geometry_get(model)
	Etk_Iconbox_Model *	model
	
     PPCODE:
       int width;
       int height;
       int xpadding;
       int ypadding;

       etk_iconbox_model_geometry_get(model, &width, &height, &xpadding,
                                      &ypadding);
       EXTEND(SP, 4);
       PUSHs(sv_2mortal(newSViv(width)));
       PUSHs(sv_2mortal(newSViv(height)));
       PUSHs(sv_2mortal(newSViv(xpadding)));
       PUSHs(sv_2mortal(newSViv(ypadding)));       

void
etk_iconbox_model_geometry_set(model, width, height, xpadding, ypadding)
	Etk_Iconbox_Model *	model
	int	width
	int	height
	int	xpadding
	int	ypadding

void
etk_iconbox_model_icon_geometry_get(model)
	Etk_Iconbox_Model *	model
      PPCODE:
	int x;
	int y;
	int width;
	int height;
	Etk_Bool fill;
	Etk_Bool keep_aspect_ratio;

	etk_iconbox_model_icon_geometry(model, &x, &y, &width, &height,
					&fill, &keep_aspect_ratio);
        EXTEND(SP, 6);
        PUSHs(sv_2mortal(newSViv(x)));
        PUSHs(sv_2mortal(newSViv(y)));
        PUSHs(sv_2mortal(newSViv(width)));
        PUSHs(sv_2mortal(newSViv(height)));
        PUSHs(sv_2mortal(newSViv(fill)));
        PUSHs(sv_2mortal(newSViv(keep_aspect_ratio)));	

void
etk_iconbox_model_icon_geometry_set(model, x, y, width, height, fill, keep_aspect_ratio)
	Etk_Iconbox_Model *	model
	int	x
	int	y
	int	width
	int	height
	Etk_Bool	fill
	Etk_Bool	keep_aspect_ratio

void
etk_iconbox_model_label_geometry_get(model, x, y, width, height, xalign, yalign)
	Etk_Iconbox_Model *	model
	int *	x
	int *	y
	int *	width
	int *	height
	float *	xalign
	float *	yalign

void
etk_iconbox_model_label_geometry_set(model, x, y, width, height, xalign, yalign)
	Etk_Iconbox_Model *	model
	int	x
	int	y
	int	width
	int	height
	float	xalign
	float	yalign

Etk_Iconbox_Model *
etk_iconbox_model_new(iconbox)
	Etk_Iconbox *	iconbox

Etk_Widget *
etk_iconbox_new()

void
etk_iconbox_select_all(iconbox)
	Etk_Iconbox *	iconbox

void
etk_iconbox_thaw(iconbox)
	Etk_Iconbox *	iconbox

Etk_Type *
etk_iconbox_type_get()

void
etk_iconbox_unselect_all(iconbox)
	Etk_Iconbox *	iconbox

void
etk_image_copy(dest_image, src_image)
	Etk_Widget *	dest_image
	Etk_Widget *	src_image

void
etk_image_edje_get(image, edje_filename, edje_group)
	Etk_Widget *	image
	char **	edje_filename
	char **	edje_group

char *
etk_image_file_get(image)
	Etk_Widget *	image

Etk_Bool
etk_image_keep_aspect_get(image)
	Etk_Widget *	image

void
etk_image_keep_aspect_set(image, keep_aspect)
	Etk_Widget *	image
	Etk_Bool	keep_aspect

Etk_Widget *
etk_image_new()

Etk_Widget *
etk_image_new_from_edje(edje_filename, edje_group)
	char *	edje_filename
	char *	edje_group

Etk_Widget *
etk_image_new_from_file(filename)
	char *	filename

Etk_Widget *
etk_image_new_from_stock(stock_id, stock_size)
	Etk_Stock_Id	stock_id
	Etk_Stock_Size	stock_size

void
etk_image_set_from_edje(image, edje_filename, edje_group)
	Etk_Widget *	image
	char *	edje_filename
	char *	edje_group

void
etk_image_set_from_file(image, filename)
	Etk_Widget *	image
	char *	filename

void
etk_image_set_from_stock(image, stock_id, stock_size)
	Etk_Widget *	image
	Etk_Stock_Id	stock_id
	Etk_Stock_Size	stock_size

void
etk_image_size_get(image, width, height)
	Etk_Widget *	image
	int *	width
	int *	height

void
etk_image_stock_get(image, stock_id, stock_size)
	Etk_Widget *	image
	Etk_Stock_Id *	stock_id
	Etk_Stock_Size *	stock_size

Etk_Type *
etk_image_type_get()

Etk_Bool
etk_init()

Etk_Bool
Init()
      CODE:
        RETVAL = etk_init();
      OUTPUT:
        RETVAL	

void
etk_label_alignment_get(label, xalign, yalign)
	Etk_Widget *	label
	float *	xalign
	float *	yalign

void
etk_label_alignment_set(label, xalign, yalign)
	Etk_Widget *	label
	float	xalign
	float	yalign

char *
etk_label_get(label)
	Etk_Widget *	label

Etk_Widget *
etk_label_new(text)
	char *	text

void
etk_label_set(label, text)
	Etk_Widget *	label
	char *	text

Etk_Type *
etk_label_type_get()

void
etk_main()

void
etk_main_iterate()

void
etk_main_iteration_queue()

void
etk_main_quit()

void
etk_main_toplevel_widget_add(widget)
	Etk_Toplevel_Widget *	widget

void
etk_main_toplevel_widget_remove(widget)
	Etk_Toplevel_Widget *	widget

Evas_List *
etk_main_toplevel_widgets_get()

Etk_Widget *
etk_menu_bar_new()

Etk_Type *
etk_menu_bar_type_get()

void
etk_menu_item_activate(menu_item)
	Etk_Menu_Item *	menu_item

Etk_Bool
etk_menu_item_check_active_get(check_item)
	Etk_Menu_Item_Check *	check_item

void
etk_menu_item_check_active_set(check_item, active)
	Etk_Menu_Item_Check *	check_item
	Etk_Bool	active

Etk_Widget *
etk_menu_item_check_new()

Etk_Widget *
etk_menu_item_check_new_with_label(label)
	char *	label

Etk_Type *
etk_menu_item_check_type_get()

void
etk_menu_item_deselect(menu_item)
	Etk_Menu_Item *	menu_item

Etk_Widget *
etk_menu_item_image_new()

Etk_Widget *
etk_menu_item_image_new_from_stock(stock_id)
	Etk_Stock_Id	stock_id

Etk_Widget *
etk_menu_item_image_new_with_label(label)
	char *	label

void
etk_menu_item_image_set(image_item, image)
	Etk_Menu_Item_Image *	image_item
	Etk_Widget *	image

Etk_Type *
etk_menu_item_image_type_get()

char *
etk_menu_item_label_get(menu_item)
	Etk_Menu_Item *	menu_item

void
etk_menu_item_label_set(menu_item, label)
	Etk_Menu_Item *	menu_item
	char *	label

Etk_Widget *
etk_menu_item_new()

Etk_Widget *
etk_menu_item_new_from_stock(stock_id)
	Etk_Stock_Id	stock_id

Etk_Widget *
etk_menu_item_new_with_label(label)
	char *	label

Evas_List **
etk_menu_item_radio_group_get(radio_item)
	Etk_Menu_Item_Radio *	radio_item

void
etk_menu_item_radio_group_set(radio_item, group)
	Etk_Menu_Item_Radio *	radio_item
	Evas_List **	group

Etk_Widget *
etk_menu_item_radio_new(group)
	Evas_List **	group

Etk_Widget *
etk_menu_item_radio_new_from_widget(radio_item)
	Etk_Menu_Item_Radio *	radio_item

Etk_Widget *
etk_menu_item_radio_new_with_label(label, group)
	char *	label
	Evas_List **	group

Etk_Widget *
etk_menu_item_radio_new_with_label_from_widget(label, radio_item)
	char *	label
	Etk_Menu_Item_Radio *	radio_item

Etk_Type *
etk_menu_item_radio_type_get()

void
etk_menu_item_select(menu_item)
	Etk_Menu_Item *	menu_item

Etk_Widget *
etk_menu_item_separator_new()

Etk_Type *
etk_menu_item_separator_type_get()

void
etk_menu_item_set_from_stock(menu_item, stock_id)
	Etk_Menu_Item *	menu_item
	Etk_Stock_Id	stock_id

void
etk_menu_item_submenu_set(menu_item, submenu)
	Etk_Menu_Item *	menu_item
	Etk_Menu *	submenu

Etk_Type *
etk_menu_item_type_get()

Etk_Widget *
etk_menu_new()

void
etk_menu_popdown(menu)
	Etk_Menu *	menu

void
etk_menu_popup(menu)
	Etk_Menu *	menu

void
etk_menu_popup_at_xy(menu, x, y)
	Etk_Menu *	menu
	int	x
	int	y

void
etk_menu_shell_append(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item

void
etk_menu_shell_append_relative(menu_shell, item, relative)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	Etk_Menu_Item *	relative

void
etk_menu_shell_insert(menu_shell, item, position)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	int	position

Evas_List *
etk_menu_shell_items_get(menu_shell)
	Etk_Menu_Shell *	menu_shell

void
etk_menu_shell_prepend(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item

void
etk_menu_shell_prepend_relative(menu_shell, item, relative)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	Etk_Menu_Item *	relative

void
etk_menu_shell_remove(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item

Etk_Type *
etk_menu_shell_type_get()

Etk_Type *
etk_menu_type_get()

Etk_Message_Dialog_Buttons
etk_message_dialog_buttons_get(dialog)
	Etk_Message_Dialog *	dialog

void
etk_message_dialog_buttons_set(dialog, buttons)
	Etk_Message_Dialog *	dialog
	Etk_Message_Dialog_Buttons	buttons

Etk_Message_Dialog_Type
etk_message_dialog_message_type_get(dialog)
	Etk_Message_Dialog *	dialog

void
etk_message_dialog_message_type_set(dialog, type)
	Etk_Message_Dialog *	dialog
	Etk_Message_Dialog_Type	type

Etk_Widget *
etk_message_dialog_new(message_type, buttons, text)
	Etk_Message_Dialog_Type	message_type
	Etk_Message_Dialog_Buttons	buttons
	char *	text

char *
etk_message_dialog_text_get(dialog)
	Etk_Message_Dialog *	dialog

void
etk_message_dialog_text_set(dialog, text)
	Etk_Message_Dialog *	dialog
	char *	text

Etk_Type *
etk_message_dialog_type_get()

int
etk_notebook_current_page_get(notebook)
	Etk_Notebook *	notebook

void
etk_notebook_current_page_set(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num

Etk_Widget *
etk_notebook_new()

int
etk_notebook_num_pages_get(notebook)
	Etk_Notebook *	notebook

int
etk_notebook_page_append(notebook, tab_label, page_child)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child

Etk_Widget *
etk_notebook_page_child_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num

void
etk_notebook_page_child_set(notebook, page_num, child)
	Etk_Notebook *	notebook
	int	page_num
	Etk_Widget *	child

int
etk_notebook_page_index_get(notebook, child)
	Etk_Notebook *	notebook
	Etk_Widget *	child

int
etk_notebook_page_insert(notebook, tab_label, page_child, position)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child
	int	position

int
etk_notebook_page_next(notebook)
	Etk_Notebook *	notebook

int
etk_notebook_page_prepend(notebook, tab_label, page_child)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child

int
etk_notebook_page_prev(notebook)
	Etk_Notebook *	notebook

void
etk_notebook_page_remove(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num

char *
etk_notebook_page_tab_label_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num

void
etk_notebook_page_tab_label_set(notebook, page_num, tab_label)
	Etk_Notebook *	notebook
	int	page_num
	char *	tab_label

Etk_Widget *
etk_notebook_page_tab_widget_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num

void
etk_notebook_page_tab_widget_set(notebook, page_num, tab_widget)
	Etk_Notebook *	notebook
	int	page_num
	Etk_Widget *	tab_widget

Etk_Type *
etk_notebook_type_get()

Etk_Object *
etk_object_check_cast(object, type)
	Etk_Object *	object
	Etk_Type *	type

void *
etk_object_data_get(object, key)
	Etk_Object *	object
	char *	key

void
etk_object_data_set(object, key, value)
	Etk_Object *	object
	char *	key
	void *	value

void
etk_object_data_set_full(object, key, value, free_cb)
	Etk_Object *	object
	char *	key
	void *	value
	void ( * ) ( void * data ) free_cb

void
etk_object_destroy(object)
	Etk_Object *	object

void
etk_object_destroy_all_objects()

Etk_Object *
etk_object_new(object_type, first_property, ...)
	Etk_Type *	object_type
	char *	first_property

void
etk_object_notification_callback_add(object, property_name, callback, data)
	Etk_Object *	object
	char *	property_name
	void ( * ) ( Etk_Object * object, char * property_name, void * data ) callback
	void *	data

void
etk_object_notification_callback_remove(object, property_name, callback)
	Etk_Object *	object
	char *	property_name
	void ( * ) ( Etk_Object * object, char * property_name, void * data ) callback

void
etk_object_notify(object, property_name)
	Etk_Object *	object
	char *	property_name

Etk_Type *
etk_object_object_type_get(object)
	Etk_Object *	object

void
etk_object_properties_get(object, first_property, ...)
	Etk_Object *	object
	char *	first_property

void
etk_object_properties_set(object, first_property, ...)
	Etk_Object *	object
	char *	first_property

void
etk_object_property_reset(object, property_name)
	Etk_Object *	object
	char *	property_name

void
etk_object_signal_callback_add(object, signal_callback, after)
	Etk_Object *	object
	Etk_Signal_Callback *	signal_callback
	Etk_Bool	after

void
etk_object_signal_callback_remove(object, signal_callback)
	Etk_Object *	object
	Etk_Signal_Callback *	signal_callback

void
etk_object_signal_callbacks_get(object, signal, callbacks, after)
	Etk_Object *	object
	Etk_Signal *	signal
	Evas_List **	callbacks
	Etk_Bool	after

Etk_Type *
etk_object_type_get()

void
etk_object_weak_pointer_add(object, pointer_location)
	Etk_Object *	object
	void **	pointer_location

void
etk_object_weak_pointer_remove(object, pointer_location)
	Etk_Object *	object
	void **	pointer_location

Etk_Widget *
etk_paned_child1_get(paned)
	Etk_Widget *	paned

void
etk_paned_child1_set(paned, child, expand)
	Etk_Widget *	paned
	Etk_Widget *	child
	Etk_Bool	expand

Etk_Widget *
etk_paned_child2_get(paned)
	Etk_Widget *	paned

void
etk_paned_child2_set(paned, child, expand)
	Etk_Widget *	paned
	Etk_Widget *	child
	Etk_Bool	expand

int
etk_paned_position_get(paned)
	Etk_Widget *	paned

void
etk_paned_position_set(paned, position)
	Etk_Widget *	paned
	int	position

Etk_Type *
etk_paned_type_get()

Etk_Widget *
etk_popup_window_focused_window_get()

void
etk_popup_window_focused_window_set(popup_window)
	Etk_Widget *	popup_window

Etk_Bool
etk_popup_window_is_popped_up(popup_window)
	Etk_Widget *	popup_window

void
etk_popup_window_popdown(popup_window)
	Etk_Widget *	popup_window

void
etk_popup_window_popdown_all()

void
etk_popup_window_popup(popup_window)
	Etk_Widget *	popup_window

void
etk_popup_window_popup_at_xy(popup_window, x, y)
	Etk_Widget *	popup_window
	int	x
	int	y

Etk_Type *
etk_popup_window_type_get()

double
etk_progress_bar_fraction_get(progress_bar)
	Etk_Widget *	progress_bar

void
etk_progress_bar_fraction_set(progress_bar, fraction)
	Etk_Widget *	progress_bar
	double	fraction

Etk_Widget *
etk_progress_bar_new()

Etk_Widget *
etk_progress_bar_new_with_text(label)
	char *	label

void
etk_progress_bar_pulse(progress_bar)
	Etk_Widget *	progress_bar

double
etk_progress_bar_pulse_step_get(progress_bar)
	Etk_Widget *	progress_bar

void
etk_progress_bar_pulse_step_set(progress_bar, pulse_step)
	Etk_Widget *	progress_bar
	double	pulse_step

char *
etk_progress_bar_text_get(progress_bar)
	Etk_Widget *	progress_bar

void
etk_progress_bar_text_set(progress_bar, label)
	Etk_Widget *	progress_bar
	char *	label

Etk_Type *
etk_progress_bar_type_get()

Etk_Bool
etk_property_default_value_set(property, default_value)
	Etk_Property *	property
	Etk_Property_Value *	default_value

void
etk_property_delete(property)
	Etk_Property *	property

Etk_Property *
etk_property_new(name, property_id, type, flags, default_value)
	char *	name
	int	property_id
	Etk_Property_Type	type
	Etk_Property_Flags	flags
	Etk_Property_Value *	default_value

Etk_Property_Type
etk_property_type_get(property)
	Etk_Property *	property

Etk_Property_Value *
etk_property_value_bool(value)
	Etk_Bool	value

Etk_Bool
etk_property_value_bool_get(value)
	Etk_Property_Value *	value

void
etk_property_value_bool_set(property_value, value)
	Etk_Property_Value *	property_value
	Etk_Bool	value

Etk_Property_Value *
etk_property_value_char(value)
	char	value

char
etk_property_value_char_get(value)
	Etk_Property_Value *	value

void
etk_property_value_char_set(property_value, value)
	Etk_Property_Value *	property_value
	char	value

Etk_Property_Value *
etk_property_value_create(type, ...)
	Etk_Property_Type	type

void
etk_property_value_delete(value)
	Etk_Property_Value *	value

Etk_Property_Value *
etk_property_value_double(value)
	double	value

double
etk_property_value_double_get(value)
	Etk_Property_Value *	value

void
etk_property_value_double_set(property_value, value)
	Etk_Property_Value *	property_value
	double	value

Etk_Property_Value *
etk_property_value_float(value)
	float	value

float
etk_property_value_float_get(value)
	Etk_Property_Value *	value

void
etk_property_value_float_set(property_value, value)
	Etk_Property_Value *	property_value
	float	value

void
etk_property_value_get(value, type, value_location)
	Etk_Property_Value *	value
	Etk_Property_Type	type
	void *	value_location

Etk_Property_Value *
etk_property_value_int(value)
	int	value

int
etk_property_value_int_get(value)
	Etk_Property_Value *	value

void
etk_property_value_int_set(property_value, value)
	Etk_Property_Value *	property_value
	int	value

Etk_Property_Value *
etk_property_value_long(value)
	long	value

long
etk_property_value_long_get(value)
	Etk_Property_Value *	value

void
etk_property_value_long_set(property_value, value)
	Etk_Property_Value *	property_value
	long	value

Etk_Property_Value *
etk_property_value_new()

Etk_Property_Value *
etk_property_value_pointer(value)
	void *	value

void *
etk_property_value_pointer_get(value)
	Etk_Property_Value *	value

void
etk_property_value_pointer_set(property_value, value)
	Etk_Property_Value *	property_value
	void *	value

void
etk_property_value_set(property_value, type, ...)
	Etk_Property_Value *	property_value
	Etk_Property_Type	type

Etk_Property_Value *
etk_property_value_short(value)
	short	value

short
etk_property_value_short_get(value)
	Etk_Property_Value *	value

void
etk_property_value_short_set(property_value, value)
	Etk_Property_Value *	property_value
	short	value

Etk_Property_Value *
etk_property_value_string(value)
	char *	value

char *
etk_property_value_string_get(value)
	Etk_Property_Value *	value

void
etk_property_value_string_set(property_value, value)
	Etk_Property_Value *	property_value
	char *	value

Etk_Property_Type
etk_property_value_type_get(value)
	Etk_Property_Value *	value

Evas_List **
etk_radio_button_group_get(radio_button)
	Etk_Radio_Button *	radio_button

void
etk_radio_button_group_set(radio_button, group)
	Etk_Radio_Button *	radio_button
	Evas_List **	group

Etk_Widget *
etk_radio_button_new(group)
	Evas_List **	group

Etk_Widget *
etk_radio_button_new_from_widget(radio_button)
	Etk_Radio_Button *	radio_button

Etk_Widget *
etk_radio_button_new_with_label(label, group)
	char *	label
	Evas_List **	group

Etk_Widget *
etk_radio_button_new_with_label_from_widget(label, radio_button)
	char *	label
	Etk_Radio_Button *	radio_button

Etk_Type *
etk_radio_button_type_get()

void
etk_range_increments_set(range, step, page)
	Etk_Range *	range
	double	step
	double	page

double
etk_range_page_size_get(range)
	Etk_Range *	range

void
etk_range_page_size_set(range, page_size)
	Etk_Range *	range
	double	page_size

void
etk_range_range_set(range, lower, upper)
	Etk_Range *	range
	double	lower
	double	upper

Etk_Type *
etk_range_type_get()

double
etk_range_value_get(range)
	Etk_Range *	range

void
etk_range_value_set(range, value)
	Etk_Range *	range
	double	value

Etk_Type *
etk_scrollbar_type_get()

void
etk_scrolled_view_add_with_viewport(scrolled_view, child)
	Etk_Scrolled_View *	scrolled_view
	Etk_Widget *	child

Etk_Range *
etk_scrolled_view_hscrollbar_get(scrolled_view)
	Etk_Scrolled_View *	scrolled_view

Etk_Widget *
etk_scrolled_view_new()

void
etk_scrolled_view_policy_get(scrolled_view, hpolicy, vpolicy)
	Etk_Scrolled_View *	scrolled_view
	Etk_Scrolled_View_Policy *	hpolicy
	Etk_Scrolled_View_Policy *	vpolicy

void
etk_scrolled_view_policy_set(scrolled_view, hpolicy, vpolicy)
	Etk_Scrolled_View *	scrolled_view
	Etk_Scrolled_View_Policy	hpolicy
	Etk_Scrolled_View_Policy	vpolicy

Etk_Type *
etk_scrolled_view_type_get()

Etk_Range *
etk_scrolled_view_vscrollbar_get(scrolled_view)
	Etk_Scrolled_View *	scrolled_view

void
etk_selection_text_request(widget)
	Etk_Widget *	widget

void
etk_selection_text_set(widget, data, length)
	Etk_Widget *	widget
	char *	data
	int	length

void
etk_shutdown()

void
Shutdown()
      CODE:
        etk_shutdown();

void
etk_signal_callback_call(callback, object, return_value, ...)
	Etk_Signal_Callback *	callback
	Etk_Object *	object
	void *	return_value

void
etk_signal_callback_del(signal_callback)
	Etk_Signal_Callback *	signal_callback

Etk_Signal_Callback *
etk_signal_callback_new(signal, callback, data, swapped)
	Etk_Signal *	signal
	Etk_Signal_Callback_Function	callback
	void *	data
	Etk_Bool	swapped

void
etk_signal_connect(signal_name, object, callback, data)
	char *	        signal_name
	Etk_Widget *	object
	SV *	        callback
	SV *            data
	
	CODE:	
	Callback_Signal_Data *cbd = NULL;
	Etk_Signal *sig = NULL;
	Etk_Marshaller marsh;
	
	cbd = calloc(1, sizeof(Callback_Signal_Data));
	cbd->signal_name = strdup(signal_name);
	cbd->object = object;
	cbd->perl_data = newSVsv(data);
	cbd->perl_callback = newSVsv(callback);	
	
	sig = etk_signal_lookup(signal_name, ETK_OBJECT(object)->type);
	if(!sig) printf("CANT GET SIG!\n");
	marsh = etk_signal_marshaller_get(sig);
	
	if(marsh == etk_marshaller_VOID__VOID)
	  etk_signal_connect(signal_name, object, callback_VOID__VOID, cbd);
	else if(marsh == etk_marshaller_VOID__INT)
	  etk_signal_connect(signal_name, object, callback_VOID__INT, cbd);
	else if(marsh == etk_marshaller_VOID__DOUBLE)
	  etk_signal_connect(signal_name, object, callback_VOID__DOUBLE, cbd);
	else if(marsh == etk_marshaller_VOID__POINTER)
	  etk_signal_connect(signal_name, object, callback_VOID__POINTER, cbd);
	else if(marsh == etk_marshaller_VOID__INT_POINTER)
	  etk_signal_connect(signal_name, object, callback_VOID__INT_POINTER, cbd);
	else if(marsh == etk_marshaller_BOOL__VOID)
	  etk_signal_connect(signal_name, object, callback_BOOL__VOID, cbd);
	else if(marsh == etk_marshaller_BOOL__DOUBLE)
	  etk_signal_connect(signal_name, object, callback_BOOL__DOUBLE, cbd);
	else if(marsh == etk_marshaller_BOOL__POINTER_POINTER)
	  etk_signal_connect(signal_name, object, callback_BOOL__POINTER_POINTER, cbd);
	else
	etk_signal_connect(signal_name, object, callback_VOID__VOID, cbd);

void
etk_signal_connect_after(signal_name, object, callback, data)
	char *	signal_name
	Etk_Widget *	object
	Etk_Signal_Callback_Function	callback
	void *	data

void
etk_signal_connect_full(signal, object, callback, data, swapped, after)
	Etk_Signal *	signal
	Etk_Object *	object
	Etk_Signal_Callback_Function	callback
	void *	data
	Etk_Bool	swapped
	Etk_Bool	after

void
etk_signal_connect_swapped(signal_name, object, callback, data)
	char *	signal_name
	Etk_Widget *	object
	Etk_Signal_Callback_Function	callback
	void *	data

void
etk_signal_delete(signal)
	Etk_Signal *	signal

void
etk_signal_disconnect(signal_name, object, callback)
	char *	signal_name
	Etk_Widget *	object
	Etk_Signal_Callback_Function	callback

void
etk_signal_emit(signal, object, return_value, ...)
	Etk_Signal *	signal
	Etk_Object *	object
	void *	return_value

void
etk_signal_emit_by_name(signal_name, object, return_value, ...)
	char *	signal_name
	Etk_Object *	object
	void *	return_value

Etk_Signal *
etk_signal_lookup(signal_name, type)
	char *	signal_name
	Etk_Type *	type

Etk_Marshaller
etk_signal_marshaller_get(signal)
	Etk_Signal *	signal

char *
etk_signal_name_get(signal)
	Etk_Signal *	signal

Etk_Signal *
etk_signal_new(signal_name, object_type, default_handler_offset, marshaller, accumulator, accum_data)
	char *	signal_name
	Etk_Type *	object_type
	long	default_handler_offset
	Etk_Marshaller	marshaller
	Etk_Accumulator	accumulator
	void *	accum_data

void
etk_signal_shutdown()

void
etk_signal_stop()

Etk_Type *
etk_slider_type_get()

Etk_Widget *
etk_spin_button_new(min, max, step)
	double	min
	double	max
	double	step

Etk_Type *
etk_spin_button_type_get()

int
etk_statusbar_context_id_get(statusbar, context)
	Etk_Statusbar *	statusbar
	char *	context

Etk_Bool
etk_statusbar_has_resize_grip_get(statusbar)
	Etk_Statusbar *	statusbar

void
etk_statusbar_has_resize_grip_set(statusbar, has_resize_grip)
	Etk_Statusbar *	statusbar
	Etk_Bool	has_resize_grip

Etk_Widget *
etk_statusbar_new()

void
etk_statusbar_pop(statusbar, context_id)
	Etk_Statusbar *	statusbar
	int	context_id

int
etk_statusbar_push(statusbar, message, context_id)
	Etk_Statusbar *	statusbar
	char *	message
	int	context_id

void
etk_statusbar_remove(statusbar, message_id)
	Etk_Statusbar *	statusbar
	int	message_id

Etk_Type *
etk_statusbar_type_get()

char *
etk_stock_key_get(stock_id, size)
	Etk_Stock_Id	stock_id
	Etk_Stock_Size	size

char *
etk_stock_label_get(stock_id)
	Etk_Stock_Id	stock_id

Etk_String *
etk_string_append(string, text)
	Etk_String *	string
	char *	text

Etk_String *
etk_string_append_char(string, c)
	Etk_String *	string
	char	c

Etk_String *
etk_string_append_printf(string, format, ...)
	Etk_String *	string
	char * format	

Etk_String *
etk_string_append_sized(string, text, length)
	Etk_String *	string
	char *	text
	int	length

Etk_String *
etk_string_copy(string)
	 Etk_String *	string

char *
etk_string_get(string)
	Etk_String *	string

Etk_String *
etk_string_insert(string, pos, text)
	Etk_String *	string
	int	pos
	char *	text

Etk_String *
etk_string_insert_char(string, pos, c)
	Etk_String *	string
	int	pos
	char	c

Etk_String *
etk_string_insert_printf(string, pos, format, ...)
	Etk_String *	string
	int	pos
	char * format	

Etk_String *
etk_string_insert_sized(string, pos, text, length)
	Etk_String *	string
	int	pos
	char *	text
	int	length

int
etk_string_length_get(string)
	Etk_String *	string

Etk_String *
etk_string_new(value)
	char *	value

Etk_String *
etk_string_new_printf(format, ...)
	char * format	

Etk_String *
etk_string_new_sized(value, size)
	char *	value
	int	size

Etk_String *
etk_string_prepend(string, text)
	Etk_String *	string
	char *	text

Etk_String *
etk_string_prepend_char(string, c)
	Etk_String *	string
	char	c

Etk_String *
etk_string_prepend_printf(string, format, ...)
	Etk_String *	string
	char * format	

Etk_String *
etk_string_prepend_sized(string, text, length)
	Etk_String *	string
	char *	text
	int	length

Etk_String *
etk_string_set(string, value)
	Etk_String *	string
	char *	value

Etk_String *
etk_string_set_printf(string, format, ...)
	Etk_String *	string
	char * format	

Etk_String *
etk_string_set_sized(string, value, length)
	Etk_String *	string
	char *	value
	int	length

Etk_String *
etk_string_truncate(string, length)
	Etk_String *	string
	int	length

Etk_Type *
etk_string_type_get()

void
etk_table_attach(table, child, left_attach, right_attach, top_attach, bottom_attach, x_padding, y_padding, fill_policy)
	Etk_Widget *	table
	Etk_Widget *	child
	int	left_attach
	int	right_attach
	int	top_attach
	int	bottom_attach
	int	x_padding
	int	y_padding
	Etk_Fill_Policy_Flags	fill_policy

void
etk_table_attach_defaults(table, child, left_attach, right_attach, top_attach, bottom_attach)
	Etk_Widget *	table
	Etk_Widget *	child
	int	left_attach
	int	right_attach
	int	top_attach
	int	bottom_attach

void
etk_table_cell_clear(table, col, row)
	Etk_Widget *	table
	int	col
	int	row

Etk_Bool
etk_table_homogeneous_get(table)
	Etk_Widget *	table

void
etk_table_homogeneous_set(table, homogeneous)
	Etk_Widget *	table
	Etk_Bool	homogeneous

Etk_Widget *
etk_table_new(num_cols, num_rows, homogeneous)
	int	num_cols
	int	num_rows
	Etk_Bool	homogeneous

void
etk_table_resize(table, num_cols, num_rows)
	Etk_Widget *	table
	int	num_cols
	int	num_rows

Etk_Type *
etk_table_type_get()

Etk_Widget *
etk_text_view_new()

Etk_Type *
etk_text_view_type_get()

void
etk_textblock_iter_copy(iter, dest_iter)
	Etk_Textblock_Iter *	iter
	Etk_Textblock_Iter *	dest_iter

void
etk_textblock_iter_free(iter)
	Etk_Textblock_Iter *	iter

void
etk_textblock_iter_goto_end(iter)
	Etk_Textblock_Iter *	iter

void
etk_textblock_iter_goto_next_char(iter)
	Etk_Textblock_Iter *	iter

void
etk_textblock_iter_goto_prev_char(iter)
	Etk_Textblock_Iter *	iter

void
etk_textblock_iter_goto_start(iter)
	Etk_Textblock_Iter *	iter

Etk_Textblock_Iter *
etk_textblock_iter_new(textblock)
	Etk_Textblock *	textblock

Etk_Textblock *
etk_textblock_new()

void
etk_textblock_realize(textblock, evas)
	Etk_Textblock *	textblock
	Evas *	evas

void
etk_textblock_text_set(textblock, text)
	Etk_Textblock *	textblock
	char *	text

Etk_Type *
etk_textblock_type_get()

void
etk_textblock_unrealize(textblock)
	Etk_Textblock *	textblock

char *
etk_theme_default_icon_theme_get()

char *
etk_theme_default_widget_theme_get()

char *
etk_theme_icon_theme_get()

Etk_Bool
etk_theme_icon_theme_set(theme_name)
	char *	theme_name

void
etk_theme_init()

Evas_Object *
etk_theme_object_load(evas, filename, group)
	Evas *	evas
	char *	filename
	char *	group

Evas_Object *
etk_theme_object_load_from_parent(evas, theme_parent, filename, group)
	Evas *	evas
	Etk_Widget *	theme_parent
	char *	filename
	char *	group

void
etk_theme_shutdown()

char *
etk_theme_widget_theme_get()

Etk_Bool
etk_theme_widget_theme_set(theme_name)
	char *	theme_name

Etk_Bool
etk_toggle_button_active_get(toggle_button)
	Etk_Widget *	toggle_button

void
etk_toggle_button_active_set(toggle_button, active)
	Etk_Widget *	toggle_button
	Etk_Bool	active

Etk_Widget *
etk_toggle_button_new()

Etk_Widget *
etk_toggle_button_new_with_label(label)
	char *	label

void
etk_toggle_button_toggle(toggle_button)
	Etk_Widget *	toggle_button

Etk_Type *
etk_toggle_button_type_get()

void
etk_tooltips_disable()

void
etk_tooltips_enable()

void
etk_tooltips_init()

void
etk_tooltips_pop_down()

void
etk_tooltips_pop_up(widget)
	Etk_Widget *	widget

void
etk_tooltips_shutdown()

char *
etk_tooltips_tip_get(widget)
	Etk_Widget *	widget

void
etk_tooltips_tip_set(widget, text)
	Etk_Widget *	widget
	char *	text

Etk_Bool
etk_tooltips_tip_visible()

Evas *
etk_toplevel_widget_evas_get(toplevel_widget)
	Etk_Toplevel_Widget *	toplevel_widget

Etk_Widget *
etk_toplevel_widget_focused_widget_get(toplevel_widget)
	Etk_Toplevel_Widget *	toplevel_widget

Etk_Widget *
etk_toplevel_widget_focused_widget_next_get(toplevel_widget)
	Etk_Toplevel_Widget *	toplevel_widget

Etk_Widget *
etk_toplevel_widget_focused_widget_prev_get(toplevel_widget)
	Etk_Toplevel_Widget *	toplevel_widget

void
etk_toplevel_widget_focused_widget_set(toplevel_widget, widget)
	Etk_Toplevel_Widget *	toplevel_widget
	Etk_Widget *	widget

void
etk_toplevel_widget_geometry_get(toplevel_widget, x, y, w, h)
	Etk_Toplevel_Widget *	toplevel_widget
	int *	x
	int *	y
	int *	w
	int *	h

void
etk_toplevel_widget_pointer_pop(toplevel_widget, pointer_type)
	Etk_Toplevel_Widget *	toplevel_widget
	Etk_Pointer_Type	pointer_type

void
etk_toplevel_widget_pointer_push(toplevel_widget, pointer_type)
	Etk_Toplevel_Widget *	toplevel_widget
	Etk_Pointer_Type	pointer_type

Etk_Type *
etk_toplevel_widget_type_get()

Etk_Tree_Row *
etk_tree_append(tree)
	Etk_Widget *	tree
      CODE:
        RETVAL = etk_tree_append(ETK_TREE(tree), NULL);
      OUTPUT:
        RETVAL

Etk_Tree_Row *
etk_tree_append_to_row(row, ...)
	Etk_Tree_Row *	row

void
etk_tree_build(tree)
	Etk_Widget *	tree

void
etk_tree_clear(tree)
	Etk_Widget *	tree

Etk_Bool
etk_tree_col_expand_get(col)
	Etk_Tree_Col *	col

void
etk_tree_col_expand_set(col, expand)
	Etk_Tree_Col *	col
	Etk_Bool	expand

int
etk_tree_col_min_width_get(col)
	Etk_Tree_Col *	col

void
etk_tree_col_min_width_set(col, min_width)
	Etk_Tree_Col *	col
	int	min_width

Etk_Tree_Col *
etk_tree_col_new(tree, title, model, width)
	Etk_Widget *	tree
	char *	title
	Etk_Tree_Model *	model
	int	width

int
etk_tree_col_place_get(col)
	Etk_Tree_Col *	col

void
etk_tree_col_reorder(col, new_place)
	Etk_Tree_Col *	col
	int	new_place

Etk_Bool
etk_tree_col_resizable_get(col)
	Etk_Tree_Col *	col

void
etk_tree_col_resizable_set(col, resizable)
	Etk_Tree_Col *	col
	Etk_Bool	resizable

void
etk_tree_col_sort_func_set(col, compare_cb, data)
        Etk_Tree_Col *  col
        SV * compare_cb
        SV * data
      CODE:
	Callback_Tree_Compare_Data *cbd;
	
        cbd = calloc(1, sizeof(Callback_Tree_Compare_Data));
	cbd->object = col;
        cbd->perl_data = newSVsv(data);
        cbd->perl_callback = newSVsv(compare_cb);	
        etk_tree_col_sort_func_set(col, tree_compare_cb, cbd);

void
etk_tree_col_sort_func_set2(col, compare_cb, data)
	Etk_Tree_Col *	col
	int ( * ) ( Etk_Tree * tree, Etk_Tree_Row * row1, Etk_Tree_Row *row2, Etk_Tree_Col * col, void * data ) compare_cb
	void *	data

char *
etk_tree_col_title_get(col)
	Etk_Tree_Col *	col

void
etk_tree_col_title_set(col, title)
	Etk_Tree_Col *	col
	char *	title

Etk_Type *
etk_tree_col_type_get()

Etk_Bool
etk_tree_col_visible_get(col)
	Etk_Tree_Col *	col

void
etk_tree_col_visible_set(col, visible)
	Etk_Tree_Col *	col
	Etk_Bool	visible

int
etk_tree_col_width_get(col)
	Etk_Tree_Col *	col

void
etk_tree_col_width_set(col, width)
	Etk_Tree_Col *	col
	int	width

Etk_Tree_Row *
etk_tree_first_row_get(tree)
	Etk_Widget *	tree

void
etk_tree_freeze(tree)
	Etk_Widget *	tree

Etk_Bool
etk_tree_headers_visible_get(tree)
	Etk_Widget *	tree

void
etk_tree_headers_visible_set(tree, headers_visible)
	Etk_Widget *	tree
	Etk_Bool	headers_visible

Etk_Tree_Row *
etk_tree_last_row_get(tree, walking_through_hierarchy, include_collapsed_children)
	Etk_Widget *	tree
	Etk_Bool	walking_through_hierarchy
	Etk_Bool	include_collapsed_children

Etk_Tree_Mode
etk_tree_mode_get(tree)
	Etk_Widget *	tree

void
etk_tree_mode_set(tree, mode)
	Etk_Widget *	tree
	Etk_Tree_Mode	mode

void
etk_tree_model_alignment_get(model, xalign, yalign)
	Etk_Tree_Model *	model
	float *	xalign
	float *	yalign

void
etk_tree_model_alignment_set(model, xalign, yalign)
	Etk_Tree_Model *	model
	float	xalign
	float	yalign

Etk_Tree_Model *
etk_tree_model_checkbox_new(tree)
	Etk_Widget *	tree

Etk_Tree_Model *
etk_tree_model_double_new(tree)
	Etk_Widget *	tree

void
etk_tree_model_free(model)
	Etk_Tree_Model *	model

int
etk_tree_model_icon_text_icon_width_get(model)
	Etk_Tree_Model *	model

void
etk_tree_model_icon_text_icon_width_set(model, icon_width)
	Etk_Tree_Model *	model
	int	icon_width

Etk_Tree_Model *
etk_tree_model_icon_text_new(tree, icon_type)
	Etk_Widget *	tree
	Etk_Tree_Model_Image_Type	icon_type

Etk_Tree_Model *
etk_tree_model_image_new(tree, image_type)
	Etk_Widget *	tree
	Etk_Tree_Model_Image_Type	image_type

Etk_Tree_Model *
etk_tree_model_int_new(tree)
	Etk_Widget *	tree

Etk_Tree_Model *
etk_tree_model_progress_bar_new(tree)
	Etk_Widget *	tree

Etk_Tree_Model *
etk_tree_model_text_new(tree)
	Etk_Widget *	tree

Etk_Bool
etk_tree_multiple_select_get(tree)
	Etk_Widget *	tree

void
etk_tree_multiple_select_set(tree, multiple_select)
	Etk_Widget *	tree
	Etk_Bool	multiple_select

Etk_Widget *
etk_tree_new()

Etk_Tree_Row *
etk_tree_next_row_get(row, walking_through_hierarchy, include_collapsed_children)
	Etk_Tree_Row *	row
	Etk_Bool	walking_through_hierarchy
	Etk_Bool	include_collapsed_children

Etk_Tree_Col *
etk_tree_nth_col_get(tree, nth)
	Etk_Widget *	tree
	int	nth

int
etk_tree_num_cols_get(tree)
	Etk_Widget *	tree

Etk_Tree_Row *
etk_tree_prev_row_get(row, walking_through_hierarchy, include_collapsed_children)
	Etk_Tree_Row *	row
	Etk_Bool	walking_through_hierarchy
	Etk_Bool	include_collapsed_children

void
etk_tree_row_collapse(row)
	Etk_Tree_Row *	row

void *
etk_tree_row_data_get(row)
	Etk_Tree_Row *	row

void
etk_tree_row_data_set(row, data)
	Etk_Tree_Row *	row
	void *	data

void
etk_tree_row_data_set_full(row, data, free_cb)
	Etk_Tree_Row *	row
	void *	data
	void ( * ) ( void * data ) free_cb

void
etk_tree_row_del(row)
	Etk_Tree_Row *	row

void
etk_tree_row_expand(row)
	Etk_Tree_Row *	row

void
etk_tree_row_field_int_set(row, col, i)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	int             i
      CODE:
        etk_tree_row_fields_set(row, col, i, NULL);
	
void
etk_tree_row_field_text_set(row, col, t)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	char         *  t
      CODE:
        etk_tree_row_fields_set(row, col, t, NULL);

void
etk_tree_row_field_double_set(row, col, d)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	double          d
      CODE:
        etk_tree_row_fields_set(row, col, d, NULL);

void
etk_tree_row_field_image_file_set(row, col, image)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	char         *  image
      CODE:
        etk_tree_row_fields_set(row, col, image, NULL);

void
etk_tree_row_field_image_edje_set(row, col, edje, group)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	char         *  edje
	char         *  group
      CODE:
        etk_tree_row_fields_set(row, col, edje, group, NULL);

void
etk_tree_row_field_icon_file_text_set(row, col, icon, t)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	char         *  icon
	char         *  t
      CODE:
        etk_tree_row_fields_set(row, col, icon, t, NULL);

void
etk_tree_row_field_icon_edje_text_set(row, col, icon, group, t)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	char         *  icon
	char         *  group
	char         *  t
      CODE:
        etk_tree_row_fields_set(row, col, icon, group, t, NULL);
	
void
etk_tree_row_field_checkbox_set(row, col, checked)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	int             checked
      CODE:
        etk_tree_row_fields_set(row, col, checked, NULL);
	
void
etk_tree_row_field_progress_bar_set(row, col, fraction, t)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
	double          fraction
	char         *  t
      CODE:
        etk_tree_row_fields_set(row, col, fraction, t, NULL);

int
etk_tree_row_field_int_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      CODE:
	int i;
        etk_tree_row_fields_get(row, col, &i, NULL);
        RETVAL = i;
      OUTPUT:
        RETVAL
	
char *
etk_tree_row_field_text_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      CODE:
        char *t;
        etk_tree_row_fields_get(row, col, &t, NULL);
        RETVAL = t;
      OUTPUT:
        RETVAL

double
etk_tree_row_field_double_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      CODE:
	double d;
        etk_tree_row_fields_get(row, col, &d, NULL);
        RETVAL = d;
      OUTPUT:
        RETVAL

char *
etk_tree_row_field_image_file_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      CODE:
	char *image;
        etk_tree_row_fields_get(row, col, &image, NULL);

void
etk_tree_row_field_image_edje_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      PPCODE:
	char *edje;
	char *group;

        etk_tree_row_fields_get(row, col, &edje, &group, NULL);
        EXTEND(SP, 2);
        PUSHs(sv_2mortal(newSVpvn(edje, strlen(edje))));
        PUSHs(sv_2mortal(newSVpvn(group, strlen(group))));

void
etk_tree_row_field_icon_file_text_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      PPCODE:
	char *icon;
	char *t;
        etk_tree_row_fields_set(row, col, &icon, &t, NULL);
        EXTEND(SP, 2);
        PUSHs(sv_2mortal(newSVpvn(icon, strlen(icon))));
        PUSHs(sv_2mortal(newSVpvn(t, strlen(t))));

void
etk_tree_row_field_icon_edje_text_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      PPCODE:
	char *icon;
	char *group;
	char *t;
        etk_tree_row_fields_get(row, col, &icon, &group, &t, NULL);
        EXTEND(SP, 3);
        PUSHs(sv_2mortal(newSVpvn(icon, strlen(icon))));
        PUSHs(sv_2mortal(newSVpvn(group, strlen(group))));
        PUSHs(sv_2mortal(newSVpvn(t, strlen(t))));
	
int
etk_tree_row_field_checkbox_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      CODE:
	int checked;
        etk_tree_row_fields_get(row, col, &checked, NULL);
        RETVAL = checked;
      OUTPUT:
        RETVAL
	
void
etk_tree_row_field_progress_bar_get(row, col)
        Etk_Tree_Row *  row
	Etk_Tree_Col *  col
      CODE:
	double fraction;
	char *t;
        etk_tree_row_fields_get(row, col, &fraction, &t, NULL);
        EXTEND(SP, 2);
        PUSHs(sv_2mortal(newSViv(fraction)));
        PUSHs(sv_2mortal(newSVpvn(t, strlen(t))));

void
etk_tree_row_fields_get(row, ...)
	Etk_Tree_Row *	row

void
etk_tree_row_fields_set(row, ...)
	Etk_Tree_Row *	row

Etk_Tree_Row *
etk_tree_row_first_child_get(row)
	Etk_Tree_Row *	row

int
etk_tree_row_height_get(tree)
	Etk_Widget *	tree

void
etk_tree_row_height_set(tree, row_height)
	Etk_Widget *	tree
	int	row_height

Etk_Tree_Row *
etk_tree_row_last_child_get(row, walking_through_hierarchy, include_collapsed_children)
	Etk_Tree_Row *	row
	Etk_Bool	walking_through_hierarchy
	Etk_Bool	include_collapsed_children

void
etk_tree_row_scroll_to(row, center_the_row)
	Etk_Tree_Row *	row
	Etk_Bool	center_the_row

void
etk_tree_row_select(row)
	Etk_Tree_Row *	row

void
etk_tree_row_unselect(row)
	Etk_Tree_Row *	row

void
etk_tree_select_all(tree)
	Etk_Widget *	tree

Etk_Tree_Row *
etk_tree_selected_row_get(tree)
	Etk_Widget *	tree

Evas_List *
etk_tree_selected_rows_get(tree)
	Etk_Widget *	tree

void
etk_tree_sort(tree, compare_cb, ascendant, col, data)
	Etk_Widget *	tree
	int ( * ) ( Etk_Tree * tree, Etk_Tree_Row * row1, Etk_Tree_Row * row2, Etk_Tree_Col * col, void * data ) compare_cb
	Etk_Bool	ascendant
	Etk_Tree_Col *	col
	void *	data

void
etk_tree_thaw(tree)
	Etk_Widget *	tree

Etk_Type *
etk_tree_type_get()

void
etk_tree_unselect_all(tree)
	Etk_Widget *	tree

void
etk_type_delete(type)
	Etk_Type *	type

void
etk_type_destructors_call(type, object)
	Etk_Type *	type
	Etk_Object *	object

Etk_Bool
etk_type_inherits_from(type, parent)
	Etk_Type *	type
	Etk_Type *	parent

char *
etk_type_name_get(type)
	Etk_Type *	type

Etk_Type *
etk_type_new(type_name, parent_type, type_size, arg3, arg4)
	char *	type_name
	Etk_Type *	parent_type
	int	type_size
	Etk_Constructor arg3
	Etk_Destructor 	arg4

void
etk_type_object_construct(type, object)
	Etk_Type *	type
	Etk_Object *	object

Etk_Type *
etk_type_parent_type_get(type)
	Etk_Type *	type

Etk_Property *
etk_type_property_add(type, name, property_id, property_type, flags, default_value)
	Etk_Type *	type
	char *	name
	int	property_id
	Etk_Property_Type	property_type
	Etk_Property_Flags	flags
	Etk_Property_Value *	default_value

Etk_Bool
etk_type_property_find(type, name, property_owner, property)
	Etk_Type *	type
	char *	name
	Etk_Type **	property_owner
	Etk_Property **	property

void
etk_type_property_list(type, properties)
	Etk_Type *	type
	Evas_List **	properties

void
etk_type_shutdown()

void
etk_type_signal_add(type, signal)
	Etk_Type *	type
	Etk_Signal *	signal

Etk_Signal *
etk_type_signal_get(type, signal_name)
	Etk_Type *	type
	char *	signal_name

void
etk_type_signal_remove(type, signal)
	Etk_Type *	type
	Etk_Signal *	signal

Etk_Widget *
etk_vbox_new(homogeneous, spacing)
	Etk_Bool	homogeneous
	int	spacing

Etk_Type *
etk_vbox_type_get()

Etk_Widget *
etk_viewport_new()

Etk_Type *
etk_viewport_type_get()

Etk_Widget *
etk_vpaned_new()

Etk_Type *
etk_vpaned_type_get()

Etk_Widget *
etk_vscrollbar_new(lower, upper, value, step_increment, page_increment, page_size)
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	double	page_size

Etk_Type *
etk_vscrollbar_type_get()

Etk_Widget *
etk_vseparator_new()

Etk_Type *
etk_vseparator_type_get()

Etk_Widget *
etk_vslider_new(lower, upper, value, step_increment, page_increment)
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment

Etk_Type *
etk_vslider_type_get()

Evas_Object *
etk_widget_clip_get(widget)
	Etk_Widget *	widget

void
etk_widget_clip_set(widget, clip)
	Etk_Widget *	widget
	Evas_Object *	clip

void
etk_widget_clip_unset(widget)
	Etk_Widget *	widget

void
etk_widget_clipboard_received(widget, event)
	Etk_Widget *	widget
	Etk_Event_Selection_Request *	event

Etk_Bool
etk_widget_dnd_dest_get(widget)
	Etk_Widget *	widget

void
etk_widget_dnd_dest_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on

Evas_List *
etk_widget_dnd_dest_widgets_get()

void
etk_widget_dnd_drag_data_set(widget, types, num_types, data, data_size)
	Etk_Widget *	widget
	char **	types
	int	num_types
	void *	data
	int	data_size

Etk_Widget *
etk_widget_dnd_drag_widget_get(widget)
	Etk_Widget *	widget

void
etk_widget_dnd_drag_widget_set(widget, drag_widget)
	Etk_Widget *	widget
	Etk_Widget *	drag_widget

char **
etk_widget_dnd_files_get(e, num_files)
	Etk_Widget *	e
	int *	num_files

Etk_Bool
etk_widget_dnd_internal_get(widget)
	Etk_Widget *	widget

void
etk_widget_dnd_internal_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on

Etk_Bool
etk_widget_dnd_source_get(widget)
	Etk_Widget *	widget

void
etk_widget_dnd_source_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on

char **
etk_widget_dnd_types_get(widget, num)
	Etk_Widget *	widget
	int *	num

void
etk_widget_dnd_types_set(widget, types, num)
	Etk_Widget *	widget
	char **	types
	int	num

void
etk_widget_drag_begin(widget)
	Etk_Widget *	widget

void
etk_widget_drag_drop(widget, event)
	Etk_Widget *	widget
	Etk_Event_Selection_Request *	event

void
etk_widget_drag_end(widget)
	Etk_Widget *	widget

void
etk_widget_drag_enter(widget)
	Etk_Widget *	widget

void
etk_widget_drag_leave(widget)
	Etk_Widget *	widget

void
etk_widget_drag_motion(widget)
	Etk_Widget *	widget

void
etk_widget_enter(widget)
	Etk_Widget *	widget

void
etk_widget_focus(widget)
	Etk_Widget *	widget

void
etk_widget_geometry_get(widget, x, y, w, h)
	Etk_Widget *	widget
	int *	x
	int *	y
	int *	w
	int *	h

Etk_Bool
etk_widget_has_event_object_get(widget)
	Etk_Widget *	widget

void
etk_widget_has_event_object_set(widget, has_event_object)
	Etk_Widget *	widget
	Etk_Bool	has_event_object

void
etk_widget_hide(widget)
	Etk_Widget *	widget

void
etk_widget_hide_all(widget)
	Etk_Widget *	widget

void
etk_widget_inner_geometry_get(widget, x, y, w, h)
	Etk_Widget *	widget
	int *	x
	int *	y
	int *	w
	int *	h

Etk_Bool
etk_widget_is_swallowed(widget)
	Etk_Widget *	widget

Etk_Bool
etk_widget_is_swallowing_object(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object

Etk_Bool
etk_widget_is_swallowing_widget(widget, swallowed_widget)
	Etk_Widget *	widget
	Etk_Widget *	swallowed_widget

Etk_Bool
etk_widget_is_visible(widget)
	Etk_Widget *	widget

void
etk_widget_key_event_propagation_stop()

void
etk_widget_leave(widget)
	Etk_Widget *	widget

void
etk_widget_lower(widget)
	Etk_Widget *	widget

Etk_Bool
etk_widget_member_object_add(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object

void
etk_widget_member_object_del(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object

void
etk_widget_member_object_lower(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object

void
etk_widget_member_object_raise(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object

void
etk_widget_member_object_stack_above(widget, object, above)
	Etk_Widget *	widget
	Evas_Object *	object
	Evas_Object *	above

void
etk_widget_member_object_stack_below(widget, object, below)
	Etk_Widget *	widget
	Evas_Object *	object
	Evas_Object *	below

char *
etk_widget_name_get(widget)
	Etk_Widget *	widget

void
etk_widget_name_set(widget, name)
	Etk_Widget *	widget
	char *	name

Etk_Widget *
etk_widget_new(widget_type, first_property, ...)
	Etk_Type *	widget_type
	char *	first_property

Etk_Widget *
etk_widget_parent_get(widget)
	Etk_Widget *	widget

void
etk_widget_parent_set(widget, parent)
	Etk_Widget *	widget
	Etk_Widget *	parent

void
etk_widget_parent_set_full(widget, parent, remove_from_container)
	Etk_Widget *	widget
	Etk_Widget *	parent
	Etk_Bool	remove_from_container

Etk_Bool
etk_widget_pass_mouse_events_get(widget)
	Etk_Widget *	widget

void
etk_widget_pass_mouse_events_set(widget, pass_mouse_events)
	Etk_Widget *	widget
	Etk_Bool	pass_mouse_events

void
etk_widget_raise(widget)
	Etk_Widget *	widget

void
etk_widget_redraw_queue(widget)
	Etk_Widget *	widget

Etk_Bool
etk_widget_repeat_mouse_events_get(widget)
	Etk_Widget *	widget

void
etk_widget_repeat_mouse_events_set(widget, repeat_mouse_events)
	Etk_Widget *	widget
	Etk_Bool	repeat_mouse_events

void
etk_widget_selection_received(widget, event)
	Etk_Widget *	widget
	Etk_Event_Selection_Request *	event

void
etk_widget_show(widget)
	Etk_Widget *	widget

void
etk_widget_show_all(widget)
	Etk_Widget *	widget

void
etk_widget_size_allocate(widget, geometry)
	Etk_Widget *	widget
	Etk_Geometry	geometry

void
etk_widget_size_recalc_queue(widget)
	Etk_Widget *	widget

void
etk_widget_size_request(widget, size_requisition)
	Etk_Widget *	widget
	Etk_Size *	size_requisition

void
etk_widget_size_request_full(widget, size_requisition, hidden_has_no_size)
	Etk_Widget *	widget
	Etk_Size *	size_requisition
	Etk_Bool	hidden_has_no_size

void
etk_widget_size_request_set(widget, w, h)
	Etk_Widget *	widget
	int	w
	int	h

Etk_Bool
etk_widget_swallow_widget(swallowing_widget, part, widget_to_swallow)
	Etk_Widget *	swallowing_widget
	char *	part
	Etk_Widget *	widget_to_swallow

char *
etk_widget_theme_file_get(widget)
	Etk_Widget *	widget

void
etk_widget_theme_file_set(widget, theme_file)
	Etk_Widget *	widget
	char *	theme_file

char *
etk_widget_theme_group_get(widget)
	Etk_Widget *	widget

void
etk_widget_theme_group_set(widget, theme_group)
	Etk_Widget *	widget
	char *	theme_group

int
etk_widget_theme_object_data_get(widget, data_name, format, ...)
	Etk_Widget *	widget
	char *	data_name
	char * format	

void
etk_widget_theme_object_min_size_calc(widget, w, h)
	Etk_Widget *	widget
	int *	w
	int *	h

void
etk_widget_theme_object_part_text_set(widget, part_name, text)
	Etk_Widget *	widget
	char *	part_name
	char *	text

void
etk_widget_theme_object_signal_emit(widget, signal_name)
	Etk_Widget *	widget
	char *	signal_name

Etk_Bool
etk_widget_theme_object_swallow(swallowing_widget, part, object)
	Etk_Widget *	swallowing_widget
	char *	part
	Evas_Object *	object

void
etk_widget_theme_object_unswallow(swallowing_widget, object)
	Etk_Widget *	swallowing_widget
	Evas_Object *	object

Etk_Widget *
etk_widget_theme_parent_get(widget)
	Etk_Widget *	widget

void
etk_widget_theme_parent_set(widget, theme_parent)
	Etk_Widget *	widget
	Etk_Widget *	theme_parent

Evas *
etk_widget_toplevel_evas_get(widget)
	Etk_Widget *	widget

Etk_Toplevel_Widget *
etk_widget_toplevel_parent_get(widget)
	Etk_Widget *	widget

Etk_Type *
etk_widget_type_get()

void
etk_widget_unfocus(widget)
	Etk_Widget *	widget

void
etk_widget_unswallow_widget(swallowing_widget, widget)
	Etk_Widget *	swallowing_widget
	Etk_Widget *	widget

Etk_Bool
etk_widget_visibility_locked_get(widget)
	Etk_Widget *	widget

void
etk_widget_visibility_locked_set(widget, visibility_locked)
	Etk_Widget *	widget
	Etk_Bool	visibility_locked

void
etk_window_center_on_window(window_to_center, window)
	Etk_Widget *	window_to_center
	Etk_Widget *	window

Etk_Bool
etk_window_decorated_get(window)
	Etk_Widget *	window

void
etk_window_decorated_set(window, decorated)
	Etk_Widget *	window
	Etk_Bool	decorated

void
etk_window_deiconify(window)
	Etk_Widget *	window

void
etk_window_dnd_aware_set(window, on)
	Etk_Widget *	window
	Etk_Bool	on

void
etk_window_focus(window)
	Etk_Widget *	window

void
etk_window_fullscreen(window)
	Etk_Widget *	window

void
etk_window_geometry_get(window, x, y, w, h)
	Etk_Widget *	window
	int *	x
	int *	y
	int *	w
	int *	h

Etk_Bool
etk_window_hide_on_delete(window, data)
	Etk_Object *	window
	void *	data

void
etk_window_iconify(window)
	Etk_Widget *	window

Etk_Bool
etk_window_is_focused(window)
	Etk_Widget *	window

Etk_Bool
etk_window_is_fullscreen(window)
	Etk_Widget *	window

Etk_Bool
etk_window_is_iconified(window)
	Etk_Widget *	window

Etk_Bool
etk_window_is_maximized(window)
	Etk_Widget *	window

Etk_Bool
etk_window_is_sticky(window)
	Etk_Widget *	window

void
etk_window_maximize(window)
	Etk_Widget *	window

void
etk_window_move(window, x, y)
	Etk_Widget *	window
	int	x
	int	y

void
etk_window_move_to_mouse(window)
	Etk_Widget *	window

Etk_Widget *
etk_window_new()

void
etk_window_resize(window, w, h)
	Etk_Widget *	window
	int	w
	int	h

Etk_Bool
etk_window_shaped_get(window)
	Etk_Widget *	window

void
etk_window_shaped_set(window, shaped)
	Etk_Widget *	window
	Etk_Bool	shaped

Etk_Bool
etk_window_skip_pager_hint_get(window)
	Etk_Widget *	window

void
etk_window_skip_pager_hint_set(window, skip_pager_hint)
	Etk_Widget *	window
	Etk_Bool	skip_pager_hint

Etk_Bool
etk_window_skip_taskbar_hint_get(window)
	Etk_Widget *	window

void
etk_window_skip_taskbar_hint_set(window, skip_taskbar_hint)
	Etk_Widget *	window
	Etk_Bool	skip_taskbar_hint

void
etk_window_stick(window)
	Etk_Widget *	window

char *
etk_window_title_get(window)
	Etk_Widget *	window

void
etk_window_title_set(window, title)
	Etk_Widget *	window
	char *	title

Etk_Type *
etk_window_type_get()

void
etk_window_unfocus(window)
	Etk_Widget *	window

void
etk_window_unfullscreen(window)
	Etk_Widget *	window

void
etk_window_unmaximize(window)
	Etk_Widget *	window
	CODE:
	etk_window_unmaximize(ETK_WINDOW(window));

void
etk_window_unstick(window)
	Etk_Widget *	window
	CODE:
	etk_window_unstick(ETK_WINDOW(window));

void
etk_window_wmclass_set(window, window_name, window_class)
	Etk_Widget *	window
	char *	window_name
	char *	window_class
	CODE:
	etk_window_wmclass_set(ETK_WINDOW(window), window_name, window_class);
	 
Ecore_Timer *
etkpl_timer_add(interval, callback)
        double interval
	SV *    callback
      CODE:        
        RETVAL = ecore_timer_add(interval, callback_timer, newSVsv(callback));
      OUTPUT:
        RETVAL
	
void
etkpl_timer_del(timer)
      Ecore_Timer * timer
    CODE:
      ecore_timer_del(timer);

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>
#include <Ecore.h>
#include <Ecore_Data.h>

#include "EtkTypes.h"
#include "EtkSignals.h"

#define mINT 1 
#define mDOUBLE 2
#define mIMAGE 3
#define mCHECKBOX 4
#define mPROGRESSBAR 5
#define mTEXT 6


static void
notification_callback(Etk_Object * object, const char * property_name, void * data)
{
   dSP;
   Notification_Callback_Data * ncb = NULL;

   ncb = data;

   PUSHMARK(SP);
   XPUSHs(sv_2mortal(newSVObject(object)));
   XPUSHs(sv_2mortal(newSVpv(property_name, strlen(property_name))));
   XPUSHs(sv_2mortal(newSVsv(ncb->perl_data)));
   PUTBACK;

   call_sv(ncb->perl_callback, G_DISCARD);
}

static void
callback_VOID__VOID(Etk_Object *object, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;
   
   PUSHMARK(SP);
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__INT(Etk_Object *object, int value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSViv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__DOUBLE(Etk_Object *object, double value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVnv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;

   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__POINTER(Etk_Object *object, void *value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   SV *event_rv;
   cbd = data;   

   event_rv = GetSignalEvent_POINTER(object, value, cbd);
   
   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(event_rv));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__POINTER_POINTER(Etk_Object *object, void *val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;
   cbd = data;   

   SV * obj1_rv, *event_rv;
   GetSignalEvent_POINTER_POINTER(object, val1, val2, cbd, &obj1_rv, &event_rv);

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(obj1_rv));
   XPUSHs(sv_2mortal(event_rv));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_VOID__INT_POINTER(Etk_Object *object, int val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSViv(val1)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__VOID(Etk_Object *object, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__DOUBLE(Etk_Object *object, double value, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVnv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
callback_BOOL__POINTER_POINTER(Etk_Object *object, void *val1, void *val2, void *data)
{
   dSP;
   Callback_Signal_Data *cbd = NULL;

   cbd = data;

   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
__etk_signal_connect_full(char *signal_name, SV *object, SV *callback, SV *data, Etk_Bool swapped, Etk_Bool after)
{
	dSP;

	Callback_Signal_Data *cbd = NULL;
	Etk_Signal *sig = NULL;
	Etk_Marshaller marsh;
	Etk_Object * obj;

	ENTER;
	SAVETMPS;

	obj = (Etk_Object *)SvObj(object, "Etk::Object");

	cbd = calloc(1, sizeof(Callback_Signal_Data));
	cbd->signal_name = strdup(signal_name);
	cbd->object = obj;
	cbd->perl_object = newSVsv(object);
	cbd->perl_data = newSVsv(data);
	cbd->perl_callback = newSVsv(callback);	
	
	sig = etk_signal_lookup(signal_name, obj->type);
	if(!sig) printf("CANT GET SIG!\n");
	marsh = etk_signal_marshaller_get(sig);

	if(marsh == etk_marshaller_VOID__VOID)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__VOID), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__INT)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__INT), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__DOUBLE)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__DOUBLE), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__POINTER)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__POINTER), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__INT_POINTER)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__INT_POINTER), cbd, swapped, after);
	else if(marsh == etk_marshaller_BOOL__VOID)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_BOOL__VOID), cbd, swapped, after);
	else if(marsh == etk_marshaller_BOOL__DOUBLE)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_BOOL__DOUBLE), cbd, swapped, after);
	else if(marsh == etk_marshaller_BOOL__POINTER_POINTER)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_BOOL__POINTER_POINTER), cbd, swapped, after);
	else if(marsh == etk_marshaller_VOID__POINTER_POINTER)
	  etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__POINTER_POINTER), cbd, swapped, after);
	else
	etk_signal_connect_full(sig, obj, ETK_CALLBACK(callback_VOID__VOID), cbd, swapped, after);

	PUTBACK;
	FREETMPS;
	LEAVE;
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
   if(cbd->perl_data)
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
tree_compare_alpha_cb(Etk_Tree * tree, Etk_Tree_Row * row1, Etk_Tree_Row *row2,
		Etk_Tree_Col * col, void * data )
{
   dSP;
   int ret, cmp;
   char * r1, * r2;

   ENTER;
   SAVETMPS;
   
   if (! (tree && row1 && row2 && col) ) {
	   ret = 0;
   } else {

	   etk_tree_row_fields_get(row1, col, r1, NULL);
	   etk_tree_row_fields_get(row2, col, r2, NULL);
	   cmp = strcmp(r1, r2);
	   if (cmp < 0)
		   ret = -1;
	   else if (cmp > 0)
		   ret = 1;
	   else
		   ret = 0;
   }
		  
   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;
}

int
tree_compare_numeric_cb(Etk_Tree * tree, Etk_Tree_Row * row1, Etk_Tree_Row *row2,
		Etk_Tree_Col * col, void * data )
{
   dSP;
   int r1, r2, ret;

   ENTER;
   SAVETMPS;
   
   if (! (tree && row1 && row2 && col) ) {
	   ret = 0;
   } else {

	   etk_tree_row_fields_get(row1, col, &r1, NULL);
	   etk_tree_row_fields_get(row2, col, &r2, NULL);
	   if (r1 < r2)
		   ret = -1;
	   else if (r1 > r2)
		   ret = 1;
	   else
		   ret = 0;
   }
		  
   PUTBACK;
   FREETMPS;
   LEAVE;

   return ret;
}

int
tree_compare_cb( Etk_Tree * tree, Etk_Tree_Row * row1, Etk_Tree_Row *row2,
Etk_Tree_Col * col, void * data )
{
   dSP;
   Callback_Tree_Compare_Data *cbd;
   int count;
   int ret;   

   ENTER ;
   SAVETMPS;   
   
   cbd = data;
   
   PUSHMARK(SP);  	  
   XPUSHs(sv_2mortal(newSVObj(tree, getClass("Etk_Tree"))));
   XPUSHs(sv_2mortal(newSVObj(row1, getClass("Etk_Tree_Row"))));
   XPUSHs(sv_2mortal(newSVObj(row2, getClass("Etk_Tree_Row"))));
   XPUSHs(sv_2mortal(newSVObj(col, getClass("Etk_Tree_Col"))));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));
   PUTBACK;

   count = call_sv(cbd->perl_callback, G_SCALAR);
   
   SPAGAIN;

   if(count != 1)
       croak("Improper return value from compare callback!\n");

   ret = POPi;

   PUTBACK;
   FREETMPS ;
   LEAVE ; 
   
   return ret;
}

	

MODULE = Etk		PACKAGE = Etk	PREFIX = etk_

Etk_Bool
etk_init()
      ALIAS:
	Init=1
	CODE:
	RETVAL = etk_init(NULL, NULL);
	__etk_perl_init();
	OUTPUT:
	RETVAL

void
etk_shutdown()
      ALIAS:
	Shutdown=1
	CODE:
	etk_shutdown();
	//FreeObjectCache();


MODULE = Etk::Alignment		PACKAGE = Etk::Alignment	PREFIX = etk_alignment_

void
etk_alignment_get(alignment)
	Etk_Alignment *	alignment
      ALIAS:
	Get=1
      PPCODE:
	float xalign;
	float yalign;
	float xscale;
	float yscale;

	etk_alignment_get(alignment, &xalign, &yalign, &xscale, &yscale);
        EXTEND(SP, 4);
        PUSHs(sv_2mortal(newSVnv(xalign)));
        PUSHs(sv_2mortal(newSVnv(yalign)));
        PUSHs(sv_2mortal(newSVnv(xscale)));
        PUSHs(sv_2mortal(newSVnv(yscale)));	

Etk_Alignment *
new(class, xalign=0.5, yalign=0.5, xscale=1, yscale=1)
	SV	*class
	float	xalign
	float	yalign
	float	xscale
	float	yscale
	CODE:
	RETVAL = ETK_ALIGNMENT(etk_alignment_new(xalign, yalign, xscale, yscale));
	OUTPUT:
	RETVAL

void
etk_alignment_set(alignment, xalign, yalign, xscale, yscale)
	Etk_Alignment *	alignment
	float	xalign
	float	yalign
	float	xscale
	float	yscale
      ALIAS:
	Set=1


MODULE = Etk::Bin		PACKAGE = Etk::Bin	PREFIX = etk_bin_

Etk_Widget *
etk_bin_child_get(bin)
	Etk_Bin *	bin
      ALIAS:
	ChildGet=1

void
etk_bin_child_set(bin, child)
	Etk_Bin *	bin
	Etk_Widget *	child
      ALIAS:
	ChildSet=1


MODULE = Etk::Box		PACKAGE = Etk::Box	PREFIX = etk_box_

void
etk_box_child_packing_get(box, child)
	Etk_Box *	box
	Etk_Widget *	child
      ALIAS:
	ChildPackingGet=1
     PPCODE:
       Etk_Box_Fill_Policy   	fill;
       int 	        padding;
       
       etk_box_child_packing_get(box, child, &fill, &padding);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSViv(fill)));
       PUSHs(sv_2mortal(newSViv(padding)));

void
etk_box_child_packing_set(box, child, fill, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Fill_Policy	fill
	int	padding
      ALIAS:
	ChildPackingSet=1

void
etk_box_child_position_get(box, child)
	Etk_Box *	box
	Etk_Widget *	child
      ALIAS:
	ChildPositionGet=1
     PPCODE:
       Etk_Box_Group   	group;
       int 	        pos;
       
       etk_box_child_position_get(box, child, &group, &pos);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSViv(group)));
       PUSHs(sv_2mortal(newSViv(pos)));

void
etk_box_child_position_set(box, child, group, pos)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	int	pos
	ALIAS:
	ChildPositionSet=1

Etk_Bool
etk_box_homogeneous_get(box)
	Etk_Box *	box
      ALIAS:
	HomogeneousGet=1

void
etk_box_homogeneous_set(box, homogeneous)
	Etk_Box *	box
	Etk_Bool	homogeneous
      ALIAS:
	HomogeneousSet=1

void
etk_box_prepend(box, child, group=ETK_BOX_START, fill=ETK_BOX_NONE, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	Etk_Box_Fill_Policy fill
	int	padding
     ALIAS:
	Prepend=1

void
etk_box_append(box, child, group=ETK_BOX_START, fill=ETK_BOX_NONE, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	Etk_Box_Fill_Policy fill
	int	padding
     ALIAS:
	Append=1

void
etk_box_insert(box, child, group, after, fill=ETK_BOX_NONE, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	Etk_Widget *	after
	Etk_Box_Fill_Policy fill
	int	padding
     ALIAS:
	Insert=1

void
etk_box_insert_at(box, child, group, pos, fill=ETK_BOX_NONE, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	int 	pos
	Etk_Box_Fill_Policy fill
	int	padding
     ALIAS:
	InsertAt=1

Etk_Widget *
etk_box_child_get_at(box, group, pos)
	Etk_Box *	box
	Etk_Box_Group	group
	int	pos
	ALIAS:
	ChildGetAt=1
	
int
etk_box_spacing_get(box)
	Etk_Box *	box
      ALIAS:
	SpacingGet=1

void
etk_box_spacing_set(box, spacing)
	Etk_Box *	box
	int	spacing
      ALIAS:
	SpacingSet=1

MODULE = Etk::Button		PACKAGE = Etk::Button	PREFIX = etk_button_

void
etk_button_alignment_get(button)
	Etk_Button *	button
      ALIAS:
	AlignmentGet=1
      PPCODE:	
       float xalign;
       float yalign;
       
       etk_button_alignment_get(button, &xalign, &yalign);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSVnv(xalign)));
       PUSHs(sv_2mortal(newSVnv(yalign)));

void
etk_button_alignment_set(button, xalign, yalign)
	Etk_Button *	button
	float	xalign
	float	yalign
      ALIAS:
	AlignmentSet=1

void
etk_button_click(button)
	Etk_Button *	button
      ALIAS:
	Click=1

Etk_Image *
etk_button_image_get(button)
	Etk_Button *	button
      ALIAS:
	ImageGet=1

void
etk_button_image_set(button, image)
	Etk_Button *	button
	Etk_Image *	image
      ALIAS:
	ImageSet=1

const char *
etk_button_label_get(button)
	Etk_Button *	button
      ALIAS:
	LabelGet=1

void
etk_button_label_set(button, label)
	Etk_Button *	button
	char *	label
      ALIAS:
	LabelSet=1

Etk_Button *
new(class)
	SV	*class
	CODE:
	RETVAL = ETK_BUTTON(etk_button_new());
	OUTPUT:
	RETVAL

Etk_Button *
new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_BUTTON(etk_button_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Button *
new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_BUTTON(etk_button_new_with_label(label));
	OUTPUT:
	RETVAL

void
etk_button_press(button)
	Etk_Button *	button
      ALIAS:
	Press=1

void
etk_button_release(button)
	Etk_Button *	button
      ALIAS:
	Release=1

void
etk_button_set_from_stock(button, stock_id)
	Etk_Button *	button
	Etk_Stock_Id	stock_id
      ALIAS:
	SetFromStock=1

void
etk_button_style_set(button, style)
	Etk_Button * 	button
	Etk_Button_Style	style
	ALIAS:
	StyleSet=1

Etk_Button_Style
etk_button_style_get(button)
	Etk_Button *	button
	ALIAS:
	StyleGet=1

void
etk_button_stock_size_set(button, size)
	Etk_Button *	button
	Etk_Stock_Size	size
	ALIAS:
	StockSizeSet=1

Etk_Stock_Size
etk_button_stock_size_get(button)
	Etk_Button *	button
	ALIAS:
	StockSizeGet=1

MODULE = Etk::Canvas		PACKAGE = Etk::Canvas	PREFIX = etk_canvas_

Etk_Widget *
etk_canvas_new()
      ALIAS:
	New=1

Etk_Bool
etk_canvas_object_add(canvas, object)
	Etk_Widget *	canvas
	Evas_Object *	object
      ALIAS:
	ObjectAdd=1
	CODE:
	Etk_Bool var;
	var = etk_canvas_object_add(ETK_CANVAS(canvas), object);
	RETVAL = var;
	OUTPUT:
	RETVAL

void
etk_canvas_object_remove(canvas, object)
	Etk_Widget *	canvas
	Evas_Object *	object
      ALIAS:
	ObjectRemove=1
	CODE:
	etk_canvas_object_remove(ETK_CANVAS(canvas), object);

void
etk_canvas_object_move(canvas, object, x, y)
	Etk_Widget *    canvas
	Evas_Object *   object
	int	x
	int	y
	ALIAS:
	ObjectMove=1
	CODE:
	etk_canvas_object_move(ETK_CANVAS(canvas), object, x, y);

void
etk_canvas_object_geometry_get(canvas, object)
	Etk_Widget *    canvas
	Evas_Object *   object
	ALIAS:
	GeometryGet=1
	PPCODE:
	int x, y, w, h;
	etk_canvas_object_geometry_get(ETK_CANVAS(canvas), object, &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));


MODULE = Etk::CheckButton		PACKAGE = Etk::CheckButton	PREFIX = etk_check_button_
	
Etk_Check_Button *
new(class)
	SV	*class
	CODE:
	RETVAL = ETK_CHECK_BUTTON(etk_check_button_new());
	OUTPUT:
	RETVAL

Etk_Check_Button *
new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_CHECK_BUTTON(etk_check_button_new_with_label(label));
	OUTPUT:
	RETVAL

MODULE = Etk::Clipboard		PACKAGE = Etk::Clipboard	PREFIX = etk_clipboard_

void
etk_clipboard_text_request(widget)
	Etk_Widget *	widget
      ALIAS:
	TextRequest=1

void
etk_clipboard_text_set(widget, data, length)
	Etk_Widget *	widget
	char *	data
	int	length
      ALIAS:
	TextSet=1


MODULE = Etk::Colorpicker		PACKAGE = Etk::Colorpicker	PREFIX = etk_colorpicker_
	
Etk_Color
etk_colorpicker_current_color_get(cp)
	Etk_Colorpicker *	cp
      ALIAS:
	CurrentColorGet=1
	
void
etk_colorpicker_use_alpha_set(cp, use_alpha)
	Etk_Colorpicker *	cp
	Etk_Bool	use_alpha
	ALIAS:
	UseAlphaSet=1

Etk_Bool
etk_colorpicker_use_alpha_get(cp)
	Etk_Colorpicker *       cp
	ALIAS:
	UseAlphaGet=1

void
etk_colorpicker_current_color_set(cp, color)
	Etk_Colorpicker *	cp
	Etk_Color	color
      ALIAS:
	CurrentColorSet=1
	

Etk_Colorpicker_Mode
etk_colorpicker_mode_get(cp)
	Etk_Colorpicker *	cp
      ALIAS:
	ModeGet=1

void
etk_colorpicker_mode_set(cp, mode)
	Etk_Colorpicker *	cp
	Etk_Colorpicker_Mode	mode
      ALIAS:
	ModeSet=1

Etk_Colorpicker *
new(class)
	SV	*class
	CODE:
	RETVAL = ETK_COLORPICKER(etk_colorpicker_new());
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

MODULE = Etk::Container	PACKAGE = Etk::Container	PREFIX = etk_container_

void
etk_container_remove_all(container)
	Etk_Container * container
	ALIAS:
	RemoveAll=1

void
etk_container_add(container, widget)
	Etk_Container *	container
	Etk_Widget *	widget
      ALIAS:
	Add=1

int
etk_container_border_width_get(container)
	Etk_Container *	container
      ALIAS:
	BorderWidthGet=1

void
etk_container_border_width_set(container, border_width)
	Etk_Container *	container
	int	border_width
      ALIAS:
	BorderWidthSet=1

void
etk_container_child_space_fill(child, child_space, hfill, vfill, xalign, yalign)
	Etk_Widget *	child
	Etk_Geometry *	child_space
	Etk_Bool	hfill
	Etk_Bool	vfill
	float	xalign
	float	yalign
      ALIAS:
	ChildSpaceFill=1
	
Evas_List *
etk_container_children_get(container)
	Etk_Container	*container
      ALIAS:
	ChildrenGet=1

Etk_Bool
etk_container_is_child(container, widget)
	Etk_Container * container
	Etk_Widget * widget
      ALIAS:
	IsChild=1

void
etk_container_remove(container, widget)
	Etk_Container *	container
	Etk_Widget *	widget
      ALIAS:
	Remove=1

MODULE = Etk::Dialog	PACKAGE = Etk::Dialog	PREFIX = etk_dialog_

Etk_Button *
etk_dialog_button_add(dialog, label, response_id)
	Etk_Dialog *	dialog
	char *	label
	int	response_id
      ALIAS:
	ButtonAdd=1
	CODE:
	RETVAL = ETK_BUTTON(etk_dialog_button_add(dialog, label, response_id));
	OUTPUT:
	RETVAL

Etk_Button *
etk_dialog_button_add_from_stock(dialog, stock_id, response_id)
	Etk_Dialog *	dialog
	int	stock_id
	int	response_id
      ALIAS:
	ButtonAddFromStock=1
	CODE:
	RETVAL = ETK_BUTTON(etk_dialog_button_add_from_stock(dialog, stock_id, response_id));
	OUTPUT:
	RETVAL
	
Etk_Bool
etk_dialog_action_area_homogeneous_get(dialog)
	Etk_Dialog *	dialog
	ALIAS:
	ActionAreaHomogeneousGet=1

void
etk_dialog_action_area_homogeneous_set(dialog, homogeneous)
	Etk_Dialog *    dialog
	Etk_Bool  	homogeneous
	ALIAS:
	ActionAreaHomogeneousSet=1

void
etk_dialog_action_area_alignment_set(dialog, align)
	Etk_Dialog *    dialog
	float	align
	ALIAS:
	ActionAreaAlignmentSet=1

float
etk_dialog_action_area_alignment_get(dialog)
	Etk_Dialog *    dialog
	ALIAS:
	ActionAreaAlignmentGet=1

Etk_Bool
etk_dialog_has_separator_get(dialog)
	Etk_Dialog *	dialog
      ALIAS:
	HasSeparatorGet=1

void
etk_dialog_has_separator_set(dialog, has_separator)
	Etk_Dialog *	dialog
	Etk_Bool	has_separator
      ALIAS:
	HasSeparatorSet=1

Etk_Widget *
etk_dialog_main_area_vbox_get(dialog)
	Etk_Dialog *    dialog
	ALIAS:
	MainAreaVboxGet=1

Etk_Widget *
etk_dialog_action_area_hbox_get(dialog)
	Etk_Dialog *    dialog
	ALIAS:
	MainAreaHboxGet=1

void
etk_dialog_button_response_id_set(dialog, button, response_id)
	Etk_Dialog *    dialog
	Etk_Button *	button
	int	response_id
	ALIAS:
	ButtonResponseIdSet=1

int
etk_dialog_button_response_id_get(button)
	Etk_Button *    button
	ALIAS:
	ButtonResponseIdGet=1
	
Etk_Dialog *
new(class)
	SV	* class
	CODE:
	RETVAL = ETK_DIALOG(etk_dialog_new());
	OUTPUT:
	RETVAL

void
etk_dialog_pack_button_in_action_area(dialog, button, response_id, group, fill_policy, padding)
	Etk_Dialog *	dialog
	Etk_Button *	button
	int	response_id
	Etk_Box_Group   group
	Etk_Box_Fill_Policy     fill_policy
	int	padding
      ALIAS:
	PackButtonInActionArea=1

void
etk_dialog_pack_in_main_area(dialog, widget, group, fill_policy, padding)
	Etk_Dialog *	dialog
	Etk_Widget *	widget
	Etk_Box_Group	group
	Etk_Box_Fill_Policy	fill_policy
	int	padding
      ALIAS:
	PackInMainArea=1

void
etk_dialog_pack_widget_in_action_area(dialog, widget, group, fill_policy, padding)
	Etk_Dialog *	dialog
	Etk_Widget *	widget
	Etk_Box_Group	group
	Etk_Box_Fill_Policy	fill_policy
	int	padding
      ALIAS:
	PackWidgetInActionArea=1


MODULE = Etk::Drag	PACKAGE = Etk::Drag	PREFIX = etk_drag_
	
void
etk_drag_begin(drag)
	Etk_Drag *	drag
      ALIAS:
	Begin=1

void
etk_drag_data_set(drag, data, size)
	Etk_Drag *	drag
	SV *	data
      ALIAS:
	DataSet=1
	CODE:
	etk_drag_data_set(drag, newSVsv(data), sizeof(SV));

Etk_Drag *
new(class, widget)
	SV * class
	Etk_Widget *	widget
	CODE:
	RETVAL = ETK_DRAG(etk_drag_new(widget));
	OUTPUT:
	RETVAL

Etk_Widget *
etk_drag_parent_widget_get(drag)
	Etk_Drag *	drag
      ALIAS:
	ParentWidgetGet=1

void
etk_drag_parent_widget_set(drag, widget)
	Etk_Drag *	drag
	Etk_Widget *	widget
      ALIAS:
	ParentWidgetSet=1

void
etk_drag_types_set(drag, types)
	Etk_Drag *	drag
	AV * types
      ALIAS:
	TypesSet=1
	CODE:
	const char **	t;
	unsigned int	num_types;
	int i;
	
	num_types = (unsigned int) av_len(types) + 1;
	t = calloc(num_types, sizeof(char *));
	for (i=0; i<num_types; i++) {
	    SV ** val;
	    val = av_fetch(types, i, 0);
	    if (val) 
		t[i] = (char *)SvIV(*val);
	    else 
		t[i] = 0;
	}   
	
	etk_drag_types_set(drag, t, num_types);


MODULE = Etk::Entry	PACKAGE = Etk::Entry	PREFIX = etk_entry_
	
Etk_Entry *
new(class)
	SV *	class
	CODE:
	RETVAL = ETK_ENTRY(etk_entry_new());
	OUTPUT:
	RETVAL

Etk_Bool
etk_entry_password_mode_get(entry)
	Etk_Entry *	entry
      ALIAS:
	PasswordModeGet=1

void
etk_entry_password_mode_set(entry, on)
	Etk_Entry *	entry
	Etk_Bool	on
      ALIAS:
	PasswordModeSet=1

const char *
etk_entry_text_get(entry)
	Etk_Entry *	entry
      ALIAS:
	TextGet=1

void
etk_entry_text_set(entry, text)
	Etk_Entry *	entry
	char *	text
      ALIAS:
	TextSet=1

void
etk_entry_clear(entry)
	Etk_Entry *	entry
	ALIAS:
	Clear=1

void
etk_entry_image_set(entry, position, image)
	Etk_Entry *	entry
	Etk_Entry_Image_Position position
	Etk_Image *	image
	ALIAS:
	ImageSet=1

Etk_Image *
etk_entry_image_get(entry, position)
	Etk_Entry *	entry
	Etk_Entry_Image_Position position
	ALIAS:
	ImageGet=1

void
etk_entry_image_highlight_set(entry, position, highlight)
	Etk_Entry *	entry
	Etk_Entry_Image_Position position
	Etk_Bool	highlight
	ALIAS:
	ImageHighlightSet=1

void
etk_entry_clear_button_add(entry)
	Etk_Entry *     entry
	ALIAS:
	ClearButtonAdd=1
	

MODULE = Etk::Filechooser	PACKAGE = Etk::Filechooser	PREFIX = etk_filechooser_widget_
	
const char *
etk_filechooser_widget_current_folder_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget
      ALIAS:
	CurrentFolderGet=1

void
etk_filechooser_widget_current_folder_set(filechooser_widget, folder)
	Etk_Filechooser_Widget *	filechooser_widget
	char *	folder
      ALIAS:
	CurrentFolderSet=1

Etk_Filechooser_Widget *
new(class)
	SV * class
      ALIAS:
	New=1
	CODE:
	RETVAL = ETK_FILECHOOSER_WIDGET(etk_filechooser_widget_new());
	OUTPUT:
	RETVAL

Etk_Bool
etk_filechooser_widget_select_multiple_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget
      ALIAS:
	SelectMultipleGet=1

void
etk_filechooser_widget_select_multiple_set(filechooser_widget, select_multiple)
	Etk_Filechooser_Widget *	filechooser_widget
	Etk_Bool	select_multiple
      ALIAS:
	SelectMultipleSet=1

const char *
etk_filechooser_widget_selected_file_get(widget)
	Etk_Filechooser_Widget *	widget
      ALIAS:
	SelectedFileGet=1

void
etk_filechooser_widget_selected_files_get(widget)
	Etk_Filechooser_Widget *	widget
      ALIAS:
	SelectedFilesGet=1
	PPCODE:
	Evas_List * list;

	list = etk_filechooser_widget_selected_files_get(widget);
	XPUSHs(sv_2mortal(newSVCharEvasList(list)));

Etk_Bool
etk_filechooser_widget_show_hidden_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget
      ALIAS:
	ShowHiddenGet=1

void
etk_filechooser_widget_show_hidden_set(filechooser_widget, show_hidden)
	Etk_Filechooser_Widget *	filechooser_widget
	Etk_Bool	show_hidden
      ALIAS:
	ShowHiddenSet=1
	

MODULE = Etk::Frame	PACKAGE = Etk::Frame	PREFIX = etk_frame_

const char *
etk_frame_label_get(frame)
	Etk_Frame *	frame
      ALIAS:
	LabelGet=1

void
etk_frame_label_set(frame, label)
	Etk_Frame *	frame
	char *	label
      ALIAS:
	LabelSet=1

Etk_Frame *
new(class, label)
	SV * class
	char *	label
	CODE:
	RETVAL = ETK_FRAME(etk_frame_new(label));
	OUTPUT:
	RETVAL

MODULE = Etk::Fixed	PACKAGE	= Etk::Fixed	PREFIX = etk_fixed_

Etk_Fixed *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_FIXED(etk_fixed_new());
	OUTPUT:
	RETVAL

void
etk_fixed_put(fixed, widget, x, y)
	Etk_Fixed * fixed
	Etk_Widget * widget
	int x
	int y
	ALIAS:
	Put=1

void
etk_fixed_move(fixed, widget, x, y)
	Etk_Fixed * fixed
	Etk_Widget * widget
	int x
	int y
	ALIAS:
	Move=1

void
etk_fixed_child_position_get(fixed, widget)
	Etk_Fixed * fixed
	Etk_Widget * widget
	ALIAS:
	ChildPositionGet=1
	PPCODE:
	
	int x, y;
	etk_fixed_child_position_get(fixed, widget, &x, &y);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));



MODULE = Etk::HBox	PACKAGE = Etk::HBox	PREFIX = etk_hbox_
	
Etk_HBox *
new(class, homogeneous=ETK_FALSE, spacing=0)
	SV	*class
	Etk_Bool	homogeneous
	int	spacing
	CODE:
	RETVAL = ETK_HBOX(etk_hbox_new(homogeneous, spacing));
	OUTPUT:
	RETVAL

MODULE = Etk::HPaned	PACKAGE = Etk::HPaned	PREFIX = etk_hpaned_

Etk_HPaned *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_HPANED(etk_hpaned_new());
	OUTPUT:
	RETVAL

MODULE = Etk::HScrollbar	PACKAGE = Etk::HScrollbar	PREFIX = etk_hscrollbar_

Etk_HScrollbar *
new(class, lower, upper, value, step_increment, page_increment, page_size)
	SV * class
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	double	page_size
	CODE:
	RETVAL = ETK_HSCROLLBAR(etk_hscrollbar_new(lower, upper, value, 
				step_increment, page_increment, page_size));
	OUTPUT:
	RETVAL


MODULE = Etk::HSeparator	PACKAGE = Etk::HSeparator	PREFIX = etk_hseparator_

Etk_HSeparator *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_HSEPARATOR(etk_hseparator_new());
	OUTPUT:
	RETVAL

MODULE = Etk::Slider	PACKAGE = Etk::Slider	PREFIX = etk_slider_

void
etk_slider_label_set(slider, label)
	Etk_Slider * slider
	const char * label
	ALIAS:
	LabelSet=1

const char *
etk_slider_label_get(slider)
	Etk_Slider * slider
	ALIAS:
	LabelGet=1

void
etk_slider_inverted_set(slider, inverted)
	Etk_Slider * slider
	Etk_Bool inverted
	ALIAS:
	InvertedSet=1

Etk_Bool
etk_slider_inverted_get(slider)
	Etk_Slider * slider
	ALIAS:
	InvertedGet=1

void
etk_slider_update_policy_set(slider, policy)
	Etk_Slider * slider
	Etk_Slider_Update_Policy policy
	ALIAS:
	UpdatePolicySet=1

Etk_Slider_Update_Policy
etk_slider_update_policy_get(slider)
	Etk_Slider * slider
	ALIAS:
	UpdatePolicyGet=1

MODULE = Etk::HSlider	PACKAGE = Etk::HSlider	PREFIX = etk_hslider_

Etk_HSlider *
new(class, lower, upper, value, step_increment, page_increment)
	SV * class
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	CODE:
	RETVAL = ETK_HSLIDER(etk_hslider_new(lower, upper, value, step_increment, page_increment));
	OUTPUT:
	RETVAL

MODULE = Etk::Iconbox	PACKAGE = Etk::Iconbox	PREFIX = etk_iconbox_

Etk_Iconbox *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_ICONBOX(etk_iconbox_new());
	OUTPUT:
	RETVAL

void
etk_iconbox_select_all(iconbox)
	Etk_Iconbox *	iconbox
      ALIAS:
	SelectAll=1

void
etk_iconbox_thaw(iconbox)
	Etk_Iconbox *	iconbox
      ALIAS:
	Thaw=1

void
etk_iconbox_unselect_all(iconbox)
	Etk_Iconbox *	iconbox
      ALIAS:
	UnselectAll=1

Etk_Iconbox_Icon *
etk_iconbox_append(iconbox, filename, edje_group, label)
	Etk_Iconbox *	iconbox
	char *	filename
	char *	edje_group
	char *	label
      ALIAS:
	Append=1

void
etk_iconbox_clear(iconbox)
	Etk_Iconbox *	iconbox
      ALIAS:
	Clear=1

Etk_Iconbox_Model *
etk_iconbox_current_model_get(iconbox)
	Etk_Iconbox *	iconbox
      ALIAS:
	CurrentModelGet=1

void
etk_iconbox_current_model_set(iconbox, model)
	Etk_Iconbox *	iconbox
	Etk_Iconbox_Model *	model
      ALIAS:
	CurrentModelSet=1

void
etk_iconbox_freeze(iconbox)
	Etk_Iconbox *	iconbox
      ALIAS:
	Freeze=1

Etk_Iconbox_Icon *
etk_iconbox_icon_get_at_xy(iconbox, x, y, over_cell, over_icon, over_label)
	Etk_Iconbox *	iconbox
	int	x
	int	y
	Etk_Bool	over_cell
	Etk_Bool	over_icon
	Etk_Bool	over_label
      ALIAS:
	IconGetAtXy=1

Etk_Scrolled_View *
etk_iconbox_scrolled_view_get(iconbox)
	Etk_Iconbox * iconbox
	ALIAS:
	ScrolledViewGet=1

	
MODULE = Etk::Iconbox::Icon	PACKAGE = Etk::Iconbox::Icon	PREFIX = etk_iconbox_icon_

SV *
etk_iconbox_icon_data_get(icon)
	Etk_Iconbox_Icon *	icon
      ALIAS:
	DataGet=1
	CODE:
	RETVAL = newSVsv((SV*)etk_iconbox_icon_data_get(icon));
	OUTPUT:
	RETVAL

void
etk_iconbox_icon_data_set(icon, data)
	Etk_Iconbox_Icon *	icon
	SV *	data
      ALIAS:
	DataSet=1
	CODE:
	etk_iconbox_icon_data_set(icon, newSVsv(data));

void
etk_iconbox_icon_del(icon)
	Etk_Iconbox_Icon *	icon
      ALIAS:
	Del=1

void
etk_iconbox_icon_file_get(icon)
	Etk_Iconbox_Icon *	icon
      ALIAS:
	FileGet=1
	
      PPCODE:
       const char *filename;
       const char *edje_group;

       etk_iconbox_icon_file_get(icon, &filename, &edje_group);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSVpv(filename, strlen(filename))));
       PUSHs(sv_2mortal(newSVpv(edje_group, strlen(edje_group))));

void
etk_iconbox_icon_file_set(icon, filename, edje_group)
	Etk_Iconbox_Icon *	icon
	const char *	filename
	const char *	edje_group
      ALIAS:
	FileSet=1


const char *
etk_iconbox_icon_label_get(icon)
	Etk_Iconbox_Icon *	icon
      ALIAS:
	LabelGet=1

void
etk_iconbox_icon_label_set(icon, label)
	Etk_Iconbox_Icon *	icon
	char *	label
      ALIAS:
	LabelSet=1

void
etk_iconbox_icon_select(icon)
	Etk_Iconbox_Icon *	icon
      ALIAS:
	Select=1

void
etk_iconbox_icon_unselect(icon)
	Etk_Iconbox_Icon *	icon
      ALIAS:
	Unselect=1

Etk_Bool
etk_iconbox_is_selected(icon)
	Etk_Iconbox_Icon *	icon
      ALIAS:
	IsSelected=1


MODULE = Etk::Iconbox::Model	PACKAGE = Etk::Iconbox::Model	PREFIX = etk_iconbox_model_

void
etk_iconbox_model_free(model)
	Etk_Iconbox_Model *	model
      ALIAS:
	Free=1

void
etk_iconbox_model_geometry_get(model)
	Etk_Iconbox_Model *	model
      ALIAS:
	GeometryGet=1
	
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
      ALIAS:
	GeometrySet=1

void
etk_iconbox_model_icon_geometry_get(model)
	Etk_Iconbox_Model *	model
      ALIAS:
	IconGeometryGet=1
      PPCODE:
	int x;
	int y;
	int width;
	int height;
	Etk_Bool fill;
	Etk_Bool keep_aspect_ratio;

	etk_iconbox_model_icon_geometry_get(model, &x, &y, &width, &height,
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
      ALIAS:
	IconGeometrySet=1

void
etk_iconbox_model_label_geometry_get(model)
	Etk_Iconbox_Model *	model
      ALIAS:
	LabelGeometryGet=1
	PPCODE:
	int x;
	int y;
	int width;
	int height;
	float xalign;
	float yalign;

	etk_iconbox_model_label_geometry_get(model, &x, &y, &width, &height,
					&xalign, &yalign);
        EXTEND(SP, 6);
        PUSHs(sv_2mortal(newSViv(x)));
        PUSHs(sv_2mortal(newSViv(y)));
        PUSHs(sv_2mortal(newSViv(width)));
        PUSHs(sv_2mortal(newSViv(height)));
        PUSHs(sv_2mortal(newSVnv(xalign)));
        PUSHs(sv_2mortal(newSVnv(yalign)));	


void
etk_iconbox_model_label_geometry_set(model, x, y, width, height, xalign, yalign)
	Etk_Iconbox_Model *	model
	int	x
	int	y
	int	width
	int	height
	float	xalign
	float	yalign
      ALIAS:
	LabelGeometrySet=1

Etk_Iconbox_Model *
new(class, iconbox)
	SV * class
	Etk_Iconbox *	iconbox
	CODE:
	RETVAL = etk_iconbox_model_new(iconbox);
	OUTPUT:
	RETVAL

MODULE = Etk::Image	PACKAGE = Etk::Image	PREFIX = etk_image_
	
void
etk_image_copy(dest_image, src_image)
	Etk_Image *	dest_image
	Etk_Image *	src_image
      ALIAS:
	Copy=1

void
etk_image_edje_get(image, edje_filename, edje_group)
	Etk_Image *	image
      ALIAS:
	EdjeGet=1
	PPCODE:
	char *	edje_filename;
	char *	edje_group;
	etk_image_edje_get(image, &edje_filename, &edje_group);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVpv(edje_filename, strlen(edje_filename))));
	PUSHs(sv_2mortal(newSVpv(edje_group, strlen(edje_group))));

void
etk_image_file_get(image)
	Etk_Image *	image
      ALIAS:
	FileGet=1
	PPCODE:
	char * filename;
	char * key;
	etk_image_file_get(image, &filename, &key);
	XPUSHs(sv_2mortal(newSVpv(filename, strlen(filename))));
	if (key) XPUSHs(sv_2mortal(newSVpv(key, strlen(key))));

Etk_Bool
etk_image_keep_aspect_get(image)
	Etk_Image *	image
      ALIAS:
	KeepAspectGet=1

void
etk_image_keep_aspect_set(image, keep_aspect)
	Etk_Image *	image
	Etk_Bool	keep_aspect
      ALIAS:
	KeepAspectSet=1

void
etk_image_aspect_ratio_set(image, aspect_ratio)
	Etk_Image *     image
	double		aspect_ratio
	ALIAS:
	AspectRatioSet=1

double
etk_image_aspect_ratio_get(image)
	Etk_Image *     image
	ALIAS:
	AspectRatioGet=1

Etk_Image *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_IMAGE(etk_image_new());
	OUTPUT:
	RETVAL

Etk_Image *
etk_image_new_from_edje(edje_filename, edje_group)
	char *	edje_filename
	char *	edje_group
      ALIAS:
	NewFromEdje=1
	CODE:
	RETVAL = ETK_IMAGE(etk_image_new_from_edje(edje_filename, edje_group));
	OUTPUT:
	RETVAL

Etk_Image *
etk_image_new_from_file(filename, key)
	const char *	filename
	const char * key
      ALIAS:
	NewFromFile=1
	CODE:
	RETVAL = ETK_IMAGE(etk_image_new_from_file(filename, key));
	OUTPUT:
	RETVAL

Etk_Image *
etk_image_new_from_stock(stock_id, stock_size)
	Etk_Stock_Id	stock_id
	int	stock_size
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_IMAGE(etk_image_new_from_stock(stock_id, stock_size));
	OUTPUT:
	RETVAL

void
etk_image_set_from_edje(image, edje_filename, edje_group)
	Etk_Image *	image
	char *	edje_filename
	char *	edje_group
      ALIAS:
	SetFromEdje=1

void
etk_image_set_from_file(image, filename, key)
	Etk_Image *	image
	const char *	filename
	const char *	key
      ALIAS:
	SetFromFile=1

void
etk_image_set_from_stock(image, stock_id, stock_size)
	Etk_Image *	image
	Etk_Stock_Id	stock_id
	Etk_Stock_Size	stock_size
      ALIAS:
	SetFromStock=1

void
etk_image_size_get(image, width, height)
	Etk_Image *	image
      ALIAS:
	SizeGet=1
	PPCODE:
	int 	width;
	int 	height;
	etk_image_size_get(image, &width, &height);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(width)));
	PUSHs(sv_2mortal(newSViv(height)));

void
etk_image_stock_get(image)
	Etk_Image *	image
      ALIAS:
	StockGet=1
	PPCODE:
	Etk_Stock_Id 	stock_id;
	Etk_Stock_Size 	stock_size;
	
	etk_image_stock_get(image, &stock_id, &stock_size);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(stock_id)));
	PUSHs(sv_2mortal(newSViv(stock_size)));

Etk_Image_Source
etk_image_source_get(image)
	Etk_Image *     image
      ALIAS:
      	SourceGet=1

void
etk_image_update(image)
	Etk_Image *     image
	ALIAS:
	Update=1

void
etk_image_update_rect(image, x, y, w, h)
	Etk_Image *     image
	int x
	int y
	int w
	int h
	ALIAS:
	UpdateRect=1



MODULE = Etk::Label	PACKAGE = Etk::Label	PREFIX = etk_label_

void
etk_label_alignment_get(label)
	Etk_Label *	label
      ALIAS:
	AlignmentGet=1
	PPCODE:
	float xalign;
	float yalign;
	etk_label_alignment_get(label, &xalign, &yalign);

	XPUSHs(sv_2mortal(newSVnv(xalign)));
	XPUSHs(sv_2mortal(newSVnv(yalign)));

void
etk_label_alignment_set(label, xalign, yalign)
	Etk_Label *	label
	float	xalign
	float	yalign
      ALIAS:
	AlignmentSet=1

const char *
etk_label_get(label)
	Etk_Label *	label
      ALIAS:
	Get=1

Etk_Label *
new(class, text)
	SV * class
	char *	text
	CODE:
	RETVAL = ETK_LABEL(etk_label_new(text));
	OUTPUT:
	RETVAL

void
etk_label_set(label, text)
	Etk_Label *	label
	const char *	text
      ALIAS:
	Set=1


MODULE = Etk::Main	PACKAGE = Etk::Main	PREFIX = etk_main_

void
etk_main_run()
      ALIAS:
	Run=1
	CODE:
	etk_main();

void
etk_main_iterate()
      ALIAS:
	Iterate=1

void
etk_main_quit()
      ALIAS:
	Quit=1


MODULE = Etk::Menu::Bar	PACKAGE = Etk::Menu::Bar	PREFIX = etk_menu_bar_
	
Etk_Menu_Bar *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_BAR(etk_menu_bar_new());
	OUTPUT:
	RETVAL

MODULE = Etk::Menu::Item	PACKAGE = Etk::Menu::Item	PREFIX = etk_menu_item_

void
etk_menu_item_activate(menu_item)
	Etk_Menu_Item *	menu_item
      ALIAS:
	Activate=1

void
etk_menu_item_deselect(menu_item)
	Etk_Menu_Item *	menu_item
      ALIAS:
	Deselect=1

const char *
etk_menu_item_label_get(menu_item)
	Etk_Menu_Item *	menu_item
      ALIAS:
	LabelGet=1

void
etk_menu_item_label_set(menu_item, label)
	Etk_Menu_Item *	menu_item
	char *	label
      ALIAS:
	LabelSet=1

Etk_Menu_Item *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM(etk_menu_item_new());
	OUTPUT:
	RETVAL

Etk_Menu_Item *
etk_menu_item_new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_MENU_ITEM(etk_menu_item_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Menu_Item *
etk_menu_item_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_MENU_ITEM(etk_menu_item_new_with_label(label));
	OUTPUT:
	RETVAL

void
etk_menu_item_select(menu_item)
	Etk_Menu_Item *	menu_item
      ALIAS:
	Select=1

void
etk_menu_item_set_from_stock(menu_item, stock_id)
	Etk_Menu_Item *	menu_item
	Etk_Stock_Id	stock_id
      ALIAS:
	SetFromStock=1

void
etk_menu_item_submenu_set(menu_item, submenu)
	Etk_Menu_Item *	menu_item
	Etk_Menu *	submenu
      ALIAS:
	SubmenuSet=1

Etk_Menu *
etk_menu_item_submenu_get(menu_item)
	Etk_Menu_Item * menu_item
	ALIAS:
	SubmenuGet=1


MODULE = Etk::Menu::Item::Check	PACKAGE = Etk::Menu::Item::Check	PREFIX = etk_menu_item_check_
	
Etk_Menu_Item_Check *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM_CHECK(etk_menu_item_check_new());
	OUTPUT:
	RETVAL

Etk_Menu_Item_Check *
etk_menu_item_check_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_MENU_ITEM_CHECK(etk_menu_item_check_new_with_label(label));
	OUTPUT:
	RETVAL

Etk_Bool
etk_menu_item_check_active_get(check_item)
	Etk_Menu_Item_Check *	check_item
      ALIAS:
	ActiveGet=1

void
etk_menu_item_check_active_set(check_item, active)
	Etk_Menu_Item_Check *	check_item
	Etk_Bool	active
      ALIAS:
	ActiveSet=1


MODULE = Etk::Menu::Item::Image	PACKAGE = Etk::Menu::Item::Image	PREFIX = etk_menu_item_image_

Etk_Menu_Item_Image *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM_IMAGE(etk_menu_item_image_new());
	OUTPUT:
	RETVAL

Etk_Menu_Item_Image *
etk_menu_item_image_new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_MENU_ITEM_IMAGE(etk_menu_item_image_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Menu_Item_Image *
etk_menu_item_image_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_MENU_ITEM_IMAGE(etk_menu_item_image_new_with_label(label));
	OUTPUT:
	RETVAL

void
etk_menu_item_image_set(image_item, image)
	Etk_Menu_Item_Image *	image_item
	Etk_Image *	image
      ALIAS:
	Set=1


MODULE = Etk::Menu::Item::Radio	PACKAGE = Etk::Menu::Item::Radio	PREFIX = etk_menu_item_radio_
	
Etk_Menu_Item_Radio *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM_RADIO(etk_menu_item_radio_new(NULL));
	OUTPUT:
	RETVAL
	
Etk_Menu_Item_Radio *
etk_menu_item_radio_new_from_widget(radio_item)
	Etk_Menu_Item_Radio *	radio_item
      ALIAS:
	NewFromWidget=1
	CODE:
	RETVAL = ETK_MENU_ITEM_RADIO(etk_menu_item_radio_new_from_widget(radio_item));
	OUTPUT:
	RETVAL

Etk_Menu_Item_Radio *
etk_menu_item_radio_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_MENU_ITEM_RADIO(etk_menu_item_radio_new_with_label(label, NULL));
	OUTPUT:
	RETVAL	

Etk_Menu_Item_Radio *
etk_menu_item_radio_new_with_label_from_widget(label, radio_item)
	char *	label
	Etk_Menu_Item_Radio *	radio_item
      ALIAS:
	NewWithLabelFromWidget=1
	CODE:
	RETVAL = ETK_MENU_ITEM_RADIO(etk_menu_item_radio_new_with_label_from_widget(label, 
				radio_item));
	OUTPUT:
	RETVAL

MODULE = Etk::Menu::Item::Separator	PACKAGE = Etk::Menu::Item::Separator	PREFIX = etk_menu_item_separator_

Etk_Menu_Item_Separator *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM_SEPARATOR(etk_menu_item_separator_new());
	OUTPUT:
	RETVAL


MODULE = Etk::Menu	PACKAGE = Etk::Menu	PREFIX = etk_menu_

Etk_Menu *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU(etk_menu_new());
	OUTPUT:
	RETVAL

void
etk_menu_popdown(menu)
	Etk_Menu *	menu
      ALIAS:
	Popdown=1

void
etk_menu_popup(menu)
	Etk_Menu *	menu
      ALIAS:
	Popup=1

void
etk_menu_popup_at_xy(menu, x, y)
	Etk_Menu *	menu
	int	x
	int	y
      ALIAS:
	PopupAtXy=1

void
etk_menu_popup_in_direction(menu, direction)
	Etk_Menu *menu
	Etk_Popup_Direction direction
	ALIAS:
	PopupInDirection=1
	
void
etk_menu_popup_at_xy_in_direction(menu, x, y, direction)
	Etk_Menu *menu
	int	x
	int	y
	Etk_Popup_Direction direction
	ALIAS:
	PopupAtXYInDirection=1

Etk_Menu_Item *
etk_menu_parent_item_get(menu)
	Etk_Menu *menu
	ALIAS:
	ParentItemGet=1


MODULE = Etk::Menu::Shell	PACKAGE = Etk::Menu::Shell	PREFIX = etk_menu_shell_

void
etk_menu_shell_append(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
      ALIAS:
	Append=1

void
etk_menu_shell_append_relative(menu_shell, item, relative)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	Etk_Menu_Item *	relative
      ALIAS:
	AppendRelative=1

void
etk_menu_shell_insert(menu_shell, item, position)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	int	position
      ALIAS:
	Insert=1

Evas_List *
etk_menu_shell_items_get(menu_shell)
	Etk_Menu_Shell *	menu_shell
      ALIAS:
	ItemsGet=1

void
etk_menu_shell_prepend(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
      ALIAS:
	Prepend=1

void
etk_menu_shell_prepend_relative(menu_shell, item, relative)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	Etk_Menu_Item *	relative
      ALIAS:
	PrependRelative=1

void
etk_menu_shell_remove(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
      ALIAS:
	Remove=1
	
MODULE = Etk::MessageDialog	PACKAGE = Etk::MessageDialog	PREFIX = etk_message_dialog_

Etk_Message_Dialog_Buttons
etk_message_dialog_buttons_get(dialog)
	Etk_Message_Dialog *	dialog
      ALIAS:
	ButtonsGet=1

void
etk_message_dialog_buttons_set(dialog, buttons)
	Etk_Message_Dialog *	dialog
	Etk_Message_Dialog_Buttons	buttons
      ALIAS:
	ButtonsSet=1

Etk_Message_Dialog_Type
etk_message_dialog_message_type_get(dialog)
	Etk_Message_Dialog *	dialog
      ALIAS:
	MessageTypeGet=1

void
etk_message_dialog_message_type_set(dialog, type)
	Etk_Message_Dialog *	dialog
	Etk_Message_Dialog_Type	type
      ALIAS:
	MessageTypeSet=1

Etk_Message_Dialog *
new(class, message_type, buttons, text)
	SV * class
	Etk_Message_Dialog_Type	message_type
	Etk_Message_Dialog_Buttons	buttons
	char *	text
	CODE:
	RETVAL = ETK_MESSAGE_DIALOG(etk_message_dialog_new(message_type, buttons, text));
	OUTPUT:
	RETVAL

const char *
etk_message_dialog_text_get(dialog)
	Etk_Message_Dialog *	dialog
      ALIAS:
	TextGet=1

void
etk_message_dialog_text_set(dialog, text)
	Etk_Message_Dialog *	dialog
	char *	text
      ALIAS:
	TextSet=1

MODULE = Etk::Notebook	PACKAGE = Etk::Notebook	PREFIX = etk_notebook_
	
int
etk_notebook_current_page_get(notebook)
	Etk_Notebook *	notebook
      ALIAS:
	CurrentPageGet=1

void
etk_notebook_current_page_set(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	CurrentPageSet=1

Etk_Notebook *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_NOTEBOOK(etk_notebook_new());
	OUTPUT:
	RETVAL

int
etk_notebook_num_pages_get(notebook)
	Etk_Notebook *	notebook
      ALIAS:
	NumPagesGet=1

int
etk_notebook_page_append(notebook, tab_label, page_child)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child
      ALIAS:
	PageAppend=1

Etk_Widget *
etk_notebook_page_child_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	PageChildGet=1

void
etk_notebook_page_child_set(notebook, page_num, child)
	Etk_Notebook *	notebook
	int	page_num
	Etk_Widget *	child
      ALIAS:
	PageChildSet=1

int
etk_notebook_page_index_get(notebook, child)
	Etk_Notebook *	notebook
	Etk_Widget *	child
      ALIAS:
	PageIndexGet=1

int
etk_notebook_page_insert(notebook, tab_label, page_child, position)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child
	int	position
      ALIAS:
	PageInsert=1

int
etk_notebook_page_next(notebook)
	Etk_Notebook *	notebook
      ALIAS:
	PageNext=1

int
etk_notebook_page_prepend(notebook, tab_label, page_child)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child
      ALIAS:
	PagePrepend=1

int
etk_notebook_page_prev(notebook)
	Etk_Notebook *	notebook
      ALIAS:
	PagePrev=1

void
etk_notebook_page_remove(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	PageRemove=1

const char *
etk_notebook_page_tab_label_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	PageTabLabelGet=1

void
etk_notebook_page_tab_label_set(notebook, page_num, tab_label)
	Etk_Notebook *	notebook
	int	page_num
	char *	tab_label
      ALIAS:
	PageTabLabelSet=1

Etk_Widget *
etk_notebook_page_tab_widget_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	PageTabWidgetGet=1

void
etk_notebook_page_tab_widget_set(notebook, page_num, tab_widget)
	Etk_Notebook *	notebook
	int	page_num
	Etk_Widget *	tab_widget
      ALIAS:
	PageTabWidgetSet=1

void
etk_notebook_tabs_visible_set(notebook, visible)
	Etk_Notebook * notebook
	Etk_Bool visible
      ALIAS:
	TabsVisibleSet=1

Etk_Bool
etk_notebook_tabs_visible_get(notebook)
	Etk_Notebook * notebook
      ALIAS:
	TabsVisibleGet=1

MODULE = Etk::Object	PACKAGE = Etk::Object	PREFIX = etk_object_

SV *
etk_object_data_get(object, key)
	Etk_Object *	object
	char *	key
      ALIAS:
	DataGet=1
	CODE:
	RETVAL = newSVsv((SV*)etk_object_data_get(object, key));
	OUTPUT:
	RETVAL

void
etk_object_data_set(object, key, value)
	Etk_Object *	object
	char *	key
	SV *	value
      ALIAS:
	DataSet=1
	CODE:
	etk_object_data_set(object, key, newSVsv(value));

void
etk_object_notification_callback_add(object, property_name, callback, data)
	Etk_Object *	object
	char *	property_name
	SV *	callback
	SV *	data
      ALIAS:
	NotificationCallbackAdd=1

	CODE:
	Notification_Callback_Data *ncb = NULL;

	ncb = calloc(1, sizeof(Notification_Callback_Data));
	ncb->object = object;
	ncb->perl_data = newSVsv(data);
	ncb->perl_callback = newSVsv(callback);

	etk_object_notification_callback_add(object, property_name, notification_callback, ncb);

void
etk_object_notification_callback_remove(object, property_name, callback)
	Etk_Object *	object
	char *	property_name
      ALIAS:
	NotificationCallbackRemove=1
	CODE:
	etk_object_notification_callback_remove(object, property_name, notification_callback);

void
etk_object_notify(object, property_name)
	Etk_Object *	object
	char *	property_name
      ALIAS:
	Notify=1


void
signal_connect(object, signal_name, callback, data=NULL)
	SV *		object
	char *	        signal_name
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnect=1
       
	CODE:	
	__etk_signal_connect_full(signal_name, newSVsv(object), newSVsv(callback), newSVsv(data), 
			ETK_FALSE, ETK_FALSE);

void
signal_connect_after(object, signal_name, callback, data=NULL)
	SV *		object
	char *	        signal_name
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnectAfter=1
	
	CODE:	
	__etk_signal_connect_full(signal_name, object, callback, data, ETK_FALSE, ETK_TRUE);

void 
signal_connect_full(object, signal_name, callback, data, swapped, after)
	SV *		object
	char *	        signal_name
	SV *	        callback
	SV *            data
	Etk_Bool	swapped
	Etk_Bool	after
      ALIAS:
	SignalConnectFull=1
	CODE:
	__etk_signal_connect_full(signal_name, object, callback, data, swapped, after);

	
void
signal_connect_swapped(object, signal_name, callback, data=NULL)
	SV *		object
	char *	        signal_name
	SV *	        callback
	SV *            data
      ALIAS:
	SignalConnectSwapped=1
	
	CODE:	
	__etk_signal_connect_full(signal_name, object, callback, data, ETK_TRUE, ETK_FALSE);

void
signal_disconnect(object, signal_name, callback)
	SV *		object
	char *	        signal_name
	SV *	        callback
      ALIAS:
	SignalDisconnect=1
	
	CODE:	
	Etk_Signal *sig = NULL;
	Etk_Marshaller marsh;
	Etk_Object * obj;

	obj = (Etk_Object *) SvObj(object, "Etk::Object");
	
	sig = etk_signal_lookup(signal_name, obj->type);
	if(!sig) printf("CANT GET SIG!\n");
	marsh = etk_signal_marshaller_get(sig);
	
	if(marsh == etk_marshaller_VOID__VOID)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__VOID));
	else if(marsh == etk_marshaller_VOID__INT)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__INT));
	else if(marsh == etk_marshaller_VOID__DOUBLE)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__DOUBLE));
	else if(marsh == etk_marshaller_VOID__POINTER)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__POINTER));
	else if(marsh == etk_marshaller_VOID__INT_POINTER)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__INT_POINTER));
	else if(marsh == etk_marshaller_BOOL__VOID)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_BOOL__VOID));
	else if(marsh == etk_marshaller_BOOL__DOUBLE)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_BOOL__DOUBLE));
	else if(marsh == etk_marshaller_BOOL__POINTER_POINTER)
	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_BOOL__POINTER_POINTER));
	else
 	  etk_signal_disconnect(signal_name, obj, ETK_CALLBACK(callback_VOID__VOID));

void
signal_disconnect_all(object, signal_name)
	SV *		object
	char *	        signal_name
      ALIAS:
	SignalDisconnectAll=1
	
	CODE:	
	Etk_Object * obj;
	obj = (Etk_Object *) SvObj(object, "Etk::Object");
	etk_signal_disconnect_all(signal_name, obj);

const char *
etk_object_name_get(object)
	Etk_Object *	object
      ALIAS:
	NameGet=1

Etk_Object *
etk_object_name_find(name)
	const char * name
	ALIAS:
	NameFind=1

void
etk_object_name_set(object, name)
	Etk_Object *	object
	char *	name
      ALIAS:
	NameSet=1

void
etk_object_destroy(object)
	Etk_Object *    object
	ALIAS:
	Destroy=1

void
etk_object_shutdown()
	ALIAS:
	Shutdown=1

void
etk_object_purge()
	ALIAS:
	Purge=1

MODULE = Etk::Paned	PACKAGE = Etk::Paned	PREFIX = etk_paned_
	
Etk_Widget *
etk_paned_child1_get(paned)
	Etk_Paned *	paned
      ALIAS:
	Child1Get=1

void
etk_paned_child1_set(paned, child, expand)
	Etk_Paned *	paned
	Etk_Widget *	child
	Etk_Bool	expand
      ALIAS:
	Child1Set=1

Etk_Widget *
etk_paned_child2_get(paned)
	Etk_Paned *	paned
      ALIAS:
	Child2Get=1

void
etk_paned_child2_set(paned, child, expand)
	Etk_Paned *	paned
	Etk_Widget *	child
	Etk_Bool	expand
      ALIAS:
	Child2Set=1

int
etk_paned_position_get(paned)
	Etk_Paned *	paned
      ALIAS:
	PositionGet=1

void
etk_paned_position_set(paned, position)
	Etk_Paned *	paned
	int	position
      ALIAS:
	PositionSet=1

void
etk_paned_child1_expand_set(paned, expand)
	Etk_Paned *	paned
	Etk_Bool	expand
	ALIAS:
	Child1ExpandSet=1

void
etk_paned_child2_expand_set(paned, expand)
	Etk_Paned *	paned
	Etk_Bool	expand
	ALIAS:
	Child2ExpandSet=1

Etk_Bool
etk_paned_child1_expand_get(paned)
	Etk_Paned *	paned
	ALIAS:
	Child1ExpandGet=1

Etk_Bool
etk_paned_child2_expand_get(paned)
	Etk_Paned *	paned
	ALIAS:
	Child2ExpandGet=1


MODULE = Etk::PopupWindow	PACKAGE = Etk::PopupWindow	PREFIX = etk_popup_window_
	
Etk_Popup_Window *
etk_popup_window_focused_window_get()
      ALIAS:
	FocusedWindowGet=1

void
etk_popup_window_focused_window_set(popup_window)
	Etk_Popup_Window *	popup_window
      ALIAS:
	FocusedWindowSet=1

Etk_Bool
etk_popup_window_is_popped_up(popup_window)
	Etk_Popup_Window *	popup_window
      ALIAS:
	IsPoppedUp=1

void
etk_popup_window_popdown(popup_window)
	Etk_Popup_Window *	popup_window
      ALIAS:
	Popdown=1

void
etk_popup_window_popdown_all()
      ALIAS:
	PopdownAll=1

void
etk_popup_window_popup(popup_window)
	Etk_Popup_Window *	popup_window
      ALIAS:
	Popup=1

void
etk_popup_window_popup_at_xy(popup_window, x, y)
	Etk_Popup_Window *	popup_window
	int	x
	int	y
      ALIAS:
	PopupAtXy=1

void
etk_popup_window_parent_set(popup_window, parent)
	Etk_Popup_Window *      popup_window
	Etk_Popup_Window *      parent
	ALIAS:
	ParentSet=1

Etk_Popup_Window *
etk_popup_window_parent_get(popup_window)
	Etk_Popup_Window *      popup_window
	ALIAS:
	ParentGet=1

void
etk_popup_window_popup_in_direction(popup_window, direction)
	Etk_Popup_Window *      popup_window
	Etk_Popup_Direction	direction
	ALIAS:
	PopupInDirection=1

void
etk_popup_window_popup_at_xy_in_direction(popup_window, x, y, direction)
	Etk_Popup_Window *      popup_window
	int	x
	int	y
	Etk_Popup_Direction     direction
	ALIAS:
	PopupAtXYInDirection=1

	
MODULE = Etk::ProgressBar	PACKAGE = Etk::ProgressBar	PREFIX = etk_progress_bar_
	
double
etk_progress_bar_fraction_get(progress_bar)
	Etk_Progress_Bar *	progress_bar
      ALIAS:
	FractionGet=1

void
etk_progress_bar_fraction_set(progress_bar, fraction)
	Etk_Progress_Bar *	progress_bar
	double	fraction
      ALIAS:
	FractionSet=1

Etk_Progress_Bar *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_PROGRESS_BAR(etk_progress_bar_new());
	OUTPUT:
	RETVAL

Etk_Progress_Bar *
etk_progress_bar_new_with_text(label)
	char *	label
      ALIAS:
	NewWithText=1
	CODE:
	RETVAL = ETK_PROGRESS_BAR(etk_progress_bar_new_with_text(label));
	OUTPUT:
	RETVAL

void
etk_progress_bar_pulse(progress_bar)
	Etk_Progress_Bar *	progress_bar
      ALIAS:
	Pulse=1

double
etk_progress_bar_pulse_step_get(progress_bar)
	Etk_Progress_Bar *	progress_bar
      ALIAS:
	PulseStepGet=1

void
etk_progress_bar_pulse_step_set(progress_bar, pulse_step)
	Etk_Progress_Bar *	progress_bar
	double	pulse_step
      ALIAS:
	PulseStepSet=1

const char *
etk_progress_bar_text_get(progress_bar)
	Etk_Progress_Bar *	progress_bar
      ALIAS:
	TextGet=1

void
etk_progress_bar_text_set(progress_bar, label)
	Etk_Progress_Bar *	progress_bar
	char *	label
      ALIAS:
	TextSet=1

void
etk_progress_bar_direction_set(progress_bar, direction)
	Etk_Progress_Bar * progress_bar
	Etk_Progress_Bar_Direction direction
      ALIAS:
	DirectionSet=1

Etk_Progress_Bar_Direction
etk_progress_bar_direction_get(progress_bar)
	Etk_Progress_Bar * progress_bar
      ALIAS:
	DirectionGet=1

MODULE = Etk::RadioButton	PACKAGE = Etk::RadioButton	PREFIX = etk_radio_button_

Etk_Widget *
etk_radio_button_new(group)
	Evas_List **	group
      ALIAS:
	New=1

Etk_Radio_Button *
etk_radio_button_new_from_widget(radio_button)
	Etk_Radio_Button *	radio_button
      ALIAS:
	NewFromWidget=1
	CODE:
	RETVAL = ETK_RADIO_BUTTON(etk_radio_button_new_from_widget(radio_button));
	OUTPUT:
	RETVAL

Etk_Radio_Button *
etk_radio_button_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_RADIO_BUTTON(etk_radio_button_new_with_label(label, NULL));
	OUTPUT:
	RETVAL
	
Etk_Radio_Button *
etk_radio_button_new_with_label_from_widget(label, radio_button)
	char *	label
	Etk_Radio_Button *	radio_button
      ALIAS:
	NewWithLabelFromWidget=1
	CODE:
	RETVAL = ETK_RADIO_BUTTON(etk_radio_button_new_with_label_from_widget(label, radio_button));
	OUTPUT:
	RETVAL

MODULE = Etk::Range	PACKAGE = Etk::Range	PREFIX = etk_range_

void
etk_range_increments_set(range, step, page)
	Etk_Range *	range
	double	step
	double	page
      ALIAS:
	IncrementsSet=1

void
etk_range_increments_get(range)
	Etk_Range * range
      ALIAS:
	IncrementsGet=1
	PPCODE:
	double step, page;
	etk_range_increments_get(range, &step, &page);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(step)));
	PUSHs(sv_2mortal(newSVnv(page)));

double
etk_range_page_size_get(range)
	Etk_Range *	range
      ALIAS:
	PageSizeGet=1

void
etk_range_page_size_set(range, page_size)
	Etk_Range *	range
	double	page_size
      ALIAS:
	PageSizeSet=1

void
etk_range_range_set(range, lower, upper)
	Etk_Range *	range
	double	lower
	double	upper
      ALIAS:
	RangeSet=1

void
etk_range_range_get(range)
	Etk_Range * range
      ALIAS:
	RangeGet=1
	PPCODE:
	double lower, upper;
	etk_range_range_get(range, &lower, &upper);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(lower)));
	PUSHs(sv_2mortal(newSVnv(upper)));

double
etk_range_value_get(range)
	Etk_Range *	range
      ALIAS:
	ValueGet=1

Etk_Bool
etk_range_value_set(range, value)
	Etk_Range *	range
	double	value
      ALIAS:
	ValueSet=1

MODULE = Etk::ScrolledView	PACKAGE = Etk::ScrolledView	PREFIX = etk_scrolled_view_

void
etk_scrolled_view_add_with_viewport(scrolled_view, child)
	Etk_Scrolled_View *	scrolled_view
	Etk_Widget *	child
      ALIAS:
	AddWithViewport=1

Etk_Range *
etk_scrolled_view_hscrollbar_get(scrolled_view)
	Etk_Scrolled_View *	scrolled_view
      ALIAS:
	HScrollbarGet=1

Etk_Scrolled_View *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_SCROLLED_VIEW(etk_scrolled_view_new());
	OUTPUT:
	RETVAL

void
etk_scrolled_view_policy_get(scrolled_view)
	Etk_Scrolled_View *	scrolled_view
      ALIAS:
	PolicyGet=1
	PPCODE:
	Etk_Scrolled_View_Policy hpolicy;
	Etk_Scrolled_View_Policy vpolicy;

	etk_scrolled_view_policy_get(scrolled_view, &hpolicy, &vpolicy);
	XPUSHs(sv_2mortal(newSViv(hpolicy)));
	XPUSHs(sv_2mortal(newSViv(vpolicy)));

void
etk_scrolled_view_policy_set(scrolled_view, hpolicy, vpolicy)
	Etk_Scrolled_View *	scrolled_view
	Etk_Scrolled_View_Policy	hpolicy
	Etk_Scrolled_View_Policy	vpolicy
      ALIAS:
	PolicySet=1

Etk_Range *
etk_scrolled_view_vscrollbar_get(scrolled_view)
	Etk_Scrolled_View *	scrolled_view
      ALIAS:
	VScrollbarGet=1


MODULE = Etk::Selection	PACKAGE = Etk::Selection	PREFIX = etk_selection_

void
etk_selection_text_request(selection, widget)
	Etk_Selection_Type selection
	Etk_Widget *	widget
      ALIAS:
	TextRequest=1

void
etk_selection_text_set(selection, text)
	Etk_Selection_Type selection
	char *	text
      ALIAS:
	TextSet=1

void
etk_selection_clear(selection)
	Etk_Selection_Type selection
	ALIAS:
	Clear=1
	
MODULE = Etk::Signal	PACKAGE = Etk::Signal	PREFIX = etk_signal_
	
void
etk_signal_shutdown()
      ALIAS:
	Shutdown=1

void
etk_signal_stop()
      ALIAS:
	Stop=1


MODULE = Etk::StatusBar	PACKAGE = Etk::StatusBar	PREFIX = etk_statusbar_

int
etk_statusbar_context_id_get(statusbar, context)
	Etk_Statusbar *	statusbar
	char *	context
      ALIAS:
	ContextIdGet=1

Etk_Bool
etk_statusbar_has_resize_grip_get(statusbar)
	Etk_Statusbar *	statusbar
      ALIAS:
	HasResizeGripGet=1

void
etk_statusbar_has_resize_grip_set(statusbar, has_resize_grip)
	Etk_Statusbar *	statusbar
	Etk_Bool	has_resize_grip
      ALIAS:
	HasResizeGripSet=1

Etk_Statusbar *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_STATUSBAR(etk_statusbar_new());
	OUTPUT:
	RETVAL

void
etk_statusbar_message_pop(statusbar, context_id)
	Etk_Statusbar *	statusbar
	int	context_id
      ALIAS:
	MessagePop=1

int
etk_statusbar_message_push(statusbar, message, context_id)
	Etk_Statusbar *	statusbar
	char *	message
	int	context_id
      ALIAS:
	MessagePush=1

void
etk_statusbar_message_remove(statusbar, message_id)
	Etk_Statusbar *	statusbar
	int	message_id
      ALIAS:
	MessageRemove=1

void
etk_statusbar_message_get(statusbar)
	Etk_Statusbar *	statusbar
      ALIAS:
	MessageGet=1
	PPCODE:
	const char ** message;
	int mid;
	int cid;
	etk_statusbar_message_get(statusbar, message, &mid, &cid);
	EXTEND(SP, 3);
	PUSHs(sv_2mortal(newSVpv(*message, strlen(*message))));
	PUSHs(sv_2mortal(newSViv(mid)));
	PUSHs(sv_2mortal(newSViv(cid)));
	

MODULE = Etk::Stock	PACKAGE = Etk::Stock	PREFIX = etk_stock_
	
const char *
etk_stock_key_get(stock_id, size)
	Etk_Stock_Id	stock_id
	Etk_Stock_Size	size
      ALIAS:
	KeyGet=1

const char *
etk_stock_label_get(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	LabelGet=1

MODULE = Etk::Table	PACKAGE = Etk::Table	PREFIX = etk_table_

void
etk_table_attach(table, child, left_attach, right_attach, top_attach, bottom_attach, x_padding, y_padding, fill_policy)
	Etk_Table *	table
	Etk_Widget *	child
	int	left_attach
	int	right_attach
	int	top_attach
	int	bottom_attach
	int	x_padding
	int	y_padding
	Etk_Table_Fill_Policy	fill_policy
      ALIAS:
	Attach=1

void
etk_table_attach_default(table, child, left_attach, right_attach, top_attach, bottom_attach)
	Etk_Table *	table
	Etk_Widget *	child
	int	left_attach
	int	right_attach
	int	top_attach
	int	bottom_attach
      ALIAS:
	AttachDefault=1

void
etk_table_cell_clear(table, col, row)
	Etk_Table *	table
	int	col
	int	row
      ALIAS:
	CellClear=1

Etk_Bool
etk_table_homogeneous_get(table)
	Etk_Table *	table
      ALIAS:
	HomogeneousGet=1

void
etk_table_homogeneous_set(table, homogeneous)
	Etk_Table *	table
	Etk_Bool	homogeneous
      ALIAS:
	HomogeneousSet=1

Etk_Table *
new(class, num_cols, num_rows, homogeneous)
	SV * class
	int	num_cols
	int	num_rows
	Etk_Bool	homogeneous
	CODE:
	RETVAL = ETK_TABLE(etk_table_new(num_cols, num_rows, homogeneous));
	OUTPUT:
	RETVAL

void
etk_table_resize(table, num_cols, num_rows)
	Etk_Table *	table
	int	num_cols
	int	num_rows
      ALIAS:
	Resize=1

MODULE = Etk::Toolbar	PACKAGE = Etk::Toolbar	PREFIX = etk_toolbar_

Etk_Toolbar *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TOOLBAR(etk_toolbar_new());
	OUTPUT:
	RETVAL

void
etk_toolbar_append(toolbar, widget)
	Etk_Toolbar * toolbar
	Etk_Widget * widget
	ALIAS:
	Append=1

void
etk_toolbar_prepend(toolbar, widget)
	Etk_Toolbar * toolbar
	Etk_Widget * widget
	ALIAS:
	Prepend=1

void
etk_toolbar_orientation_set(toolbar, orientation)
	Etk_Toolbar * toolbar
	Etk_Toolbar_Orientation orientation
	ALIAS:
	OrientationSet=1

Etk_Toolbar_Orientation
etk_toolbar_orientation_get(toolbar)
	Etk_Toolbar * toolbar
	ALIAS:
	OrientationGet=1

void
etk_toolbar_style_set(toolbar, style)
	Etk_Toolbar * toolbar
	Etk_Toolbar_Style style
	ALIAS:
	StyleSet=1

Etk_Toolbar_Style
etk_toolbar_style_get(toolbar)
	Etk_Toolbar * toolbar
	ALIAS:
	StyleGet=1

void
etk_toolbar_stock_size_set(toolbar, size)
	Etk_Toolbar * toolbar
	Etk_Stock_Size size
	ALIAS:
	StockSizeSet=1

Etk_Stock_Size
etk_toolbar_stock_size_get(toolbar)
	Etk_Toolbar * toolbar
	ALIAS:
	StockSizeGet=1

MODULE = Etk::ToolButton	PACKAGE = Etk::ToolButton	PREFIX = etk_tool_button_

Etk_Tool_Button *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TOOL_BUTTON(etk_tool_button_new());
	OUTPUT:
	RETVAL

Etk_Tool_Button *
new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_TOOL_BUTTON(etk_tool_button_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Tool_Button *
new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_TOOL_BUTTON(etk_tool_button_new_with_label(label));
	OUTPUT:
	RETVAL

MODULE = Etk::ToolToggleButton	PACKAGE = Etk::ToolToggleButton	PREFIX = etk_tool_toggle_button_

Etk_Tool_Toggle_Button *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TOOL_TOGGLE_BUTTON(etk_tool_toggle_button_new());
	OUTPUT:
	RETVAL

Etk_Tool_Toggle_Button *
new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_TOOL_TOGGLE_BUTTON(etk_tool_toggle_button_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Tool_Toggle_Button *
new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_TOOL_TOGGLE_BUTTON(etk_tool_toggle_button_new_with_label(label));
	OUTPUT:
	RETVAL


MODULE = Etk::TextView	PACKAGE = Etk::TextView	PREFIX = etk_text_view_

Etk_Text_View *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TEXT_VIEW(etk_text_view_new());
	OUTPUT:
	RETVAL
	

Etk_Textblock *
etk_text_view_textblock_get(text_view)
	Etk_Text_View * text_view
      ALIAS:
	TextblockGet=1

Etk_Textblock_Iter *
etk_text_view_cursor_get(text_view)
	Etk_Text_View *text_view
      ALIAS:
	CursorGet=1

Etk_Textblock_Iter *
etk_text_view_selection_bound_get(text_view)
	Etk_Text_View *text_view
      ALIAS:
	SelectionBoundGet=1


MODULE = Etk::TextBlock::Iter	PACKAGE = Etk::TextBlock::Iter	PREFIX = etk_textblock_iter_

void
etk_textblock_iter_copy(iter, dest_iter)
	Etk_Textblock_Iter *	iter
	Etk_Textblock_Iter *	dest_iter
      ALIAS:
	Copy=1

void
etk_textblock_iter_free(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	Free=1

void 
etk_textblock_iter_gravity_set(iter, gravity)
	Etk_Textblock_Iter *iter
	Etk_Textblock_Gravity gravity
      ALIAS:
	GravitySet=1

Etk_Textblock_Gravity
etk_textblock_iter_gravity_get(iter)
	Etk_Textblock_Iter *iter
      ALIAS:
	GravityGet=1

void
etk_textblock_iter_forward_end(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	ForwardEnd=1

void
etk_textblock_iter_backward_char(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	BackwardChar=1

void
etk_textblock_iter_forward_char(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	ForwardChar=1

void
etk_textblock_iter_backward_start(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	BackwardStart=1

int
etk_textblock_iter_compare(iter1, iter2)
	Etk_Textblock_Iter *iter1
	Etk_Textblock_Iter *iter2
      ALIAS:
	Compare=1

Etk_Textblock_Iter *
new(class, textblock)
	SV * class
	Etk_Textblock *	textblock
	CODE:
	RETVAL = etk_textblock_iter_new(textblock);
	OUTPUT:
	RETVAL

MODULE = Etk::TextBlock	PACKAGE = Etk::TextBlock	PREFIX = etk_textblock_

Etk_Textblock *
new(class)
	SV * class
	CODE:
	RETVAL = etk_textblock_new();
	OUTPUT:
	RETVAL

void
etk_textblock_text_set(textblock, text, markup)
	Etk_Textblock *	textblock
	char *	text
        Etk_Bool markup
      ALIAS:
	TextSet=1
	
const char *
etk_textblock_text_get(tb, markup)
	Etk_Textblock * tb
	Etk_Bool markup
      ALIAS:
	TextGet=1
	CODE:
	RETVAL = etk_string_get(etk_textblock_text_get(tb, markup));
	OUTPUT:
	RETVAL

const char *
etk_textblock_range_text_get(tb, iter1, iter2, markup)
	Etk_Textblock *tb
	Etk_Textblock_Iter * iter1
	Etk_Textblock_Iter * iter2
	Etk_Bool markup
      ALIAS:
	RangeTextGet=1
	CODE:
	RETVAL = etk_string_get(etk_textblock_range_text_get(tb, iter1, iter2, markup));
	OUTPUT:
	RETVAL

void
etk_textblock_insert(tb, iter, txt)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter
	SV * txt
      ALIAS:
	Insert=1
	CODE:
	int length;
	const char * text;
	text = SvPV(txt, length);
	etk_textblock_insert(tb, iter, text, length);

void
etk_textblock_insert_markup(tb, iter, txt)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter
	SV * txt
      ALIAS:
	InsertMarkup=1
	CODE:
	int length;
	const char * text;
	text = SvPV(txt, length);
	etk_textblock_insert_markup(tb, iter, text, length);

void
etk_textblock_clear(tb)
	Etk_Textblock *tb
      ALIAS:
	Clear=1

void
etk_textblock_delete_before(tb, iter)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter
      ALIAS:
	DeleteBefore=1

void
etk_textblock_delete_after(tb, iter)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter
      ALIAS:
	DeleteAfter=1

void
etk_textblock_delete_range(tb, iter1, iter2)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter1
	Etk_Textblock_Iter *iter2
      ALIAS:
	DeleteRange=1

void
etk_textblock_unicode_length_get(unicode_string)
	const char *unicode_string
	ALIAS:
	UnicodeLengthGet=1

Etk_Bool etk_textblock_is_white_char(c)
	int c
	ALIAS:
	IsWhiteChar=1

void
etk_textblock_char_size_get(evas, font_face, font_size)
	Evas * evas
	const char * font_face
	int font_size
	ALIAS:
	CharSizeGet=1
	PPCODE:
	int w, h;
	etk_textblock_char_size_get(evas, font_face, font_size, &w, &h);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));



MODULE = Etk::Theme	PACKAGE = Etk::Theme	PREFIX = etk_theme_
	
void
etk_theme_init()
	ALIAS:
	Init=1

void
etk_theme_shutdown()
      ALIAS:
	Shutdown=1

const char *
etk_theme_widget_path_get()
      ALIAS:
	WidgetPathGet=1

const char *
etk_theme_widget_name_get()
      ALIAS:
	WidgetNameGet=1

Etk_Bool
etk_theme_widget_set_from_name(theme_name)
	char *	theme_name
      ALIAS:
	WidgetSetFromName=1

Etk_Bool
etk_theme_widget_set_from_path(theme_path)
	char *	theme_path
      ALIAS:
	WidgetSetFromPath=1

void
etk_theme_widget_available_themes_get()
	ALIAS:
	WidgetAvailableThemesGet=1
	PPCODE:
	Evas_List * list;

	list = etk_theme_widget_available_themes_get();
	XPUSHs(sv_2mortal(newSVCharEvasList(list)));

char *
etk_theme_widget_find(theme_name)
	const char * 	theme_name
	ALIAS:
	WidgetFind=1

const char *
etk_theme_icon_path_get()
	ALIAS:
	IconPathGet=1

const char *
etk_theme_icon_name_get()
	ALIAS:
	IconNameGet=1

Etk_Bool
etk_theme_icon_set_from_path(theme)
	const char * theme
	ALIAS:
	IconSetFromPath=1

Etk_Bool
etk_theme_icon_set_from_name(theme)
	const char * theme
	ALIAS:
	IconSetFromName=1

void
etk_theme_icon_available_themes_get()
	ALIAS:
	IconAvailableThemesGet=1
	PPCODE:
	Evas_List * list;

	list = etk_theme_icon_available_themes_get();
	XPUSHs(sv_2mortal(newSVCharEvasList(list)));
	
char *
etk_theme_icon_find(theme_name)
	const char * 	theme_name
	ALIAS:
	IconFind=1



Etk_Bool
etk_theme_group_exists(file, group, parent)
	const char * file
	const char * group
	const char * parent
	ALIAS:
	GroupExists=1

MODULE = Etk::ToggleButton	PACKAGE = Etk::ToggleButton	PREFIX = etk_toggle_button_

Etk_Bool
etk_toggle_button_active_get(toggle_button)
	Etk_Toggle_Button *	toggle_button
      ALIAS:
	ActiveGet=1

void
etk_toggle_button_active_set(toggle_button, active)
	Etk_Toggle_Button *	toggle_button
	Etk_Bool	active
      ALIAS:
	ActiveSet=1

Etk_Toggle_Button *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TOGGLE_BUTTON(etk_toggle_button_new());
	OUTPUT:
	RETVAL

Etk_Toggle_Button *
etk_toggle_button_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_TOGGLE_BUTTON(etk_toggle_button_new_with_label(label));
	OUTPUT:
	RETVAL

void
etk_toggle_button_toggle(toggle_button)
	Etk_Toggle_Button *	toggle_button
      ALIAS:
	Toggle=1


MODULE = Etk::Tooltips	PACKAGE = Etk::Tooltips	PREFIX = etk_tooltips_

void
etk_tooltips_disable()
      ALIAS:
	Disable=1

void
etk_tooltips_enable()
      ALIAS:
	Enable=1

void
etk_tooltips_init()
      ALIAS:
	Init=1

void
etk_tooltips_pop_down()
      ALIAS:
	PopDown=1

void
etk_tooltips_pop_up(widget)
	Etk_Widget *	widget
      ALIAS:
	PopUp=1

void
etk_tooltips_shutdown()
      ALIAS:
	Shutdown=1

const char *
etk_tooltips_tip_get(widget)
	Etk_Widget *	widget
      ALIAS:
	TipGet=1

void
etk_tooltips_tip_set(widget, text)
	Etk_Widget *	widget
	const char *	text
      ALIAS:
	TipSet=1

Etk_Bool
etk_tooltips_tip_visible()
      ALIAS:
	TipVisible=1


MODULE = Etk::Toplevel	PACKAGE = Etk::Toplevel	PREFIX = etk_toplevel_

Evas *
etk_toplevel_evas_get(toplevel_widget)
	Etk_Toplevel *	toplevel_widget
      ALIAS:
	EvasGet=1

Etk_Widget *
etk_toplevel_focused_widget_get(toplevel_widget)
	Etk_Toplevel *	toplevel_widget
      ALIAS:
	FocusedWidgetGet=1

Etk_Widget *
etk_toplevel_focused_widget_next_get(toplevel_widget)
	Etk_Toplevel *	toplevel_widget
      ALIAS:
	FocusedWidgetNextGet=1

Etk_Widget *
etk_toplevel_focused_widget_prev_get(toplevel_widget)
	Etk_Toplevel *	toplevel_widget
      ALIAS:
	FocusedWidgetPrevGet=1

void
etk_toplevel_focused_widget_set(toplevel_widget, widget)
	Etk_Toplevel *	toplevel_widget
	Etk_Widget *	widget
      ALIAS:
	FocusedWidgetSet=1

void
etk_toplevel_pointer_pop(toplevel_widget, pointer_type)
	Etk_Toplevel *	toplevel_widget
	Etk_Pointer_Type	pointer_type
      ALIAS:
	PointerPop=1

void
etk_toplevel_pointer_push(toplevel_widget, pointer_type)
	Etk_Toplevel *	toplevel_widget
	Etk_Pointer_Type	pointer_type
      ALIAS:
	PointerPush=1

void
etk_toplevel_evas_position_get(toplevel)
	Etk_Toplevel *  toplevel
	ALIAS:
	EvasPositionGet=1
	PPCODE:
	int x, y;
	etk_toplevel_evas_position_get(toplevel, &x, &y);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));

void
etk_toplevel_screen_position_get(toplevel)
	Etk_Toplevel *  toplevel
	ALIAS:
	ScreenPositionGet=1
	PPCODE:
	int x, y;
	etk_toplevel_screen_position_get(toplevel, &x, &y);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));

void
etk_toplevel_size_get(toplevel)
	Etk_Toplevel *  toplevel
	ALIAS:
	SizeGet=1
	PPCODE:
	int w, h;
	etk_toplevel_size_get(toplevel, &w, &h);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

Evas_List *
etk_toplevel_widgets_get()
	ALIAS:
	WidgetsGet=1


MODULE = Etk::Tree	PACKAGE = Etk::Tree	PREFIX = etk_tree_
	
Etk_Tree_Row *
etk_tree_row_append(tree, parent=NULL)
	Etk_Tree *	tree
	Etk_Tree_Row *	parent
      ALIAS:
	RowAppend=1
      CODE:
        RETVAL = etk_tree_row_append(tree, parent, NULL);
      OUTPUT:
        RETVAL

Etk_Tree_Row *
etk_tree_row_prepend(tree, parent=NULL)
	Etk_Tree *	tree
	Etk_Tree_Row *	parent
      ALIAS:
	RowPrepend=1
      CODE:
        RETVAL = etk_tree_row_prepend(tree, parent, NULL);
      OUTPUT:
        RETVAL

Etk_Tree_Row *
etk_tree_row_insert(tree, parent=NULL, after)
	Etk_Tree *	tree
	Etk_Tree_Row *	parent
	Etk_Tree_Row *	after
	ALIAS:
	RowInsert=1
	CODE:
	RETVAL = etk_tree_row_insert(tree, parent, after, NULL);
	OUTPUT:
	RETVAL

Etk_Scrolled_View *
etk_tree_scrolled_view_get(tree)
	Etk_Tree * tree
      ALIAS:
	ScrolledViewGet=1

void
etk_tree_build(tree)
	Etk_Tree *	tree
      ALIAS:
	Build=1

void
etk_tree_clear(tree)
	Etk_Tree *	tree
      ALIAS:
	Clear=1

Etk_Tree_Row *
etk_tree_first_row_get(tree)
	Etk_Tree *	tree
      ALIAS:
	FirstRowGet=1

void
etk_tree_freeze(tree)
	Etk_Tree *	tree
      ALIAS:
	Freeze=1

Etk_Bool
etk_tree_headers_visible_get(tree)
	Etk_Tree *	tree
      ALIAS:
	HeadersVisibleGet=1

void
etk_tree_headers_visible_set(tree, headers_visible)
	Etk_Tree *	tree
	Etk_Bool	headers_visible
      ALIAS:
	HeadersVisibleSet=1

Etk_Tree_Row *
etk_tree_last_row_get(tree)
	Etk_Tree *	tree
      ALIAS:
	LastRowGet=1

Etk_Tree_Mode
etk_tree_mode_get(tree)
	Etk_Tree *	tree
      ALIAS:
	ModeGet=1

void
etk_tree_mode_set(tree, mode)
	Etk_Tree *	tree
	Etk_Tree_Mode	mode
      ALIAS:
	ModeSet=1

Etk_Bool
etk_tree_multiple_select_get(tree)
	Etk_Tree *	tree
      ALIAS:
	MultipleSelectGet=1

void
etk_tree_multiple_select_set(tree, multiple_select)
	Etk_Tree *	tree
	Etk_Bool	multiple_select
      ALIAS:
	MultipleSelectSet=1

Etk_Tree *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TREE(etk_tree_new());
	OUTPUT:
	RETVAL

Etk_Tree_Col *
etk_tree_nth_col_get(tree, nth)
	Etk_Tree *	tree
	int	nth

int
etk_tree_num_cols_get(tree)
	Etk_Tree *	tree
      ALIAS:
	NumColsGet=1
	
void
etk_tree_select_all(tree)
	Etk_Tree *	tree
      ALIAS:
	SelectAll=1

Etk_Tree_Row *
etk_tree_selected_row_get(tree)
	Etk_Tree *	tree
      ALIAS:
	SelectedRowGet=1

#
# void
# etk_tree_sort(tree, compare_cb, ascendant, col, data)
#	Etk_Tree *	tree
#        SV *compare_cb
#	Etk_Bool	ascendant
#	Etk_Tree_Col *	col
#	SV *	data
#      ALIAS:
#	Sort=1
#      CODE:
#        Callback_Tree_Compare_Data *cbd;
#        
#        cbd = calloc(1, sizeof(Callback_Tree_Compare_Data));
#        cbd->object = ETK_OBJECT(col);
#        cbd->perl_data = newSVsv(data);
#        cbd->perl_callback = newSVsv(compare_cb);
#        etk_tree_sort(tree, tree_compare_cb, ascendant, col, cbd);
#
# void 
# etk_tree_sort_alpha(tree, ascendant, col, data)
#	Etk_Tree *    tree
#	Etk_Bool        ascendant
#	Etk_Tree_Col *  col
#	SV *    data
#      ALIAS:
#	SortAlpha=1
#	CODE:
#	etk_tree_sort(tree, tree_compare_alpha_cb, ascendant, col, data);
#
# void 
# etk_tree_sort_numeric(tree, ascendant, col, data)
#	Etk_Tree *    tree
#	Etk_Bool        ascendant
#	Etk_Tree_Col *  col
#	SV *    data
#      ALIAS:
#	SortNumeric=1
#	CODE:
#	etk_tree_sort(tree, tree_compare_numeric_cb, ascendant, col, data);

void
etk_tree_thaw(tree)
	Etk_Tree *	tree
      ALIAS:
	Thaw=1

void
etk_tree_unselect_all(tree)
	Etk_Tree *	tree
      ALIAS:
	UnselectAll=1

SV *
etk_tree_col_new(tree, title, width, alignment)
	Etk_Tree *	tree
	char *	title
	int	width
	float	alignment
	ALIAS:
	ColNew=1
	CODE:
	Etk_Tree_Col * col;
	col = etk_tree_col_new(tree, title, width, alignment);
	RETVAL = newSVObject(col);
	OUTPUT:
	RETVAL

void
etk_tree_rows_height_set(tree, rows_height)
	Etk_Tree *tree
	int rows_height
	ALIAS:
	RowsHeightSet=1

int
etk_tree_rows_height_get(tree)
	Etk_Tree *tree
	ALIAS:
	RowsHeightGet=1


MODULE = Etk::Tree::Col	PACKAGE = Etk::Tree::Col	PREFIX = etk_tree_col_

Etk_Bool
etk_tree_col_expand_get(col)
	Etk_Tree_Col *	col
      ALIAS:
	ExpandGet=1

void
etk_tree_col_expand_set(col, expand)
	Etk_Tree_Col *	col
	Etk_Bool	expand
      ALIAS:
	ExpandSet=1

int
etk_tree_col_min_width_get(col)
	Etk_Tree_Col *	col
      ALIAS:
	MinWidthGet=1

void
etk_tree_col_min_width_set(col, min_width)
	Etk_Tree_Col *	col
	int	min_width
      ALIAS:
	MinWidthSet=1

int
etk_tree_col_position_get(col)
	Etk_Tree_Col *	col
      ALIAS:
	PositionGet=1

void
etk_tree_col_position_set(col, position)
	Etk_Tree_Col *	col
	int	position
      ALIAS:
	PositionSet=1

Etk_Bool
etk_tree_col_resizable_get(col)
	Etk_Tree_Col *	col
      ALIAS:
	ResizableGet=1

void
etk_tree_col_resizable_set(col, resizable)
	Etk_Tree_Col *	col
	Etk_Bool	resizable
      ALIAS:
	ResizableSet=1

void
etk_tree_col_sort_set(col, compare_cb, data)
        Etk_Tree_Col *  col
        SV * compare_cb
        SV * data
      ALIAS:
	SortSet=1
      CODE:
	Callback_Tree_Compare_Data *cbd;
	
        cbd = calloc(1, sizeof(Callback_Tree_Compare_Data));
	cbd->object = ETK_OBJECT(col);
        cbd->perl_data = newSVsv(data);
        cbd->perl_callback = newSVsv(compare_cb);	
        etk_tree_col_sort_set(col, tree_compare_cb, cbd);

void
etk_tree_col_sort_alpha_set(col, data)
	Etk_Tree_Col *  col
	SV * data
      ALIAS:
	SortAlphaSet=1
	CODE:
	etk_tree_col_sort_set(col, tree_compare_alpha_cb, data);
	
void
etk_tree_col_sort_numeric_set(col, data)
	Etk_Tree_Col *  col
	SV * data
      ALIAS:
	SortNumericSet=1
	CODE:
	etk_tree_col_sort_set(col, tree_compare_numeric_cb, data);

const char *
etk_tree_col_title_get(col)
	Etk_Tree_Col *	col
      ALIAS:
	TitleGet=1

void
etk_tree_col_title_set(col, title)
	Etk_Tree_Col *	col
	char *	title
      ALIAS:
	TitleSet=1

Etk_Bool
etk_tree_col_visible_get(col)
	Etk_Tree_Col *	col
      ALIAS:
	VisibleGet=1

void
etk_tree_col_visible_set(col, visible)
	Etk_Tree_Col *	col
	Etk_Bool	visible
      ALIAS:
	VisibleSet=1

int
etk_tree_col_width_get(col)
	Etk_Tree_Col *	col
      ALIAS:
	WidthGet=1

void
etk_tree_col_width_set(col, width)
	Etk_Tree_Col *	col
	int	width
      ALIAS:
	WidthSet=1

Etk_Tree *
etk_tree_col_tree_get(col)
	Etk_Tree_Col	*col
	ALIAS:
	TreeGet=1

void
XS_etk_tree_col_model_add(col, model)
	Etk_Tree_Col	*col
	Etk_Tree_Model	*model
	CODE:
	etk_tree_col_model_add(col, model);

Etk_Widget *
etk_tree_col_header_get(col)
	Etk_Tree_Col    *col
	ALIAS:
	HeaderGet=1

void
etk_tree_col_alignment_set(col, alignment)
	Etk_Tree_Col    *col
	float	alignment
	ALIAS:
	AlignmentSet=1

float
etk_tree_col_alignment_get(col)
	Etk_Tree_Col    *col
	ALIAS:
	AlignmentGet=1

MODULE = Etk::Tree::Model	PACKAGE = Etk::Tree::Model	PREFIX = etk_tree_model_

void
etk_tree_model_free(model)
	Etk_Tree_Model *	model
      ALIAS:
	Free=1


MODULE = Etk::Tree::Model::Checkbox	PACKAGE = Etk::Tree::Model::Checkbox	PREFIX = etk_tree_model_checkbox_

void
new(class)
	SV * class
	PPCODE:
	Etk_Tree_Model * model;
	SV * ret;
	model = etk_tree_model_checkbox_new();
	ret = newSVObj(model, getClass("Etk_Tree_Model"));
	hv_store( (HV*)SvRV(ret), "_model", 6, newSViv(mCHECKBOX), 0);
	XPUSHs(sv_2mortal(ret));


MODULE = Etk::Tree::Model::Double	PACKAGE = Etk::Tree::Model::Double	PREFIX = etk_tree_model_double_

void
new(class)
	SV * class
	PPCODE:
	Etk_Tree_Model * model;
	SV * ret;
	model = etk_tree_model_double_new();
	ret = newSVObj(model, getClass("Etk_Tree_Model"));
	hv_store( (HV*)SvRV(ret), "_model", 6, newSViv(mDOUBLE), 0);
	XPUSHs(sv_2mortal(ret));


MODULE = Etk::Tree::Model::Image	PACKAGE = Etk::Tree::Model::Image	PREFIX = etk_tree_model_image_

void
new(class)
	SV * class
	PPCODE:
	Etk_Tree_Model * model;
	SV * ret;
	model = etk_tree_model_image_new();
	ret = newSVObj(model, getClass("Etk_Tree_Model"));
	hv_store( (HV*)SvRV(ret), "_model", 6, newSViv(mIMAGE), 0);
	XPUSHs(sv_2mortal(ret));

void
etk_tree_model_image_width_set(model, width, alignment)
	Etk_Tree_Model *	model
	int	width
	float	alignment
	ALIAS:
	WidthSet=1

void
etk_tree_model_image_width_get(model)
	Etk_Tree_Model *	model
	ALIAS:
	WidthGet=1
	PPCODE:
	int width;
	float alignment;
	etk_tree_model_image_width_get(model, &width, &alignment);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(width)));
	PUSHs(sv_2mortal(newSViv(alignment)));

MODULE = Etk::Tree::Model::Int	PACKAGE = Etk::Tree::Model::Int	PREFIX = etk_tree_model_int_

void
new(class)
	SV * class
	PPCODE:
	Etk_Tree_Model * model;
	SV * ret;
	model = etk_tree_model_int_new();
	ret = newSVObj(model, getClass("Etk_Tree_Model"));
	hv_store( (HV*)SvRV(ret), "_model", 6, newSViv(mINT), 0);
	XPUSHs(sv_2mortal(ret));

	

MODULE = Etk::Tree::Model::ProgressBar	PACKAGE = Etk::Tree::Model::ProgressBar	PREFIX = etk_tree_model_progress_bar_

void
new(class)
	SV * class
	PPCODE:
	Etk_Tree_Model * model;
	SV * ret;
	model = etk_tree_model_progress_bar_new();
	ret = newSVObj(model, getClass("Etk_Tree_Model"));
	hv_store( (HV*)SvRV(ret), "_model", 6, newSViv(mPROGRESSBAR), 0);
	XPUSHs(sv_2mortal(ret));


MODULE = Etk::Tree::Model::Text	PACKAGE = Etk::Tree::Model::Text	PREFIX = etk_tree_model_text_

void
new(class)
	SV * class
	PPCODE:
	Etk_Tree_Model * model;
	SV * ret;
	model = etk_tree_model_text_new();
	ret = newSVObj(model, getClass("Etk_Tree_Model"));
	hv_store( (HV*)SvRV(ret), "_model", 6, newSViv(mTEXT), 0);
	XPUSHs(sv_2mortal(ret));


MODULE = Etk::Tree::Row	PACKAGE = Etk::Tree::Row	PREFIX = etk_tree_row_


Etk_Tree_Row *
etk_tree_row_next_get(row)
	Etk_Tree_Row *	row
      ALIAS:
	NextGet=1

Etk_Tree_Row *
etk_tree_row_prev_get(row)
	Etk_Tree_Row *	row
      ALIAS:
	PrevGet=1

void etk_tree_row_delete(row)
	Etk_Tree_Row *	row
	ALIAS:
	Delete=1

void
etk_tree_row_fold(row)
	Etk_Tree_Row *	row
      ALIAS:
	Fold=1

void
etk_tree_row_unfold(row)
	Etk_Tree_Row *	row
	ALIAS:
	Unfold=1

Etk_Bool
etk_tree_row_is_folded(row)
	Etk_Tree_Row *	row
	ALIAS:
	IsFolded=1

SV *
etk_tree_row_data_get(row)
	Etk_Tree_Row *	row
      ALIAS:
	DataGet=1
	CODE:
	RETVAL = newSVsv((SV*)etk_tree_row_data_get(row));
	OUTPUT:
	RETVAL

void
etk_tree_row_data_set(row, data)
	Etk_Tree_Row *	row
	SV *	data
      ALIAS:
	DataSet=1
        CODE:
        etk_tree_row_data_set(row, newSVsv(data));


void
model_fields_set(row, emit_signal, model, ...)
	Etk_Tree_Row * row
	Etk_Bool	emit_signal
	SV * model
     ALIAS:
	ModelFieldsSet=1
	PREINIT:
	Etk_Tree_Model * etkmodel;
	SV ** mod;
	CODE:
	etkmodel = (Etk_Tree_Model *) SvObj(model, "Etk::Tree::Model");
	mod = hv_fetch( (HV*)SvRV(model), "_model", 6, 0);
	if (mod) {
	int type = SvIV(*mod);
	switch(type) {
	   case mINT:
		etk_tree_row_model_fields_set(row, emit_signal, etkmodel, SvIV(ST(3)), NULL);
		break;
	   case mDOUBLE:
		     etk_tree_row_model_fields_set(row, emit_signal, etkmodel, SvNV(ST(3)), NULL);
		     break;
		  case mCHECKBOX:
		     etk_tree_row_model_fields_set(row, emit_signal, etkmodel, SvIV(ST(3)), NULL);
		     break;
		  case mIMAGE:
		     if (items == 4)
		        etk_tree_row_model_fields_set(row, emit_signal, etkmodel,
		     	   SvPV_nolen(ST(3)), NULL, NULL);
		     else
		        etk_tree_row_model_fields_set(row, emit_signal, etkmodel,
		     	   SvPV_nolen(ST(3)), SvPV_nolen(ST(4)), NULL);
		     break;
		  case mPROGRESSBAR:
		     break;
		  case mTEXT:
		     etk_tree_row_model_fields_set(row, emit_signal, etkmodel, SvPV_nolen(ST(3)), NULL);
		     break;
               }
	     }


void
model_fields_get(row, model)
	Etk_Tree_Row * row
	SV * model
      ALIAS:
	ModelFieldsGet=1
	PREINIT:
	Etk_Tree_Model * mod;
	SV ** models;
	int i;
	Etk_Bool c;
	double d;
	char *c1, *c2, *c3;
	PPCODE:
	mod = (Etk_Tree_Model *) SvObj(model, "Etk::Tree::Model");
	models = hv_fetch( (HV*)SvRV(model), "_model", 6, 0);
	if (models) {
	       int type = SvIV(*models);
	       switch(type) {
	          case mINT:
			etk_tree_row_model_fields_get(row, mod, &i, NULL);
			XPUSHs(sv_2mortal(newSViv(i)));
		     break;
		  case mDOUBLE:
			etk_tree_row_model_fields_get(row, mod, &d, NULL);
			XPUSHs(sv_2mortal(newSVnv(d)));
			break;
		  case mCHECKBOX:
			etk_tree_row_model_fields_get(row, mod, &c, NULL);
			XPUSHs(sv_2mortal(newSViv(c)));
			break;
		  case mIMAGE:
			etk_tree_row_model_fields_get(row, mod, &c1, &c2, NULL);
			XPUSHs(sv_2mortal(newSVpv(c1, strlen(c1))));
			if (c2) XPUSHs(sv_2mortal(newSVpv(c2, strlen(c2))));
		     break;
		  case mPROGRESSBAR:
			//etk_tree_row_model_fields_get(row, mod, &d, &c1, NULL);
			//XPUSHs(sv_2mortal(newSVnv(d)));
			//XPUSHs(sv_2mortal(newSVpv(c1, strlen(c1))));
		     break;
		  case mTEXT:
			etk_tree_row_model_fields_get(row, mod, &c1, NULL);
			XPUSHs(sv_2mortal(newSVpv(c1, strlen(c1))));
		     break;
               }
	     }

void
fields_set(row, emit_signal, col, ...)
	Etk_Tree_Row * row
	Etk_Bool	emit_signal
	SV * col
     ALIAS:
	FieldsSet=1
	PREINIT:
	Etk_Tree_Col * column;
	SV ** models;
	SV ** model;
	CODE:
	column = (Etk_Tree_Col *) SvObj(col, "Etk::Tree::Col");
	models = hv_fetch( (HV*)SvRV(col), "_models", 7, 0);
	if (models) {
           AV * mods = (AV *) SvRV(*models);
	   int total = (int)av_len(mods);
	      model = av_fetch(mods, 0, 0);
	      if (model) {
	       int type = SvIV(*model);
	       switch(type) {
	          case mINT:
		     etk_tree_row_fields_set(row, emit_signal, column, SvIV(ST(3)), NULL);
		     break;
		  case mDOUBLE:
		     etk_tree_row_fields_set(row, emit_signal, column, SvNV(ST(3)), NULL);
		     break;
		  case mCHECKBOX:
		     etk_tree_row_fields_set(row, emit_signal, column, SvIV(ST(3)), NULL);
		     break;
		  case mIMAGE:
		     if (items == 4)
		        etk_tree_row_fields_set(row, emit_signal, column, 
		     	   SvPV_nolen(ST(3)), NULL, NULL);
		     else
		        etk_tree_row_fields_set(row, emit_signal, column, 
		     	   SvPV_nolen(ST(3)), SvPV_nolen(ST(4)), NULL);
		     break;
		  case mPROGRESSBAR:
		     break;
		  case mTEXT:
		     etk_tree_row_fields_set(row, emit_signal, column, SvPV_nolen(ST(3)), NULL);
		     break;
               }
	     }
	}


void
fields_get(row, col)
	Etk_Tree_Row * row
	SV * col
      ALIAS:
	FieldsGet=1
	PREINIT:
	Etk_Tree_Col * column;
	SV ** models;
	SV ** model;
	int i;
	Etk_Bool c;
	double d;
	char *c1, *c2, *c3;
	PPCODE:
	column = (Etk_Tree_Col *) SvObj(col, "Etk::Tree::Col");
	models = hv_fetch( (HV*)SvRV(col), "_models", 7, 0);
	if (models) {
           AV * mods = (AV *) SvRV(*models);
	   int total = (int)av_len(mods);
	      model = av_fetch(mods, 0, 0);
	      if (model) {
	       int type = SvIV(*model);
	       switch(type) {
	          case mINT:
			etk_tree_row_fields_get(row, column, &i, NULL);
			XPUSHs(sv_2mortal(newSViv(i)));
		     break;
		  case mDOUBLE:
			etk_tree_row_fields_get(row, column, &d, NULL);
			XPUSHs(sv_2mortal(newSVnv(d)));
			break;
		  case mCHECKBOX:
			etk_tree_row_fields_get(row, column, &c, NULL);
			XPUSHs(sv_2mortal(newSViv(c)));
			break;
		  case mIMAGE:
			etk_tree_row_fields_get(row, column, &c1, &c2, NULL);
			XPUSHs(sv_2mortal(newSVpv(c1, strlen(c1))));
			if (c2) XPUSHs(sv_2mortal(newSVpv(c2, strlen(c2))));
		     break;
		  case mPROGRESSBAR:
			//etk_tree_row_fields_get(row, column, &d, &c1, NULL);
			//XPUSHs(sv_2mortal(newSVnv(d)));
			//XPUSHs(sv_2mortal(newSVpv(c1, strlen(c1))));
		     break;
		  case mTEXT:
			etk_tree_row_fields_get(row, column, &c1, NULL);
			XPUSHs(sv_2mortal(newSVpv(c1, strlen(c1))));
		     break;
               }
	     }
	}


Etk_Tree_Row *
etk_tree_row_first_child_get(row)
	Etk_Tree_Row *	row
      ALIAS:
	FirstChildGet=1

Etk_Tree_Row *
etk_tree_row_last_child_get(row)
	Etk_Tree_Row *	row
      ALIAS:
	LastChildGet=1

void
etk_tree_row_scroll_to(row, center)
	Etk_Tree_Row *	row
	Etk_Bool	center
      ALIAS:
	ScrollTo=1

void
etk_tree_row_select(row)
	Etk_Tree_Row *	row
      ALIAS:
	Select=1

void
etk_tree_row_unselect(row)
	Etk_Tree_Row *	row
      ALIAS:
	Unselect=1

Etk_Bool
etk_tree_row_is_selected(row)
	Etk_Tree_Row *  row
	ALIAS:
	IsSelected=1

Etk_Tree *
etk_tree_row_tree_get(row)
	Etk_Tree_Row *	row
	ALIAS:
	TreeGet=1

Etk_Tree_Row *
etk_tree_row_parent_get(row)
	Etk_Tree_Row *	row
	ALIAS:
	ParentGet=1
	
Etk_Tree_Row *
etk_tree_row_walk_prev(row, include_folded)
	Etk_Tree_Row *	row
	Etk_Bool	include_folded
	ALIAS:
	WalkPrev=1

Etk_Tree_Row *
etk_tree_row_walk_next(row, include_folded)
	Etk_Tree_Row *	row
	Etk_Bool	include_folded
	ALIAS:
	WalkNext=1

MODULE = Etk::VBox	PACKAGE = Etk::VBox	PREFIX = etk_vbox_
	
Etk_VBox *
new(class, homogeneous=ETK_FALSE, spacing=0)
	SV	*class
	Etk_Bool	homogeneous
	int	spacing
	CODE:
	RETVAL = ETK_VBOX(etk_vbox_new(homogeneous, spacing));
	OUTPUT:
	RETVAL

MODULE = Etk::Viewport	PACKAGE = Etk::Viewport	PREFIX = etk_viewport_

Etk_Widget *
new(class)
	SV * class
	CODE:
	RETVAL = etk_viewport_new();
	OUTPUT:
	RETVAL

MODULE = Etk::VPaned	PACKAGE = Etk::VPaned	PREFIX = etk_vpaned_

Etk_VPaned *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_VPANED(etk_vpaned_new());
	OUTPUT:
	RETVAL

MODULE = Etk::VScrollbar	PACKAGE = Etk::VScrollbar	PREFIX = etk_vscrollbar_

Etk_VScrollbar *
new(class, lower, upper, value, step_increment, page_increment, page_size)
	SV * class
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	double	page_size
	CODE:
	RETVAL = ETK_VSCROLLBAR(etk_vscrollbar_new(lower, upper, value, 
				step_increment, page_increment, page_size));
	OUTPUT:
	RETVAL

MODULE = Etk::VSeparator	PACKAGE = Etk::VSeparator	PREFIX = etk_vseparator_

Etk_VSeparator *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_VSEPARATOR(etk_vseparator_new());
	OUTPUT:
	RETVAL

MODULE = Etk::VSlider	PACKAGE = Etk::VSlider	PREFIX = etk_vslider_

Etk_VSlider *
new(class, lower, upper, value, step_increment, page_increment)
	SV * class
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	CODE:
	RETVAL = ETK_VSLIDER(etk_vslider_new(lower, upper, value, step_increment, page_increment));
	OUTPUT:
	RETVAL

MODULE = Etk::Widget	PACKAGE = Etk::Widget	PREFIX = etk_widget_


void
etk_widget_disabled_set(widget, disabled)
	Etk_Widget *	widget
	Etk_Bool	disabled
	ALIAS:
	DisabledSet=1

Etk_Bool
etk_widget_disabled_get(widget)
	Etk_Widget *    widget
	ALIAS:
	DisabledGet=1

Evas_Object *
etk_widget_clip_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ClipGet=1

void
etk_widget_clip_set(widget, clip)
	Etk_Widget *	widget
	Evas_Object *	clip
      ALIAS:
	ClipSet=1

void
etk_widget_clip_unset(widget)
	Etk_Widget *	widget
      ALIAS:
	ClipUnset=1

Etk_Bool
etk_widget_dnd_dest_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndDestGet=1

void
etk_widget_dnd_dest_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on
      ALIAS:
	DndDestSet=1

Evas_List *
etk_widget_dnd_dest_widgets_get()
      ALIAS:
	DndDestWidgetsGet=1

void
etk_widget_dnd_drag_data_set(widget, types, num_types, data, data_size)
	Etk_Widget *	widget
	const char **	types
	int	num_types
	void *	data
	int	data_size
      ALIAS:
	DndDragDataSet=1

Etk_Widget *
etk_widget_dnd_drag_widget_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndDragWidgetGet=1

void
etk_widget_dnd_drag_widget_set(widget, drag_widget)
	Etk_Widget *	widget
	Etk_Widget *	drag_widget
      ALIAS:
	DndDragWidgetSet=1

void
etk_widget_dnd_files_get(e)
	Etk_Widget *	e
      ALIAS:
	DndFilesGet=1
	PPCODE:
	const char ** files;
	int 	* num_files;
	int 	i;

	files = etk_widget_dnd_files_get(e, num_files);
	for (i=0; i<*num_files; i++)
		XPUSHs(sv_2mortal(newSVpv(files[i], strlen(files[i]))));
	
void
etk_widget_focusable_set(widget, focusable)
	Etk_Widget * widget
	Etk_Bool focusable
	ALIAS:
	FocusableSet=1

Etk_Bool
etk_widget_focusable_get(widget)
	Etk_Widget * widget
	ALIAS:
	FocusableGet=1

Etk_Bool
etk_widget_dnd_internal_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndInternalGet=1

void
etk_widget_dnd_internal_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on
      ALIAS:
	DndInternalSet=1

Etk_Bool
etk_widget_dnd_source_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndSourceGet=1

void
etk_widget_dnd_source_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on
      ALIAS:
	DndSourceSet=1

void
etk_widget_dnd_types_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndTypesGet=1
	PPCODE:
	const char ** types;
	int 	* num;
	int 	i;

	types = etk_widget_dnd_types_get(widget, num);
	for (i=0; i<*num; i++)
		XPUSHs(sv_2mortal(newSVpv(types[i], strlen(types[i]))));

void
etk_widget_dnd_types_set(widget, perl_types)
	Etk_Widget *	widget
	AV * perl_types
      ALIAS:
	DndTypesSet=1
	CODE:
	const char **	types;
	int	num;
	int	i;
	
	num = (int) av_len(perl_types) + 1;
	types = calloc(num, sizeof(char *));
	for (i=0; i<num; i++) 
	{
		SV ** val;
		val = av_fetch(perl_types, i, 0);
		if (val)
			types[i] = (char *) SvIV(*val);
		else
			types[i] = 0;

	}
	etk_widget_dnd_types_set(widget, types, num);


void
etk_widget_enter(widget)
	Etk_Widget *	widget
      ALIAS:
	Enter=1

void
etk_widget_focus(widget)
	Etk_Widget *	widget
      ALIAS:
	Focus=1

void
etk_widget_geometry_get(widget)
	Etk_Widget *	widget
      ALIAS:
	GeometryGet=1
	PPCODE:
	int 	x;
	int 	y;
	int 	w;
	int 	h;

	etk_widget_geometry_get(widget, &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

void
etk_widget_inner_geometry_get(widget)
	Etk_Widget *	widget
      ALIAS:
	InnerGeometryGet=1
	PPCODE:
	int 	x;
	int 	y;
	int 	w;
	int 	h;
	etk_widget_inner_geometry_get(widget, &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

Etk_Bool
etk_widget_has_event_object_get(widget)
	Etk_Widget *	widget
      ALIAS:
	HasEventObjectGet=1

void
etk_widget_has_event_object_set(widget, has_event_object)
	Etk_Widget *	widget
	Etk_Bool	has_event_object
      ALIAS:
	HasEventObjectSet=1

void
etk_widget_hide(widget)
	Etk_Widget *	widget
      ALIAS:
	Hide=1

void
etk_widget_hide_all(widget)
	Etk_Widget *	widget
      ALIAS:
	HideAll=1

Etk_Bool
etk_widget_is_swallowed(widget)
	Etk_Widget *	widget
      ALIAS:
	IsSwallowed=1

Etk_Bool
etk_widget_is_visible(widget)
	Etk_Widget *	widget
      ALIAS:
	IsVisible=1

void
etk_widget_leave(widget)
	Etk_Widget *	widget
      ALIAS:
	Leave=1

void
etk_widget_lower(widget)
	Etk_Widget *	widget
      ALIAS:
	Lower=1

Etk_Bool
etk_widget_member_object_add(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object
      ALIAS:
	MemberObjectAdd=1

void
etk_widget_member_object_del(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object
      ALIAS:
	MemberObjectDel=1

void
etk_widget_member_object_lower(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object
      ALIAS:
	MemberObjectLower=1

void
etk_widget_member_object_raise(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object
      ALIAS:
	MemberObjectRaise=1

void
etk_widget_member_object_stack_above(widget, object, above)
	Etk_Widget *	widget
	Evas_Object *	object
	Evas_Object *	above
      ALIAS:
	MemberObjectStackAbove=1

void
etk_widget_member_object_stack_below(widget, object, below)
	Etk_Widget *	widget
	Evas_Object *	object
	Evas_Object *	below
      ALIAS:
	MemberObjectStackBelow=1

Etk_Widget *
etk_widget_parent_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ParentGet=1

void
etk_widget_parent_set(widget, parent)
	Etk_Widget *	widget
	Etk_Widget *	parent
      ALIAS:
	ParentSet=1

void
etk_widget_parent_set_full(widget, parent, remove_from_container)
	Etk_Widget *	widget
	Etk_Widget *	parent
	Etk_Bool	remove_from_container
      ALIAS:
	ParentSetFull=1

Etk_Bool
etk_widget_pass_mouse_events_get(widget)
	Etk_Widget *	widget
      ALIAS:
	PassMouseEventsGet=1

void
etk_widget_pass_mouse_events_set(widget, pass_mouse_events)
	Etk_Widget *	widget
	Etk_Bool	pass_mouse_events
      ALIAS:
	PassMouseEventsSet=1

void
etk_widget_raise(widget)
	Etk_Widget *	widget
      ALIAS:
	Raise=1

void
etk_widget_redraw_queue(widget)
	Etk_Widget *	widget
      ALIAS:
	RedrawQueue=1

Etk_Bool
etk_widget_repeat_mouse_events_get(widget)
	Etk_Widget *	widget
      ALIAS:
	RepeatMouseEventsGet=1

void
etk_widget_repeat_mouse_events_set(widget, repeat_mouse_events)
	Etk_Widget *	widget
	Etk_Bool	repeat_mouse_events
      ALIAS:
	RepeatMouseEventsSet=1

void
etk_widget_show(widget)
	Etk_Widget *	widget
      ALIAS:
	Show=1

void
etk_widget_show_all(widget)
	Etk_Widget *	widget
      ALIAS:
	ShowAll=1

void
etk_widget_size_allocate(widget, geometry)
	Etk_Widget *	widget
	Etk_Geometry 	geometry
      ALIAS:
	SizeAllocate=1

void
etk_widget_size_recalc_queue(widget)
	Etk_Widget *	widget
      ALIAS:
	SizeRecalcQueue=1

void
etk_widget_size_request(widget, size_requisition)
	Etk_Widget *	widget
	Etk_Size *	size_requisition
      ALIAS:
	SizeRequest=1

void
etk_widget_size_request_full(widget, size_requisition, hidden_has_no_size)
	Etk_Widget *	widget
	Etk_Size *	size_requisition
	Etk_Bool	hidden_has_no_size
      ALIAS:
	SizeRequestFull=1

void
etk_widget_size_request_set(widget, w, h)
	Etk_Widget *	widget
	int	w
	int	h
      ALIAS:
	SizeRequestSet=1

Etk_Bool
etk_widget_swallow_widget(swallowing_widget, part, widget_to_swallow)
	Etk_Widget *	swallowing_widget
	char *	part
	Etk_Widget *	widget_to_swallow
      ALIAS:
	SwallowWidget=1

const char *
etk_widget_theme_file_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ThemeFileGet=1

void
etk_widget_theme_file_set(widget, theme_file)
	Etk_Widget *	widget
	char *	theme_file
      ALIAS:
	ThemeFileSet=1

const char *
etk_widget_theme_group_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ThemeGroupGet=1

void
etk_widget_theme_group_set(widget, theme_group)
	Etk_Widget *	widget
	char *	theme_group
      ALIAS:
	ThemeGroupSet=1

Etk_Widget *
etk_widget_theme_parent_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ThemeParentGet=1

void
etk_widget_theme_parent_set(widget, theme_parent)
	Etk_Widget *	widget
	Etk_Widget *	theme_parent
      ALIAS:
	ThemeParentSet=1

Evas *
etk_widget_toplevel_evas_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ToplevelEvasGet=1

Etk_Toplevel *
etk_widget_toplevel_parent_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ToplevelParentGet=1

void
etk_widget_unfocus(widget)
	Etk_Widget *	widget
      ALIAS:
	Unfocus=1

Etk_Bool
etk_widget_is_focused(widget)
	Etk_Widget *	widget
      ALIAS:
	IsFocused=1

void
etk_widget_unswallow_widget(swallowing_widget, widget)
	Etk_Widget *	swallowing_widget
	Etk_Widget *	widget
      ALIAS:
	UnswallowWidget=1

Etk_Bool
etk_widget_internal_get(widget)
	Etk_Widget *	widget
      ALIAS:
	InternalGet=1

void
etk_widget_internal_set(widget, internal)
	Etk_Widget *	widget
	Etk_Bool	internal
      ALIAS:
	InternalSet=1

void
etk_widget_padding_set(widget, left, right, top, bottom)
	Etk_Widget *    widget
	int	left
	int	right
	int	top
	int	bottom
	ALIAS:
	PaddingSet=1

void
etk_widget_padding_get(widget)
	Etk_Widget *    widget
	ALIAS:
	PaddingGet=1
	PPCODE:
	int left, right, top, bottom;
	etk_widget_padding_get(widget, &left, &right, &top, &bottom);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(left)));
	PUSHs(sv_2mortal(newSViv(right)));
	PUSHs(sv_2mortal(newSViv(top)));
	PUSHs(sv_2mortal(newSViv(bottom)));

void
etk_widget_theme_set(widget, theme_file, theme_group)
	Etk_Widget *    widget
	const char * 	theme_file
	const char * 	theme_group
	ALIAS:
	ThemeSet=1
	
void
etk_widget_theme_signal_emit(widget, signal_name, size_recalc)
	Etk_Widget *    widget
	const char * 	signal_name
	Etk_Bool	size_recalc
	ALIAS:
	ThemeSignalEmit=1

void
etk_widget_theme_part_text_set(widget, part_name, text)
	Etk_Widget *    widget
	const char *    part_name
	char *		text
	ALIAS:
	ThemePartTextSet=1
	
Etk_Bool
etk_widget_swallow_object(swallower, part, object)
	Etk_Widget *    swallower
	const char * 	part
	Evas_Object *	object
	ALIAS:
	SwallowObject=1

void
etk_widget_unswallow_object(swallower, object)
	Etk_Widget *    swallower
	Evas_Object *	object
	ALIAS:
	UnswallowObject=1

Etk_Widget_Swallow_Error
etk_widget_swallow_error_get()
	ALIAS:
	SwallowErrorGet=1

	 
MODULE = Etk::Shadow	PACKAGE = Etk::Shadow	PREFIX = etk_shadow_

Etk_Shadow *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_SHADOW(etk_shadow_new());
	OUTPUT:
	RETVAL

void
etk_shadow_shadow_set(shadow, type, edges, radius, offset_x, offset_y, opacity)
	Etk_Shadow *shadow
	Etk_Shadow_Type type
	Etk_Shadow_Edges edges
	int radius
	int offset_x
	int offset_y 
	int opacity
	ALIAS:
	ShadowSet=1

void
etk_shadow_shadow_get(shadow)
	Etk_Shadow *shadow
	ALIAS:
	ShadowGet=1
	PPCODE:
	Etk_Shadow_Type type;
	Etk_Shadow_Edges edges;
	int radius;
	int offset_x;
	int offset_y;
	int opacity;

	etk_shadow_shadow_get(shadow, &type, &edges, &radius, &offset_x, &offset_y, &opacity);
	EXTEND(SP, 6);
	PUSHs(sv_2mortal(newSViv(type)));
	PUSHs(sv_2mortal(newSViv(edges)));
	PUSHs(sv_2mortal(newSViv(radius)));
	PUSHs(sv_2mortal(newSViv(offset_x)));
	PUSHs(sv_2mortal(newSViv(offset_y)));
	PUSHs(sv_2mortal(newSViv(opacity)));

void
etk_shadow_shadow_color_set(shadow, r, g, b)
	Etk_Shadow * shadow
	int r
	int g
	int b
	ALIAS:
	ShadowColorSet=1

void
etk_shadow_shadow_color_get(shadow)
	Etk_Shadow * shadow
	ALIAS:
	ShadowColorGet=1
	PPCODE:
	int r, g, b;

	etk_shadow_shadow_color_get(shadow, &r, &g, &b);

	EXTEND(SP, 3);
	PUSHs(sv_2mortal(newSViv(r)));
	PUSHs(sv_2mortal(newSViv(g)));
	PUSHs(sv_2mortal(newSViv(b)));

void 
etk_shadow_border_set(shadow, border_width)
	Etk_Shadow *shadow
	int border_width
	ALIAS:
	BorderSet=1

int
etk_shadow_border_get(shadow)
	Etk_Shadow * shadow
	ALIAS:
	BorderGet=1

void
etk_shadow_border_color_set(shadow, r, g, b, a)
	Etk_Shadow *shadow
	int r
	int g
	int b
	int a
	ALIAS:
	BorderColorSet=1

void
etk_shadow_border_color_get(shadow)
	Etk_Shadow *shadow
	ALIAS:
	BorderColorGet=1
	PPCODE:
	int r, g, b, a;
	etk_shadow_border_color_get(shadow, &r, &g, &b, &a);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(r)));
	PUSHs(sv_2mortal(newSViv(g)));
	PUSHs(sv_2mortal(newSViv(b)));
	PUSHs(sv_2mortal(newSViv(a)));


MODULE = Etk::Window	PACKAGE = Etk::Window	PREFIX = etk_window_

void
etk_window_raise(window)
	Etk_Window * window
      ALIAS:
	Raise=1

void
etk_window_lower(window)
	Etk_Window * window
      ALIAS:
	Lower=1

void
etk_window_modal_for_window(window_to_modal, window)
	Etk_Window *window_to_modal
	Etk_Window *window
      ALIAS:
	ModalForWindow=1

void
etk_window_center_on_window(window_to_center, window)
	Etk_Window *	window_to_center
	Etk_Window *	window
      ALIAS:
	CenterOnWindow=1

Etk_Bool
etk_window_decorated_get(window)
	Etk_Window *	window
      ALIAS:
	DecoratedGet=1

void
etk_window_decorated_set(window, decorated)
	Etk_Window *	window
	Etk_Bool	decorated
      ALIAS:
	DecoratedSet=1

void
etk_window_iconified_set(window, iconified)
	Etk_Window *	window
	Etk_Bool	iconified
      ALIAS:
	IconifiedSet=1

Etk_Bool
etk_window_iconified_get(window)
	Etk_Window *	window
      ALIAS:
	IconifiedGet=1

Etk_Bool
etk_window_focused_get(window)
	Etk_Window *	window
      ALIAS:
	FocusedGet=1

void
etk_window_focused_set(window, focused)
	Etk_Window *	window
	Etk_Bool	focused
      ALIAS:
	FocusedSet=1

Etk_Bool
etk_window_fullscreen_get(window)
	Etk_Window *	window
      ALIAS:
	FullscreenGet=1

void
etk_window_fullscreen_set(window, fullscreen)
	Etk_Window *	window
	Etk_Bool	fullscreen
      ALIAS:
	FullscreenSet=1

void
etk_window_geometry_get(window)
	Etk_Window *	window
      ALIAS:
	GeometryGet=1
	PPCODE:
	int 	x;
	int 	y;
	int 	w;
	int 	h;
	etk_window_geometry_get(window, &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

Etk_Bool
etk_window_hide_on_delete(window, data)
	Etk_Object *	window
	void *	data
      ALIAS:
	HideOnDelete=1

Etk_Bool
etk_window_maximized_get(window)
	Etk_Window *	window
      ALIAS:
	MaximizedGet=1

void
etk_window_maximized_set(window, maximized)
	Etk_Window *	window
	Etk_Bool	maximized
      ALIAS:
	MaximizedSet=1

void
etk_window_move(window, x, y)
	Etk_Window *	window
	int	x
	int	y
      ALIAS:
	Move=1

void
etk_window_move_to_mouse(window)
	Etk_Window *	window
      ALIAS:
	MoveToMouse=1

Etk_Window *
new(class)
	SV	* class
	CODE:
	RETVAL = ETK_WINDOW(etk_window_new());
	OUTPUT:
	RETVAL

void
etk_window_resize(window, w, h)
	Etk_Window *	window
	int	w
	int	h
      ALIAS:
	Resize=1

Etk_Bool
etk_window_shaped_get(window)
	Etk_Window *	window
      ALIAS:
	ShapedGet=1

void
etk_window_shaped_set(window, shaped)
	Etk_Window *	window
	Etk_Bool	shaped
      ALIAS:
	ShapedSet=1

Etk_Bool
etk_window_skip_pager_hint_get(window)
	Etk_Window *	window
      ALIAS:
	SkipPagerHintGet=1

void
etk_window_skip_pager_hint_set(window, skip_pager_hint)
	Etk_Window *	window
	Etk_Bool	skip_pager_hint
      ALIAS:
	SkipPagerHintSet=1

Etk_Bool
etk_window_skip_taskbar_hint_get(window)
	Etk_Window *	window
      ALIAS:
	SkipTaskbarHintGet=1

void
etk_window_skip_taskbar_hint_set(window, skip_taskbar_hint)
	Etk_Window *	window
	Etk_Bool	skip_taskbar_hint
      ALIAS:
	SkipTaskbarHintSet=1

Etk_Bool
etk_window_sticky_get(window)
	Etk_Window *	window
      ALIAS:
	StickyGet=1

void
etk_window_sticky_set(window, sticky)
	Etk_Window *	window
	Etk_Bool	sticky
      ALIAS:
	StickySet=1

const char *
etk_window_title_get(window)
	Etk_Window *	window
      ALIAS:
	TitleGet=1

void
etk_window_title_set(window, title)
	Etk_Window *	window
	char *	title
      ALIAS:
	TitleSet=1

void
etk_window_wmclass_set(window, window_name, window_class)
	Etk_Window *	window
	char *	window_name
	char *	window_class
      ALIAS:
	WmclassSet=1

void
etk_window_delete_request(window)
	Etk_Window *	window
	ALIAS:
	DeleteRequest=1

void
etk_window_stacking_set(window, stacking)
	Etk_Window *    window
	Etk_Window_Stacking stacking
	ALIAS:
	StackingSet=1

Etk_Window_Stacking
etk_window_stacking_get(window)
	Etk_Window *    window
	ALIAS:
	StackingGet=1


MODULE = Etk::Spinner	PACKAGE = Etk::Spinner	PREFIX = etk_spinner_

Etk_Spinner *
new(class, lower, upper, value, step_increment, page_increment)
	SV * class
	double lower
	double upper
	double value
	double step_increment
	double page_increment
	CODE:
	RETVAL = ETK_SPINNER(etk_spinner_new(lower, upper, value, step_increment, page_increment));

void
etk_spinner_digits_set(spinner, digits)
	Etk_Spinner * spinner
	int digits
	ALIAS:
	DigitsSet=1

int
etk_spinner_digits_get(spinner)
	Etk_Spinner * spinner
	ALIAS:
	DigitsGet=1

void
etk_spinner_snap_to_ticks_set(spinner, snap)
	Etk_Spinner * spinner
	Etk_Bool	snap
	ALIAS:
	SnapToTicksSet=1

Etk_Bool
etk_spinner_snap_to_ticks_get(spinner)
	Etk_Spinner * spinner
	ALIAS:
	SnapToTicksGet=1
	
void
etk_spinner_wrap_set(spinner, wrap)
	Etk_Spinner * spinner
	Etk_Bool	wrap
	ALIAS:
	WrapSet=1

Etk_Bool
etk_spinner_wrap_get(spinner)
	Etk_Spinner * spinner
	ALIAS:
	WrapGet=1


MODULE = Etk::Editable	PACKAGE = Etk::Editable	PREFIX = etk_editable_

Evas_Object *
etk_editable_add(Evas * evas)
	ALIAS:
	Add=1

void
etk_editable_disabled_set(editable, disabled)
	Evas_Object *	editable
	Etk_Bool	disabled
	ALIAS:
	DisabledSet=1

Etk_Bool
etk_editable_disabled_get(editable)
	Evas_Object *   editable
	ALIAS:
	DisabledGet=1

void
etk_editable_theme_set(editable, file, group)
	Evas_Object *editable
	const char *file
	const char *group
	ALIAS:
	ThemeSet=1

void
etk_editable_align_set(editable, align)
	Evas_Object *editable
	float	align
	ALIAS:
	AlignSet=1

float
etk_editable_align_get(editable)
	Evas_Object *editable
	ALIAS:
	AlignGet=1

void
etk_editable_password_mode_set(editable, password_mode)
	Evas_Object *editable
	Etk_Bool	password_mode
	ALIAS:
	PasswordModeSet=1

Etk_Bool
etk_editable_password_mode_get(editable)
	Evas_Object *editable
	ALIAS:
	PasswordModeGet=1

void
etk_editable_text_set(editable, text)
	Evas_Object *editable
	const char * text
	ALIAS:
	TextSet=1

const char *
etk_editable_text_get(editable)
	Evas_Object *editable
	ALIAS:
	TextGet=1

char *
etk_editable_text_range_get(editable, start, end)
	Evas_Object *editable
	int start
	int end
	ALIAS:
	TextRangeGet=1

int
etk_editable_text_length_get(editable)
	Evas_Object *editable
	ALIAS:
	TextLengthGet=1

Etk_Bool
etk_editable_insert(editable, pos, text)
	Evas_Object *editable
	int pos
	const char * text
	ALIAS:
	Insert=1

Etk_Bool
etk_editable_delete(editable, start, end)
	Evas_Object *editable
	int start
	int end
	ALIAS:
	Delete=1

void
etk_editable_cursor_pos_set(editable, pos)
	Evas_Object *editable
	int pos
	ALIAS:
	CursorPosSet=1

int
etk_editable_cursor_pos_get(editable)
	Evas_Object *editable
	ALIAS:
	CursorPosGet=1

void 
etk_editable_cursor_move_to_start(editable)
	Evas_Object *editable
	ALIAS:
	CursorMoveToStart=1

void
etk_editable_cursor_move_to_end(editable)
	Evas_Object *editable
	ALIAS:
	CursorMoveToEnd=1

void
etk_editable_cursor_move_left(editable)
	Evas_Object *editable
	ALIAS:
	CursorMoveLeft=1

void
etk_editable_cursor_move_right(editable)
	Evas_Object *editable
	ALIAS:
	CursorMoveRight=1

void
etk_editable_cursor_show(editable)
	Evas_Object *editable
	ALIAS:
	CursorShow=1

void
etk_editable_cursor_hide(editable)
	Evas_Object *editable
	ALIAS:
	CursorHide=1

void
etk_editable_selection_pos_set(editable, pos)
	Evas_Object *editable
	int pos
	ALIAS:
	SelectionPosSet=1

int 
etk_editable_selection_pos_get(editable)
	Evas_Object *editable
	ALIAS:
	SelectionPosGet=1

void
etk_editable_selection_move_to_start(editable)
	Evas_Object *editable
	ALIAS:
	SelectionMoveToStart=1

void
etk_editable_selection_move_to_end(editable)
	Evas_Object *editable
	ALIAS:
	SelectionMoveToEnd=1

void
etk_editable_selection_move_left(editable)
	Evas_Object *editable
	ALIAS:
	SelectionMoveLeft=1

void 
etk_editable_selection_move_right(editable)
	Evas_Object *editable
	ALIAS:
	SelectionMoveRight=1

void 
etk_editable_select_all(editable)
	Evas_Object *editable
	ALIAS:
	SelectAll=1

void 
etk_editable_unselect_all(editable)
	Evas_Object *editable
	ALIAS:
	UnselectAll=1

void
etk_editable_selection_show(editable)
	Evas_Object *editable
	ALIAS:
	SelectionShow=1

void
etk_editable_selection_hide(editable)
	Evas_Object *editable
	ALIAS:
	SelectionHide=1

int
etk_editable_pos_get_from_coords(editable, x, y)
	Evas_Object *editable
	int x
	int y
	ALIAS:
	PosGetFromCoods=1

void
etk_editable_char_size_get(editable)
	Evas_Object *editable
	ALIAS:
	CharSizeGet=1
	PPCODE:
	int w, h;
	etk_editable_char_size_get(editable, &w, &h);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));
	
	
MODULE = Etk::Timer	PACKAGE = Etk::Timer 

Ecore_Timer *
new(class, interval, callback, data=&PL_sv_undef)
	SV * class
        double interval
	SV *    callback
        SV *    data
      CODE:        
        Callback_Timer_Data *cbd;
        
        cbd = calloc(1, sizeof(Callback_Timer_Data));
        cbd->perl_data = newSVsv(data);
        cbd->perl_callback = newSVsv(callback);
        RETVAL = ecore_timer_add(interval, callback_timer, cbd);
      OUTPUT:
        RETVAL
	
void
Delete(timer)
      Ecore_Timer * timer
    CODE:
      ecore_timer_del(timer);

MODULE = Etk::Config	PACKAGE = Etk::Config	PREFIX = etk_config_

Etk_Bool
etk_config_init()
	ALIAS:
	Init=1

void
etk_config_shutdown()
	ALIAS:
	Shutdown=1

Etk_Bool
etk_config_load()
	ALIAS:
	Load=1

Etk_Bool
etk_config_save()
	ALIAS:
	Save=1

const char *
etk_config_widget_theme_get()
	ALIAS:
	WidgetThemeGet=1

void
etk_config_widget_theme_set(widget_theme)
	const char *widget_theme
	ALIAS:
	WidgetThemeSet=1

const char *
etk_config_font_get()
	ALIAS:
	FontGet=1

void
etk_config_font_set(font)
	const char * font
	ALIAS:
	FontSet=1

const char *
etk_config_engine_get()
	ALIAS:
	EngineGet=1

void
etk_config_engine_set(engine)
	const char *engine
	ALIAS:
	EngineSet=1


MODULE = Etk		PACKAGE = Etk	PREFIX = etk_


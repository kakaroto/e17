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

#include "const-c.inc"

typedef struct _Callback_Signal_Data Callback_Signal_Data;
typedef struct _Notification_Callback_Data Notification_Callback_Data;
typedef struct _Callback_Tree_Compare_Data Callback_Tree_Compare_Data;
typedef struct _Callback_Timer_Data Callback_Timer_Data;

/* Carries info for normal signals */
struct _Callback_Signal_Data
{
   char       *signal_name;   /* etk signal name */
   Etk_Object *object;        /* object signal is connected to */
   SV         *perl_object;   /* reference to the perl object */
   SV         *perl_callback; /* perl callback to be called */
   void       *perl_data;     /* perl data to pass to the perl callback */
};

/* Carries info for the notification callback */
struct _Notification_Callback_Data
{
   char		*property_name;
   Etk_Object	*object;
   SV		*perl_callback;
   void		*perl_data;
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
notification_callback(Etk_Object * object, const char * property_name, void * data)
{
   dSP;
   Notification_Callback_Data * ncb = NULL;

   ncb = data;

   PUSHMARK(SP);
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
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
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
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
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
   Etk_Event_Mouse_Up_Down *event = value;   
   HV *event_hv;
   SV *event_rv;
   cbd = data;   
   
   if(!strcmp(cbd->signal_name, "mouse_up"))
     {	
	event_hv = (HV*)sv_2mortal((SV*)newHV());	
	event_rv = newSViv(event->canvas.x);
	hv_store(event_hv, "canvas_x", strlen("canvas_x"), event_rv, 0);
	event_rv = newSViv(event->canvas.y);
	hv_store(event_hv, "canvas_y", strlen("canvas_y"), event_rv, 0);
	event_rv = newSViv(event->widget.x);
	hv_store(event_hv, "widget_x", strlen("widget_x"), event_rv, 0);
	event_rv = newSViv(event->widget.y);
	hv_store(event_hv, "widget_y", strlen("widget_y"), event_rv, 0);
	event_rv = newRV((SV*)event_hv);	
     }
   else if(!strcmp(cbd->signal_name, "row_mouse_in"))
     {
	SV *row_rv;
	HV *row_hv;
	row_rv = newRV(newSViv(0));
	sv_setref_iv(row_rv, "Etk_Tree_RowPtr", (IV) value);
	row_hv = newHV();
	hv_store(row_hv, "WIDGET", strlen("WIDGET"), row_rv, 0);
	event_rv = newRV((SV*)row_hv);
	sv_bless(event_rv, gv_stashpv("Etk::Tree::Row", FALSE));
     }
   else if(!strcmp(cbd->signal_name, "row_mouse_out"))
     {
	SV *row_rv;
	HV *row_hv;
	row_rv = newRV(newSViv(0));
	sv_setref_iv(row_rv, "Etk_Tree_RowPtr", (IV) value);
	row_hv = newHV();
	hv_store(row_hv, "WIDGET", strlen("WIDGET"), row_rv, 0);
	event_rv = newRV((SV*)row_hv);
	sv_bless(event_rv, gv_stashpv("Etk::Tree::Row", FALSE));
     }      
   else
     {
	event_hv = (HV*)sv_2mortal((SV*)newHV());
	event_rv = newRV((SV*)event_hv);
     }
   
   PUSHMARK(SP) ;
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   XPUSHs(sv_2mortal(event_rv));
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
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
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
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
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
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
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
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
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
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_object)));
   //XPUSHs(sv_2mortal(newSViv(value)));
   XPUSHs(sv_2mortal(newSVsv(cbd->perl_data)));   
   PUTBACK ;
      
   /* Call the Perl sub */
   call_sv(cbd->perl_callback, G_DISCARD);
}

static void
__etk_signal_connect_full(char *signal_name, SV *object, SV *callback, SV *data, Etk_Bool swapped, Etk_Bool after)
{
	dSP;

	Etk_Object *	obj;
	Callback_Signal_Data *cbd = NULL;
	Etk_Signal *sig = NULL;
	Etk_Marshaller marsh;

	HV * ref;
	SV ** o;

	ENTER;
	SAVETMPS;

	ref = (HV *)SvRV(object);
	o = hv_fetch( ref, "WIDGET", strlen("WIDGET"), 0);

	obj = ETK_OBJECT( (void *) SvIV(SvRV(*o)) );

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
   row1_rv = newRV((SV*)tree_hv);
   sv_bless(tree_rv, gv_stashpv("Etk::Tree", FALSE));   

   row1_rv = newRV(newSViv(0));
   sv_setref_iv(row1_rv, "Etk_Tree_RowPtr", (IV) row1);
   row1_hv = newHV();
   hv_store(row1_hv, "WIDGET", strlen("WIDGET"), row1_rv, 0);
   row1_rv = newRV((SV*)row1_hv);
   sv_bless(row1_rv, gv_stashpv("Etk::Tree::Row", FALSE));
   
   row2_rv = newRV(newSViv(0));
   sv_setref_iv(row2_rv, "Etk_Tree_RowPtr", (IV) row2);
   row2_hv = newHV();
   hv_store(row2_hv, "WIDGET", strlen("WIDGET"), row2_rv, 0);
   row2_rv = newRV((SV*)row2_hv);
   sv_bless(row2_rv, gv_stashpv("Etk::Tree::Row", FALSE));
   
   col_rv = newRV(newSViv(0));
   sv_setref_iv(col_rv, "Etk_Tree_ColPtr", (IV) col);
   col_hv = newHV();
   hv_store(col_hv, "WIDGET", strlen("WIDGET"), col_rv, 0);
   col_rv = newRV((SV*)col_hv);
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

   if(count != 1)
       croak("Improper return value from compare callback!\n");

   ret = POPi;

   PUTBACK;
   FREETMPS ;
   LEAVE ; 
   
   return ret;
}

Etk_Color
perl_hash_to_color(SV * color)
{
	dSP;
	Etk_Color col;

	ENTER;
	SAVETMPS;
	
	if (SvROK(color) && SvTYPE(SvRV(color)) == SVt_PVHV) 
	{
	    HV * hash;
	    SV ** val;

	    hash = (HV*)SvRV(color);

	    val = hv_fetch(hash, "r", strlen("r"), 0);
	    col.r = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "g", strlen("g"), 0);
	    col.g = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "b", strlen("b"), 0);
	    col.b = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "a", strlen("a"), 0);
	    col.a = val ? SvIV(*val) : 0;
		
	}

	PUTBACK;
	FREETMPS;
	LEAVE;

	return col;
}	

HV *
color_to_perl_hash(Etk_Color col)
{
	dSP;
	HV * hv;
	SV * color;

	ENTER;
	SAVETMPS;
	
	hv = (HV*)sv_2mortal((SV*)newHV());

	color = newSViv(col.r);
        hv_store(hv, "r", strlen("r"), color, 0);
	color = newSViv(col.g);
        hv_store(hv, "g", strlen("g"), color, 0);
	color = newSViv(col.b);
        hv_store(hv, "b", strlen("b"), color, 0);
	color = newSViv(col.a);
        hv_store(hv, "a", strlen("a"), color, 0);
	
	PUTBACK;
	FREETMPS;
	LEAVE;

	return hv;
}	


Etk_Geometry
perl_hash_to_geometry(SV * geometry)
{
	dSP;
	
	Etk_Geometry geo;

	ENTER;
	SAVETMPS;
	
	if (SvROK(geometry) && SvTYPE(SvRV(geometry)) == SVt_PVHV) 
	{
	    HV * hash;
	    SV ** val;

	    hash = (HV*)SvRV(geometry);

	    val = hv_fetch(hash, "x", strlen("x"), 0);
	    geo.x = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "y", strlen("y"), 0);
	    geo.y = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "w", strlen("w"), 0);
	    geo.w = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "h", strlen("h"), 0);
	    geo.h = val ? SvIV(*val) : 0;
		
	}

	PUTBACK;
	FREETMPS;
	LEAVE;
	
	return geo;
}	

HV *
geometry_to_perl_hash(Etk_Geometry geo)
{
	dSP;
	HV * hv;
	SV * geometry;
	
	ENTER;
	SAVETMPS;
	
	hv = (HV*)sv_2mortal((SV*)newHV());

	geometry = newSViv(geo.x);
        hv_store(hv, "x", strlen("x"), geometry, 0);
	geometry = newSViv(geo.y);
        hv_store(hv, "y", strlen("y"), geometry, 0);
	geometry = newSViv(geo.w);
        hv_store(hv, "w", strlen("w"), geometry, 0);
	geometry = newSViv(geo.h);
        hv_store(hv, "h", strlen("h"), geometry, 0);
	
	PUTBACK;
	FREETMPS;
	LEAVE;

	return hv;
}	

Etk_Size
perl_hash_to_size(SV * size)
{
	dSP;
	Etk_Size s;
	
	ENTER;
	SAVETMPS;
	
	if (SvROK(size) && SvTYPE(SvRV(size)) == SVt_PVHV) 
	{
	    HV * hash;
	    SV ** val;

	    hash = (HV*)SvRV(size);

	    val = hv_fetch(hash, "w", strlen("w"), 0);
	    s.w = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "h", strlen("h"), 0);
	    s.h = val ? SvIV(*val) : 0;

	}

	PUTBACK;
	FREETMPS;
	LEAVE;
	
	return s;
}	

HV *
size_to_perl_hash(Etk_Size s)
{
	dSP;
	HV * hv;
	SV * size;
	
	ENTER;
	SAVETMPS;
	
	hv = (HV*)sv_2mortal((SV*)newHV());

	size = newSViv(s.w);
        hv_store(hv, "w", strlen("w"), size, 0);
	size = newSViv(s.h);
        hv_store(hv, "h", strlen("h"), size, 0);
	
	PUTBACK;
	FREETMPS;
	LEAVE;
	
	return hv;
}	

AV *
evas_list_to_perl(Evas_List * list) 
{
	dSP;
	AV * av;
	Evas_List * l;

	ENTER;
	SAVETMPS;
	
	av = newAV();
	for (l = list; l; l = l->next)
		av_push(av, newSViv((IV)(l->data)));
	
	PUTBACK;
	FREETMPS;
	LEAVE;

	return av;
}

MODULE = Etk		PACKAGE = Etk		

INCLUDE: const-xs.inc

void
etk_alignment_get(alignment)
	Etk_Widget *	alignment
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
	Etk_Widget *	alignment
	float	xalign
	float	yalign
	float	xscale
	float	yscale
	CODE:
	etk_alignment_set(ETK_ALIGNMENT(alignment), xalign, yalign, xscale, yscale);

Etk_Widget *
etk_bin_child_get(bin)
	Etk_Widget *	bin
	CODE:
	RETVAL = etk_bin_child_get(ETK_BIN(bin));
	OUTPUT:
	RETVAL

void
etk_bin_child_set(bin, child)
	Etk_Widget *	bin
	Etk_Widget *	child
	CODE:
	etk_bin_child_set(ETK_BIN(bin), child);

void
etk_box_child_packing_get(box, child)
	Etk_Widget *	box
	Etk_Widget *	child
     PPCODE:
       int 	        padding;
       Etk_Bool   	expand;
       Etk_Bool   	fill;
       Etk_Bool   	pack_end;
       
       etk_box_child_packing_get(ETK_BOX(box), child, &padding, &expand, &fill,
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
	CODE:
	etk_box_child_packing_set(ETK_BOX(box), child, padding, expand, fill, pack_end);

Etk_Bool
etk_box_homogeneous_get(box)
	Etk_Widget *	box
	CODE:
	RETVAL = etk_box_homogeneous_get(ETK_BOX(box));
	OUTPUT:
	RETVAL

void
etk_box_homogeneous_set(box, homogeneous)
	Etk_Widget *	box
	Etk_Bool	homogeneous
	CODE:
	etk_box_homogeneous_set(ETK_BOX(box), homogeneous);

void
etk_box_pack_end(box, child, expand, fill, padding)
	Etk_Widget *	box
	Etk_Widget *	child
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding
	CODE:
	etk_box_pack_end(ETK_BOX(box), child, expand, fill, padding);

void
etk_box_pack_start(box, child, expand, fill, padding)
	Etk_Widget *	box
	Etk_Widget *	child
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding
	CODE:
	etk_box_pack_start(ETK_BOX(box), child, expand, fill, padding);

int
etk_box_spacing_get(box)
	Etk_Widget *	box
	CODE:
	RETVAL = etk_box_spacing_get(ETK_BOX(box));
	OUTPUT:
	RETVAL

void
etk_box_spacing_set(box, spacing)
	Etk_Widget *	box
	int	spacing
	CODE:
	etk_box_spacing_set(ETK_BOX(box), spacing);

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
	CODE:
	etk_button_alignment_set(ETK_BUTTON(button), xalign, yalign);

void
etk_button_click(button)
	Etk_Widget *	button
	CODE:
	etk_button_click(ETK_BUTTON(button));

Etk_Widget *
etk_button_image_get(button)
	Etk_Widget *	button
	CODE:
	Etk_Image * var;
	var = etk_button_image_get(ETK_BUTTON(button));
	RETVAL = ETK_WIDGET(var);
	OUTPUT:
	RETVAL

void
etk_button_image_set(button, image)
	Etk_Widget *	button
	Etk_Widget *	image
	CODE:
	etk_button_image_set(ETK_BUTTON(button), ETK_IMAGE(image));

const char *
etk_button_label_get(button)
	Etk_Widget *	button
	CODE:
	RETVAL = etk_button_label_get(ETK_BUTTON(button));
	OUTPUT:
	RETVAL

void
etk_button_label_set(button, label)
	Etk_Widget *	button
	char *	label
	CODE:
	etk_button_label_set(ETK_BUTTON(button), label);

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
	CODE:
	etk_button_press(ETK_BUTTON(button));

void
etk_button_release(button)
	Etk_Widget *	button
	CODE:
	etk_button_release(ETK_BUTTON(button));

void
etk_button_set_from_stock(button, stock_id)
	Etk_Widget *	button
	Etk_Stock_Id	stock_id
	CODE:
	etk_button_set_from_stock(ETK_BUTTON(button), stock_id);

Etk_Widget *
etk_canvas_new()

Etk_Bool
etk_canvas_object_add(canvas, object)
	Etk_Widget *	canvas
	Evas_Object *	object
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
	CODE:
	etk_canvas_object_remove(ETK_CANVAS(canvas), object);

Etk_Widget *
etk_check_button_new()

Etk_Widget *
etk_check_button_new_with_label(label)
	char *	label

void
etk_clipboard_text_request(widget)
	Etk_Widget *	widget

void
etk_clipboard_text_set(widget, data, length)
	Etk_Widget *	widget
	char *	data
	int	length

void
etk_colorpicker_current_color_get(cp)
	Etk_Widget *	cp
	PPCODE:
	Etk_Color var;
	HV * hv;
	SV * color;
	
	var = etk_colorpicker_current_color_get(ETK_COLORPICKER(cp));
	hv = color_to_perl_hash(var);

	color = newRV((SV*)hv);
	XPUSHs(sv_2mortal(color));
	

void
etk_colorpicker_current_color_set(cp, color)
	Etk_Widget *	cp
	SV *	color
	PPCODE:
	Etk_Color col;
	
	col = perl_hash_to_color(color);
	etk_colorpicker_current_color_set(ETK_COLORPICKER(cp), col);
	

Etk_Colorpicker_Mode
etk_colorpicker_mode_get(cp)
	Etk_Widget *	cp
	CODE:
	RETVAL = etk_colorpicker_mode_get(ETK_COLORPICKER(cp));
	OUTPUT:
	RETVAL

void
etk_colorpicker_mode_set(cp, mode)
	Etk_Widget *	cp
	Etk_Colorpicker_Mode	mode
	CODE:
	etk_colorpicker_mode_set(ETK_COLORPICKER(cp), mode);

Etk_Widget *
etk_colorpicker_new()

Etk_Combobox_Item *
etk_combobox_active_item_get(combobox)
	Etk_Widget *	combobox
	CODE:
	Etk_Combobox_Item * var;
	var = etk_combobox_active_item_get(ETK_COMBOBOX(combobox));
	RETVAL = var;
	OUTPUT:
	RETVAL

void
etk_combobox_active_item_set(combobox, item)
	Etk_Widget *	combobox
	Etk_Combobox_Item *	item
	CODE:
	etk_combobox_active_item_set(ETK_COMBOBOX(combobox), item);

void
etk_combobox_build(combobox)
	Etk_Widget *	combobox
	CODE:
	etk_combobox_build(ETK_COMBOBOX(combobox));

void
etk_combobox_clear(combobox)
	Etk_Widget *	combobox
	CODE:
	etk_combobox_clear(ETK_COMBOBOX(combobox));

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
	CODE:
	etk_combobox_column_add(ETK_COMBOBOX(combobox), col_type, size, expand, hfill, vfill, xalign, yalign);

void
etk_combobox_item_activate(item)
	Etk_Combobox_Item *	item

Etk_Combobox_Item *
etk_combobox_item_append_complex(combobox, ...)
        Etk_Widget * combobox
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
		{
		   ptr[i] = SvPV_nolen(ST(i + 1));
		}
	      else if (sv_derived_from(ST(i + 1), "Etk_WidgetPtr")) 
		{
		   IV tmp = SvIV((SV*)SvRV(ST(i + 1)));
		   ptr[i] = INT2PTR(Etk_Widget *,tmp);
		}
	   }
        switch(items)
        {	   
	   case 2:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0]);
	   break;
	   case 3:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1]);
	   break;
	   case 4:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2]);
	   break;
	   case 5:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4]);
	   break;
	   case 7:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5]);
	   break;
	   case 8:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4],
					     ptr[5], ptr[6]);
	   break;
	   case 9:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7]);
	   break;
	   case 10:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8]);
	   break;
	   case 11:
	   RETVAL = etk_combobox_item_append(ETK_COMBOBOX(combobox), ptr[0],
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
etk_combobox_item_prepend_complex(combobox, ...)
        Etk_Widget * combobox
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
		{
		   ptr[i] = SvPV_nolen(ST(i + 1));
		}
	      else if (sv_derived_from(ST(i + 1), "Etk_WidgetPtr")) 
		{
		   IV tmp = SvIV((SV*)SvRV(ST(i + 1)));
		   ptr[i] = INT2PTR(Etk_Widget *,tmp);
		}
	   }
        switch(items)
        {	   
	   case 2:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0]);
	   break;
	   case 3:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1]);
	   break;
	   case 4:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2]);
	   break;
	   case 5:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4]);
	   break;
	   case 7:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5]);
	   break;
	   case 8:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4],
					     ptr[5], ptr[6]);
	   break;
	   case 9:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7]);
	   break;
	   case 10:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
					     ptr[1], ptr[2], ptr[3], ptr[4], 
					     ptr[5], ptr[6], ptr[7], ptr[8]);
	   break;
	   case 11:
	   RETVAL = etk_combobox_item_prepend(ETK_COMBOBOX(combobox), ptr[0],
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
etk_combobox_item_prepend_relative_complex(combobox, relative, ...)
        Etk_Widget * combobox
        Etk_Combobox_Item * relative
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
	      if(SvPOK(ST(i + 2)))
		{
		   ptr[i] = SvPV_nolen(ST(i + 2));
		}
	      else if (sv_derived_from(ST(i + 2), "Etk_WidgetPtr")) 
		{
		   IV tmp = SvIV((SV*)SvRV(ST(i + 2)));
		   ptr[i] = INT2PTR(Etk_Widget *,tmp);
		}
	   }
        switch(items)
        {	   
	   case 2:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0]);
	   break;
	   case 3:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0], 
						       ptr[1]);
	   break;
	   case 4:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2]);
	   break;
	   case 5:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4]);
	   break;
	   case 7:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5]);
	   break;
	   case 8:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6]);
	   break;
	   case 9:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6],
						       ptr[7]);
	   break;
	   case 10:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6],
						       ptr[7], ptr[8]);
	   break;
	   case 11:
	   RETVAL = etk_combobox_item_prepend_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3], 
						       ptr[4], ptr[5], ptr[6], 
						       ptr[7], ptr[8], ptr[9]);
	   break;
	}
        if(ptr)
          free(ptr);
    OUTPUT:
        RETVAL
  
Etk_Combobox_Item *
etk_combobox_item_append_relative_complex(combobox, relative, ...)
        Etk_Widget * combobox
        Etk_Combobox_Item * relative
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
	      if(SvPOK(ST(i + 2)))
		{
		   ptr[i] = SvPV_nolen(ST(i + 2));
		}
	      else if (sv_derived_from(ST(i + 2), "Etk_WidgetPtr")) 
		{
		   IV tmp = SvIV((SV*)SvRV(ST(i + 2)));
		   ptr[i] = INT2PTR(Etk_Widget *,tmp);
		}
	   }
        switch(items)
        {	   
	   case 2:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0]);
	   break;
	   case 3:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0], 
						       ptr[1]);
	   break;
	   case 4:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2]);
	   break;
	   case 5:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3]);
	   break;
	   case 6:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4]);
	   break;
	   case 7:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5]);
	   break;
	   case 8:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6]);
	   break;
	   case 9:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6],
						       ptr[7]);
	   break;
	   case 10:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3],
						       ptr[4], ptr[5], ptr[6],
						       ptr[7], ptr[8]);
	   break;
	   case 11:
	   RETVAL = etk_combobox_item_append_relative(ETK_COMBOBOX(combobox), 
						       relative, ptr[0],
						       ptr[1], ptr[2], ptr[3], 
						       ptr[4], ptr[5], ptr[6], 
						       ptr[7], ptr[8], ptr[9]);
	   break;
	}
        if(ptr)
          free(ptr);
    OUTPUT:
        RETVAL  
  
SV *
etk_combobox_item_data_get(item)
	Etk_Combobox_Item *	item

void
etk_combobox_item_data_set(item, data)
	Etk_Combobox_Item *	item
	SV *	data
      CODE:
        etk_combobox_item_data_set(ETK_COMBOBOX_ITEM(item), newSVsv(data));

void
etk_combobox_item_data_set_full(item, data, free_cb)
	Etk_Combobox_Item *	item
	void *	data
	void ( * ) ( void * data ) free_cb

int
etk_combobox_item_height_get(combobox)
	Etk_Widget *	combobox
	CODE:
	RETVAL = etk_combobox_item_height_get(ETK_COMBOBOX(combobox));
	OUTPUT:
	RETVAL

void
etk_combobox_item_height_set(combobox, item_height)
	Etk_Widget *	combobox
	int	item_height
	CODE:
	etk_combobox_item_height_set(ETK_COMBOBOX(combobox), item_height);

void
etk_combobox_item_remove(combobox, item)
	Etk_Widget *	combobox
	Etk_Combobox_Item *	item
	CODE:
	etk_combobox_item_remove(ETK_COMBOBOX(combobox), item);

Etk_Widget *
etk_combobox_new()

Etk_Widget *
etk_combobox_new_default()

Etk_Combobox_Item *
etk_combobox_nth_item_get(combobox, index)
	Etk_Widget *	combobox
	int	index
	CODE:
	RETVAL = etk_combobox_nth_item_get(ETK_COMBOBOX(combobox), index);
	OUTPUT:
	RETVAL

void
etk_container_add(container, widget)
	Etk_Widget *	container
	Etk_Widget *	widget
	CODE:
	etk_container_add(ETK_CONTAINER(container), widget);

int
etk_container_border_width_get(container)
	Etk_Widget *	container
	CODE:
	RETVAL = etk_container_border_width_get(ETK_CONTAINER(container));
	OUTPUT:
	RETVAL

void
etk_container_border_width_set(container, border_width)
	Etk_Widget *	container
	int	border_width
	CODE:
	etk_container_border_width_set(ETK_CONTAINER(container), border_width);

void
etk_container_child_space_fill(child, child_space, hfill, vfill, xalign, yalign)
	Etk_Widget *	child
	SV *	child_space
	Etk_Bool	hfill
	Etk_Bool	vfill
	float	xalign
	float	yalign
	CODE:
	Etk_Geometry sp;
	sp = perl_hash_to_geometry(child_space);
	etk_container_child_space_fill(child, &sp, hfill, vfill, xalign, yalign);
	

void
etk_container_remove(container, widget)
	Etk_Widget *	container
	Etk_Widget *	widget
	CODE:
	etk_container_remove(ETK_CONTAINER(container), widget);

Etk_Widget *
etk_dialog_button_add(dialog, label, response_id)
	Etk_Widget *	dialog
	char *	label
	int	response_id
	CODE:
	RETVAL = etk_dialog_button_add(ETK_DIALOG(dialog), label, response_id);
	OUTPUT:
	RETVAL

Etk_Widget *
etk_dialog_button_add_from_stock(dialog, stock_id, response_id)
	Etk_Widget *	dialog
	int	stock_id
	int	response_id
	CODE:
	RETVAL = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), stock_id, response_id);
	OUTPUT:
	RETVAL
	

Etk_Bool
etk_dialog_has_separator_get(dialog)
	Etk_Widget *	dialog
	CODE:
	RETVAL = etk_dialog_has_separator_get(ETK_DIALOG(dialog));
	OUTPUT:
	RETVAL

void
etk_dialog_has_separator_set(dialog, has_separator)
	Etk_Widget *	dialog
	Etk_Bool	has_separator
	CODE:
	etk_dialog_has_separator_set(ETK_DIALOG(dialog), has_separator);

Etk_Widget *
etk_dialog_new()

void
etk_dialog_pack_button_in_action_area(dialog, button, response_id, expand, fill, padding, pack_at_end)
	Etk_Widget *	dialog
	Etk_Widget *	button
	int	response_id
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding
	Etk_Bool	pack_at_end
	CODE:
	etk_dialog_pack_button_in_action_area(ETK_DIALOG(dialog), ETK_BUTTON(button), response_id, expand, fill, padding, pack_at_end);

void
etk_dialog_pack_in_main_area(dialog, widget, expand, fill, padding, pack_at_end)
	Etk_Widget *	dialog
	Etk_Widget *	widget
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding
	Etk_Bool	pack_at_end
	CODE:
	etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), widget, expand, fill, padding, pack_at_end);

void
etk_dialog_pack_widget_in_action_area(dialog, widget, expand, fill, padding, pack_at_end)
	Etk_Widget *	dialog
	Etk_Widget *	widget
	Etk_Bool	expand
	Etk_Bool	fill
	int	padding
	Etk_Bool	pack_at_end
	CODE:
	etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog), widget, expand, fill, padding, pack_at_end);

Etk_Bool
etk_dnd_init()

void
etk_dnd_shutdown()

void
etk_drag_begin(drag)
	Etk_Widget *	drag
	CODE:
	etk_drag_begin(ETK_DRAG(drag));

void
etk_drag_data_set(drag, data, size)
	Etk_Widget *	drag
	SV *	data
	CODE:
	etk_drag_data_set(ETK_DRAG(drag), newSVsv(data), sizeof(SV));

Etk_Widget *
etk_drag_new(widget)
	Etk_Widget *	widget

Etk_Widget *
etk_drag_parent_widget_get(drag)
	Etk_Widget *	drag
	CODE:
	etk_drag_parent_widget_get(ETK_DRAG(drag));

void
etk_drag_parent_widget_set(drag, widget)
	Etk_Widget *	drag
	Etk_Widget *	widget
	CODE:
	etk_drag_parent_widget_set(ETK_DRAG(drag), widget);

void
etk_drag_types_set(drag, types)
	Etk_Widget *	drag
	AV * types
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
	
	etk_drag_types_set(ETK_DRAG(drag), t, num_types);


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

const char *
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
	CODE:
	RETVAL = etk_entry_password_get(ETK_ENTRY(entry));
	OUTPUT:
	RETVAL

void
etk_entry_password_set(entry, on)
	Etk_Widget *	entry
	Etk_Bool	on
	CODE:
	etk_entry_password_set(ETK_ENTRY(entry), on);

const char *
etk_entry_text_get(entry)
	Etk_Widget *	entry
	CODE:
	RETVAL = etk_entry_text_get(ETK_ENTRY(entry));
	OUTPUT:
	RETVAL

void
etk_entry_text_set(entry, text)
	Etk_Widget *	entry
	char *	text
	CODE:
	etk_entry_text_set(ETK_ENTRY(entry), text);

const char *
etk_filechooser_widget_current_folder_get(filechooser_widget)
	Etk_Widget *	filechooser_widget
	CODE:
	RETVAL = etk_filechooser_widget_current_folder_get(ETK_FILECHOOSER_WIDGET(filechooser_widget));
	OUTPUT:
	RETVAL
	

void
etk_filechooser_widget_current_folder_set(filechooser_widget, folder)
	Etk_Widget *	filechooser_widget
	char *	folder
	CODE:
	etk_filechooser_widget_current_folder_set(ETK_FILECHOOSER_WIDGET(filechooser_widget), folder);


Etk_Widget *
etk_filechooser_widget_new()

Etk_Bool
etk_filechooser_widget_select_multiple_get(filechooser_widget)
	Etk_Widget *	filechooser_widget
	CODE:
	RETVAL = etk_filechooser_widget_select_multiple_get(ETK_FILECHOOSER_WIDGET(filechooser_widget));
	OUTPUT:
	RETVAL

void
etk_filechooser_widget_select_multiple_set(filechooser_widget, select_multiple)
	Etk_Widget *	filechooser_widget
	Etk_Bool	select_multiple
	CODE:
	etk_filechooser_widget_select_multiple_set(ETK_FILECHOOSER_WIDGET(filechooser_widget), select_multiple);

const char *
etk_filechooser_widget_selected_file_get(widget)
	Etk_Widget *	widget
	CODE:
	const char * val;
	val = etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(widget));
	if (val)
		RETVAL = val;
	else
		RETVAL = "";
	OUTPUT:
	RETVAL


void
etk_filechooser_widget_selected_files_get(widget)
	Etk_Widget *	widget
	PPCODE:
	Evas_List * list;
	AV * av;
	int i;

	list = etk_filechooser_widget_selected_files_get(ETK_FILECHOOSER_WIDGET(widget));
	av = evas_list_to_perl(list);
	for (i=0; i<=av_len(av); i++) 
	{
		SV * sv;
		const char * filename;

		filename = (const char *)SvIV(av_shift(av));
		sv = newSVpv(filename, strlen(filename));

		XPUSHs(sv_2mortal(sv));
	}


Etk_Bool
etk_filechooser_widget_show_hidden_get(filechooser_widget)
	Etk_Widget *	filechooser_widget
	CODE:
	RETVAL = etk_filechooser_widget_show_hidden_get(ETK_FILECHOOSER_WIDGET(filechooser_widget));
	OUTPUT:
	RETVAL

void
etk_filechooser_widget_show_hidden_set(filechooser_widget, show_hidden)
	Etk_Widget *	filechooser_widget
	Etk_Bool	show_hidden
	CODE:
	etk_filechooser_widget_show_hidden_set(ETK_FILECHOOSER_WIDGET(filechooser_widget), show_hidden);
	

const char *
etk_frame_label_get(frame)
	Etk_Widget *	frame
	CODE:
	const char * var;
	var = etk_frame_label_get(ETK_FRAME(frame));
	RETVAL = var;
	OUTPUT:
	RETVAL

void
etk_frame_label_set(frame, label)
	Etk_Widget *	frame
	char *	label
	CODE:
	etk_frame_label_set(ETK_FRAME(frame), label);

Etk_Widget *
etk_frame_new(label)
	char *	label

Etk_Widget *
etk_hbox_new(homogeneous, spacing)
	Etk_Bool	homogeneous
	int	spacing

Etk_Widget *
etk_hpaned_new()

Etk_Widget *
etk_hscrollbar_new(lower, upper, value, step_increment, page_increment, page_size)
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	double	page_size

Etk_Widget *
etk_hseparator_new()

Etk_Widget *
etk_hslider_new(lower, upper, value, step_increment, page_increment)
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment

Etk_Iconbox_Icon *
etk_iconbox_append(iconbox, filename, edje_group, label)
	Etk_Widget *	iconbox
	char *	filename
	char *	edje_group
	char *	label
	CODE:
	RETVAL = etk_iconbox_append(ETK_ICONBOX(iconbox), filename, edje_group, label);
	OUTPUT:
	RETVAL

void
etk_iconbox_clear(iconbox)
	Etk_Widget *	iconbox
	CODE:
	etk_iconbox_clear(ETK_ICONBOX(iconbox));

Etk_Iconbox_Model *
etk_iconbox_current_model_get(iconbox)
	Etk_Widget *	iconbox
	CODE:
	RETVAL = etk_iconbox_current_model_get(ETK_ICONBOX(iconbox));
	OUTPUT:
	RETVAL

void
etk_iconbox_current_model_set(iconbox, model)
	Etk_Widget *	iconbox
	Etk_Iconbox_Model *	model
	CODE:
	etk_iconbox_current_model_set(ETK_ICONBOX(iconbox), model);

void
etk_iconbox_freeze(iconbox)
	Etk_Widget *	iconbox
	CODE:
	etk_iconbox_freeze(ETK_ICONBOX(iconbox));

SV *
etk_iconbox_icon_data_get(icon)
	Etk_Iconbox_Icon *	icon

void
etk_iconbox_icon_data_set(icon, data)
	Etk_Iconbox_Icon *	icon
	SV *	data

void
etk_iconbox_icon_del(icon)
	Etk_Iconbox_Icon *	icon

void
etk_iconbox_icon_file_get(icon)
	Etk_Iconbox_Icon *	icon
	
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

Etk_Iconbox_Icon *
etk_iconbox_icon_get_at_xy(iconbox, x, y, over_cell, over_icon, over_label)
	Etk_Widget *	iconbox
	int	x
	int	y
	Etk_Bool	over_cell
	Etk_Bool	over_icon
	Etk_Bool	over_label
	CODE:
	RETVAL = etk_iconbox_icon_get_at_xy(ETK_ICONBOX(iconbox), x, y, over_cell, over_icon, over_label);
	OUTPUT:
	RETVAL

const char *
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
        PUSHs(sv_2mortal(newSViv(xalign)));
        PUSHs(sv_2mortal(newSViv(yalign)));	


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
	Etk_Widget *	iconbox
	CODE:
	RETVAL = etk_iconbox_model_new(ETK_ICONBOX(iconbox));
	OUTPUT:
	RETVAL

Etk_Widget *
etk_iconbox_new()

void
etk_iconbox_select_all(iconbox)
	Etk_Widget *	iconbox
	CODE:
	etk_iconbox_select_all(ETK_ICONBOX(iconbox));

void
etk_iconbox_thaw(iconbox)
	Etk_Widget *	iconbox
	CODE:
	etk_iconbox_thaw(ETK_ICONBOX(iconbox));

void
etk_iconbox_unselect_all(iconbox)
	Etk_Widget *	iconbox
	CODE:
	etk_iconbox_unselect_all(ETK_ICONBOX(iconbox));

void
etk_image_copy(dest_image, src_image)
	Etk_Widget *	dest_image
	Etk_Widget *	src_image
	CODE:
	etk_image_copy(ETK_IMAGE(dest_image), ETK_IMAGE(src_image));

void
etk_image_edje_get(image, edje_filename, edje_group)
	Etk_Widget *	image
	PPCODE:
	char *	edje_filename;
	char *	edje_group;
	etk_image_edje_get(ETK_IMAGE(image), &edje_filename, &edje_group);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVpv(edje_filename, strlen(edje_filename))));
	PUSHs(sv_2mortal(newSVpv(edje_group, strlen(edje_group))));

const char *
etk_image_file_get(image)
	Etk_Widget *	image
	CODE:
	RETVAL = etk_image_file_get(ETK_IMAGE(image));
	OUTPUT:
	RETVAL

Etk_Bool
etk_image_keep_aspect_get(image)
	Etk_Widget *	image
	CODE:
	RETVAL = etk_image_keep_aspect_get(ETK_IMAGE(image));
	OUTPUT:
	RETVAL

void
etk_image_keep_aspect_set(image, keep_aspect)
	Etk_Widget *	image
	Etk_Bool	keep_aspect
	CODE:
	etk_image_keep_aspect_set(ETK_IMAGE(image), keep_aspect);

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
	int	stock_size

void
etk_image_set_from_edje(image, edje_filename, edje_group)
	Etk_Widget *	image
	char *	edje_filename
	char *	edje_group
	CODE:
	etk_image_set_from_edje(ETK_IMAGE(image), edje_filename, edje_group);

void
etk_image_set_from_file(image, filename)
	Etk_Widget *	image
	char *	filename
	CODE:
	etk_image_set_from_file(ETK_IMAGE(image), filename);

void
etk_image_set_from_stock(image, stock_id, stock_size)
	Etk_Widget *	image
	Etk_Stock_Id	stock_id
	Etk_Stock_Size	stock_size
	CODE:
	etk_image_set_from_stock(ETK_IMAGE(image), stock_id, stock_size);

void
etk_image_size_get(image, width, height)
	Etk_Widget *	image
	PPCODE:
	int 	width;
	int 	height;
	etk_image_size_get(ETK_IMAGE(image), &width, &height);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(width)));
	PUSHs(sv_2mortal(newSViv(height)));

void
etk_image_stock_get(image)
	Etk_Widget *	image
	PPCODE:
	Etk_Stock_Id 	stock_id;
	Etk_Stock_Size 	stock_size;
	
	etk_image_stock_get(ETK_IMAGE(image), &stock_id, &stock_size);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(stock_id)));
	PUSHs(sv_2mortal(newSViv(stock_size)));

Etk_Bool
etk_init()

void
etk_label_alignment_get(label)
	Etk_Widget *	label
	PPCODE:
	float xalign;
	float yalign;
	etk_label_alignment_get(ETK_LABEL(label), &xalign, &yalign);

	XPUSHs(sv_2mortal(newSViv(xalign)));
	XPUSHs(sv_2mortal(newSViv(yalign)));

void
etk_label_alignment_set(label, xalign, yalign)
	Etk_Widget *	label
	float	xalign
	float	yalign
	CODE:
	etk_label_alignment_set(ETK_LABEL(label), xalign, yalign);

const char *
etk_label_get(label)
	Etk_Widget *	label
	CODE:
	RETVAL = etk_label_get(ETK_LABEL(label));
	OUTPUT:
	RETVAL

Etk_Widget *
etk_label_new(text)
	char *	text

void
etk_label_set(label, text)
	Etk_Widget *	label
	char *	text
	CODE:
	etk_label_set(ETK_LABEL(label), text);

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
	Etk_Widget *	widget
	CODE:
	etk_main_toplevel_widget_add(ETK_TOPLEVEL_WIDGET(widget));

void
etk_main_toplevel_widget_remove(widget)
	Etk_Widget *	widget
	CODE:
	etk_main_toplevel_widget_remove(ETK_TOPLEVEL_WIDGET(widget));

void
etk_main_toplevel_widgets_get()
	PPCODE:
	Evas_List * list;
	AV * av;
	int i;

	list = etk_main_toplevel_widgets_get();
	av = evas_list_to_perl(list);

	for (i = av_len(av); i>=0; i--)
	{
		SV * sv;
		sv = newRV(newSViv(0));
		sv_setref_iv(sv, "Etk_WidgetPtr", SvIV(av_shift(av)));

		XPUSHs(sv_2mortal(sv));
	}
	av_undef(av);

Etk_Widget *
etk_menu_bar_new()

void
etk_menu_item_activate(menu_item)
	Etk_Widget *	menu_item
	CODE:
	etk_menu_item_activate(ETK_MENU_ITEM(menu_item));

Etk_Bool
etk_menu_item_check_active_get(check_item)
	Etk_Widget *	check_item
	CODE:
	RETVAL = etk_menu_item_check_active_get(ETK_MENU_ITEM_CHECK(check_item));
	OUTPUT:
	RETVAL

void
etk_menu_item_check_active_set(check_item, active)
	Etk_Widget *	check_item
	Etk_Bool	active
	CODE:
	etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(check_item), active);

Etk_Widget *
etk_menu_item_check_new()

Etk_Widget *
etk_menu_item_check_new_with_label(label)
	char *	label

void
etk_menu_item_deselect(menu_item)
	Etk_Widget *	menu_item
	CODE:
	etk_menu_item_deselect(ETK_MENU_ITEM(menu_item));

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
	Etk_Widget *	image_item
	Etk_Widget *	image
	CODE:
	etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(image_item), ETK_IMAGE(image));

const char *
etk_menu_item_label_get(menu_item)
	Etk_Widget *	menu_item
	CODE:
	RETVAL = etk_menu_item_label_get(ETK_MENU_ITEM(menu_item));
	OUTPUT:
	RETVAL

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

Etk_Widget *
etk_menu_item_radio_new_from_widget(radio_item)
	Etk_Widget *	radio_item
	CODE:
	RETVAL = etk_menu_item_radio_new_from_widget(ETK_MENU_ITEM_RADIO(radio_item));
	OUTPUT:
	RETVAL

Etk_Widget *
etk_menu_item_radio_new_with_label(label)
	char *	label
	CODE:
	RETVAL = etk_menu_item_radio_new_with_label(label, NULL);
	OUTPUT:
	RETVAL	

Etk_Widget *
etk_menu_item_radio_new_with_label_from_widget(label, radio_item)
	char *	label
	Etk_Widget *	radio_item
	CODE:
	RETVAL = etk_menu_item_radio_new_with_label_from_widget(label, ETK_MENU_ITEM_RADIO(radio_item));
	OUTPUT:
	RETVAL

void
etk_menu_item_select(menu_item)
	Etk_Widget *	menu_item
	CODE:
	etk_menu_item_select(ETK_MENU_ITEM(menu_item));

Etk_Widget *
etk_menu_item_separator_new()

void
etk_menu_item_set_from_stock(menu_item, stock_id)
	Etk_Widget *	menu_item
	Etk_Stock_Id	stock_id
	CODE:
	etk_menu_item_set_from_stock(ETK_MENU_ITEM(menu_item), stock_id);

void
etk_menu_item_submenu_set(menu_item, submenu)
	Etk_Widget *	menu_item
	Etk_Widget *	submenu
	CODE:
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));

Etk_Widget *
etk_menu_new()

void
etk_menu_popdown(menu)
	Etk_Widget *	menu
	CODE:
	etk_menu_popdown(ETK_MENU(menu));

void
etk_menu_popup(menu)
	Etk_Widget *	menu
	CODE:
	etk_menu_popup(ETK_MENU(menu));

void
etk_menu_popup_at_xy(menu, x, y)
	Etk_Widget *	menu
	int	x
	int	y
	CODE:
	etk_menu_popup_at_xy(ETK_MENU(menu), x, y);

void
etk_menu_shell_append(menu_shell, item)
	Etk_Widget *	menu_shell
	Etk_Widget *	item
	CODE:
	etk_menu_shell_append(ETK_MENU_SHELL(menu_shell), ETK_MENU_ITEM(item));

void
etk_menu_shell_append_relative(menu_shell, item, relative)
	Etk_Widget *	menu_shell
	Etk_Widget *	item
	Etk_Widget *	relative
	CODE:
	etk_menu_shell_append_relative(ETK_MENU_SHELL(menu_shell), ETK_MENU_ITEM(item), ETK_MENU_ITEM(relative));

void
etk_menu_shell_insert(menu_shell, item, position)
	Etk_Widget *	menu_shell
	Etk_Widget *	item
	int	position
	CODE:
	etk_menu_shell_insert(ETK_MENU_SHELL(menu_shell), ETK_MENU_ITEM(item), position);

void
etk_menu_shell_items_get(menu_shell)
	Etk_Widget *	menu_shell
	PPCODE:
	Evas_List * list;
	AV * av;
	int i;

	list = etk_menu_shell_items_get(ETK_MENU_SHELL(menu_shell));
	av = evas_list_to_perl(list);

        for (i = av_len(av) ; i >=0 ; i--)
        {
               SV * sv;
               sv = newRV(newSViv(0));
               sv_setref_iv(sv, "Etk_WidgetPtr", SvIV(av_shift(av)));
   
               XPUSHs(sv_2mortal(sv));
        }
         av_undef(av);  

void
etk_menu_shell_prepend(menu_shell, item)
	Etk_Widget *	menu_shell
	Etk_Widget *	item
	CODE:
	etk_menu_shell_prepend(ETK_MENU_SHELL(menu_shell), ETK_MENU_ITEM(item));

void
etk_menu_shell_prepend_relative(menu_shell, item, relative)
	Etk_Widget *	menu_shell
	Etk_Widget *	item
	Etk_Widget *	relative
	CODE:
	etk_menu_shell_prepend_relative(ETK_MENU_SHELL(menu_shell), ETK_MENU_ITEM(item), ETK_MENU_ITEM(relative));

void
etk_menu_shell_remove(menu_shell, item)
	Etk_Widget *	menu_shell
	Etk_Widget *	item
	CODE:
	etk_menu_shell_remove(ETK_MENU_SHELL(menu_shell), ETK_MENU_ITEM(item));
	

Etk_Message_Dialog_Buttons
etk_message_dialog_buttons_get(dialog)
	Etk_Widget *	dialog
	CODE:
	RETVAL = etk_message_dialog_buttons_get(ETK_MESSAGE_DIALOG(dialog));
	OUTPUT:
	RETVAL

void
etk_message_dialog_buttons_set(dialog, buttons)
	Etk_Widget *	dialog
	Etk_Message_Dialog_Buttons	buttons
	CODE:
	etk_message_dialog_buttons_set(ETK_MESSAGE_DIALOG(dialog), buttons);

void
etk_message_dialog_message_type_set(dialog, type)
	Etk_Widget *	dialog
	Etk_Message_Dialog_Type	type
	CODE:
	etk_message_dialog_message_type_set(ETK_MESSAGE_DIALOG(dialog), type);

Etk_Widget *
etk_message_dialog_new(message_type, buttons, text)
	Etk_Message_Dialog_Type	message_type
	Etk_Message_Dialog_Buttons	buttons
	char *	text

const char *
etk_message_dialog_text_get(dialog)
	Etk_Widget *	dialog
	CODE:
	RETVAL = etk_message_dialog_text_get(ETK_MESSAGE_DIALOG(dialog));
	OUTPUT:
	RETVAL

void
etk_message_dialog_text_set(dialog, text)
	Etk_Widget *	dialog
	char *	text
	CODE:
	etk_message_dialog_text_set(ETK_MESSAGE_DIALOG(dialog), text);

int
etk_notebook_current_page_get(notebook)
	Etk_Widget *	notebook
	CODE:
	RETVAL = etk_notebook_current_page_get(ETK_NOTEBOOK(notebook));
	OUTPUT:
	RETVAL

void
etk_notebook_current_page_set(notebook, page_num)
	Etk_Widget *	notebook
	int	page_num
	CODE:
	etk_notebook_current_page_set(ETK_NOTEBOOK(notebook), page_num);

Etk_Widget *
etk_notebook_new()

int
etk_notebook_num_pages_get(notebook)
	Etk_Widget *	notebook
	CODE:
	RETVAL = etk_notebook_num_pages_get(ETK_NOTEBOOK(notebook));
	OUTPUT:
	RETVAL

int
etk_notebook_page_append(notebook, tab_label, page_child)
	Etk_Widget *	notebook
	char *	tab_label
	Etk_Widget *	page_child
	CODE:
	RETVAL = etk_notebook_page_append(ETK_NOTEBOOK(notebook), tab_label, page_child);
	OUTPUT:
	RETVAL

Etk_Widget *
etk_notebook_page_child_get(notebook, page_num)
	Etk_Widget *	notebook
	int	page_num
	CODE:
	RETVAL = etk_notebook_page_child_get(ETK_NOTEBOOK(notebook), page_num);
	OUTPUT:
	RETVAL

void
etk_notebook_page_child_set(notebook, page_num, child)
	Etk_Widget *	notebook
	int	page_num
	Etk_Widget *	child
	CODE:
	etk_notebook_page_child_set(ETK_NOTEBOOK(notebook), page_num, child);

int
etk_notebook_page_index_get(notebook, child)
	Etk_Widget *	notebook
	Etk_Widget *	child
	CODE:
	RETVAL = etk_notebook_page_index_get(ETK_NOTEBOOK(notebook), child);
	OUTPUT:
	RETVAL

int
etk_notebook_page_insert(notebook, tab_label, page_child, position)
	Etk_Widget *	notebook
	char *	tab_label
	Etk_Widget *	page_child
	int	position
	CODE:
	RETVAL = etk_notebook_page_insert(ETK_NOTEBOOK(notebook), tab_label, page_child, position);
	OUTPUT:
	RETVAL

int
etk_notebook_page_next(notebook)
	Etk_Widget *	notebook
	CODE:
	RETVAL = etk_notebook_page_next(ETK_NOTEBOOK(notebook));
	OUTPUT:
	RETVAL

int
etk_notebook_page_prepend(notebook, tab_label, page_child)
	Etk_Widget *	notebook
	char *	tab_label
	Etk_Widget *	page_child
	CODE:
	RETVAL = etk_notebook_page_prepend(ETK_NOTEBOOK(notebook), tab_label, page_child);
	OUTPUT:
	RETVAL

int
etk_notebook_page_prev(notebook)
	Etk_Widget *	notebook
	CODE:
	RETVAL = etk_notebook_page_prev(ETK_NOTEBOOK(notebook));
	OUTPUT:
	RETVAL

void
etk_notebook_page_remove(notebook, page_num)
	Etk_Widget *	notebook
	int	page_num
	CODE:
	etk_notebook_page_remove(ETK_NOTEBOOK(notebook), page_num);

const char *
etk_notebook_page_tab_label_get(notebook, page_num)
	Etk_Widget *	notebook
	int	page_num
	CODE:
	RETVAL = etk_notebook_page_tab_label_get(ETK_NOTEBOOK(notebook), page_num);
	OUTPUT:
	RETVAL

void
etk_notebook_page_tab_label_set(notebook, page_num, tab_label)
	Etk_Widget *	notebook
	int	page_num
	char *	tab_label
	CODE:
	etk_notebook_page_tab_label_set(ETK_NOTEBOOK(notebook), page_num, tab_label);

Etk_Widget *
etk_notebook_page_tab_widget_get(notebook, page_num)
	Etk_Widget *	notebook
	int	page_num
	CODE:
	RETVAL = etk_notebook_page_tab_widget_get(ETK_NOTEBOOK(notebook), page_num);
	OUTPUT:
	RETVAL

void
etk_notebook_page_tab_widget_set(notebook, page_num, tab_widget)
	Etk_Widget *	notebook
	int	page_num
	Etk_Widget *	tab_widget
	CODE:
	etk_notebook_page_tab_widget_set(ETK_NOTEBOOK(notebook), page_num, tab_widget);

SV *
etk_object_data_get(object, key)
	Etk_Widget *	object
	char *	key
	CODE:
	RETVAL = etk_object_data_get(ETK_OBJECT(object), key);
	OUTPUT:
	RETVAL

void
etk_object_data_set(object, key, value)
	Etk_Widget *	object
	char *	key
	SV *	value
	CODE:
	etk_object_data_set(ETK_OBJECT(object), key, value);

void
etk_object_notification_callback_add(object, property_name, callback, data)
	Etk_Widget *	object
	char *	property_name
	SV *	callback
	SV *	data

	CODE:
	Notification_Callback_Data *ncb = NULL;

	ncb = calloc(1, sizeof(Notification_Callback_Data));
	ncb->property_name = strdup(property_name);
	ncb->object = ETK_OBJECT(object);
	ncb->perl_data = newSVsv(data);
	ncb->perl_callback = newSVsv(callback);

	etk_object_notification_callback_add(ETK_OBJECT(object), property_name, notification_callback, ncb);

void
etk_object_notification_callback_remove(object, property_name, callback)
	Etk_Widget *	object
	char *	property_name
	CODE:
	etk_object_notification_callback_remove(ETK_OBJECT(object), property_name, notification_callback);

void
etk_object_notify(object, property_name)
	Etk_Widget *	object
	char *	property_name
	CODE:
	etk_object_notify(ETK_OBJECT(object), property_name);

Etk_Widget *
etk_paned_child1_get(paned)
	Etk_Widget *	paned
	CODE:
	RETVAL = etk_paned_child1_get(ETK_PANED(paned));
	OUTPUT:
	RETVAL

void
etk_paned_child1_set(paned, child, expand)
	Etk_Widget *	paned
	Etk_Widget *	child
	Etk_Bool	expand
	CODE:
	etk_paned_child1_set(ETK_PANED(paned), child, expand);

Etk_Widget *
etk_paned_child2_get(paned)
	Etk_Widget *	paned
	CODE:
	RETVAL = etk_paned_child2_get(ETK_PANED(paned));
	OUTPUT:
	RETVAL

void
etk_paned_child2_set(paned, child, expand)
	Etk_Widget *	paned
	Etk_Widget *	child
	Etk_Bool	expand
	CODE:
	etk_paned_child2_set(ETK_PANED(paned), child, expand);

int
etk_paned_position_get(paned)
	Etk_Widget *	paned
	CODE:
	RETVAL = etk_paned_position_get(ETK_PANED(paned));
	OUTPUT:
	RETVAL

void
etk_paned_position_set(paned, position)
	Etk_Widget *	paned
	int	position
	CODE:
	etk_paned_position_set(ETK_PANED(paned), position);

Etk_Popup_Window *
etk_popup_window_focused_window_get()

void
etk_popup_window_focused_window_set(popup_window)
	Etk_Widget *	popup_window
	CODE:
	etk_popup_window_focused_window_set(ETK_POPUP_WINDOW(popup_window));

Etk_Bool
etk_popup_window_is_popped_up(popup_window)
	Etk_Widget *	popup_window
	CODE:
	RETVAL = etk_popup_window_is_popped_up(ETK_POPUP_WINDOW(popup_window));
	OUTPUT:
	RETVAL

void
etk_popup_window_popdown(popup_window)
	Etk_Widget *	popup_window
	CODE:
	etk_popup_window_popdown(ETK_POPUP_WINDOW(popup_window));

void
etk_popup_window_popdown_all()

void
etk_popup_window_popup(popup_window)
	Etk_Widget *	popup_window
	CODE:
	etk_popup_window_popup(ETK_POPUP_WINDOW(popup_window));

void
etk_popup_window_popup_at_xy(popup_window, x, y)
	Etk_Widget *	popup_window
	int	x
	int	y
	CODE:
	etk_popup_window_popup_at_xy(ETK_POPUP_WINDOW(popup_window), x, y);

double
etk_progress_bar_fraction_get(progress_bar)
	Etk_Widget *	progress_bar
	CODE:
	RETVAL = etk_progress_bar_fraction_get(ETK_PROGRESS_BAR(progress_bar));
	OUTPUT:
	RETVAL

void
etk_progress_bar_fraction_set(progress_bar, fraction)
	Etk_Widget *	progress_bar
	double	fraction
	CODE:
	etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(progress_bar), fraction);

Etk_Widget *
etk_progress_bar_new()

Etk_Widget *
etk_progress_bar_new_with_text(label)
	char *	label

void
etk_progress_bar_pulse(progress_bar)
	Etk_Widget *	progress_bar
	CODE:
	etk_progress_bar_pulse(ETK_PROGRESS_BAR(progress_bar));

double
etk_progress_bar_pulse_step_get(progress_bar)
	Etk_Widget *	progress_bar
	CODE:
	RETVAL = etk_progress_bar_pulse_step_get(ETK_PROGRESS_BAR(progress_bar));
	OUTPUT:
	RETVAL

void
etk_progress_bar_pulse_step_set(progress_bar, pulse_step)
	Etk_Widget *	progress_bar
	double	pulse_step
	CODE:
	etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(progress_bar), pulse_step);

const char *
etk_progress_bar_text_get(progress_bar)
	Etk_Widget *	progress_bar
	CODE:
	RETVAL = etk_progress_bar_text_get(ETK_PROGRESS_BAR(progress_bar));
	OUTPUT:
	RETVAL

void
etk_progress_bar_text_set(progress_bar, label)
	Etk_Widget *	progress_bar
	char *	label
	CODE:
	etk_progress_bar_text_set(ETK_PROGRESS_BAR(progress_bar), label);

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

const char *
etk_property_value_string_get(value)
	Etk_Property_Value *	value

void
etk_property_value_string_set(property_value, value)
	Etk_Property_Value *	property_value
	char *	value


Etk_Widget *
etk_radio_button_new(group)
	Evas_List **	group

Etk_Widget *
etk_radio_button_new_from_widget(radio_button)
	Etk_Widget *	radio_button
	CODE:
	RETVAL = etk_radio_button_new_from_widget(ETK_RADIO_BUTTON(radio_button));
	OUTPUT:
	RETVAL

Etk_Widget *
etk_radio_button_new_with_label(label)
	char *	label
	CODE:
	RETVAL = etk_radio_button_new_with_label(label, NULL);
	OUTPUT:
	RETVAL
	
Etk_Widget *
etk_radio_button_new_with_label_from_widget(label, radio_button)
	char *	label
	Etk_Widget *	radio_button
	CODE:
	RETVAL = etk_radio_button_new_with_label_from_widget(label, ETK_RADIO_BUTTON(radio_button));
	OUTPUT:
	RETVAL

void
etk_range_increments_set(range, step, page)
	Etk_Widget *	range
	double	step
	double	page
	CODE:
	etk_range_increments_set(ETK_RANGE(range), step, page);

void
etk_range_increments_get(range)
	Etk_Widget * range
	PPCODE:
	double step, page;
	etk_range_increments_get(ETK_RANGE(range), &step, &page);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(step)));
	PUSHs(sv_2mortal(newSVnv(page)));

double
etk_range_page_size_get(range)
	Etk_Widget *	range
	CODE:
	RETVAL = etk_range_page_size_get(ETK_RANGE(range));
	OUTPUT:
	RETVAL

void
etk_range_page_size_set(range, page_size)
	Etk_Widget *	range
	double	page_size
	CODE:
	etk_range_page_size_set(ETK_RANGE(range), page_size);

void
etk_range_range_set(range, lower, upper)
	Etk_Widget *	range
	double	lower
	double	upper
	CODE:
	etk_range_range_set(ETK_RANGE(range), lower, upper);

void
etk_range_range_get(range)
	Etk_Widget * range
	PPCODE:
	double lower, upper;
	etk_range_range_get(ETK_RANGE(range), &lower, &upper);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(lower)));
	PUSHs(sv_2mortal(newSVnv(upper)));

double
etk_range_value_get(range)
	Etk_Widget *	range
	CODE:
	RETVAL = etk_range_value_get(ETK_RANGE(range));
	OUTPUT:
	RETVAL

void
etk_range_value_set(range, value)
	Etk_Widget *	range
	double	value
	CODE:
	etk_range_value_set(ETK_RANGE(range), value);

void
etk_scrolled_view_add_with_viewport(scrolled_view, child)
	Etk_Widget *	scrolled_view
	Etk_Widget *	child
	CODE:
	etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(scrolled_view), child);

double
etk_scrolled_view_hscrollbar_get(scrolled_view)
	Etk_Widget *	scrolled_view
	CODE:
	Etk_Range * var;
	var = etk_scrolled_view_hscrollbar_get(ETK_SCROLLED_VIEW(scrolled_view));
	RETVAL = etk_range_value_get(var);
	OUTPUT:
	RETVAL

Etk_Widget *
etk_scrolled_view_new()

void
etk_scrolled_view_policy_get(scrolled_view)
	Etk_Widget *	scrolled_view
	PPCODE:
	Etk_Scrolled_View_Policy hpolicy;
	Etk_Scrolled_View_Policy vpolicy;

	etk_scrolled_view_policy_get(ETK_SCROLLED_VIEW(scrolled_view), &hpolicy, &vpolicy);
	XPUSHs(sv_2mortal(newSViv(hpolicy)));
	XPUSHs(sv_2mortal(newSViv(vpolicy)));
	

void
etk_scrolled_view_policy_set(scrolled_view, hpolicy, vpolicy)
	Etk_Widget *	scrolled_view
	Etk_Scrolled_View_Policy	hpolicy
	Etk_Scrolled_View_Policy	vpolicy
	CODE:
	etk_scrolled_view_policy_set(ETK_SCROLLED_VIEW(scrolled_view), hpolicy, vpolicy);
	

double
etk_scrolled_view_vscrollbar_get(scrolled_view)
	Etk_Widget *	scrolled_view
	CODE:
	Etk_Range * var;
	var = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(scrolled_view));
	RETVAL = etk_range_value_get(var);
	OUTPUT:
	RETVAL

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
etk_signal_callback_call(callback, object, return_value, ...)
	Etk_Signal_Callback *	callback
	Etk_Object *	object
	void *	return_value

void
etk_signal_callback_del(signal_callback)
	Etk_Signal_Callback *	signal_callback

# 
# Etk_Signal_Callback *
# etk_signal_callback_new(signal, callback, data, swapped)
#	Etk_Signal *	signal
#	Etk_Signal_Callback_Function	callback
#	void *	data
#	Etk_Bool	swapped

void
etk_signal_connect(signal_name, object, callback, data)
	char *	        signal_name
	SV *		object
	SV *	        callback
	SV *            data
	
	CODE:	
	__etk_signal_connect_full(signal_name, object, callback, data, ETK_FALSE, ETK_FALSE);

void
etk_signal_connect_after(signal_name, object, callback, data)
	char *	        signal_name
	SV *		object
	SV *	        callback
	SV *            data
	
	CODE:	
	__etk_signal_connect_full(signal_name, object, callback, data, ETK_FALSE, ETK_TRUE);

void etk_signal_connect_full(signal_name, object, callback, data, swapped, after)
	char *	        signal_name
	SV *		object
	SV *	        callback
	SV *            data
	Etk_Bool	swapped
	Etk_Bool	after
	CODE:
	__etk_signal_connect_full(signal_name, object, callback, data, swapped, after);

	
void
etk_signal_connect_swapped(signal_name, object, callback, data)
	char *	        signal_name
	SV *		object
	SV *	        callback
	SV *            data
	
	CODE:	
	__etk_signal_connect_full(signal_name, object, callback, data, ETK_TRUE, ETK_FALSE);

void
etk_signal_delete(signal)
	Etk_Signal *	signal

void
etk_signal_disconnect(signal_name, object, callback)
	char *	        signal_name
	SV *		object
	SV *	        callback
	
	CODE:	
	Etk_Signal *sig = NULL;
	Etk_Marshaller marsh;
	Etk_Object * obj;
	HV * ref;
	SV ** o;
	
	ref = (HV *)SvRV(object);
	o = hv_fetch( ref, "WIDGET", strlen("WIDGET"), 0);

	obj = ETK_OBJECT( (void *) SvIV(SvRV(*o)) );

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

const char *
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

int
etk_statusbar_context_id_get(statusbar, context)
	Etk_Widget *	statusbar
	char *	context
	CODE:
	RETVAL = etk_statusbar_context_id_get(ETK_STATUSBAR(statusbar), context);
	OUTPUT:
	RETVAL

Etk_Bool
etk_statusbar_has_resize_grip_get(statusbar)
	Etk_Widget *	statusbar
	CODE:
	RETVAL = etk_statusbar_has_resize_grip_get(ETK_STATUSBAR(statusbar));
	OUTPUT:
	RETVAL

void
etk_statusbar_has_resize_grip_set(statusbar, has_resize_grip)
	Etk_Widget *	statusbar
	Etk_Bool	has_resize_grip
	CODE:
	etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(statusbar), has_resize_grip);

Etk_Widget *
etk_statusbar_new()

void
etk_statusbar_pop(statusbar, context_id)
	Etk_Widget *	statusbar
	int	context_id
	CODE:
	etk_statusbar_pop(ETK_STATUSBAR(statusbar), context_id);

int
etk_statusbar_push(statusbar, message, context_id)
	Etk_Widget *	statusbar
	char *	message
	int	context_id
	CODE:
	RETVAL = etk_statusbar_push(ETK_STATUSBAR(statusbar), message, context_id);
	OUTPUT:
	RETVAL

void
etk_statusbar_remove(statusbar, message_id)
	Etk_Widget *	statusbar
	int	message_id
	CODE:
	etk_statusbar_remove(ETK_STATUSBAR(statusbar), message_id);

const char *
etk_stock_key_get(stock_id, size)
	Etk_Stock_Id	stock_id
	Etk_Stock_Size	size

const char *
etk_stock_label_get(stock_id)
	Etk_Stock_Id	stock_id

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
	CODE:
	etk_table_attach(ETK_TABLE(table), child, left_attach, right_attach, top_attach, bottom_attach, x_padding, y_padding, fill_policy);

void
etk_table_attach_defaults(table, child, left_attach, right_attach, top_attach, bottom_attach)
	Etk_Widget *	table
	Etk_Widget *	child
	int	left_attach
	int	right_attach
	int	top_attach
	int	bottom_attach
	CODE:
	etk_table_attach_defaults(ETK_TABLE(table), child, left_attach, right_attach, top_attach, bottom_attach);

void
etk_table_cell_clear(table, col, row)
	Etk_Widget *	table
	int	col
	int	row
	CODE:
	etk_table_cell_clear(ETK_TABLE(table), col, row);

Etk_Bool
etk_table_homogeneous_get(table)
	Etk_Widget *	table
	CODE:
	RETVAL = etk_table_homogeneous_get(ETK_TABLE(table));
	OUTPUT:
	RETVAL

void
etk_table_homogeneous_set(table, homogeneous)
	Etk_Widget *	table
	Etk_Bool	homogeneous
	CODE:
	etk_table_homogeneous_set(ETK_TABLE(table), homogeneous);

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
	CODE:
	etk_table_resize(ETK_TABLE(table), num_cols, num_rows);

Etk_Widget *
etk_text_view_new()

Etk_Textblock *
etk_text_view_textblock_get(text_view)
	Etk_Widget * text_view
	CODE:
	RETVAL = etk_text_view_textblock_get(ETK_TEXT_VIEW(text_view));
	OUTPUT:
	RETVAL

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
etk_textblock_text_set(textblock, text, markup)
	Etk_Textblock *	textblock
	char *	text
        Etk_Bool markup
	CODE:
	etk_textblock_text_set(ETK_TEXTBLOCK(textblock), text, markup);
	
const char *
etk_textblock_text_get(tb, markup)
	Etk_Textblock * tb
	Etk_Bool markup
	CODE:
	RETVAL = etk_string_get(etk_textblock_text_get(tb, markup));
	OUTPUT:
	RETVAL

void
etk_textblock_unrealize(textblock)
	Etk_Textblock *	textblock

const char *
etk_theme_default_icon_theme_get()

const char *
etk_theme_default_widget_theme_get()

const char *
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

const char *
etk_theme_widget_theme_get()

Etk_Bool
etk_theme_widget_theme_set(theme_name)
	char *	theme_name

Etk_Bool
etk_toggle_button_active_get(toggle_button)
	Etk_Widget *	toggle_button
	CODE:
	RETVAL = etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(toggle_button));
	OUTPUT:
	RETVAL

void
etk_toggle_button_active_set(toggle_button, active)
	Etk_Widget *	toggle_button
	Etk_Bool	active
	CODE:
	etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(toggle_button), active);

Etk_Widget *
etk_toggle_button_new()

Etk_Widget *
etk_toggle_button_new_with_label(label)
	char *	label

void
etk_toggle_button_toggle(toggle_button)
	Etk_Widget *	toggle_button
	CODE:
	etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(toggle_button));

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

const char *
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
	Etk_Widget *	toplevel_widget
	CODE:
	RETVAL = etk_toplevel_widget_focused_widget_get(ETK_TOPLEVEL_WIDGET(toplevel_widget));
	OUTPUT:
	RETVAL

Etk_Widget *
etk_toplevel_widget_focused_widget_next_get(toplevel_widget)
	Etk_Widget *	toplevel_widget
	CODE:
	RETVAL = etk_toplevel_widget_focused_widget_next_get(ETK_TOPLEVEL_WIDGET(toplevel_widget));
	OUTPUT:
	RETVAL

Etk_Widget *
etk_toplevel_widget_focused_widget_prev_get(toplevel_widget)
	Etk_Widget *	toplevel_widget
	CODE:
	RETVAL = etk_toplevel_widget_focused_widget_prev_get(ETK_TOPLEVEL_WIDGET(toplevel_widget));
	OUTPUT:
	RETVAL

void
etk_toplevel_widget_focused_widget_set(toplevel_widget, widget)
	Etk_Widget *	toplevel_widget
	Etk_Widget *	widget
	CODE:
	etk_toplevel_widget_focused_widget_set(ETK_TOPLEVEL_WIDGET(toplevel_widget), widget);

void
etk_toplevel_widget_geometry_get(toplevel_widget, x, y, w, h)
	Etk_Widget *	toplevel_widget
	PPCODE:
	int 	x;
	int 	y;
	int 	w;
	int 	h;
	etk_toplevel_widget_geometry_get(ETK_TOPLEVEL_WIDGET(toplevel_widget),
			&x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

void
etk_toplevel_widget_pointer_pop(toplevel_widget, pointer_type)
	Etk_Widget *	toplevel_widget
	Etk_Pointer_Type	pointer_type
	CODE:
	etk_toplevel_widget_pointer_pop(ETK_TOPLEVEL_WIDGET(toplevel_widget), pointer_type);

void
etk_toplevel_widget_pointer_push(toplevel_widget, pointer_type)
	Etk_Widget *	toplevel_widget
	Etk_Pointer_Type	pointer_type
	CODE:
	etk_toplevel_widget_pointer_push(ETK_TOPLEVEL_WIDGET(toplevel_widget), pointer_type);

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
	CODE:
	etk_tree_build(ETK_TREE(tree));

void
etk_tree_clear(tree)
	Etk_Widget *	tree
	CODE:
	etk_tree_clear(ETK_TREE(tree));

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
	CODE:
	RETVAL = etk_tree_col_new(ETK_TREE(tree), title, model, width);
	OUTPUT:
	RETVAL

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
	cbd->object = ETK_OBJECT(col);
        cbd->perl_data = newSVsv(data);
        cbd->perl_callback = newSVsv(compare_cb);	
        etk_tree_col_sort_func_set(col, tree_compare_cb, cbd);

void
etk_tree_col_sort_func_set2(col, compare_cb, data)
	Etk_Tree_Col *	col
	int ( * ) ( Etk_Tree * tree, Etk_Tree_Row * row1, Etk_Tree_Row *row2, Etk_Tree_Col * col, void * data ) compare_cb
	void *	data

const char *
etk_tree_col_title_get(col)
	Etk_Tree_Col *	col

void
etk_tree_col_title_set(col, title)
	Etk_Tree_Col *	col
	char *	title

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
	CODE:
	RETVAL = etk_tree_first_row_get(ETK_TREE(tree));
	OUTPUT:
	RETVAL

void
etk_tree_freeze(tree)
	Etk_Widget *	tree
	CODE:
	etk_tree_freeze(ETK_TREE(tree));

Etk_Bool
etk_tree_headers_visible_get(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_headers_visible_get(ETK_TREE(tree));
	OUTPUT:
	RETVAL

void
etk_tree_headers_visible_set(tree, headers_visible)
	Etk_Widget *	tree
	Etk_Bool	headers_visible
	CODE:
	etk_tree_headers_visible_set(ETK_TREE(tree), headers_visible);

Etk_Tree_Row *
etk_tree_last_row_get(tree, walking_through_hierarchy, include_collapsed_children)
	Etk_Widget *	tree
	Etk_Bool	walking_through_hierarchy
	Etk_Bool	include_collapsed_children
	CODE:
	RETVAL = etk_tree_last_row_get(ETK_TREE(tree), walking_through_hierarchy, include_collapsed_children);
	OUTPUT:
	RETVAL

Etk_Tree_Mode
etk_tree_mode_get(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_mode_get(ETK_TREE(tree));
	OUTPUT:
	RETVAL

void
etk_tree_mode_set(tree, mode)
	Etk_Widget *	tree
	Etk_Tree_Mode	mode
	CODE:
	etk_tree_mode_set(ETK_TREE(tree), mode);

void
etk_tree_model_alignment_get(model)
	Etk_Tree_Model *	model
	PPCODE:
	
	float xalign;
	float yalign;
	etk_tree_model_alignment_get(model, &xalign, &yalign);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(xalign)));
	PUSHs(sv_2mortal(newSVnv(yalign)));

void
etk_tree_model_alignment_set(model, xalign, yalign)
	Etk_Tree_Model *	model
	float	xalign
	float	yalign

Etk_Tree_Model *
etk_tree_model_checkbox_new(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_model_checkbox_new(ETK_TREE(tree));
	OUTPUT:
	RETVAL

Etk_Tree_Model *
etk_tree_model_double_new(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_model_double_new(ETK_TREE(tree));
	OUTPUT:
	RETVAL

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
	CODE:
	RETVAL = etk_tree_model_icon_text_new(ETK_TREE(tree), icon_type);
	OUTPUT:
	RETVAL

Etk_Tree_Model *
etk_tree_model_image_new(tree, image_type)
	Etk_Widget *	tree
	Etk_Tree_Model_Image_Type	image_type
	CODE:
	RETVAL = etk_tree_model_image_new(ETK_TREE(tree), image_type);
	OUTPUT:
	RETVAL

Etk_Tree_Model *
etk_tree_model_int_new(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_model_int_new(ETK_TREE(tree));
	OUTPUT:
	RETVAL

Etk_Tree_Model *
etk_tree_model_progress_bar_new(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_model_progress_bar_new(ETK_TREE(tree));
	OUTPUT:
	RETVAL

Etk_Tree_Model *
etk_tree_model_text_new(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_model_text_new(ETK_TREE(tree));
	OUTPUT:
	RETVAL

Etk_Bool
etk_tree_multiple_select_get(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_multiple_select_get(ETK_TREE(tree));
	OUTPUT:
	RETVAL

void
etk_tree_multiple_select_set(tree, multiple_select)
	Etk_Widget *	tree
	Etk_Bool	multiple_select
	CODE:
	etk_tree_multiple_select_set(ETK_TREE(tree), multiple_select);

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
	CODE:
	RETVAL = etk_tree_nth_col_get(ETK_TREE(tree), nth);
	OUTPUT:
	RETVAL

int
etk_tree_num_cols_get(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_num_cols_get(ETK_TREE(tree));
	OUTPUT:
	RETVAL

Etk_Tree_Row *
etk_tree_prev_row_get(row, walking_through_hierarchy, include_collapsed_children)
	Etk_Tree_Row *	row
	Etk_Bool	walking_through_hierarchy
	Etk_Bool	include_collapsed_children

void
etk_tree_row_collapse(row)
	Etk_Tree_Row *	row

SV *
etk_tree_row_data_get(row)
	Etk_Tree_Row *	row

void
etk_tree_row_data_set(row, data)
	Etk_Tree_Row *	row
	SV *	data
        CODE:
        etk_tree_row_data_set(row, newSVsv(data));

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
	RETVAL = image;
      OUTPUT:
	RETVAL

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
	CODE:
	RETVAL = etk_tree_row_height_get(ETK_TREE(tree));
	OUTPUT:
	RETVAL

void
etk_tree_row_height_set(tree, row_height)
	Etk_Widget *	tree
	int	row_height
	CODE:
	etk_tree_row_height_set(ETK_TREE(tree), row_height);

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
	CODE:
	etk_tree_select_all(ETK_TREE(tree));

Etk_Tree_Row *
etk_tree_selected_row_get(tree)
	Etk_Widget *	tree
	CODE:
	RETVAL = etk_tree_selected_row_get(ETK_TREE(tree));
	OUTPUT:
	RETVAL

void 
etk_tree_selected_rows_get(tree)
	Etk_Widget *	tree
	PPCODE:
	Evas_List * list;
	AV * av;
	int i;

	list = etk_tree_selected_rows_get(ETK_TREE(tree));
	av = evas_list_to_perl(list);
	for (i=0; i<=av_len(av); i++) 
	{
		SV * sv;
		sv = newRV(newSViv(0));
		sv_setref_iv(sv, "Etk_WidgetPtr", SvIV(av_shift(av)));

		XPUSHs(sv_2mortal(sv));
	}

void
etk_tree_sort(tree, compare_cb, ascendant, col, data)
	Etk_Widget *	tree
        SV *compare_cb
	Etk_Bool	ascendant
	Etk_Tree_Col *	col
	SV *	data
      CODE:
        Callback_Tree_Compare_Data *cbd;
        
        cbd = calloc(1, sizeof(Callback_Tree_Compare_Data));
        cbd->object = ETK_OBJECT(col);
        cbd->perl_data = newSVsv(data);
        cbd->perl_callback = newSVsv(compare_cb);
        etk_tree_sort(ETK_TREE(tree), tree_compare_cb, ascendant, col, cbd);

void
etk_tree_thaw(tree)
	Etk_Widget *	tree
	CODE:
	etk_tree_thaw(ETK_TREE(tree));

void
etk_tree_unselect_all(tree)
	Etk_Widget *	tree
	CODE:
	etk_tree_unselect_all(ETK_TREE(tree));

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

const char *
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

Etk_Widget *
etk_viewport_new()

Etk_Widget *
etk_vpaned_new()

Etk_Widget *
etk_vscrollbar_new(lower, upper, value, step_increment, page_increment, page_size)
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	double	page_size

Etk_Widget *
etk_vseparator_new()

Etk_Widget *
etk_vslider_new(lower, upper, value, step_increment, page_increment)
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment

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

void
etk_widget_dnd_dest_widgets_get()
	PPCODE:
	AV * av;
	int i;
	
	av = evas_list_to_perl(etk_widget_dnd_dest_widgets_get());
	for (i = av_len(av) ; i >=0 ; i--)
	{
		SV * sv;
		sv = newRV(newSViv(0));
		sv_setref_iv(sv, "Etk_WidgetPtr", SvIV(av_shift(av)));
		
		XPUSHs(sv_2mortal(sv));
	}
	av_undef(av);


void
etk_widget_dnd_drag_data_set(widget, types, num_types, data, data_size)
	Etk_Widget *	widget
	const char **	types
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

void
etk_widget_dnd_files_get(e)
	Etk_Widget *	e
	PPCODE:
	const char ** files;
	int 	* num_files;
	int 	i;

	files = etk_widget_dnd_files_get(e, num_files);
	for (i=0; i<*num_files; i++)
		XPUSHs(sv_2mortal(newSVpv(files[i], strlen(files[i]))));
	

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

void
etk_widget_dnd_types_get(widget)
	Etk_Widget *	widget
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
etk_widget_geometry_get(widget)
	Etk_Widget *	widget
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

const char *
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
	SV *	geometry
	CODE:
	Etk_Geometry	g;
	g = perl_hash_to_geometry(geometry);
	etk_widget_size_allocate(widget, g);

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

const char *
etk_widget_theme_file_get(widget)
	Etk_Widget *	widget

void
etk_widget_theme_file_set(widget, theme_file)
	Etk_Widget *	widget
	char *	theme_file

const char *
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
etk_widget_theme_object_min_size_calc(widget)
	Etk_Widget *	widget
	PPCODE:
	int 	w;
	int 	h;

	etk_widget_theme_object_min_size_calc(widget, &w, &h);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

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

Etk_Widget *
etk_widget_toplevel_parent_get(widget)
	Etk_Widget *	widget
	CODE:
	RETVAL = ETK_WIDGET(etk_widget_toplevel_parent_get(widget));
	OUTPUT:
	RETVAL

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
	CODE:
	etk_window_center_on_window(ETK_WINDOW(window_to_center), ETK_WINDOW(window));

Etk_Bool
etk_window_decorated_get(window)
	Etk_Widget *	window
	CODE:
	Etk_Bool var;
	var = etk_window_decorated_get(ETK_WINDOW(window));
	RETVAL = var;
	OUTPUT:
	RETVAL

void
etk_window_decorated_set(window, decorated)
	Etk_Widget *	window
	Etk_Bool	decorated
	CODE:
	etk_window_decorated_set(ETK_WINDOW(window), decorated);

void
etk_window_deiconify(window)
	Etk_Widget *	window
	CODE:
	etk_window_deiconify(ETK_WINDOW(window));

void
etk_window_dnd_aware_set(window, on)
	Etk_Widget *	window
	Etk_Bool	on
	CODE:
	etk_window_dnd_aware_set(ETK_WINDOW(window), on);

void
etk_window_focus(window)
	Etk_Widget *	window
	CODE:
	etk_window_focus(ETK_WINDOW(window));

void
etk_window_fullscreen(window)
	Etk_Widget *	window
	CODE:
	etk_window_fullscreen(ETK_WINDOW(window));

void
etk_window_geometry_get(window)
	Etk_Widget *	window
	PPCODE:
	int 	x;
	int 	y;
	int 	w;
	int 	h;
	etk_window_geometry_get(ETK_WINDOW(window), &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

Etk_Bool
etk_window_hide_on_delete(window, data)
	Etk_Object *	window
	void *	data

void
etk_window_iconify(window)
	Etk_Widget *	window
	CODE:
	etk_window_iconify(ETK_WINDOW(window));

Etk_Bool
etk_window_is_focused(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_is_focused(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

Etk_Bool
etk_window_is_fullscreen(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_is_fullscreen(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

Etk_Bool
etk_window_is_iconified(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_is_iconified(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

Etk_Bool
etk_window_is_maximized(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_is_maximized(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

Etk_Bool
etk_window_is_sticky(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_is_sticky(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

void
etk_window_maximize(window)
	Etk_Widget *	window
	CODE:
	etk_window_maximize(ETK_WINDOW(window));

void
etk_window_move(window, x, y)
	Etk_Widget *	window
	int	x
	int	y
	CODE:
	etk_window_move(ETK_WINDOW(window), x, y);

void
etk_window_move_to_mouse(window)
	Etk_Widget *	window
	CODE:
	etk_window_move_to_mouse(ETK_WINDOW(window));

Etk_Widget *
etk_window_new()

void
etk_window_resize(window, w, h)
	Etk_Widget *	window
	int	w
	int	h
	CODE:
	etk_window_resize(ETK_WINDOW(window), w, h);

Etk_Bool
etk_window_shaped_get(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_shaped_get(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

void
etk_window_shaped_set(window, shaped)
	Etk_Widget *	window
	Etk_Bool	shaped
	CODE:
	etk_window_shaped_set(ETK_WINDOW(window), shaped);

Etk_Bool
etk_window_skip_pager_hint_get(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_skip_pager_hint_get(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

void
etk_window_skip_pager_hint_set(window, skip_pager_hint)
	Etk_Widget *	window
	Etk_Bool	skip_pager_hint
	CODE:
	etk_window_skip_pager_hint_set(ETK_WINDOW(window), skip_pager_hint);

Etk_Bool
etk_window_skip_taskbar_hint_get(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_skip_taskbar_hint_get(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

void
etk_window_skip_taskbar_hint_set(window, skip_taskbar_hint)
	Etk_Widget *	window
	Etk_Bool	skip_taskbar_hint
	CODE:
	etk_window_skip_taskbar_hint_set(ETK_WINDOW(window), skip_taskbar_hint);

void
etk_window_stick(window)
	Etk_Widget *	window
	CODE:
	etk_window_stick(ETK_WINDOW(window));

const char *
etk_window_title_get(window)
	Etk_Widget *	window
	CODE:
	RETVAL = etk_window_title_get(ETK_WINDOW(window));
	OUTPUT:
	RETVAL

void
etk_window_title_set(window, title)
	Etk_Widget *	window
	char *	title
	CODE:
	etk_window_title_set(ETK_WINDOW(window), title);

void
etk_window_unfocus(window)
	Etk_Widget *	window
	CODE:
	etk_window_unfocus(ETK_WINDOW(window));

void
etk_window_unfullscreen(window)
	Etk_Widget *	window
	CODE:
	etk_window_unfullscreen(ETK_WINDOW(window));

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
etkpl_timer_add(interval, callback, data)
        double interval
	SV *    callback
        SV *    data
      CODE:        
        Callback_Timer_Data *cbd;
        
        cbd = calloc(1, sizeof(Callback_Timer_Data));
        if(SvOK(data))
           cbd->perl_data = newSVsv(data);
        else
           cbd->perl_data = NULL;
        cbd->perl_callback = newSVsv(callback);
        RETVAL = ecore_timer_add(interval, callback_timer, cbd);
      OUTPUT:
        RETVAL
	
void
etkpl_timer_del(timer)
      Ecore_Timer * timer
    CODE:
      ecore_timer_del(timer);


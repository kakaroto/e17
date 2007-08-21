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

#define mINT 1 
#define mDOUBLE 2
#define mIMAGE 3
#define mCHECKBOX 4
#define mPROGRESSBAR 5
#define mTEXT 6

int
tree_compare_alpha_cb(Etk_Tree_Col * col, Etk_Tree_Row * row1, Etk_Tree_Row *row2, void * data )
{
   dSP;
   int ret, cmp;
   char * r1, * r2;

   ENTER;
   SAVETMPS;
   
   if (! (row1 && row2 && col) ) {
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
tree_compare_numeric_cb(Etk_Tree_Col * col, Etk_Tree_Row * row1, Etk_Tree_Row *row2, void * data )
{
   dSP;
   int r1, r2, ret;

   ENTER;
   SAVETMPS;
   
   if (! (row1 && row2 && col) ) {
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
tree_compare_cb( Etk_Tree_Col * col, Etk_Tree_Row * row1, Etk_Tree_Row *row2, void * data )
{
   dSP;
   Callback_Tree_Compare_Data *cbd;
   int count;
   int ret;   

   ENTER ;
   SAVETMPS;   
   
   cbd = data;
   
   PUSHMARK(SP);  	  
   XPUSHs(sv_2mortal(newSVObj(col, getClass("Etk_Tree_Col"))));
   XPUSHs(sv_2mortal(newSVObj(row1, getClass("Etk_Tree_Row"))));
   XPUSHs(sv_2mortal(newSVObj(row2, getClass("Etk_Tree_Row"))));
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

MODULE = Etk::Tree::Model	PACKAGE = Etk::Tree::Model	PREFIX = etk_tree_model_

void
etk_tree_model_free(model)
	Etk_Tree_Model *	model
      ALIAS:
	Free=1




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


MODULE = Etk::Tree	PACKAGE = Etk::Tree	PREFIX = etk_tree_

void
etk_tree_alternating_row_colors_set(tree, alternating_row_colors)
	Etk_Tree *      tree
	Etk_Bool alternating_row_colors
	ALIAS:
	AlternatingRowColorsSet=1

Etk_Bool
etk_tree_alternating_row_colors_get(tree)
	Etk_Tree *      tree
	ALIAS:
	AlternatingRowColorsGet=1

void
etk_tree_column_separators_visible_set(tree, col_separators_visible)
	Etk_Tree *      tree
	Etk_Bool col_separators_visible
	ALIAS:
	ColumnSeperatorsVisibleSet=1

Etk_Bool
etk_tree_column_separators_visible_get(tree)
	Etk_Tree *      tree
	ALIAS:
	ColumnSeperatorsVisibleGet=1

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



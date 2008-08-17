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




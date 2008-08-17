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



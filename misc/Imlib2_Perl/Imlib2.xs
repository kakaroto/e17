#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <X11/Xlib.h>
#include <Imlib2.h>
#include "Imlib2Types.h"

MODULE = Imlib2		PACKAGE = Imlib2 	PREFIX = imlib_

Imlib2::Imlib_Image 
load_image(file)
	char *file
	CODE:
	RETVAL = imlib_load_image(file);
	OUTPUT:
	RETVAL

void
save_image(file)	
	char *file
	CODE:
       	imlib_save_image(file);


void
set_context(image)
	Imlib2::Imlib_Image image
	CODE:
	imlib_context_set_image(image);

void 
free_image()
	CODE:
	imlib_free_image();
	
void 
free_image_and_decache()
	CODE:
	imlib_free_image_and_decache();
	

void
blend_image_onto_image(source_image, merge_alpha, source_x, source_y, source_width,source_height, destination_x, destination_y, destination_width,destination_height)
	Imlib2::Imlib_Image source_image
	char merge_alpha
	int source_x
	int source_y
	int source_height
	int source_width
	int destination_x
	int destination_y
	int destination_width
	int destination_height
	CODE:
	imlib_blend_image_onto_image(source_image, merge_alpha, source_x, source_y, source_width,source_height, destination_x, destination_y, destination_width,destination_height);
	
int
get_width()
	CODE:
	RETVAL = imlib_image_get_width();
	OUTPUT:
	RETVAL
	
int
get_height()
	CODE:
	RETVAL = imlib_image_get_height();
	OUTPUT:
	RETVAL
	
char
get_file_name()
	CODE:
	RETVAL = imlib_image_get_file_name();
	OUTPUT:
	RETVAL

Imlib2::Imlib_Image
create_image(w, h)
	int w
	int h
	CODE:
	RETVAL = imlib_create_image(w, h);
	OUTPUT:
	RETVAL

void
render_pixmaps_for_whole_image(pixmap,mask)
    	Imlib2::Pixmap pixmap
  	Imlib2::Pixmap mask
  	CODE:
	imlib_render_pixmaps_for_whole_image(pixmap,mask);

void
free_pixmap_and_mask(pixmap)
    	Imlib2::Pixmap pixmap
  	CODE:
	imlib_free_pixmap_and_mask(pixmap);

Imlib2::Pixmap
context_get_mask()
    	CODE:
	RETVAL = imlib_context_get_mask();
	OUTPUT:
	RETVAL

void
context_set_drawable(drawable)
	Imlib2::Drawable drawable
	CODE:
	imlib_context_set_drawable(drawable);

void
render_image_on_drawable(x,y)
	int x
	int y
	CODE:
	imlib_render_image_on_drawable(x,y);

void
context_set_colormap(colormap)
	Imlib2::Colormap colormap
	CODE:
	imlib_context_set_colormap(colormap);

void
context_set_display(display)
	Imlib2::Display display
	CODE:
	imlib_context_set_display(display);

void
context_set_visual(visual)
	Imlib2::Visual visual
	CODE:
	imlib_context_set_visual(visual);
	

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

Imlib2::Imlib_Image
create_image_using_data(w,h,data)
    	int w
  	int h
  	DATA32 *data
  	CODE:
	{
/*		if (!data) croak("Data must be a reference!\n");*/
		RETVAL = imlib_create_image_using_data(w,h,data);
	}	
	OUTPUT:
	RETVAL
	
DATA32 *  
image_get_data()
    	CODE:
	RETVAL = imlib_image_get_data();
	OUTPUT:
	RETVAL

DATA32 *
image_put_back_data(data)
	DATA32 *data
	CODE:
	imlib_image_put_back_data(data);
	
DATA32 *
image_get_data_for_reading_only()
	CODE:
	RETVAL = imlib_image_get_data_for_reading_only();
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
	
const char *
get_filename()
	CODE:
	RETVAL = imlib_image_get_filename();
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

Imlib2::Imlib_Image
create_image_from_drawable(mask,x,y,w,h,grab_x)
	Imlib2::Pixmap mask
	int x
	int y
	int w
	int h
	char grab_x
	CODE:
	RETVAL = imlib_create_image_from_drawable(mask,x,y,w,h,grab_x);
	OUTPUT:
	RETVAL
	
void
render_pixmaps_for_whole_image()
	PPCODE:		
	{
		Pixmap pixmap,mask;
		imlib_render_pixmaps_for_whole_image(&pixmap,&mask);
		EXTEND(sp,2);
		PUSHs(sv_2mortal(newSVuv(pixmap)));
		PUSHs(sv_2mortal(newSVuv(mask)));
	}

void
render_pixmaps_for_whole_image_at_size(x,y)
	int x
	int y
	PPCODE:
	{
		Pixmap pixmap,mask;
		imlib_render_pixmaps_for_whole_image_at_size(&pixmap,&mask,x,y);
		EXTEND(sp,2);
		PUSHs(sv_2mortal(newSVuv(pixmap)));
		PUSHs(sv_2mortal(newSVuv(mask)));
	}	
		

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
render_image_on_drawable_at_size(x,y,width,height)
	int x
	int y
	int width
	int height
	CODE:
	imlib_render_image_on_drawable_at_size(x,y,width,height);
	

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
	
void
context_set_dither_mask(dither_mask)
    	char dither_mask
 	CODE:
	imlib_context_set_dither_mask(dither_mask);

void
context_set_anti_alias(anti_alias)
    	char anti_alias
  	CODE:
	imlib_context_set_anti_alias(anti_alias);

void
context_set_blend(blend)
    	char blend
  	CODE:
	imlib_context_set_blend(blend);

void
context_set_angle(angle)
    	double angle
  	CODE:
	imlib_context_set_angle(angle);

void
context_set_color(red,green,blue,alpha)
    	int red
  	int green
  	int blue
  	int alpha
        CODE:
	imlib_context_set_color(red,green,blue,alpha);

void
context_set_dither(dither)
    	char dither
 	CODE:
	imlib_context_set_dither(dither);

int 
get_cache_size()
	CODE:
	RETVAL = imlib_get_cache_size();
	OUTPUT:
	RETVAL

void
set_cache_size(bytes)
    	int bytes
  	CODE:
	imlib_set_cache_size(bytes);

int
image_has_alpha()
    	CODE:
	RETVAL = imlib_image_has_alpha();
	OUTPUT:
	RETVAL

Imlib2::Imlib_Image
clone_image()
    	CODE:
	RETVAL = imlib_clone_image();
  	OUTPUT:
	RETVAL

Imlib2::Imlib_Image
create_cropped_image(x,y,width,height)
    	int x
  	int y
  	int width
  	int height
  	CODE:
	RETVAL = imlib_create_cropped_image(x,y,width,height);
	OUTPUT:
	RETVAL
  
Imlib2::Imlib_Image
create_cropped_scaled_image(source_x,source_y,source_width,source_height,destination_width,destination_height)
    	int source_x
  	int source_y
  	int source_width
  	int source_height
  	int destination_width
  	int destination_height
  	CODE:
	RETVAL = imlib_create_cropped_scaled_image(source_x,source_y,source_width,source_height,destination_width,destination_height);
	OUTPUT:
	RETVAL

void
flip_horizontal()
    	CODE:
	imlib_image_flip_horizontal();
    	
void
flip_vertical()
    	CODE:
	imlib_image_flip_vertical();

void
flip_diagonal()
    	CODE:
	imlib_image_flip_diagonal();

void
orientate(orientation)
    	int orientation
        CODE:
	imlib_image_orientate(orientation);

void
blur(radius)
    	int radius
  	CODE:
	imlib_image_blur(radius);

void
sharpen(radius)
    	int radius
  	CODE:
	imlib_image_sharpen(radius);

void
tile()
    	CODE:
	imlib_image_tile();

void
tile_vertical()
    	CODE:
	imlib_image_tile_vertical();

void
tile_horizontal()	
	CODE:
	imlib_image_tile_horizontal();

void
context_set_font(font)
	Imlib2::Imlib_Font font
	CODE:
	imlib_context_set_font(font);

Imlib2::Imlib_Font
context_get_font()
	CODE:
	RETVAL = imlib_context_get_font();
	OUTPUT:
	RETVAL
	
Imlib2::Imlib_Font
load_font(fontname)
	char *fontname
	CODE:
	RETVAL = imlib_load_font(fontname);
	OUTPUT:
	RETVAL
	
void
free_font()
	CODE:
	imlib_free_font();
	
void
text_draw(x,y,text)
	int x
	int y
	char *text
	CODE:
	imlib_text_draw(x,y,text);


void
get_text_size(text)
	const char *text
	PPCODE:
	{
		int text_w;
		int text_h;
		imlib_get_text_size(text,&text_w,&text_h);
		EXTEND(sp,2);
		PUSHs(sv_2mortal(newSViv(text_w)));
		PUSHs(sv_2mortal(newSViv(text_h)));
	}
	

void
add_path_to_font_path(path)
	char *path
	CODE:
	imlib_add_path_to_font_path(path);

void 
context_set_direction(direction)
	int direction;
	CODE:
	imlib_context_set_direction(direction);
	
void
context_set_color_modifier(modifier)
	Imlib2::Imlib_Color_Modifier modifier
	CODE:
	imlib_context_set_color_modifier(modifier);
	
Imlib2::Imlib_Color_Modifier
context_get_color_modifier()
	CODE:
	RETVAL = imlib_context_get_color_modifier();
	OUTPUT:
	RETVAL

Imlib2::Imlib_Color_Modifier
create_color_modifier()
	CODE:
	RETVAL = imlib_create_color_modifier();
	OUTPUT:
	RETVAL

void
free_color_modifier()
	CODE:
	imlib_free_color_modifier();
	
void
modify_color_modifier_gamma(gamma_value)
	double gamma_value
	CODE:
	imlib_modify_color_modifier_gamma(gamma_value);

void
modify_color_modifier_brightness(brightness_value)
	double brightness_value
	CODE:
	imlib_modify_color_modifier_brightness(brightness_value);

void
modify_color_modifier_contrast(contrast_value)
	double contrast_value
	CODE:
	imlib_modify_color_modifier_contrast(contrast_value);

void
reset_color_modifier()
	CODE:
	imlib_reset_color_modifier();
	
void
apply_color_modifier()
	CODE:
	imlib_apply_color_modifier();
	
void
apply_color_modifier_to_rectangle(x,y,width,height)
	int x
	int y
	int width
	int height
	CODE:
	imlib_apply_color_modifier_to_rectangle(x,y,width,height);
	
void
draw_line(x1,y1,x2,y2)
	int x1
	int y1
	int x2
	int y2
	CODE:
	imlib_image_draw_line(x1,y1,x2,y2,0);

void
draw_rectangle(x,y,width,height)
	int x
	int y
	int width
	int height
	CODE:
	imlib_image_draw_rectangle(x,y,width,height);
	
void
fill_rectangle(x,y,width,height)
	int x
	int y
	int width
	int height
	CODE:
	imlib_image_fill_rectangle(x,y,width,height);

void
copy_alpha_to_image(image_source, x, y)
	Imlib2::Imlib_Image image_source
	int x
	int y
	CODE:
	imlib_image_copy_alpha_to_image(image_source,x,y);

void
copy_alpha_rectangle_to_image(image_source,x,y,width,height,destination_x,destination_y)
	Imlib2::Imlib_Image image_source
	int x
	int y
	int width 
	int height
	int destination_x
	int destination_y
	CODE:
	imlib_image_copy_alpha_rectangle_to_image(image_source,x,y,width,height,destination_x,destination_y);
	
void 
scroll_rect(x,y,width,height,delta_x,delta_y)
	int x
	int y
	int width 
	int height
	int delta_x
	int delta_y
	CODE:
	imlib_image_scroll_rect(x,y,width,height,delta_x,delta_y);
	
void
copy_rect(x,y,width,height,new_x,new_y)
	int x
	int y
	int width
	int height
	int new_x
	int new_y
	CODE:
	imlib_image_copy_rect(x,y,width,height,new_x,new_y);
	
void
draw_ellipse(xc,yc,a,b)
	int xc
	int yc
	int a
	int b
	CODE:
	imlib_image_draw_ellipse(xc,yc,a,b);
	
void
fill_ellipse(xc,yc,a,b)
	int xc
	int yc
	int a
	int b
	CODE:
	imlib_image_fill_ellipse(xc,yc,a,b);
	
void
context_set_color_range(color_range)
	Imlib2::Imlib_Color_Range	color_range
	CODE:
	imlib_context_set_color_range(color_range);
	
Imlib2::Imlib_Color_Range
get_color_range()
	CODE:
	imlib_context_get_color_range();
	
Imlib2::Imlib_Color_Range
create_color_range()
	CODE:
	RETVAL = imlib_create_color_range();
	OUTPUT:
	RETVAL
	
void
free_color_range()
	CODE:
	imlib_free_color_range();
	
void
add_color_to_color_range(distance_away)
	int distance_away
	CODE:
	imlib_add_color_to_color_range(distance_away);
	
void
fill_color_range_rectangle(x,y,width,height,angle)
	int x
	int y
	int width
	int height
	double angle
	CODE:
	imlib_image_fill_color_range_rectangle(x,y,width,height,angle);

void 
set_format(format)
	char *format
	CODE:
	imlib_image_set_format(format);


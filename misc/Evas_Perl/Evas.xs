#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <Evas.h>
#include "EvasTypes.h"

MODULE = Evas		PACKAGE = Evas

Evas::Evas
new_all(display, parent_window, x, y, w, h, render_method, colors, font_cache, image_cache, font_dir)
	     Evas::Display display
	     Evas::Window parent_window
	     int x
	     int y
   	     int w
	     int h
	     int render_method
	     int colors
	     int font_cache
	     int image_cache
	     char *font_dir
	     CODE:
	     RETVAL = evas_new_all(display,parent_window,x,y,w,h,render_method,colors,font_cache,image_cache,font_dir);
	     OUTPUT:
	     RETVAL
	     
Evas::Evas
new()
	CODE:
	RETVAL = evas_new();
	OUTPUT:
	RETVAL
	

Evas::Window
get_window(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_get_widnow(e);
	OUTPUT:
	RETVAL


Evas::Display
get_display(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_get_display(e);
	OUTPUT:
	RETVAL
	

Evas::Colormap
get_colormap(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_get_colormap(e);
	OUTPUT:
	RETVAL
	

Evas::Visual
get_visual(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_get_visual(e);
	OUTPUT:
	RETVAL
	

Imlib2::Imlib_Image
get_image(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_get_image(e);
	OUTPUT:
	RETVAL
	

int 
get_render_method(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_get_render_method(e);
	OUTPUT:
	RETVAL
	
void
free(e)
	Evas::Evas e
	CODE:
        evas_free(e);
	
void
render(e)
	Evas::Evas e
	CODE:
	evas_render(e);
	
Evas::Visual
get_optimal_visual(e, disp)
	Evas::Evas e
	Evas::Display disp
	CODE:
	RETVAL = evas_get_optimal_visual(e,disp);
	OUTPUT:
	RETVAL


Evas::Colormap
get_optimal_colormap(e, disp)
	Evas::Evas e
	Evas::Display disp
	CODE:
	RETVAL = evas_get_optimal_colormap(e,disp);
	OUTPUT:
	RETVAL

void
add_obscured_rect(e,x,y,w,h)
	Evas::Evas e
	int x
	int y
	int w
	int h
	CODE:
	evas_add_obscured_rect(e,x,y,w,h);

void
clear_obscured_rect(e)
	Evas::Evas e
	CODE:
	evas_clear_obscured_rects(e);
	
void 
get_drawable_size(e)
	Evas::Evas e
	PPCODE:
	{
		int w;
		int h;
		evas_get_drawable_size(e,&w,&h);
		EXTEND(sp,2);
		PUSHs(sv_2mortal(newSViv(w)));
		PUSHs(sv_2mortal(newSViv(h)));
	}

void
set_output(e,disp,d,v,c)
	Evas::Evas e
	Evas::Display disp
	Evas::Window d
	Evas::Visual v
	Evas::Colormap c
	CODE:
	evas_set_output(e,disp,d,v,c);
	
void
set_output_image(e,image)
	Evas::Evas e
	Imlib2::Imlib_Image image
	CODE:
	evas_set_output_image(e,image);

void
set_output_colors(e,colors)
	Evas::Evas e
	int colors
	CODE:
	evas_set_output_colors(e,colors);
	
void
set_output_size(e,w,h)
	Evas::Evas e
	int w
	int h
	CODE:
	evas_set_output_size(e,w,h);
	

Evas::Object
add_image_from_file(e, file)
	Evas::Evas e
	char *file
	CODE:
	RETVAL = evas_add_image_from_file(e,file);
	OUTPUT:
	RETVAL
	
Evas::Object
add_text(e, font, size, text)
	Evas::Evas e
	char *font
	int size
	char *text
	CODE:
	RETVAL = evas_add_text(e,font,size,text);
	OUTPUT:
	RETVAL
	
Evas::Object
add_rectangle(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_add_rectangle(e);
	OUTPUT:
	RETVAL
	
Evas::Object 
add_line(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_add_line(e);
	OUTPUT:
	RETVAL
	
Evas::Object
add_gradient_box(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_add_gradient_box(e);
	OUTPUT:
	RETVAL

void
set_gradient(e,o,g)
	Evas::Evas e
	Evas::Object o
	Evas::Gradient g
	CODE:
	evas_set_gradient(e,o,g);

Evas::Gradient
gradient_new()
	CODE:
	RETVAL = evas_gradient_new();
	OUTPUT:
	RETVAL
	
void
gradient_free(g)
	Evas::Gradient g
	CODE:
	evas_gradient_free(g);

void
gradient_add_color(grad,r,g,b,a,dist)
	Evas::Gradient grad
	int r
	int g
	int b
	int a
	int dist
	CODE:
	evas_gradient_add_color(grad,r,g,b,a,dist);
	
Evas::Object
add_poly(e)
	Evas::Evas e
	CODE:
	RETVAL = evas_add_poly(e);
	OUTPUT:
	RETVAL
	
void
show(e,o)
	Evas::Evas e
	Evas::Object o
	CODE:
	evas_show(e,o);

void
hide(e,o)
	Evas::Evas e
	Evas::Object o
	CODE:
	evas_hide(e,o);

void
set_layer(e,o,l)
	Evas::Evas e
	Evas::Object o
	int l
	CODE:
	evas_set_layer(e,o,l);

void
set_output_method(e,method)
	Evas::Evas e
	int method
	CODE:
	evas_set_output_method(e,method);

void
update_rect(e,x,y,w,h)
	Evas::Evas e
	int x
	int y
	int w
	int h
	CODE:
	evas_update_rect(e,x,y,w,h);
	
void
move(e,o,x,y)
	Evas::Evas e
	Evas::Object o
	double x
	double y
	CODE:
	evas_move(e,o,x,y);
	
void
resize(e,o,w,h)
	Evas::Evas e
	Evas::Object o
	double w
	double h
	CODE:
	evas_resize(e,o,w,h);
	
void
set_output_viewport(e,x,y,w,h)
	Evas::Evas e
	double x
	double y
	double w
	double h
	CODE:
	evas_set_output_viewport(e,x,y,w,h);

void
set_color(e,o,r,g,b,a)
	Evas::Evas e
	Evas::Object o
	int r
	int g
	int b
	int a
	CODE:
	evas_set_color(e,o,r,g,b,a);


void 
font_add_path(e,path)
	Evas::Evas e
	char *path
	CODE:
	evas_font_add_path(e,path);
	
void 
font_del_path(e,path)
	Evas::Evas e
	char *path
	CODE:
	evas_font_del_path(e,path);

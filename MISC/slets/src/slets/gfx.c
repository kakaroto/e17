#include "main.h"

Evas e;
Display *d;
Visual *vis;
Colormap cmap;
Window win;
GFX_ATOM a_win_del;

int gfx_init(char *title, int win_w, int win_h, char *font_path)
{
 e = evas_new();
 evas_set_output_method(e, RENDER_METHOD_ALPHA_SOFTWARE);

 if ((d = XOpenDisplay(NULL)) == NULL)
   return 0;

 vis = evas_get_optimal_visual(e, d);
 cmap = evas_get_optimal_colormap(e, d);
 a_win_del = XInternAtom(d, "WM_DELETE_WINDOW", False);
 
 win = XCreateSimpleWindow(d, DefaultRootWindow(d), 0, 0,
                           win_w, win_h, 0, 0, 0);
 
 XSelectInput(d, win, ButtonPressMask | ExposureMask);
 
 XSetWindowColormap(d, win, cmap);
 XSetWindowBorder(d, win, 0);
 
 XStoreName(d, win, title);
 
 XSync(d, False);
 XMapWindow(d, win);
 XSetWMProtocols(d, win, &a_win_del, 1);
 
 evas_set_output(e, d, win, vis, cmap);
 evas_set_output_size(e, win_w, win_h);
 evas_set_output_viewport(e, 0, 0, win_w, win_h);
 evas_font_add_path(e, font_path);

 return 1;
}

GFX_OBJECT gfx_add_image_from_file(char *file)
{
 GFX_OBJECT tmp;

 tmp = evas_add_image_from_file(e, file);

 return tmp;
}

GFX_OBJECT gfx_add_text(char *name, int size, char *text)
{
 GFX_OBJECT tmp;
                           
 tmp = evas_add_text(e, name, size, text);

 return tmp;
}

GFX_OBJECT gfx_add_rectangle(void)
{
 GFX_OBJECT tmp;
 
 tmp = evas_add_rectangle(e);

 return tmp;
}

void gfx_resize(GFX_OBJECT tmp, int w, int h)
{
 evas_resize(e, tmp, w, h);
}

void gfx_move(GFX_OBJECT tmp, int x, int y)
{
 evas_move(e, tmp, x, y);
}

void gfx_set_color(GFX_OBJECT tmp, int r, int g, int b, int a)
{
 evas_set_color(e, tmp, r, g, b, a);
}

void gfx_set_image_file(GFX_OBJECT tmp, char *file)
{
 evas_set_image_file(e, tmp, file);
}

void gfx_set_text(GFX_OBJECT tmp, char *text)
{
 evas_set_text(e, tmp, text);
}

void gfx_show(GFX_OBJECT tmp)
{
 evas_show(e, tmp);
}

void gfx_hide(GFX_OBJECT tmp)
{
 evas_hide(e, tmp);
}

void gfx_render(void)
{
 evas_render(e);
}

void gfx_flush(void)
{
 evas_flush_image_cache(e);
 evas_flush_font_cache(e);
}

void gfx_update_rect(int x, int y, int w, int h)
{
 evas_update_rect(e, x, y, w, h);
}

int gfx_pending(void)
{
 return XPending(d);
}

void gfx_next_event(GFX_EVENT *ev)
{
 XNextEvent(d, ev);
}

void gfx_done(void)
{
 evas_free(e);
}

#ifndef __SLETS_GFX_H__
#define __SLETS_GFX_H__

typedef Evas_Object GFX_OBJECT;
typedef XEvent GFX_EVENT;
typedef Atom GFX_ATOM;

int gfx_init(char *title, int win_w, int win_h, char *font_path);

GFX_OBJECT gfx_add_image_from_file(char *file);
GFX_OBJECT gfx_add_text(char *name, int size, char *text);
GFX_OBJECT gfx_add_rectangle(void);

void gfx_resize(GFX_OBJECT tmp, int w, int h);
void gfx_move(GFX_OBJECT tmp, int x, int y);

void gfx_set_color(GFX_OBJECT tmp, int r, int g, int b, int a);
void gfx_set_image_file(GFX_OBJECT tmp, char *file);
void gfx_set_text(GFX_OBJECT tmp, char *text);

void gfx_show(GFX_OBJECT tmp);
void gfx_hide(GFX_OBJECT tmp);

void gfx_render(void);
void gfx_flush(void);

void gfx_update_rect(int x, int y, int w, int h);
int gfx_pending(void);
void gfx_next_event(GFX_EVENT *ev);

void gfx_done(void);

#endif

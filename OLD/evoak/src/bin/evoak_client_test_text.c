#include <Evoak.h>
#include <Ecore.h>
#include <math.h>

#include "config.h"

Evoak_Object *oo_title, *oo_close, *oo_resize;
Evoak_Coord   oo_x, oo_y, oo_w, oo_h;
char          oo_do_move = 0, oo_do_resize = 0;
static void (*oo_resize_func) (Evoak_Coord x, Evoak_Coord y, Evoak_Coord w, Evoak_Coord h) = NULL;
static void (*oo_raise_func) (void);


void oo_configure(void);
void oo_raise(void);

static void
oo_title_down(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Down *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_title, PACKAGE_DATA_DIR"/data/bar2.png", NULL);
   if (ev->button == 1) oo_do_move = 1;
   oo_raise();
   evoak_thaw(e);
}

static void
oo_title_up(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Up *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_title, PACKAGE_DATA_DIR"/data/bar1.png", NULL);
   if (ev->button == 1) oo_do_move = 0;
   evoak_thaw(e);
}

static void
oo_title_move(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Move *ev;
   
   ev = event_info;
   evoak_freeze(e);
   if (oo_do_move)
     {
	oo_x += ev->cur.x - ev->prev.x;
	oo_y += ev->cur.y - ev->prev.y;
	oo_configure();
     }
   evoak_thaw(e);
}

static void
oo_title_in(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_In *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_title, PACKAGE_DATA_DIR"/data/bar1.png", NULL);
   evoak_object_image_file_set(oo_close, PACKAGE_DATA_DIR"/data/close1.png", NULL);
   evoak_thaw(e);
}

static void
oo_title_out(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Out *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_title, PACKAGE_DATA_DIR"/data/bar0.png", NULL);
   evoak_object_image_file_set(oo_close, PACKAGE_DATA_DIR"/data/close0.png", NULL);
   evoak_thaw(e);
}

static void
oo_resize_down(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Down *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_resize, PACKAGE_DATA_DIR"/data/res2.png", NULL);
   if (ev->button == 1) oo_do_resize = 1;
   evoak_thaw(e);
}

static void
oo_resize_up(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Up *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_resize, PACKAGE_DATA_DIR"/data/res1.png", NULL);
   if (ev->button == 1) oo_do_resize = 0;
   evoak_thaw(e);
}

static void
oo_resize_move(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Move *ev;
   
   ev = event_info;
   evoak_freeze(e);
   if (oo_do_resize)
     {
	oo_w += ev->cur.x - ev->prev.x;
	oo_h += ev->cur.y - ev->prev.y;
	if (oo_w < 0) oo_w = 0;
	if (oo_h < 0) oo_h = 0;
	oo_configure();
     }
   evoak_thaw(e);
}

static void
oo_close_down(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Down *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_close, PACKAGE_DATA_DIR"/data/close2.png", NULL);
   evoak_thaw(e);
}

static void
oo_close_up(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Up *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_close, PACKAGE_DATA_DIR"/data/close1.png", NULL);
   evoak_thaw(e);
   ecore_main_loop_quit();
}

static void
oo_close_in(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_In *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_close, PACKAGE_DATA_DIR"/data/close1.png", NULL);
   evoak_object_image_file_set(oo_title, PACKAGE_DATA_DIR"/data/bar1.png", NULL);
   evoak_thaw(e);
}

static void
oo_close_out(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Out *ev;
   
   ev = event_info;
   evoak_freeze(e);
   evoak_object_image_file_set(oo_close, PACKAGE_DATA_DIR"/data/close0.png", NULL);
   evoak_object_image_file_set(oo_title, PACKAGE_DATA_DIR"/data/bar0.png", NULL);
   evoak_thaw(e);
}

void
oo_configure(void)
{
   Evoak_Object *o;
   
   evoak_freeze(evoak_object_evoak_get(oo_title));
   o = oo_title;
   evoak_object_move(o, oo_x - 8, oo_y - 16);
   evoak_object_resize(o, oo_w + 16, 20);
   evoak_object_image_fill_set(o, 0, 0, oo_w + 16, 20);
   o = oo_close;
   evoak_object_move(o, oo_x + 8 + oo_w - 18, oo_y - 8 - 16);
   evoak_object_resize(o, 28, 26);
   evoak_object_image_fill_set(o, 0, 0, 28, 26);
   o = oo_resize;
   evoak_object_move(o, oo_x + oo_w - 6, oo_y + oo_h - 6);
   evoak_object_resize(o, 24, 24);
   evoak_object_image_fill_set(o, 0, 0, 24, 24);
   
   if (oo_resize_func) oo_resize_func(oo_x, oo_y, oo_w, oo_h);

   evoak_thaw(evoak_object_evoak_get(oo_title));   
}

void
oo_raise(void)
{
   evoak_freeze(evoak_object_evoak_get(oo_title));
   
   if (oo_raise_func) oo_raise_func();
   
   evoak_object_raise(oo_title);
   evoak_object_raise(oo_resize);
   evoak_object_raise(oo_close);
   evoak_thaw(evoak_object_evoak_get(oo_title));
}

void
oo_setup(Evoak *e)
{
   Evoak_Object *o;
   
   oo_x = rand() % 50;
   oo_y = rand() % 50;
   oo_w = 80;
   oo_h = 60;
   
   evoak_freeze(e);
   o = evoak_object_image_add(e);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_DOWN, oo_title_down, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_UP,   oo_title_up,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_MOVE, oo_title_move, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_IN,   oo_title_in,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_OUT,  oo_title_out,  NULL);
   evoak_object_image_border_set(o, 10, 10, 0, 0);
   evoak_object_image_file_set(o, PACKAGE_DATA_DIR"/data/bar0.png", NULL);
   evoak_object_show(o);
   oo_title = o;

   o = evoak_object_image_add(e);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_DOWN, oo_resize_down, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_UP,   oo_resize_up,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_MOVE, oo_resize_move, NULL);
   evoak_object_image_file_set(o, PACKAGE_DATA_DIR"/data/res1.png", NULL);
   evoak_object_show(o);
   oo_resize = o;
   
   o = evoak_object_image_add(e);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_DOWN, oo_close_down, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_UP,   oo_close_up,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_IN,   oo_close_in,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_OUT,  oo_close_out,  NULL);
   evoak_object_image_file_set(o, PACKAGE_DATA_DIR"/data/close0.png", NULL);
   evoak_object_show(o);
   oo_close = o;
   
   oo_configure();
   
   evoak_thaw(e);
}




Evoak_Object *o_text, *o_text_brk, *o_rect, *o_rect_chr, *o_base, *o_bottom;
Evoak_Coord cpx = 0, cpy = 0;

int set = 0;

static void
resize(Evoak_Coord x, Evoak_Coord y, Evoak_Coord w, Evoak_Coord h)
{
   Evoak_Coord tx, ty, tw, th, cx, cy, cw, ch, ascent, descent;
   int pos;
   
   ascent = evoak_object_text_max_ascent_get(o_text);
   descent = evoak_object_text_max_descent_get(o_text);
   evoak_object_geometry_get(o_text, &tx, &ty, &tw, &th);
   pos = evoak_object_text_char_coords_get(o_text, cpx - x, cpy - y,
					   &cx, &cy, &cw, &ch);
   if (pos >= 0)
     {
	char buf[16];
	const char *txt;
	
	evoak_object_move(o_rect_chr, x + cx, y + cy);
	evoak_object_resize(o_rect_chr, cw, ch);
	txt = evoak_object_text_text_get(o_text);
	buf[0] = txt[pos];
	buf[1] = 0;
	evoak_object_text_text_set(o_text_brk, buf);
	evoak_object_show(o_rect_chr);
     }
   else
     {
	evoak_object_text_text_set(o_text_brk, "");
	evoak_object_hide(o_rect_chr);
     }
   evoak_object_move(o_text_brk, x, y + th);
   
   evoak_object_move(o_rect, x, y);
   evoak_object_resize(o_rect, w, h);

   evoak_object_move(o_text, x, y);

   evoak_object_move(o_base, x, y + ascent);
   evoak_object_resize(o_base, tw, 1);
   
   evoak_object_move(o_bottom, x, y + ascent + descent);
   evoak_object_resize(o_bottom, tw, 1);

   evoak_object_move(o_text_brk, x, y + th);
}

static void
rais(void)
{
   evoak_object_raise(o_rect);
   evoak_object_raise(o_text_brk);
   evoak_object_raise(o_rect_chr);
   evoak_object_raise(o_base);
   evoak_object_raise(o_bottom);
   evoak_object_raise(o_text);
}

static void
mouse_move(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Move *ev;
   Evoak_Coord x, y, cx, cy, cw, ch;
   int pos;
   
   ev = event_info;
   cpx = ev->cur.x;
   cpy = ev->cur.y;
   evoak_freeze(e);
   resize(oo_x, oo_y, oo_w, oo_h);
   evoak_thaw(e);
}

static void
key_down(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Key_Down *ev;
   
   ev = event_info;
   printf("KEY DOWN %p, key \"%s\", string \"%s\" mod 0x%x lock 0x%x\n",
	  obj, ev->keyname, ev->string, (int)ev->modmask, (int)ev->lockmask);
   if ((!strcmp(ev->keyname, "Escape")) ||
       (!strcmp(ev->keyname, "Return")))
     {
	evoak_object_text_text_set(o_text, "");
     }
   else if (!strcmp(ev->keyname, "BackSpace"))
     {
	const char *txt;
	char buf[4096];
	int p, dec;
	
	txt = evoak_object_text_text_get(o_text);
	strcpy(buf, txt);
	p = strlen(buf);
	p = evoak_string_char_prev_get(buf, p, &dec);
	if (p >= 0)
	  {
	     buf[p] = 0;
	     evoak_object_text_text_set(o_text, buf);
	  }
     }
   else if (!strcmp(ev->keyname, "space"))
     {
	const char *txt;
	char buf[4096];
	int p, dec;
	
	txt = evoak_object_text_text_get(o_text);
	strcpy(buf, txt);
	strcat(buf, " ");
	evoak_object_text_text_set(o_text, buf);
     }
   else if (!strcmp(ev->keyname, "Up"))
     {
	Evoak_Font_Size s;
	const char *fn;
	
	evoak_object_text_font_get(o_text, &fn, &s);
	evoak_object_text_font_set(o_text, fn, s + 1);
     }
   else if (!strcmp(ev->keyname, "Down"))
     {
	Evoak_Font_Size s;
	const char *fn;
	
	evoak_object_text_font_get(o_text, &fn, &s);
	evoak_object_text_font_set(o_text, fn, s - 1);
     }
   else if ((!strcmp(ev->keyname, "0")) ||
	    (!strcmp(ev->keyname, "1")) ||
	    (!strcmp(ev->keyname, "2")) ||
	    (!strcmp(ev->keyname, "3")) ||
	    (!strcmp(ev->keyname, "4")) ||
	    (!strcmp(ev->keyname, "5")) ||
	    (!strcmp(ev->keyname, "6")) ||
	    (!strcmp(ev->keyname, "7")) ||
	    (!strcmp(ev->keyname, "8")) ||
	    (!strcmp(ev->keyname, "9")))
     {
	Evoak_Coord tx, ty, tw, th, cx, cy, cw, ch;
	int n;
	
	n = atoi(ev->keyname);
	if (evoak_object_text_char_pos_get(o_text, n, &cx, &cy, &cw, &ch))
	  {
	     evoak_object_geometry_get(o_text, &tx, &ty, &tw, &th);
	     cpx = cx + tx;
	     cpy = cy + ty;
	  }
     }
   else
     {
	const char *txt;
	char buf[4096];
	int p, dec;
	
	txt = evoak_object_text_text_get(o_text);
	strcpy(buf, txt);
	strcat(buf, ev->keyname);
	evoak_object_text_text_set(o_text, buf);
     }
   resize(oo_x, oo_y, oo_w, oo_h);   
}

static void
key_up(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Key_Up *ev;
   
   ev = event_info;
   printf("KEY UP %p, key \"%s\", string \"%s\" mod 0x%x lock 0x%x\n",
	  obj, ev->keyname, ev->string, (int)ev->modmask, (int)ev->lockmask);
}

void
setup(Evoak *e)
{
   Evoak_Object *o;
   Evoak_Coord w, h;

   evoak_freeze(e);
      
   evoak_font_path_append(e, PACKAGE_DATA_DIR"/data/fonts");

   o = evoak_object_text_add(e);
   evoak_object_text_font_set(o, "Vera", 24);
   evoak_object_color_set(o, 0, 0, 0, 255);
   evoak_object_text_text_set(o, "A test string!");
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_MOVE, mouse_move, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_KEY_DOWN, key_down, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_KEY_UP, key_up, NULL);
   evoak_object_show(o);
   evoak_object_focus_set(o, 1);
   o_text = o;
   
   o = evoak_object_rectangle_add(e);
   evoak_object_color_set(o, 200, 80, 160, 60);
   evoak_object_show(o);
   o_rect = o;

   o = evoak_object_rectangle_add(e);
   evoak_object_color_set(o, 255, 180, 20, 200);
   evoak_object_show(o);
   o_base = o;

   o = evoak_object_rectangle_add(e);
   evoak_object_color_set(o, 40, 80, 200, 200);
   evoak_object_show(o);
   o_bottom = o;
   
   o = evoak_object_text_add(e);
   evoak_object_text_font_set(o, "Vera", 24);
   evoak_object_color_set(o, 10, 10, 80, 255);
   evoak_object_text_text_set(o, "");
   evoak_object_show(o);
   evoak_object_focus_set(o, 1);
   o_text_brk = o;

   o = evoak_object_rectangle_add(e);
   evoak_object_color_set(o, 80, 20, 10, 160);
   evoak_object_show(o);
   o_rect_chr = o;

   evoak_object_raise(o_text);
   evoak_object_raise(o_text_brk);
   
   oo_resize_func = resize;
   oo_raise_func = rais;
   oo_setup(e);
   
   evoak_thaw(e);
}

int
cb_canvas_info(void *data, int type, void *event)
{
   Evoak_Event_Canvas_Info *ev;
   
   ev = event;
   if (!set)
     {
	setup(ev->evoak);
	set = 1;
     }
   return 1;
}

int
cb_disconnect(void *data, int type, void *event)
{
   printf("disconnected!\n");
   ecore_main_loop_quit();
   return 1;
}

int
main(int argc, char **argv)
{
   if (evoak_init())
     {
	Evoak *ev;
	
	ecore_event_handler_add(EVOAK_EVENT_CANVAS_INFO, cb_canvas_info, NULL);
	ecore_event_handler_add(EVOAK_EVENT_DISCONNECT, cb_disconnect, NULL);
	
	ev = evoak_connect(NULL, "evoak_test_text", "custom");
	if (ev)
	  {
	     ecore_main_loop_begin();
	     evoak_disconnect(ev);
	  }
	evoak_shutdown();
     }
}

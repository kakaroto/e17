#include <Evoak.h>
#include <Ecore.h>
#include <math.h>

#include "config.h"

Evoak_Object *o_rect, *o_title, *o_resize;
Evoak_Object *o_bd[5];
int d_title = 0, d_resize = 0;
int set = 0;

static void
mouse_in(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_In *ev;
   
   ev = event_info;
   if (obj == o_title)
     {
	evoak_object_color_set(o_title, 255, 255, 255, 180);
     }
   if (obj == o_rect)
     {
	evoak_freeze(e);
	evoak_object_color_set(o_title, 255, 255, 255, 180);
	evoak_object_color_set(o_rect, 200, 80, 100, 200);
	evoak_thaw(e);
     }
}

static void
mouse_out(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Out *ev;
     
   ev = event_info;
   if (obj == o_title)
     {
	evoak_object_color_set(obj, 255, 255, 255, 120);
     }
   if (obj == o_rect)
     {
	evoak_freeze(e);
	evoak_object_color_set(obj, 255, 255, 255, 120);
	evoak_object_color_set(o_rect, 40, 50, 60, 180);
	evoak_thaw(e);
     }
}

static void
mouse_down(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Down *ev;
   
   ev = event_info;
   if (ev->button == 1)
     {
	if (obj == o_title)
	  {
	     evoak_freeze(e);
	     evoak_object_color_set(o_title, 255, 220, 80, 220);
	     evoak_object_raise(o_rect);
	     evoak_object_raise(o_title);
	     evoak_object_raise(o_resize);
	     evoak_object_raise(o_bd[0]);
	     evoak_object_raise(o_bd[1]);
	     evoak_object_raise(o_bd[2]);
	     evoak_object_raise(o_bd[3]);
	     evoak_object_raise(o_bd[4]);
	     evoak_thaw(e);
	     d_title = 1;
	  }
	else if (obj == o_resize)
	  {
	     evoak_object_color_set(o_resize, 255, 50, 20, 200);
	     d_resize = 1;
	  }
     }
}

static void
mouse_up(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Up *ev;
   
   ev = event_info;
   if (ev->button == 1)
     {
	if (obj == o_title)
	  {
	     evoak_object_color_set(obj, 255, 255, 255, 180);
	     d_title = 0;
	  }
	else if (obj == o_resize)
	  {
	     evoak_object_color_set(o_resize, 0, 0, 0, 220);
	     d_resize = 0;
	  }
     }
}

static void
mouse_move(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Move *ev;
   
   ev = event_info;
   if (d_title)
     {
	Evoak_Coord x, y, w, h;
	
	evoak_freeze(e);
	evoak_object_geometry_get(o_rect, &x, &y, &w, &h);
	evoak_object_move(o_title, 
			  x + (ev->cur.x - ev->prev.x),
			  y + (ev->cur.y - ev->prev.y));
	evoak_object_move(o_resize, 
			  x + w - 8 + (ev->cur.x - ev->prev.x),
			  y + h - 8 + (ev->cur.y - ev->prev.y));
	evoak_object_move(o_rect, 
			  x + (ev->cur.x - ev->prev.x),
			  y + (ev->cur.y - ev->prev.y));
	evoak_object_move(o_bd[0], 
			  x + (ev->cur.x - ev->prev.x),
			  y + (ev->cur.y - ev->prev.y));
	evoak_object_move(o_bd[1], 
			  x + (ev->cur.x - ev->prev.x),
			  y + 7 + (ev->cur.y - ev->prev.y));
	evoak_object_move(o_bd[2], 
			  x + (ev->cur.x - ev->prev.x),
			  y + h - 1  + (ev->cur.y - ev->prev.y));
	evoak_object_move(o_bd[3], 
			  x + (ev->cur.x - ev->prev.x),
			  y + (ev->cur.y - ev->prev.y));
	evoak_object_move(o_bd[4], 
			  x + w - 1 + (ev->cur.x - ev->prev.x),
			  y + (ev->cur.y - ev->prev.y));
	evoak_thaw(e);
     }
   if (d_resize)
     {
	Evoak_Coord x, y, w, h;
	
	evoak_freeze(e);
	evoak_object_geometry_get(o_rect, &x, &y, &w, &h);
	evoak_object_resize(o_title, 
			    w + (ev->cur.x - ev->prev.x),
			    8);
	evoak_object_move(o_resize, 
			  x + w - 8 + (ev->cur.x - ev->prev.x),
			  y + h - 8 + (ev->cur.y - ev->prev.y));
	evoak_object_resize(o_rect, 
			    w + (ev->cur.x - ev->prev.x),
			    h + (ev->cur.y - ev->prev.y));
	evoak_object_resize(o_bd[0], 
			    w + (ev->cur.x - ev->prev.x),
			    1);
	evoak_object_resize(o_bd[1], 
			    w + (ev->cur.x - ev->prev.x),
			    1);
	evoak_object_move(o_bd[2], 
			  x,
			  y + h - 1 + (ev->cur.y - ev->prev.y));
	evoak_object_resize(o_bd[2], 
			    w + (ev->cur.x - ev->prev.x),
			    1);
	evoak_object_move(o_bd[3], 
			  x,
			  y);
	evoak_object_resize(o_bd[3], 
			    1,
			    h + (ev->cur.y - ev->prev.y));
	evoak_object_move(o_bd[4], 
			  x + w - 1 + (ev->cur.x - ev->prev.x),
			  y);
	evoak_object_resize(o_bd[4], 
			    1,
			    h + (ev->cur.y - ev->prev.y));
	evoak_thaw(e);
     }
}

static void
key_down(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Key_Down *ev;
   
   ev = event_info;
   printf("KEY DOWN %p, key \"%s\", string \"%s\" mod 0x%x lock 0x%x\n",
	  obj, ev->keyname, ev->string, (int)ev->modmask, (int)ev->lockmask);
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
setup(Evoak *ev)
{
   Evoak_Object *o;
   int i;

   evoak_freeze(ev);
   
   evoak_font_path_append(ev, PACKAGE_DATA_DIR"/data/fonts");

   o = evoak_object_rectangle_add(ev);
   evoak_object_move(o, 20, 20);
   evoak_object_resize(o, 64, 64);
   evoak_object_color_set(o, 40, 50, 60, 180);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_IN,   mouse_in,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_OUT,  mouse_out,  NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_DOWN, mouse_down, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_UP,   mouse_up,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_KEY_DOWN,   key_down,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_KEY_UP,     key_up,     NULL);
   evoak_object_show(o);
   evoak_object_focus_set(o, 1);
   o_rect = o;
   
   o = evoak_object_rectangle_add(ev);
   evoak_object_move(o, 20, 20);
   evoak_object_resize(o, 64, 8);
   evoak_object_color_set(o, 255, 255, 255, 120);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_IN,   mouse_in,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_OUT,  mouse_out,  NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_DOWN, mouse_down, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_UP,   mouse_up,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_MOVE, mouse_move, NULL);
   evoak_object_show(o);
   o_title = o;
   
   o = evoak_object_rectangle_add(ev);
   evoak_object_move(o, 20 + 64 - 8, 20 + 64 - 8);
   evoak_object_resize(o, 8, 8);
   evoak_object_color_set(o, 0, 0, 0, 220);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_IN,   mouse_in,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_OUT,  mouse_out,  NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_DOWN, mouse_down, NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_UP,   mouse_up,   NULL);
   evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_MOVE, mouse_move, NULL);
   evoak_object_show(o);
   o_resize = o;
   
   o = evoak_object_rectangle_add(ev);
   evoak_object_move(o, 20, 20);
   evoak_object_resize(o, 64, 1);
   evoak_object_color_set(o, 0, 0, 0, 255);
   evoak_object_pass_events_set(o, 1);
   evoak_object_show(o);
   o_bd[0] = o;
   
   o = evoak_object_rectangle_add(ev);
   evoak_object_move(o, 20, 20 + 8 - 1);
   evoak_object_resize(o, 64, 1);
   evoak_object_color_set(o, 0, 0, 0, 255);
   evoak_object_pass_events_set(o, 1);
   evoak_object_show(o);
   o_bd[1] = o;
   
   o = evoak_object_rectangle_add(ev);
   evoak_object_move(o, 20, 20 + 64 - 1);
   evoak_object_resize(o, 64, 1);
   evoak_object_color_set(o, 0, 0, 0, 255);
   evoak_object_pass_events_set(o, 1);
   evoak_object_show(o);
   o_bd[2] = o;
   
   o = evoak_object_rectangle_add(ev);
   evoak_object_move(o, 20, 20);
   evoak_object_resize(o, 1, 64);
   evoak_object_color_set(o, 0, 0, 0, 255);
   evoak_object_pass_events_set(o, 1);
   evoak_object_show(o);
   o_bd[3] = o;

   o = evoak_object_rectangle_add(ev);
   evoak_object_move(o, 20 + 64 - 1, 20);
   evoak_object_resize(o, 1, 64);
   evoak_object_color_set(o, 0, 0, 0, 255);
   evoak_object_pass_events_set(o, 1);
   evoak_object_show(o);
   o_bd[4] = o;
   
   evoak_thaw(ev);
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
	
	ev = evoak_connect(NULL, "evoak_test_rect", "custom");
	if (ev)
	  {
	     ecore_main_loop_begin();
	     evoak_disconnect(ev);
	  }
	evoak_shutdown();
     }
}

#include <Evoak.h>
#include <Ecore.h>
#include <math.h>

#include "config.h"

static int set = 0;
static int canvas_w, canvas_h;
Evoak_Object *o_rect[20];
double start_t = 0.0;

static int sync_wait = 0;

static void
mouse_in(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_In *ev;
   
   ev = event_info;
   printf("mouse IN %p @ %i, %i - bt %x mod %x lock %x\n",
	  obj, ev->x, ev->y, (int)ev->buttons, (int)ev->modmask, (int)ev->lockmask);
}

static void
mouse_out(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Out *ev;
     
   ev = event_info;
   printf("mouse OUT %p @ %i, %i - bt %x mod %x lock %x\n",
	  obj, ev->x, ev->y, (int)ev->buttons, (int)ev->modmask, (int)ev->lockmask);
}

static void
mouse_down(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Down *ev;
   
   ev = event_info;
   printf("mouse DOWN %p, b %i @ %i, %i - mod %x lock %x\n",
	  obj, ev->button, ev->x, ev->y, (int)ev->modmask, (int)ev->lockmask);
}

static void
mouse_up(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Up *ev;
   
   ev = event_info;
   printf("mouse UP %p, b %i @ %i, %i - mod %x lock %x\n",
	  obj, ev->button, ev->x, ev->y, (int)ev->modmask, (int)ev->lockmask);
   evoak_object_del(obj);
}

static void
mouse_move(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Move *ev;
   
   ev = event_info;
   printf("mouse MOVE %p, @ %i, %i -> %i %i - bt %x mod %x lock %x\n",
	  obj, ev->prev.x, ev->prev.y, ev->cur.x, ev->cur.y, (int)ev->buttons, (int)ev->modmask, (int)ev->lockmask);
}

static void
mouse_wheel(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Mouse_Wheel *ev;
   
   ev = event_info;
   printf("mouse WHEEL %p, dir %i z %i @ %i, %i - mod %x lock %x\n",
	  obj, ev->dir, ev->z, ev->x, ev->y, (int)ev->modmask, (int)ev->lockmask);
}

static void
key_down(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Key_Down *ev;
   
   ev = event_info;
   printf("key DOWN %p, key \"%s\", string \"%s\" mod %x lock %x\n",
	  obj, ev->keyname, ev->string, (int)ev->modmask, (int)ev->lockmask);
}

static void
key_up(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   Evoak_Event_Key_Up *ev;
   
   ev = event_info;
   printf("key UP %p, key \"%s\", string \"%s\" mod %x lock %x\n",
	  obj, ev->keyname, ev->string, (int)ev->modmask, (int)ev->lockmask);
}

static void
obj_del(void *data, Evoak *e, Evoak_Object *obj, void *event_info)
{
   printf("obj %p del\n", obj);
}


static int
timer(void *data)
{
   int i;
   double t;
   Evoak *ev;
   
   ev = data;
   if (sync_wait)
     {
	printf("server too slow!\n");
	return 1;
     }
   t = ecore_time_get() - start_t;
   evoak_freeze(ev);
   for (i = 0; i < 20; i++)
     {
	Evoak_Object *o;
	int x, y, w, h, r, g, b, a;
	
	o = o_rect[i];
	x = cos((double)i + (t * 1.5)) * (canvas_w / 3);
	y = sin((double)i + (t * 2.5)) * (canvas_h / 3);
	w = (double)i * (3.0 + sin((t * 5.0)));
	h = (double)i * (3.0 + sin((t * 5.0)));
//	evoak_object_image_size_get(o, &w, &h);
	r = sin((double)i + (t * 1.5)) * 255;
	g = sin((double)i + (t * 0.5)) * 255;
	b = sin((double)i + (t * 1.0)) * 255;
	a = sin((double)i + (t * 0.25)) * 255;
	evoak_object_move(o, (canvas_w / 2) + x - (w / 2), (canvas_h / 2) + y - (h / 2));
	evoak_object_resize(o, w, h);
	evoak_object_image_fill_set(o, 0, 0, w, h);
//	evoak_object_color_set(o, r, g, b, a);
     }
   evoak_thaw(ev);
   evoak_sync(ev, 1);
   sync_wait++;
   return 1;
}

void
setup(Evoak *ev)
{
   Evoak_Object *o;
   int i;

   evoak_font_path_append(ev, PACKAGE_DATA_DIR"/data/fonts");
   o = evoak_object_text_add(ev);
   evoak_object_move(o, 0, 0);
   evoak_object_text_font_set(o, "Vera", 10);
   evoak_object_color_set(o, 0, 0, 0, 255);
   evoak_object_text_text_set(o, "A Test String");
   evoak_object_show(o);
   
   for (i = 0; i < 20; i++)
     {
	char buf[4096];
	
	o = evoak_object_image_add(ev);
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/data/heart%i.png", (i % 3) + 1);
	evoak_object_image_file_set(o, buf, NULL);
	evoak_object_move(o, 20, 30);
	evoak_object_resize(o, 60, 80);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_IN, mouse_in, NULL);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_OUT, mouse_out, NULL);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_DOWN, mouse_down, NULL);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_UP, mouse_up, NULL);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_MOVE, mouse_move, NULL);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_MOUSE_WHEEL, mouse_wheel, NULL);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_KEY_DOWN, key_down, NULL);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_KEY_UP, key_up, NULL);
	evoak_object_event_callback_add(o, EVOAK_CALLBACK_FREE, obj_del, NULL);
	evoak_object_focus_set(o, 1);
//	evoak_object_color_set(o, 200, 140, 50, 200);
	evoak_object_show(o);
	o_rect[i] = o;
     }
   ecore_timer_add(1.0 / 20.0, timer, ev);
}

int
cb_canvas_info(void *data, int type, void *event)
{
   Evoak_Event_Canvas_Info *ev;
   
   ev = event;
   canvas_w = ev->w;
   canvas_h = ev->h;
   if (!set)
     {
	setup(ev->evoak);
	set = 1;
     }
   return 1;
}

int
cb_sync_reply(void *data, int type, void *event)
{
   Evoak_Event_Sync_Reply *ev;
   
   ev = event;
   printf("GOT sync\n");
   sync_wait--;
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
	ecore_event_handler_add(EVOAK_EVENT_SYNC_REPLY, cb_sync_reply, NULL);
	ecore_event_handler_add(EVOAK_EVENT_DISCONNECT, cb_disconnect, NULL);
	
	ev = evoak_connect(NULL, "evoak_test_client", "custom");
	if (ev)
	  {
	     start_t = ecore_time_get();
	     
	     ecore_main_loop_begin();
	     evoak_disconnect(ev);
	  }
     }
   evoak_shutdown();
}

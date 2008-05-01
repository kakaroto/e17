#include "evoak_main.h"

double       start_time = 0.0;
Ecore_Evas  *ecore_evas = NULL;
Evas        *evas       = NULL;

static int  main_start(int argc, char **argv);
static void main_stop(void);
static void main_resize(Ecore_Evas *ee);
static int  main_signal_exit(void *data, int ev_type, void *ev);
static void main_delete_request(Ecore_Evas *ee);
static void main_pre_rend(Ecore_Evas *ee);
static void main_post_rend(Ecore_Evas *ee);

static void key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void key_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void edje_signal_cb(void *data, Evas_Object *o, const char *emission, const char *source);
static void edje_text_change_cb(void *data, Evas_Object *o, const char *part);

static int ipc_start(int argc, char **argv);
static int ipc_cb_client_add(void *data, int type, void *event);
static int ipc_cb_client_del(void *data, int type, void *event);
static int ipc_cb_client_data(void *data, int type, void *event);

int   hash_store  (Evoak_Hash *hash, int id, void *data);
void *hash_unstore(Evoak_Hash *hash, int id);
void *hash_find   (Evoak_Hash *hash, int id);

#define SOFT_X 0
#define FB     1
#define GL_X   2

static int
main_start(int argc, char **argv)
{
   int w = 240;
   int h = 320;
   int rot = 270;
   int im_cache = 4096 * 1024;
   int fn_cache = 512 * 1024;
   int engine = SOFT_X;
   int resizable = 1;
   int borderless = 0;
   int sticky = 0;
   int avoid_damage = 0;
   int shaped = 0;
   int fullscreen = 0;
   int lay = -9999;
   int ssize = 0;
   double fps = 30.0;
   int i;
   
   start_time = ecore_time_get();
   if (!ecore_init()) return -1;
   ecore_app_args_set(argc, (const char **)argv);
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, main_signal_exit, NULL);
   
   for (i = 1; i < argc; i++)
     {
	if      ((!strcmp(argv[i], "-x11"))) engine = SOFT_X;
	else if ((!strcmp(argv[i], "-fb")))  engine = FB;
	else if ((!strcmp(argv[i], "-gl")))  engine = GL_X;
	else if ((!strcmp(argv[i], "-g")) && (i < (argc - 1)))
	  {
	     char b1[32], b2[32];
	     
	     i++;
	     b1[0] = 0;
	     b2[0] = 0;
	     sscanf(argv[i], "%30[^x]x%30s", b1, b2);
	     w = atoi(b1);
	     h = atoi(b2);
	     if (((w < 1) || (w > 8000)) ||
		 ((h < 1) || (h > 8000)))
	       {
		  printf("Error parsing options to -g option.\n"
			 "Please see -h help output for syntax.\n");
		  ecore_evas_shutdown();
		  ecore_shutdown();
		  return 0;
	       }
	  }
	else if ((!strcmp(argv[i], "-r")) && (i < (argc - 1)))
	  {
	     i++;
	     rot = atoi(argv[i]);
	     if ((rot != 0) && (rot != 90) && (rot != 270))
	       {
		  printf("Error parsing options to -r option.\n"
			 "Options must be 0, 90 or 270.\n");
		  ecore_evas_shutdown();
		  ecore_shutdown();
		  return 0;
	       }
	  }
	else if ((!strcmp(argv[i], "-ic")) && (i < (argc - 1)))
	  {
	     i++;
	     im_cache = atoi(argv[i]);
	     if ((im_cache < 0) || (im_cache > 65536 * 1024))
	       {
		  printf("Invalid image cache value. use between 0 and 65536\n");
		  ecore_evas_shutdown();
		  ecore_shutdown();
		  return 0;
	       }
	  }
	else if ((!strcmp(argv[i], "-fc")) && (i < (argc - 1)))
	  {
	     i++;
	     fn_cache = atoi(argv[i]);
	     if ((fn_cache < 0) || (fn_cache > 65536 * 1024))
	       {
		  printf("Invalid font cache value. use between 0 and 65536\n");
		  ecore_evas_shutdown();
		  ecore_shutdown();
		  return 0;
	       }
	  }
	else if ((!strcmp(argv[i], "-fr")) && (i < (argc - 1)))
	  {
	     i++;
	     fps = atof(argv[i]);
	     if (fps <= 0.0)
	       {
		  printf("Invalid fps value. must be greater than 0.0\n");
		  ecore_evas_shutdown();
		  ecore_shutdown();
		  return 0;
	       }
	  }
	else if ((!strcmp(argv[i], "-fr")) && (i < (argc - 1)))
	  {
	     i++;
	     fps = atof(argv[i]);
	     if (fps <= 0.0)
	       {
		  printf("Invalid fps value. must be greater than 0.0\n");
		  ecore_evas_shutdown();
		  ecore_shutdown();
		  return 0;
	       }
	  }
	else if ((!strcmp(argv[i], "-fr")) && (i < (argc - 1)))
	  {
	     i++;
	     fps = atof(argv[i]);
	     if (fps <= 0.0)
	       {
		  printf("Invalid fps value. must be greater than 0.0\n");
		  ecore_evas_shutdown();
		  ecore_shutdown();
		  return 0;
	       }
	  }
	else if ((!strcmp(argv[i], "-fr")) && (i < (argc - 1)))
	  {
	     i++;
	     fps = atof(argv[i]);
	     if (fps <= 0.0)
	       {
		  printf("Invalid fps value. must be greater than 0.0\n");
		  ecore_evas_shutdown();
		  ecore_shutdown();
		  return 0;
	       }
	  }
	else if (!strcmp(argv[i], "-fixed"))
	  {
	     resizable = 0;
	  }
	else if (!strcmp(argv[i], "-bd"))
	  {
	     borderless = 1;
	  }
	else if (!strcmp(argv[i], "-stick"))
	  {
	     sticky = 1;
	  }
	else if (!strcmp(argv[i], "-nodamage"))
	  {
	     avoid_damage = 1;
	  }
	else if (!strcmp(argv[i], "-shape"))
	  {
	     shaped = 1;
	  }
	else if (!strcmp(argv[i], "-fs"))
	  {
	     fullscreen = 1;
	  }
	else if ((!strcmp(argv[i], "-l")) && (i < (argc - 1)))
	  {
	     i++;
	     lay = atoi(argv[i]);
	  }
	else if (!strcmp(argv[i], "-fsize"))
	  {
	     ssize = 1;
	  }
	else if ((!strcmp(argv[i], "-h")))
	  {
	     printf("%s [OPTIONS]\n"
		    "\tWhere OPTIONS is 0 or more of the following:\n"
		    "\n"
		    "-x11       Display using software X11\n"
		    "-fb        Display using the software in the framebuffer\n"
		    "-gl        Display using OpenGL in X11\n"
		    "-g WxH     Set display size to W x H pixels\n"
		    "-r rot     Set rotation of the screen (0, 90, 270)\n"
		    "-ic Kb     Set image cache in Kb\n"
		    "-fc Kb     Set font cache in Kb\n"
		    "-fr fps    Set attempted framerate in frames per second\n"
		    "-fixed     Don't allow window to be resized\n"
		    "-bd        Request borderless window\n"
		    "-stick     Request sticky window\n"
		    "-nodamage  Request to use backing store to avoid damage\n"
		    "-shape     Shaped window\n"
		    "-fs        Fullscreen\n"
		    "-l N       Request to be on layer N\n"
		    "-fsize     Make window full-screen sized\n"
		    "-h         Display this help\n",
		    argv[0]);
	     ecore_evas_shutdown();
	     ecore_shutdown();
	     return 0;
	  }
     }
   if (engine == SOFT_X)
     {
	ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, w, h);
	if (!ecore_evas)
	  ecore_evas = ecore_evas_fb_new(NULL, 0, w, h);
     }
   else if (engine == FB)
     {
	ecore_evas = ecore_evas_fb_new(NULL, 270, w, h);
	if (!ecore_evas)
	  ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, w, h);
     }
   else if (engine == GL_X)
     {
	ecore_evas = ecore_evas_gl_x11_new(NULL, 0, 0, 0, w, h);
	if (!ecore_evas)
	  ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, w, h);
	if (!ecore_evas)
	  ecore_evas = ecore_evas_fb_new(NULL, rot,  w, h);
     }
   if (!ecore_evas)
     {
	printf("Error. Cannot create canvas. Abort.\n");
	ecore_evas_shutdown();
	ecore_shutdown();
	return 0;
     }
   
   ecore_evas_callback_delete_request_set(ecore_evas, main_delete_request);
   ecore_evas_callback_resize_set(ecore_evas, main_resize);
   ecore_evas_callback_pre_render_set(ecore_evas, main_pre_rend);
   ecore_evas_callback_post_render_set(ecore_evas, main_post_rend);
   ecore_evas_title_set(ecore_evas, "Evoak");
   ecore_evas_name_class_set(ecore_evas, "evoak", "Evoak");

   if (!resizable)
     {
	ecore_evas_size_min_set(ecore_evas, w, h);
	ecore_evas_size_max_set(ecore_evas, w, h);
     }
   if (borderless)
     {
	ecore_evas_borderless_set(ecore_evas, 1);
     }
   if (sticky)
     {
	ecore_evas_sticky_set(ecore_evas, 1);
     }
   if (avoid_damage)
     {
	ecore_evas_avoid_damage_set(ecore_evas, 1);
     }
   if (shaped)
     {
	ecore_evas_shaped_set(ecore_evas, 1);
     }
   if (fullscreen)
     {
	ecore_evas_fullscreen_set(ecore_evas, 1);
     }
   if (lay > -9999)
     {
	ecore_evas_layer_set(ecore_evas, lay);
     }
   if (ssize)
     {
	if ((engine == SOFT_X) || (engine == GL_X))
	  {
	     int w, h, i, num;
	     Ecore_X_Window win, *wlist;
	     
	     wlist = ecore_x_window_root_list(&num);
	     if (wlist)
	       {
		  win = wlist[0];
		  ecore_x_window_size_get(win, &w, &h);
		  ecore_evas_resize(ecore_evas, w, h);
		  free(wlist);
	       }
	  }
     }
   
   ecore_evas_show(ecore_evas);
   evas = ecore_evas_get(ecore_evas);
   evas_image_cache_set(evas, im_cache);
   evas_font_cache_set(evas, fn_cache);
   evas_font_path_append(evas, PACKAGE_DATA_DIR"/data/fonts");
   if (!ecore_ipc_init())
     {
	ecore_evas_shutdown();
	ecore_shutdown();
	return 0;
     }
   edje_init();
   edje_frametime_set(1.0 / fps);
   
   return 1;
}

static void
main_stop(void)
{
   ecore_ipc_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
}

static void
main_resize(Ecore_Evas *ee)
{
   Evoak_PR_Canvas_Info p;
   void *d;
   int s;
   Evoak_Server *es;
   Evas_Object *o;
   
   o = evas_object_name_find(ecore_evas_get(ee), "base");
   es = evas_object_data_get(o, "srv");
   ecore_evas_geometry_get(ee, NULL, NULL, &(p.w), &(p.h));
   evas_object_resize(o, p.w, p.h);
   p.rot = ecore_evas_rotation_get(ee);
   d = _evoak_proto[EVOAK_PR_CANVAS_INFO].enc(&p, &s);
   if (d)
     {
	Evas_List *l;
	
	for (l = es->clients; l; l = l->next)
	  {
	     Evoak_Client *ec;
	     
	     ec = l->data;
	     ecore_ipc_client_send(ec->client,
				   EVOAK_PR_CANVAS_INFO,
				   EVOAK_PROTOCOL_VERSION,
				   ec->request_id, 0,
				   0,
				   d, s);
	     ec->request_id++;
	  }
	free(d);
     }
}

static int
main_signal_exit(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
   return 1;
}

static void
main_delete_request(Ecore_Evas *ee)
{
   ecore_main_loop_quit();
}

#define SETMODS(m) \
{ \
   m = 0; \
   if (evas_key_modifier_is_set(ev->modifiers, "Shift")) m |= EVOAK_MODIFIER_MASK_SHIFT; \
   if (evas_key_modifier_is_set(ev->modifiers, "Control")) m |= EVOAK_MODIFIER_MASK_CTRL; \
   if (evas_key_modifier_is_set(ev->modifiers, "Alt")) m |= EVOAK_MODIFIER_MASK_ALT; \
   if (evas_key_modifier_is_set(ev->modifiers, "Super")) m |= EVOAK_MODIFIER_MASK_SUPER; \
   if (evas_key_modifier_is_set(ev->modifiers, "Hyper")) m |= EVOAK_MODIFIER_MASK_HYPER; \
}

#define SETLOCKS(m) \
{ \
   m = 0; \
   if (evas_key_lock_is_set(ev->locks, "Caps_Lock")) m |= EVOAK_LOCK_MASK_CAPS; \
   if (evas_key_lock_is_set(ev->locks, "Scroll_Lock")) m |= EVOAK_LOCK_MASK_SCROLL; \
   if (evas_key_lock_is_set(ev->locks, "Num_Lock")) m |= EVOAK_LOCK_MASK_NUM; \
}

static void
key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev;
   Evoak_Server *es;
   
   ev = event_info;
   es = data;
     {
	Evas_List *l, *ll;

	for (l = es->clients; l; l = l->next)
	  {
	     Evoak_Client *ec;
	     
	     ec = l->data;
	     for (ll = ec->focused; ll; ll = ll->next)
	       {
		  Evoak_Object *eo;
		  
		  eo = ll->data;
		  if (eo->mask1 & EVOAK_EVENT_MASK1_KEY_DOWN)
		    {
		       Evoak_PR_Object_Event_Key_Downup p;
		       int s;
		       void *d;
		       
		       SETMODS(p.modmask);
		       SETLOCKS(p.lockmask);
		       p.downup = 1;
		       p.keyname = ev->keyname;
		       p.string = ""; /* FIXME: string shoudl be what woudl be typed by this press */
		       d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_KEY_DOWNUP].enc(&p, &s);
		       if (d)
			 {
			    ecore_ipc_client_send(ec->client,
						  EVOAK_PR_OBJECT_EVENT_KEY_DOWNUP,
						  EVOAK_PROTOCOL_VERSION,
						  ec->request_id, eo->id,
						  0,
						  d, s);
			    ec->request_id++;
			    free(d);
			 }
		    }
	       }
	  }
     }
}

static void
main_pre_rend(Ecore_Evas *ee)
{
   edje_thaw();
}
   
static void
main_post_rend(Ecore_Evas *ee)
{
   edje_freeze();
}

static void
key_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Up *ev;
   Evoak_Server *es;
   
   ev = event_info;
   es = data;
     {
	Evas_List *l, *ll;
	
	for (l = es->clients; l; l = l->next)
	  {
	     Evoak_Client *ec;
	     
	     ec = l->data;
	     for (ll = ec->focused; ll; ll = ll->next)
	       {
		  Evoak_Object *eo;
		  
		  eo = ll->data;
		  if (eo->mask1 & EVOAK_EVENT_MASK1_KEY_UP)
		    {
		       Evoak_PR_Object_Event_Key_Downup p;
		       int s;
		       void *d;
		       
		       SETMODS(p.modmask);
		       SETLOCKS(p.lockmask);
		       p.downup = 0;
		       p.keyname = ev->keyname;
		       p.string = ""; /* FIXME: string shoudl be what woudl be typed by this release */
		       d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_KEY_DOWNUP].enc(&p, &s);
		       if (d)
			 {
			    ecore_ipc_client_send(ec->client,
						  EVOAK_PR_OBJECT_EVENT_KEY_DOWNUP,
						  EVOAK_PROTOCOL_VERSION,
						  ec->request_id, eo->id,
						  0,
						  d, s);
			    ec->request_id++;
			    free(d);
			 }
		    }
	       }
	  }
     }
}

static void
mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   Evoak_Object *eo;
   
   ev = event_info;
   eo = data;
   if (eo->mask1 & EVOAK_EVENT_MASK1_MOUSE_MOVE)
     {
	Evoak_PR_Object_Event_Mouse_Move p;
	int s;
	void *d;
		       
	SETMODS(p.modmask);
	SETLOCKS(p.lockmask);
	p.x = ev->cur.canvas.x;
	p.y = ev->cur.canvas.y;
	p.px = ev->prev.canvas.x;
	p.py = ev->prev.canvas.y;
	p.bmask = ev->buttons;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MOUSE_MOVE].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_MOUSE_MOVE,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
}

static void
mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   Evoak_Object *eo;
   
   ev = event_info;
   eo = data;
   if (eo->mask1 & EVOAK_EVENT_MASK1_MOUSE_IN)
     {
	Evoak_PR_Object_Event_Mouse_Inout p;
	int s;
	void *d;
		       
	SETMODS(p.modmask);
	SETLOCKS(p.lockmask);
	p.x = ev->canvas.x;
	p.y = ev->canvas.y;
	p.bmask = ev->buttons;
	p.inout = 1;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MOUSE_INOUT].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_MOUSE_INOUT,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
}

static void
mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   Evoak_Object *eo;
   
   ev = event_info;
   eo = data;
   if (eo->mask1 & EVOAK_EVENT_MASK1_MOUSE_OUT)
     {
	Evoak_PR_Object_Event_Mouse_Inout p;
	int s;
	void *d;
		       
	SETMODS(p.modmask);
	SETLOCKS(p.lockmask);
	p.x = ev->canvas.x;
	p.y = ev->canvas.y;
	p.bmask = ev->buttons;
	p.inout = 0;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MOUSE_INOUT].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_MOUSE_INOUT,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
}

static void
mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   Evoak_Object *eo;
   
   ev = event_info;
   eo = data;
   if (eo->mask1 & EVOAK_EVENT_MASK1_MOUSE_UP)
     {
	Evoak_PR_Object_Event_Mouse_Downup p;
	int s;
	void *d;
		       
	SETMODS(p.modmask);
	SETLOCKS(p.lockmask);
	p.x = ev->canvas.x;
	p.y = ev->canvas.y;
	p.b = ev->button;
	p.downup = 0;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MOUSE_DOWNUP].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_MOUSE_DOWNUP,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
}

static void
mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Evoak_Object *eo;
   
   ev = event_info;
   eo = data;
   if (eo->mask1 & EVOAK_EVENT_MASK1_MOUSE_DOWN)
     {
	Evoak_PR_Object_Event_Mouse_Downup p;
	int s;
	void *d;
		       
	SETMODS(p.modmask);
	SETLOCKS(p.lockmask);
	p.x = ev->canvas.x;
	p.y = ev->canvas.y;
	p.b = ev->button;
	p.downup = 1;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MOUSE_DOWNUP].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_MOUSE_DOWNUP,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
}

static void
mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Wheel *ev;
   Evoak_Object *eo;
   
   ev = event_info;
   eo = data;
   if (eo->mask1 & EVOAK_EVENT_MASK1_MOUSE_WHEEL)
     {
	Evoak_PR_Object_Event_Mouse_Wheel p;
	int s;
	void *d;
		       
	SETMODS(p.modmask);
	SETLOCKS(p.lockmask);
	p.x = ev->canvas.x;
	p.y = ev->canvas.y;
	p.dir = ev->direction;
	p.z = ev->z;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_MOUSE_WHEEL].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_MOUSE_WHEEL,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
}

static void
edje_signal_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Evoak_Edje_Callback *ecb;
   Evoak_Object *eo;
   
   ecb = data;
   eo = ecb->eo;
     {
	Evoak_PR_Object_Edje_Signal p;
	int s;
	void *d;
	
	p.callback_id = ecb->id;
	p.emission = (char *)emission;
	p.source = (char *)source;
	d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SIGNAL].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EDJE_SIGNAL,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
}

static void
edje_text_change_cb(void *data, Evas_Object *o, const char *part)
{
   Evoak_Object *eo;
   
   eo = data;
     {
        Evoak_PR_Object_Edje_Text_Changed p;
	int s;
	void *d;
	
	p.part = part;
	p.text = edje_object_part_text_get(eo->object, part);
	d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_TEXT_CHANGED].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EDJE_TEXT_CHANGED,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
}

static void
edje_swallow_show(void *data, Evas_Object *obj)
{
   Evoak_Object *eo;

   eo = data;
   ecore_ipc_client_send(eo->ec->client,
			 EVOAK_PR_OBJECT_EVENT_OBJECT_SHOW,
			 EVOAK_PROTOCOL_VERSION,
			 eo->ec->request_id, eo->id,
			 0,
			 NULL, 0);
   eo->ec->request_id++;
   evas_object_show(obj);
}

static void
edje_swallow_hide(void *data, Evas_Object *obj)
{
   Evoak_Object *eo;
   
   eo = data;
   ecore_ipc_client_send(eo->ec->client,
			 EVOAK_PR_OBJECT_EVENT_OBJECT_HIDE,
			 EVOAK_PROTOCOL_VERSION,
			 eo->ec->request_id, eo->id,
			 0,
			 NULL, 0);
   eo->ec->request_id++;
   evas_object_hide(obj);
}

static void
edje_swallow_move(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Evoak_Object *eo;
   
   eo = data;
     {
	Evoak_PR_Object_Event_Object_Move p;
	int s;
	void *d;
	
	p.x = x;
	p.y = y;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_OBJECT_MOVE].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_OBJECT_MOVE,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
   evas_object_move(obj, x, y);
}

static void
edje_swallow_resize(void *data, Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Evoak_Object *eo;
   
   eo = data;
     {
	Evoak_PR_Object_Event_Object_Resize p;
	int s;
	void *d;
	
	p.w = w;
	p.h = h;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_OBJECT_RESIZE].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_OBJECT_RESIZE,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
   evas_object_resize(obj, w, h);
}

static void
edje_swallow_raise(void *data, Evas_Object *obj)
{
   Evoak_Object *eo;
   
   eo = data;
     {
	Evoak_PR_Object_Event_Object_Restack p;
	int s;
	void *d;
	
	p.relative_id = 0;
	p.abovebelow = 1;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
   evas_object_raise(obj);
}

static void
edje_swallow_lower(void *data, Evas_Object *obj)
{
   Evoak_Object *eo;
   
   eo = data;
     {
	Evoak_PR_Object_Event_Object_Restack p;
	int s;
	void *d;
	
	p.relative_id = 0;
	p.abovebelow = 0;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
   evas_object_lower(obj);
}

static void
edje_swallow_stack_above(void *data, Evas_Object *obj, Evas_Object *above)
{
   Evoak_Object *eo;
   
   eo = data;
     {
	Evoak_PR_Object_Event_Object_Restack p;
	int s;
	void *d;
	
	p.relative_id = 0;
	p.abovebelow = 1;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
   evas_object_stack_above(obj, above);
}

static void
edje_swallow_stack_below(void *data, Evas_Object *obj, Evas_Object *below)
{
   Evoak_Object *eo;
   
   eo = data;
     {
	Evoak_PR_Object_Event_Object_Restack p;
	int s;
	void *d;
	
	p.relative_id = 0;
	p.abovebelow = 0;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
   evas_object_stack_below(obj, below);
}

static void
edje_swallow_layer_set(void *data, Evas_Object *obj, int l)
{
   Evoak_Object *eo;
   
   eo = data;
     {
	Evoak_PR_Object_Event_Object_Layer_Set p;
	int s;
	void *d;
	
	p.l = l;
	d = _evoak_proto[EVOAK_PR_OBJECT_EVENT_OBJECT_LAYER_SET].enc(&p, &s);
	if (d)
	  {
	     ecore_ipc_client_send(eo->ec->client,
				   EVOAK_PR_OBJECT_EVENT_OBJECT_LAYER_SET,
				   EVOAK_PROTOCOL_VERSION,
				   eo->ec->request_id, eo->id,
				   0,
				   d, s);
	     eo->ec->request_id++;
	     free(d);
	  }
     }
   evas_object_layer_set(obj, l);
}


static int
ipc_start(int argc, char **argv)
{
   Ecore_Ipc_Server *server;
   Evoak_Server *es;
   int server_num;
   Evas_Object *o;
   int w, h;
   
   server_num = 0;
   es = calloc(1, sizeof(Evoak_Server));
   if (!es) return -1;
   server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, "evoak", server_num, es);
   if (!server)
     {
	free(es);
	return -1;
     }
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, ipc_cb_client_add, es);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, ipc_cb_client_del, es);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_cb_client_data, es);
   es->ecore_evas = ecore_evas;
   es->evas       = ecore_evas_get(ecore_evas);
   es->start_time = start_time;
   
   o = evas_object_rectangle_add(es->evas);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, key_down, es);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_UP, key_up, es);
   evas_object_focus_set(o, 1);
   evas_object_name_set(o, "base");
   if (ecore_evas_shaped_get(es->ecore_evas)) evas_object_color_set(o, 255, 255, 255, 0);
   else evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_data_set(o, "srv", es);
   evas_object_layer_set(o, EVAS_LAYER_MIN);
   evas_object_move(o, 0, 0);
   ecore_evas_geometry_get(es->ecore_evas, NULL, NULL, &(w), &(h));
   evas_object_resize(o, w, h);
   evas_object_show(o);
   return 1;
}

static int
ipc_cb_client_add(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Add *e;
   Evoak_Server *es;
   Evoak_Client *ec;
   
   e = event;
   ec = calloc(1, sizeof(Evoak_Client));
   if (!ec) return 1;
   ecore_ipc_client_data_set(e->client, ec);
   es = ecore_ipc_server_data_get(ecore_ipc_client_server_get(e->client));
   ec->server = es;
   ec->client = e->client;
   ec->request_id = 1;
   ec->server->clients = evas_list_append(ec->server->clients, ec);
   printf("!!! client %p connected to server!\n", e->client);
   return 1;
}
   
static int
ipc_cb_client_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;
   Evoak_Client *ec;
   
   e = event;
   ec = ecore_ipc_client_data_get(e->client);
     {
	int i;
	
	ec->server->clients = evas_list_remove(ec->server->clients, ec);
	evas_list_free(ec->focused);
	for (i = 0; i < 64; i++)
	  {
	     while (ec->hash.buckets[i])
	       {
		  Evoak_Object *eo;
		  Evoak_Hash_Node *hn;
		  
		  hn = ec->hash.buckets[i]->data;
		  eo = hn->data;
		  ec->hash.buckets[i] = evas_list_remove(ec->hash.buckets[i], hn);
		  free(hn);
		  evas_object_del(eo->object);
		  free(eo);
	       }
	     while (ec->freeze_q)
	       {
		  Ecore_Ipc_Event_Client_Data *e2;
		  
		  e2 = ec->freeze_q->data;
		  ec->freeze_q = evas_list_remove(ec->freeze_q, e2);
		  if (e2->data) free(e2->data);
		  free(e2);
	       }
	  }
     }
   free(ec);
   ecore_ipc_client_del(e->client);
   printf("!!! client %p disconnected from server!\n", e->client);
   return 1;
}
      
static int
ipc_cb_client_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Data *e;
   Evoak_Client *ec;
   
   e = event;
//   printf("!!! client %p sent data!\n", e->client);
   ec = ecore_ipc_client_data_get(e->client);
   if ((e->minor != EVOAK_PROTOCOL_VERSION) ||
       (e->major <= EVOAK_PR_NONE) ||
       (e->major >= EVOAK_PR_LAST))
     return 1;
   if (!ec->authenticated)
     {
	if (e->major == EVOAK_PR_CONNECT)
	  {
	       {
		  Evoak_PR_Connect p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    ec->authenticated = 1;
	       }
	       {
		  Evoak_PR_Canvas_Info p;
		  void *d;
		  int s;
		  
		  ecore_evas_geometry_get(ec->server->ecore_evas, NULL, NULL, &(p.w), &(p.h));
		  p.rot = ecore_evas_rotation_get(ec->server->ecore_evas);
		  d = _evoak_proto[EVOAK_PR_CANVAS_INFO].enc(&p, &s);
		  if (d)
		    {
		       ecore_ipc_client_send(e->client,
					     EVOAK_PR_CANVAS_INFO,
					     EVOAK_PROTOCOL_VERSION,
					     ec->request_id, e->ref,
					     0,
					     d, s);
		       ec->request_id++;
		       free(d);
		    }
	       }
	     return 1;
	  }
	return 1;
     }
   if ((!ec->frozen) && (e->major == EVOAK_PR_CLIENT_FREEZE))
     ec->frozen = 1;
   else if ((ec->frozen) && (e->major == EVOAK_PR_CLIENT_THAW))
     ec->frozen = 0;
   else
     {
	Ecore_Ipc_Event_Client_Data *e2;
	
	e2 = malloc(sizeof(Ecore_Ipc_Event_Client_Data));
	if (e2)
	  {
	     memcpy(e2, e, sizeof(Ecore_Ipc_Event_Client_Data));
	     e->data = NULL;
	     e->size = 0;
	     ec->freeze_q = evas_list_append(ec->freeze_q, e2);
	  }
     }
   if (ec->frozen) return 1;
   while (ec->freeze_q)
     {
	e = ec->freeze_q->data;
	ec->freeze_q = evas_list_remove(ec->freeze_q, e);
	switch (e->major)
	  {
	   case EVOAK_PR_CONNECT: /* handled above */
	     break;
	   case EVOAK_PR_CANVAS_INFO: /* server doesnt handle this */
	     break;
	   case EVOAK_PR_SYNC:
	     ecore_ipc_client_send(e->client,
				   EVOAK_PR_SYNC_REPLY,
				   EVOAK_PROTOCOL_VERSION,
				   ec->request_id, e->ref,
				   e->response,
				   NULL, 0);
	     ec->request_id++;	
	     break;
	   case EVOAK_PR_SYNC_REPLY: /* server doesnt handle this */
	     break;
	   case EVOAK_PR_OBJECT_ADD:
	       {
		  Evoak_PR_Object_Add p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evas_Object *o;
		       
		       switch (p.type)
			 {
			  case EVOAK_OBJECT_TYPE_RECTANGLE:
			    o = evas_object_rectangle_add(ec->server->evas);
			    break;
			  case EVOAK_OBJECT_TYPE_IMAGE:
			    o = evas_object_image_add(ec->server->evas);
			    break;
			  case EVOAK_OBJECT_TYPE_TEXT:
			    o = evas_object_text_add(ec->server->evas);
			    break;
			  case EVOAK_OBJECT_TYPE_LINE:
			    o = evas_object_line_add(ec->server->evas);
			    break;
			  case EVOAK_OBJECT_TYPE_POLYGON:
			    o = evas_object_polygon_add(ec->server->evas);
			    break;
			  case EVOAK_OBJECT_TYPE_GRADIENT:
			    o = evas_object_gradient_add(ec->server->evas);
			    break;
			  case EVOAK_OBJECT_TYPE_EDJE:
			    o = edje_object_add(ec->server->evas);
			    break;
			  default:
			    break;
			 }
		       if (o)
			 {
			    Evoak_Object *eo;
			    
			    if (p.type == EVOAK_OBJECT_TYPE_EDJE)
			      edje_object_part_text_set(o, edje_text_change_cb, eo);
			    evas_object_resize(o, 0, 0);
			    evas_object_move(o, 0, 0);
			    evas_object_color_set(o, 255, 255, 255, 255);
			    eo = calloc(1, sizeof(Evoak_Object));
			    if (eo)
			      {
				 eo->ec = ec;
				 eo->object = o;
				 eo->id = e->ref_to;
				 eo->hint.min.w = 0;
				 eo->hint.min.h = 0;
				 eo->hint.max.w = -1;
				 eo->hint.max.h = -1;
				 evas_object_data_set(o, "eo", eo);
				 if (!hash_store(&ec->hash, eo->id, eo))
				   {
				      evas_object_del(o);
				      free(eo);
				   }
			      }
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_DEL:
	       {
		  Evoak_Object *eo;
		  
		  eo = hash_unstore(&ec->hash, e->ref_to);
		  if (eo)
		    {
		       if (eo->focused)
			 ec->focused = evas_list_remove(ec->focused, eo);
		       evas_object_del(eo->object);
		       while (eo->edje_callbacks)
			 {
			    free(eo->edje_callbacks->data);
			    eo->edje_callbacks = evas_list_remove_list(eo->edje_callbacks, eo->edje_callbacks);
			 }
		       free(eo);
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_MOVE:
	       {
		  Evoak_PR_Object_Move p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.x = p.x;
			    eo->rel_context.y = p.y;
			    evas_object_move(eo->object, p.x, p.y);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_RESIZE:
	       {
		  Evoak_PR_Object_Resize p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.w = p.w;
			    eo->rel_context.h = p.h;
			    evas_object_resize(eo->object, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_SHOW:
	       {
		  Evoak_Object *eo;
		  
		  eo = hash_find(&ec->hash, e->ref_to);
		  if (eo)
		    {
		       evas_object_show(eo->object);
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_HIDE:
	       {
		  Evoak_Object *eo;
		  
		  eo = hash_find(&ec->hash, e->ref_to);
		  if (eo)
		    {
		       evas_object_hide(eo->object);
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_CLIP_SET:
	       {
		  Evoak_PR_Object_Clip_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    if (!p.clipper_id)
			      evas_object_clip_unset(eo->object);
			    else
			      {
				 Evoak_Object *eo2;
				 
				 eo2 = hash_find(&ec->hash, p.clipper_id);
				 if (eo2)
				   {
				      evas_object_clip_set(eo->object, eo2->object);
				   }
			      }
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_COLOR_SET:
	       {
		  Evoak_PR_Object_Color_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_color_set(eo->object, p.r, p.g, p.b, p.a);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_LAYER_SET:
	       {
		  Evoak_PR_Object_Layer_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_layer_set(eo->object, p.layer);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_STACK:
	       {
		  Evoak_PR_Object_Stack p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    if (p.relative_id == 0)
			      {
				 if (p.relative == EVOAK_RELATIVE_ABOVE)
				   evas_object_raise(eo->object);
				 else if (p.relative == EVOAK_RELATIVE_BELOW)
				   evas_object_lower(eo->object);
			      }
			    else
			      {
				 Evoak_Object *eo2;
				 
				 eo2 = hash_find(&ec->hash, p.relative_id);
				 if (eo2)
				   {
				      if (p.relative == EVOAK_RELATIVE_ABOVE)
					evas_object_stack_above(eo->object, eo2->object);
				      else if (p.relative == EVOAK_RELATIVE_BELOW)
					evas_object_stack_below(eo->object, eo2->object);
				   }
			      }
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EVENT_PROP_SET:
	       {
		  Evoak_PR_Object_Event_Prop_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    if (p.property == EVOAK_PROPERTY_PASS)
			      evas_object_pass_events_set(eo->object, p.value);
			    else if (p.property == EVOAK_PROPERTY_REPEAT)
			      evas_object_repeat_events_set(eo->object, p.value);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EVENT_MASK_SET:
	       {
		  Evoak_PR_Object_Event_Mask_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
#define MASK_ENABLED(m) ((!(eo->mask1 & m)) && ((p.mask1 & m)))
#define MASK_DISABLED(m) (((eo->mask1 & m)) && (!(p.mask1 & m)))
			    if (MASK_DISABLED(EVOAK_EVENT_MASK1_MOUSE_MOVE))
			      evas_object_event_callback_del(eo->object, EVAS_CALLBACK_MOUSE_MOVE, mouse_move);
			    if (MASK_ENABLED(EVOAK_EVENT_MASK1_MOUSE_MOVE))
			      evas_object_event_callback_add(eo->object, EVAS_CALLBACK_MOUSE_MOVE, mouse_move, eo);
			    
			    if (MASK_DISABLED(EVOAK_EVENT_MASK1_MOUSE_IN))
			      evas_object_event_callback_del(eo->object, EVAS_CALLBACK_MOUSE_IN, mouse_in);
			    if (MASK_ENABLED(EVOAK_EVENT_MASK1_MOUSE_IN))
			      evas_object_event_callback_add(eo->object, EVAS_CALLBACK_MOUSE_IN, mouse_in, eo);
			    
			    if (MASK_DISABLED(EVOAK_EVENT_MASK1_MOUSE_OUT))
			      evas_object_event_callback_del(eo->object, EVAS_CALLBACK_MOUSE_OUT, mouse_out);
			    if (MASK_ENABLED(EVOAK_EVENT_MASK1_MOUSE_OUT))
			      evas_object_event_callback_add(eo->object, EVAS_CALLBACK_MOUSE_OUT, mouse_out, eo);
			    
			    if (MASK_DISABLED(EVOAK_EVENT_MASK1_MOUSE_UP))
			      evas_object_event_callback_del(eo->object, EVAS_CALLBACK_MOUSE_UP, mouse_up);
			    if (MASK_ENABLED(EVOAK_EVENT_MASK1_MOUSE_UP))
			      evas_object_event_callback_add(eo->object, EVAS_CALLBACK_MOUSE_UP, mouse_up, eo);
			    
			    if (MASK_DISABLED(EVOAK_EVENT_MASK1_MOUSE_DOWN))
			      evas_object_event_callback_del(eo->object, EVAS_CALLBACK_MOUSE_DOWN, mouse_down);
			    if (MASK_ENABLED(EVOAK_EVENT_MASK1_MOUSE_DOWN))
			      evas_object_event_callback_add(eo->object, EVAS_CALLBACK_MOUSE_DOWN, mouse_down, eo);
			    
			    if (MASK_DISABLED(EVOAK_EVENT_MASK1_MOUSE_WHEEL))
			      evas_object_event_callback_del(eo->object, EVAS_CALLBACK_MOUSE_WHEEL, mouse_wheel);
			    if (MASK_ENABLED(EVOAK_EVENT_MASK1_MOUSE_WHEEL))
			      evas_object_event_callback_add(eo->object, EVAS_CALLBACK_MOUSE_WHEEL, mouse_wheel, eo);
			    
			    eo->mask1 = p.mask1;
			    eo->mask2 = p.mask2;
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_FOCUS_SET:
	       {
		  Evoak_PR_Object_Focus_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    if (p.onoff)
			      {
				 if (!eo->focused)
				   {
				      ec->focused = evas_list_append(ec->focused, eo);
				      eo->focused = 1;
				   }
			      }
			    else
			      {
				 if (eo->focused)
				   {
				      ec->focused = evas_list_remove(ec->focused, eo);
				      eo->focused = 0;
				   }
			      }
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_FILE_SET:
	       {
		  Evoak_PR_Object_Image_File_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_file_set(eo->object, p.file, p.key);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_FILL_SET:
	       {
		  Evoak_PR_Object_Image_Fill_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_fill_set(eo->object, p.x, p.y, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_CLIENT_FREEZE:
	     break;
	   case EVOAK_PR_CLIENT_THAW:
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_BORDER_SET:
	       {
		  Evoak_PR_Object_Image_Border_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_border_set(eo->object, p.l, p.r, p.t, p.b);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_SMOOTH_SCALE_SET:
	       {
		  Evoak_PR_Object_Image_Smooth_Scale_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_smooth_scale_set(eo->object, p.onoff);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_SIZE_SET:
	       {
		  Evoak_PR_Object_Image_Size_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_size_set(eo->object, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_ALPHA_SET:
	       {
		  Evoak_PR_Object_Image_Alpha_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_alpha_set(eo->object, p.onoff);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EVENT_MOUSE_MOVE:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_MOUSE_DOWNUP:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_MOUSE_INOUT:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_MOUSE_WHEEL:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_KEY_DOWNUP:
	     break;
	   case EVOAK_PR_OBJECT_LINE_SET:
	       {
		  Evoak_PR_Object_Line_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_line_xy_set(eo->object, p.x1, p.y1, p.x2, p.y2);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_GRAD_ANGLE_SET:
	       {
		  Evoak_PR_Object_Grad_Angle_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_gradient_angle_set(eo->object, (double)(p.ang100) / 100.0);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_GRAD_COLOR_ADD:
	       {
		  Evoak_PR_Object_Grad_Color_Add p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_gradient_color_add(eo->object, p.r, p.g, p.b, p.a, p.dist);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_GRAD_COLOR_CLEAR:
	       {
		  Evoak_Object *eo;
		  
		  eo = hash_find(&ec->hash, e->ref_to);
		  if (eo)
		    {
		       evas_object_gradient_colors_clear(eo->object);
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_POLY_POINT_ADD:
	       {
		  Evoak_PR_Object_Poly_Point_Add p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_polygon_point_add(eo->object, p.x, p.y);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_POLY_POINT_CLEAR:
	       {
		  Evoak_Object *eo;
		  
		  eo = hash_find(&ec->hash, e->ref_to);
		  if (eo)
		    {
		       evas_object_polygon_points_clear(eo->object);
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_TEXT_SOURCE_SET:
	       {
		  Evoak_PR_Object_Text_Source_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    if (p.source[0] == 0)
			      evas_object_text_font_source_set(eo->object, NULL);
			    else
			      evas_object_text_font_source_set(eo->object, p.source);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_TEXT_FONT_SET:
	       {
		  Evoak_PR_Object_Text_Font_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_text_font_set(eo->object, p.font, p.size);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_TEXT_TEXT_SET:
	       {
		  Evoak_PR_Object_Text_Text_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_text_text_set(eo->object, p.text);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_MOVE8:
	       {
		  Evoak_PR_Object_Move8 p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.x = p.x;
			    eo->rel_context.y = p.y;
			    evas_object_move(eo->object, p.x, p.y);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_MOVE16:
	       {
		  Evoak_PR_Object_Move16 p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.x = p.x;
			    eo->rel_context.y = p.y;
			    evas_object_move(eo->object, p.x, p.y);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_MOVE_REL8:
	       {
		  Evoak_PR_Object_Move_Rel8 p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.x += p.x;
			    eo->rel_context.y += p.y;
			    evas_object_move(eo->object, eo->rel_context.x, eo->rel_context.y);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_MOVE_REL16:
	       {
		  Evoak_PR_Object_Move_Rel16 p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.x += p.x;
			    eo->rel_context.y += p.y;
			    evas_object_move(eo->object, eo->rel_context.x, eo->rel_context.y);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_RESIZE8:
	       {
		  Evoak_PR_Object_Resize8 p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.w = p.w;
			    eo->rel_context.h = p.h;
			    evas_object_resize(eo->object, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_RESIZE16:
	       {
		  Evoak_PR_Object_Resize16 p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.w = p.w;
			    eo->rel_context.h = p.h;
			    evas_object_resize(eo->object, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_RESIZE_REL8:
	       {
		  Evoak_PR_Object_Resize_Rel8 p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.w += p.w;
			    eo->rel_context.h += p.h;
			    evas_object_resize(eo->object, eo->rel_context.w, eo->rel_context.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_RESIZE_REL16:
	       {
		  Evoak_PR_Object_Resize_Rel16 p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    eo->rel_context.w += p.w;
			    eo->rel_context.h += p.h;
			    evas_object_resize(eo->object, eo->rel_context.w, eo->rel_context.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_FILL8_SET:
	       {
		  Evoak_PR_Object_Image_Fill8_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_fill_set(eo->object, p.x, p.y, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_FILL16_SET:
	       {
		  Evoak_PR_Object_Image_Fill16_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_fill_set(eo->object, p.x, p.y, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_FILL_SIZE8_SET:
	       {
		  Evoak_PR_Object_Image_Fill_Size8_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_fill_set(eo->object, 0, 0, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_FILL_SIZE16_SET:
	       {
		  Evoak_PR_Object_Image_Fill_Size16_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    evas_object_image_fill_set(eo->object, 0, 0, p.w, p.h);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_IMAGE_FILL_ALL_SET:
	       {
		  Evoak_Object *eo;
		  
		  eo = hash_find(&ec->hash, e->ref_to);
		  if (eo)
		    {
		       evas_object_image_fill_set(eo->object, 0, 0, eo->rel_context.w, eo->rel_context.h);
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EVENT_OBJECT_MOVE:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_OBJECT_RESIZE:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_OBJECT_LAYER_SET:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_OBJECT_SHOW:
	     break;
	   case EVOAK_PR_OBJECT_EVENT_OBJECT_HIDE:
	     break;
	   case EVOAK_PR_OBJECT_EDJE_FILE_SET:
	       {
		  Evoak_PR_Object_Edje_File_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    edje_object_file_set(eo->object, p.file, p.group);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EDJE_SWALLOW:
	       {
		  Evoak_PR_Object_Edje_Swallow p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo, *eo2;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       eo2 = hash_find(&ec->hash, p.swallow_id);
		       if ((eo) && (eo2))
			 {
			    if ((eo2->hint.min.w > 0) || (eo2->hint.min.h > 0))
			      edje_extern_object_min_size_set(eo2->object, eo2->hint.min.w, eo2->hint.min.h);
			    if ((eo2->hint.max.w >= 0) || (eo2->hint.max.h >= 0))
			      edje_extern_object_max_size_set(eo2->object, eo2->hint.max.w, eo2->hint.max.h);
			    evas_object_intercept_show_callback_add(eo->object, edje_swallow_show, eo);
			    evas_object_intercept_hide_callback_add(eo->object, edje_swallow_hide, eo);
			    evas_object_intercept_move_callback_add(eo->object, edje_swallow_move, eo);
			    evas_object_intercept_resize_callback_add(eo->object, edje_swallow_resize, eo);
			    evas_object_intercept_raise_callback_add(eo->object, edje_swallow_raise, eo);
			    evas_object_intercept_lower_callback_add(eo->object, edje_swallow_lower, eo);
			    evas_object_intercept_stack_above_callback_add(eo->object, edje_swallow_stack_above, eo);
			    evas_object_intercept_stack_below_callback_add(eo->object, edje_swallow_stack_below, eo);
			    evas_object_intercept_layer_set_callback_add(eo->object, edje_swallow_layer_set, eo);
			    edje_object_part_swallow(eo->object, p.part, eo2->object);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EDJE_UNSWALLOW:
	       {
		  Evoak_PR_Object_Edje_Unswallow p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo, *eo2;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       eo2 = hash_find(&ec->hash, p.swallow_id);
		       if ((eo) && (eo2))
			 {
			    edje_object_part_unswallow(eo->object, eo2->object);
			    evas_object_intercept_show_callback_del(eo->object, edje_swallow_show);
			    evas_object_intercept_hide_callback_del(eo->object, edje_swallow_hide);
			    evas_object_intercept_move_callback_del(eo->object, edje_swallow_move);
			    evas_object_intercept_resize_callback_del(eo->object, edje_swallow_resize);
			    evas_object_intercept_raise_callback_del(eo->object, edje_swallow_raise);
			    evas_object_intercept_lower_callback_del(eo->object, edje_swallow_lower);
			    evas_object_intercept_stack_above_callback_del(eo->object, edje_swallow_stack_above);
			    evas_object_intercept_stack_below_callback_del(eo->object, edje_swallow_stack_below);
			    evas_object_intercept_layer_set_callback_del(eo->object, edje_swallow_layer_set);
			    edje_extern_object_min_size_set(eo2->object, 0, 0);
			    edje_extern_object_max_size_set(eo2->object, -1, -1);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EDJE_TEXT_SET:
	       {
		  Evoak_PR_Object_Edje_Text_Set p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    edje_object_part_text_set(eo->object, p.part, p.text);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EDJE_TEXT_CHANGED:
	     break;
	   case EVOAK_PR_OBJECT_EDJE_SIGNAL_LISTEN:
	       {
		  Evoak_PR_Object_Edje_Signal_Listen p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    Evoak_Edje_Callback *ecb;
			    
			    ecb = malloc(sizeof(Evoak_Edje_Callback));
			    if (ecb)
			      {
				 ecb->id = p.callback_id;
				 ecb->eo = eo;
				 edje_object_signal_callback_add(eo->object, p.emission, p.source, edje_signal_cb, ecb);
			      }
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EDJE_SIGNAL_UNLISTEN:
	       {
		  Evoak_PR_Object_Edje_Signal_Unlisten p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    Evas_List *l;
			    
			    for (l = eo->edje_callbacks; l; l = l->next)
			      {
				 Evoak_Edje_Callback *ecb;
				 
				 ecb = l->data;
				 if (ecb->id == p.callback_id)
				   {
				      eo->edje_callbacks = evas_list_remove_list(eo->edje_callbacks, l);
				      free(ecb);
				      edje_object_signal_callback_del(eo->object, p.emission, p.source, edje_signal_cb);
				      break;
				   }
			      }
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EDJE_SIGNAL_EMIT:
	       {
		  Evoak_PR_Object_Edje_Signal_Emit p;
		  
		  if (_evoak_proto[e->major].dec(e->data, e->size, &p))
		    {
		       Evoak_Object *eo;
		       
		       eo = hash_find(&ec->hash, e->ref_to);
		       if (eo)
			 {
			    edje_object_signal_emit(eo->object, p.emission, p.source);
			 }
		    }
	       }
	     break;
	   case EVOAK_PR_OBJECT_EDJE_SIGNAL:
	     break;
	   default:
	     break;
	  }
	if (e->data) free(e->data);
	free(e);
     }
   return 1;
}

static int hash_keygen(Evoak_Hash *hash, int key);

static int
hash_keygen(Evoak_Hash *hash, int key)
{
   return (key & (64 - 1));
}

int
hash_store(Evoak_Hash *hash, int id, void *data)
{
   Evoak_Hash_Node *hn;
   int h;
   
   hn = malloc(sizeof(Evoak_Hash_Node));
   if (!hn) return 0;
   h = hash_keygen(hash, id);
   hn->id = id;
   hn->data = data;
   hash->buckets[h] = evas_list_prepend(hash->buckets[h], hn);
   return 1;
}

void *
hash_unstore(Evoak_Hash *hash, int id)
{
   Evoak_Hash_Node *hn;
   Evas_List *l;
   int h;

   h = hash_keygen(hash, id);
   for (l = hash->buckets[h]; l; l = l->next)
     {
	hn = l->data;
	if (hn->id == id)
	  {
	     void *data;
	     
	     hash->buckets[h] = evas_list_remove_list(hash->buckets[h], l);
	     data = hn->data;
	     free(hn);
	     return data;
	  }
     }
   return NULL;
}

void *
hash_find(Evoak_Hash *hash, int id)
{
   Evoak_Hash_Node *hn;
   Evas_List *l;
   int h;

   h = hash_keygen(hash, id);
   for (l = hash->buckets[h]; l; l = l->next)
     {
	hn = l->data;
	if (hn->id == id)
	  {
	     hash->buckets[h] = evas_list_remove_list(hash->buckets[h], l);
	     hash->buckets[h] = evas_list_prepend(hash->buckets[h], hn);
	     return hn->data;
	  }
     }
   return NULL;
}

int
main(int argc, char **argv)
{   
   if (main_start(argc, argv) < 1) return -1;
   
   _evoak_protocol_init();
   
   if (ipc_start(argc, argv) < 1)
     {
	main_stop();
	return -1;
     }
   
   ecore_main_loop_begin();
   
   _evoak_protocol_shutdown();
   main_stop();
   
   return 0;
}


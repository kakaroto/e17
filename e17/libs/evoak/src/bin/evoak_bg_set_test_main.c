#include <Evoak.h>
#include <Ecore.h>
#include <Ecore_Config.h>
#include <math.h>

static int set;
static int bg_w, bg_h;
Evoak_Object *background;
double start_t = 0.0;
char *bg_file;

void
setup(Evoak *ev)
{
   Evoak_Object *o;
   
   o = evoak_object_image_add(ev);
   evoak_object_image_file_set(o, bg_file, NULL);
   evoak_object_move(o, 0, 0);
   evoak_object_layer_set(o, -2000000000);
   evoak_object_show(o);
   background = o;
   
   evoak_sync(ev, 1);
}

void
resize(int w, int h)
{
   evoak_object_resize(background, w, h);
   evoak_object_image_fill_set(background, 0, 0, w, h);
}

int
cb_canvas_info(void *data, int type, void *event)
{
   Evoak_Event_Canvas_Info *ev;
   
   ev = event;
   bg_w = ev->w;
   bg_h = ev->h;
   if (!set)
     {
	setup(ev->evoak);
	set = 1;
     }
   resize(ev->w, ev->h);
   return 1;
}

int
bg_listener(const char *key, const Ecore_Config_Type type, const int tag, void *data)
{
   char *new_file;
   
   new_file = ecore_config_string_get(key);

   if (set) evoak_object_image_file_set(background, new_file, NULL);
}

int
main(int argc, char **argv)
{
   char *file;
   if (evoak_init())
     {
	Evoak *ev;
	
	ecore_app_args_set(argc, (const char **)argv);
	ecore_config_init("evoak_background_client");
	ecore_config_app_describe("A background setting app for the Evoak canvas server\n\
Version 0.0.1\n\
Author HandyAndE\n\
usage: evoak_bg_set_test [options]");
	ecore_config_string_create("evoak.background", "data/e_bg.jpg", 'b',
				   "bgimage", "Override the saved location for your background image");
        ecore_config_load();

	if (ecore_config_args_parse() != ECORE_CONFIG_PARSE_CONTINUE)
	  {
	     ecore_config_shutdown();
	     evoak_shutdown();
	     exit(0);
	  }
	ecore_config_listen("bg_file", "evoak.background", bg_listener, 0, NULL);
        
	if (argc == 2)
	  {
	     bg_file = argv[1];
	     ecore_config_string_set("evoak.background", argv[1]);
	  }
	else
	  bg_file = ecore_config_string_get("evoak.background");
	
	ecore_event_handler_add(EVOAK_EVENT_CANVAS_INFO, cb_canvas_info, NULL);
	
	ev = evoak_connect(NULL, "evoak_background_client", "custom");
	if (ev)
	  {
	     ecore_main_loop_begin();
	     evoak_disconnect(ev);
	  }
     }
   evoak_shutdown();
   ecore_config_save();
   ecore_config_shutdown();
}



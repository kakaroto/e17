#include "Elation.h"

int  main_start(int argc, char **argv);
void main_stop(void);
void main_resize(Ecore_Evas *ee);
int  main_signal_exit(void *data, int ev_type, void *ev);
void main_delete_request(Ecore_Evas *ee);
void bg_setup(void);
void bg_resize(void);
void media_setup(void);
void disk_setup(void);

static Evas_Object *o_bg = NULL;
static Elation_Module *em_disk = NULL;
static Elation_Module *em_media = NULL;

Ecore_Evas  *ecore_evas = NULL;
Evas        *evas       = NULL;
int          startw     = 800;
int          starth     = 600;

Elation_Info elation_info;

int
main(int argc, char **argv)
{
   /* methods modules can call */
   elation_info.func.action_broadcast = elation_module_action_broadcast;
   elation_info.func.module_open = elation_module_open;
   
   if (main_start(argc, argv) < 1) return -1;
   bg_setup();
   media_setup();
   disk_setup();
   bg_resize();
   elation_module_resize_broadcast();
   ecore_main_loop_begin();
   main_stop();
   return 0;
}

int
main_start(int argc, char **argv)
{
   int mode = 0;
   int fullscreen = 0;
   
   if (!ecore_init()) return -1;
   ecore_app_args_set(argc, (const char **)argv);
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, main_signal_exit, NULL);
   if (!ecore_evas_init()) return -1;
#ifndef FB_ONLY  
     {
        int i;

        for (i = 1; i < argc; i++)
          {
	     if (((!strcmp(argv[i], "-g")) ||
		  (!strcmp(argv[i], "-geometry")) ||
		  (!strcmp(argv[i], "--geometry"))) && (i < (argc - 1)))
	       {
		  int n, w, h;
		  char buf[16], buf2[16];
		  
		  n = sscanf(argv[i +1], "%10[^x]x%10s", buf, buf2);
		  if (n == 2)
		    {
		       w = atoi(buf);
		       h = atoi(buf2);
		       startw = w;
		       starth = h;
		    }
		  i++;
	       }
             else if ((!strcmp(argv[i], "-gl")) ||
		      (!strcmp(argv[i], "-opengl")) ||
		      (!strcmp(argv[i], "--opengl")))
               {
		  mode = 1;
               }
             else if ((!strcmp(argv[i], "-fb")) ||
		      (!strcmp(argv[i], "-framebuffer")) ||
		      (!strcmp(argv[i], "--framebuffer")))
               {
		  mode = 2;
               }
	     else if ((!strcmp(argv[i], "-f")) ||
		      (!strcmp(argv[i], "-fullscreen")) ||
		      (!strcmp(argv[i], "--fullscreen")))
	       {
		  fullscreen = 1;
	       }
          }
     }
   if (mode == 0)
     ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, startw, starth);
   else if (mode == 1)
     ecore_evas = ecore_evas_gl_x11_new(NULL, 0, 0, 0, startw, starth);
   else if (mode == 2)
     ecore_evas = ecore_evas_fb_new(NULL, 0, startw, starth);
#else
   startw = 240;
   starth = 320;
   ecore_evas = ecore_evas_fb_new(NULL, 270,  startw, starth);
#endif
   if (!ecore_evas) return -1;
   ecore_evas_title_set(ecore_evas, "Elation Media Manager");
   ecore_evas_name_class_set(ecore_evas, "elation", "Elation");
   if (fullscreen) ecore_evas_fullscreen_set(ecore_evas, 1);
   ecore_evas_callback_delete_request_set(ecore_evas, main_delete_request);
   ecore_evas_callback_resize_set(ecore_evas, main_resize);
   ecore_evas_cursor_set(ecore_evas, "", 99999, 0, 0);
   ecore_evas_show(ecore_evas);
   evas = ecore_evas_get(ecore_evas);
   evas_image_cache_set(evas, 8 * 1024 * 1024);
   evas_font_cache_set(evas, 1 * 1024 * 1024);
   evas_font_path_append(evas, PACKAGE_DATA_DIR"/data/fonts");
   
   edje_init();
   edje_frametime_set(1.0 / 30.0);
   
   elation_info.evas = evas;
   
   return 1;
}

void
main_stop(void)
{
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
}

void
main_resize(Ecore_Evas *ee)
{
   bg_resize();
   elation_module_resize_broadcast();
}

int
main_signal_exit(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
   return 1;
}

void
main_delete_request(Ecore_Evas *ee)
{
   ecore_main_loop_quit();
}

/*** a background ***/

void
bg_setup(void)
{
   Evas_Object *o;

   o = edje_object_add(evas);
   o_bg = o;
   edje_object_file_set(o, PACKAGE_DATA_DIR"/data/theme.edj", "background");
   evas_object_move(o, 0, 0);
   evas_object_layer_set(o, -999);
   evas_object_show(o);   
   
}

void
bg_resize(void)
{
   Evas_Coord w, h;
   
   evas_output_viewport_get(evas, NULL, NULL, &w, &h);
   evas_object_move(o_bg, 0, 0);
   evas_object_resize(o_bg, w, h);
}

/*** media ***/

void
media_setup(void)
{
   Elation_Module *em;

   em = elation_module_open(&elation_info, NULL, "media");
   em_media = em;
   if (em)
     {
	em->show(em);
	em->focus(em);
     }
}

/*** disk ***/

void
disk_setup(void)
{
   Elation_Module *em;

   em = elation_module_open(&elation_info, NULL, "disk");
   em_disk = em;
   if (em)
     {
	em->show(em);
	em->focus(em);
     }
}

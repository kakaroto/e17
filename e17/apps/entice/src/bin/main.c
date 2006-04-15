/**
 * Filename: main.c
 * Corey Donohoe <atmos@atmos.org>
 * October 10, 2003
 * Description: main.c for entice, an edje based image viewer
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Ipc.h>
#include <Ecore_Job.h>
#include <Esmart/Esmart_Trans_X11.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "entice.h"
#include "ipc.h"
#include "prefs.h"
#include "../config.h"

/** 
 * win_mouse_out_cb - for later
 */
static void
win_mouse_out_cb(Ecore_Evas * ee)
{
   return;
   ee = NULL;
}

/** 
 * win_mouse_in_cb - for later
 */
static void
win_mouse_in_cb(Ecore_Evas * ee)
{
   return;
   ee = NULL;

}

/**
 * win_resize_cb - when our ecore evas gets resized
 * @ee - pointer to our ecore evas
 */
static void
win_resize_cb(Ecore_Evas * ee)
{
   Evas_Object *o = NULL;
   int x, y, w, h;

   if (ee)
   {
      ecore_evas_geometry_get(ee, &x, &y, &w, &h);
      if (!ecore_evas_fullscreen_get(ee))
         entice_config_geometry_set(x, y, w, h);
      entice_resize(w, h);
      if ((o = evas_object_name_find(ecore_evas_get(ee), "trans")))
      {
         evas_object_resize(o, (Evas_Coord) w, (Evas_Coord) h);
         esmart_trans_x11_freshen(o, (Evas_Coord) x, (Evas_Coord) y,
                                  (Evas_Coord) w, (Evas_Coord) h);
      }
   }
}

/**
 * win_move_cb - when our ecore evas gets moved
 * @ee - pointer to our ecore evas
 */
static void
win_move_cb(Ecore_Evas * ee)
{
   int x, y, w, h;
   Evas_Object *o = NULL;

   if (ee)
   {
      ecore_evas_geometry_get(ee, &x, &y, &w, &h);
      entice_config_geometry_set(x, y, w, h);
      if ((o = evas_object_name_find(ecore_evas_get(ee), "trans")))
         esmart_trans_x11_freshen(o, (Evas_Coord) x, (Evas_Coord) y,
                                  (Evas_Coord) w, (Evas_Coord) h);
   }
}

/**
 * win_move_cb - when our ecore evas gets a delete request
 * @ee - pointer to our ecore evas
 */
static void
win_del_cb(Ecore_Evas * ee)
{
   int x, y, w, h;

   if (ee)
   {
      ecore_evas_geometry_get(ee, &x, &y, &w, &h);
      entice_config_geometry_set(x, y, w, h);
   }
   ecore_main_loop_quit();
   return;
   ee = NULL;
}

/**
 * win_post_render_cb - freeze after we render
 */
static void
win_post_render_cb(Ecore_Evas * ee)
{
   edje_freeze();
}

/**
 * win_pre_render_cb - thaw before we render
 */
static void
win_pre_render_cb(Ecore_Evas * ee)
{
   edje_thaw();
}

/**
 * exit_cb - when our ecore event loop gets killed, i.e ctrl-c
 * data - should be NULL
 * type - exit type
 * event - event data
 */
static int
exit_cb(void *data, int type, void *event)
{
   ecore_main_loop_quit();
   return (0);
   data = NULL;
   type = 0;
   event = NULL;
}

static int
entice_pipe_foo(int argc, const char **argv)
{
   int n, i;
   pid_t pid;
   int fd[2];
   char line[PATH_MAX];
   struct stat status;
   Ecore_Ipc_Server *server;

   if (pipe(fd) < 0)
     {
       	perror("pipe");
	exit(1);
     }

   if ((pid = fork()) < 0)
     {
	perror("fork");
	exit(1);
     }

   if (pid > 0)            /* parent */
     {
       	close(fd[0]);
	return fd[1];
     }

   /* child */
   close(fd[1]);
   while ((n = read(fd[0], line, PATH_MAX)) > 0)
      if (!strncmp(line, "ok", n))
         break;
   if ( !(server=entice_ipc_client_init()) )
     {
	fprintf(stderr, "Could not intialise client IPC\n");
	exit(1);
     }
   for (i = 1; i < argc; i++)
     {
      	snprintf(line, PATH_MAX, "%s", argv[i]);
	if (stat(line, &status))
	  continue;
	if (S_ISDIR(status.st_mode))
	  entice_file_add_job_dir(server, line);
	else
	  entice_file_add_job(server, line, IPC_FILE_APPEND);
	/* Flush rather than waiting for all thumbnails before displaying 
	 * anything */
	if ((i & 0xf) == 0x1)
    	  entice_ipc_client_wait(server); 
     }

   entice_ipc_client_wait(server); 
   entice_ipc_client_shutdown(server);
   exit(0);

   /* Not reached */
   close(fd[0]);
   return (0);
}

/**
 * main - does a few things
 * 1. startup ecore, ecore_evas, ecore_ipc, and edje
 * 2. parse our config file
 * 3. Create either a software or gl x11 windows based on the config 
 * 4. Append our ecore_evas callbacks to the ecore_evas
 * 5. Initialize entice with our ecore_evas
 * 6. Fork off thumbnail generation, and show the window
 * 7. for(;;)
 */
int
main(int argc, char **argv)
{
   int pnum = -1;
   int x, y, w, h;
   Ecore_Evas *ee = NULL;

   if (argc > 1)
      pnum = entice_pipe_foo(argc, argv);

   ecore_init();
   ecore_app_args_set(argc, (const char **) argv);
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);

   if (ecore_evas_init())
   {

      edje_init();
      edje_frametime_set(1.0 / 30.0);

      entice_config_init();
      if (!entice_ipc_init(argc, ((const char **) argv)))
      {
         entice_config_geometry_get(&x, &y, &w, &h);
#if HAVE_ECORE_EVAS_GL
         if (entice_config_engine_get() == GL_X11)
            ee = ecore_evas_gl_x11_new(NULL, 0, x, y, w, h);
         else
#endif
            ee = ecore_evas_software_x11_new(NULL, 0, x, y, w, h);

         if (ee)
         {
            ecore_evas_callback_mouse_out_set(ee, win_mouse_out_cb);
            ecore_evas_callback_mouse_in_set(ee, win_mouse_in_cb);
            ecore_evas_callback_resize_set(ee, win_resize_cb);
            ecore_evas_callback_move_set(ee, win_move_cb);
            ecore_evas_callback_delete_request_set(ee, win_del_cb);
            ecore_evas_callback_post_render_set(ee, win_post_render_cb);
            ecore_evas_callback_pre_render_set(ee, win_pre_render_cb);

            ecore_evas_name_class_set(ee, "Entice", "Entice");
            ecore_evas_title_set(ee, "Entice !!!!");

            evas_font_cache_set(ecore_evas_get(ee),
                                entice_config_font_cache_get() * 1024 * 1024);
            evas_image_cache_set(ecore_evas_get(ee),
                                 entice_config_image_cache_get() * 1024 *
                                 1024);

            evas_font_path_append(ecore_evas_get(ee),
                                  PACKAGE_DATA_DIR "/fonts");

            if (entice_init(ee))
            {
               ecore_evas_move_resize(ee, x, y, w, h);
               ecore_evas_show(ee);
               if (pnum >= 0)
               {
                  write(pnum, "ok", 2);
                  close(pnum);
                  pnum = -1;
               }
               ecore_main_loop_begin();
            }
            entice_free();
         }
      }
      ecore_evas_shutdown();
   }
   if (pnum >= 0)
      close(pnum);
   ecore_shutdown();
   return (0);
}

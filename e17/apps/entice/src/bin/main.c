#include "entice.h"
#include "entice_ipc.h"

/* globals */
Evas_Object        *o_bg;
Evas_Object        *o_logo;
Evas_Object        *o_panel;
Evas_Object        *o_showpanel;
Evas_Object        *o_showbuttons;
Evas_Object        *o_arrow_l;
Evas_Object        *o_arrow_r;
Evas_Object        *o_arrow_t;
Evas_Object        *o_arrow_b;
Evas_Object        *o_image = NULL;
Evas_Object        *o_mini_image = NULL;
Evas_Object        *o_mini_select;
Evas_Object        *o_list_select;
Evas_Object        *o_panel_arrow_u;
Evas_Object        *o_panel_arrow_d;
Evas_Object        *o_bt_delete;
Evas_Object        *o_bt_close;
Evas_Object        *o_bt_expand;
Evas_Object        *o_bt_full;
Evas_Object        *o_bt_next;
Evas_Object        *o_bt_prev;
Evas_Object        *o_bt_zoom_in;
Evas_Object        *o_bt_zoom_normal;
Evas_Object        *o_bt_zoom_out;
Evas_Object        *o_txt_info[10];
Evas_Object        *o_txt_tip[5];
Evas_Object        *icon_drag;
Evas_Object        *o_trash = NULL;
Evas_Object        *o_trash_can = NULL;

char                txt_info[2][4096];

Evas_List          *images = NULL;
Evas_List          *current_image = NULL;
Image              *generating_image = NULL;

Evas               *evas;
Ecore_Evas	   *ecore_evas;
int                 render_method = 0;
int                 max_colors = MAX_EVAS_COLORS;
int                 smoothness = 1;
int                 win_w = W, win_h = H;
int                 win_x = 0, win_y = 0;

int                 icon_x = 0;
int                 icon_y = 0;

int                 dragging = 0;
enum active_state   panel_active = active_out;
enum active_state   buttons_active = active_out;
int                 scroll_x = 0, scroll_y = 0;
int                 arrow_l = 0, arrow_r = 0, arrow_t = 0, arrow_b = 0;
int                 down_x = 0, down_y = 0;
int                 down_sx = 0, down_sy = 0;
double              scale = 1.0;
double              focus_pos = 0.0;
int                 need_thumbs = 1;

int                 dnd_num_files;
char              **dnd_files = NULL;

/*****************************************************************************/

static int main_signal_exit(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
}

int
main(int argc, char **argv)
{
  enum active_state command;
   /* environment parsing */
   render_method = evas_render_method_lookup("software_x11");
   if (getenv("ENTICE_RENDERER"))
     {
	char               *entice_renderer;

	entice_renderer = getenv("ENTICE_RENDERER");
	if (!strcmp(entice_renderer, "soft"))
	   render_method = evas_render_method_lookup("software_x11");
	else if (!strcmp(entice_renderer, "x11"))
	   render_method = evas_render_method_lookup("software_x11");
     }
   if (getenv("ENTICE_COLORS"))
     {
	char               *entice_colors;

	entice_colors = getenv("EVS_COLORS");
	max_colors = atoi(entice_colors);
     }
   if (getenv("ENTICE_SMOOTH"))
     {
	char               *entice_smooth;

	entice_smooth = getenv("EVS_SMOOTH");
	smoothness = atoi(entice_smooth);
     }
   /* image list parsing */
   if (argc == 1)
     {
        /* to be honest, I don't like this behaviour
	char               *homedir;

	homedir = getenv("HOME");

	printf("%s\n", homedir);

	if (homedir == NULL)
	  {
	     fprintf(stderr, "Please set you HOME environment!\n");
	     return EXIT_FAILURE;
	  }

	image_create_list_dir(homedir);
	*/
      char* dir;
      /* portability nightmare: this goes in configure. If you don't
       * like it like this, put it there yourself and send me a patch.
       */
#ifdef _GNU_SOURCE
      dir = get_current_dir_name();
#else
      dir = getcwd(NULL, 0);
#endif
      if (!dir) {
	fprintf(stderr, "No file name passed on command line and unable to get the current directory. Exiting.");
	return EXIT_FAILURE;
      }
      image_create_list_dir(dir);
      
     }
   else
      image_create_list(argc, argv);
   /* initialise Ecore */
   if (!ecore_init()) {
     printf("Maximal evil: unable to init Ecore!\n");
     return -1;
   }
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, main_signal_exit, NULL);
   
   if (!entice_ipc_init(argc, (const char **) argv)) {
     /* init X */
     if (!ecore_evas_init()) {
	if (getenv("DISPLAY"))
	  {
	     printf("Cannot initialize default display:\n");
	     printf("DISPLAY=%s\n", getenv("DISPLAY"));
	  }
	else
	  {
	     printf("No DISPLAY variable set!\n");
	  }
	printf("Exit.\n");
	exit(-1);
     }

    /* program does its data setup here */
    setup();

    /* setup thumbnails */
    image_create_thumbnails();
    icon_x = -100;
    e_fix_icons();
    /* call the animator once to start it up */
    command = active_in;
    e_fade_logo(&command);
    e_fade_scroller_in((void *)1);
    /* and now loop forever handling events */
    ecore_main_loop_begin();
    ecore_evas_shutdown();
    entice_ipc_shutdown();
   }
   ecore_shutdown();
   return(0);
}

#include "entice.h"

/* globals */
Evas_Object        *o_bg;
Evas_Object        *o_logo;
Evas_Object        *o_panel;
Evas_Object        *o_showpanel;
Evas_Object        *o_hidepanel;
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
int                 render_method = 0;
int                 max_colors = MAX_EVAS_COLORS;
int                 smoothness = 1;
int                 win_w = W, win_h = H;
int                 win_x = 0, win_y = 0;
Window              main_win;
Window              ewin;

int                 icon_x = 0;
int                 icon_y = 0;

int                 dragging = 0;
int                 panel_active = 0;
int                 buttons_active = 0;
int                 scroll_x = 0, scroll_y = 0;
int                 scroll_sx = 0, scroll_sy = 0;
int                 arrow_l = 0, arrow_r = 0, arrow_t = 0, arrow_b = 0;
int                 down_x = 0, down_y = 0;
int                 down_sx = 0, down_sy = 0;
double              scale = 1.0;
double              focus_pos = 0.0;
int                 need_thumbs = 1;

int                 dnd_num_files;
char              **dnd_files = NULL;

/*****************************************************************************/

Window
find_current_rep(Window win, Atom atom)
{
   Window             *wlist;
   int                 i, n;

   wlist = ecore_window_get_children(win, &n);
   if (wlist)
     {
	for (i = 0; i < n; i++)
	  {
	     void               *data;
	     int                 size;
	     Window              w;

	     w = wlist[i];
	     data = ecore_window_property_get(w, atom, XA_STRING, &size);
	     if (data)
	       {
		  free(data);
		  free(wlist);
		  return w;
	       }
	     else
	       {
		  Window              ww;

		  ww = find_current_rep(w, atom);
		  if (ww)
		    {
		       free(wlist);
		       return ww;
		    }
	       }
	  }
	free(wlist);
     }
   return 0;
}

void
find_current(void)
{
   Atom                a_entice;
   Atom                a_entice_newfiles;
   Window              win;
   Evas_List          *l;
   int                 size;
   char               *files;

   if (!images)
      return;
   a_entice = ecore_atom_get("_ENTICE_APP_WINDOW");
   win = find_current_rep(0, a_entice);
   if (!win)
      return;
   a_entice_newfiles = ecore_atom_get("_ENTICE_NEWFILES");
   size = 0;
   for (l = images; l; l = l->next)
     {
	Image              *im;

	im = l->data;
	size += strlen(im->file) + 1;
     }
   files = malloc(size);
   files[0] = 0;
   for (l = images; l; l = l->next)
     {
	Image              *im;

	im = l->data;
	strcat(files, im->file);
	strcat(files, "\n");
     }
   ecore_window_property_set(win, a_entice_newfiles, XA_STRING, 8, files, size);
   ecore_sync();
   ecore_sync();
   exit(0);
}

/*****************************************************************************/

int
main(int argc, char **argv)
{
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
   if (!(argc - 1))
     {
	char               *homedir;

	/* CS */
	printf("CS\n");

	homedir = getenv("HOME");
	/* homedir="/home/omarhawk/tmp/pics"; */

	/* CS */
	printf("%s\n", homedir);

	if (homedir == NULL)
	  {
	     fprintf(stderr, "Please set you HOME environment!\n");
	     return EXIT_FAILURE;
	  }

	image_create_list_dir(homedir);
     }
   else
      image_create_list(argc, argv);
   /* init X */
   if (!ecore_display_init(NULL))
     {
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
   /* setup handlers for system signals */
   ecore_event_signal_init();
   /* setup the event filter */
   ecore_event_filter_init();
   /* setup the X event internals */
   ecore_event_x_init();

   /* find if another entice is running... if it is .. message it. */
   find_current();

   /* program does its data setup here */
   setup();

   /* setup thumbnails */
   image_create_thumbnails();
   icon_x = -100;
   e_fix_icons();
   /* call the animator once to start it up */
   e_fade_logo_in(0, NULL);
   e_fade_scroller_in(0, (void *)1);
   /* and now loop forever handling events */
   ecore_event_loop();
}

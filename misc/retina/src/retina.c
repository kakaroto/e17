/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"

/* Global Evas Variables */
Evas e_area;
Evas_Object e_img;
Evas_Object e_checks;
Evas_Object e_btn1, e_btn2, e_btn3, e_btn4, e_btn5, e_bs;
Evas_Object e_br_bg, e_scr_t;

/* Global GTK+ Variables */
GtkWidget *window, *area;
GtkWidget *filesel;
GtkWidget *bwin, *list;
GdkVisual *gdk_vis;
GdkColormap *gdk_cmap;
Visual *vis;
Colormap cmap;
guint current_idle = 0;

/* Other Globals */
char window_title[255] = "Retina - Nothing Loaded...";
char image_title[255];
int tb_status = 0, br_status = 0;
int mouse_button;

int
main(int argc, char **argv)
{
   /* GTK+ Widgets */
   
   /* Get things started */
   gtk_init(&argc, &argv);
   r_evas_create();
   r_gtk_init();
   r_evas_init();
   
   /* Check for a command line arg, do things appropriately */
   if(argc > 1){
      sprintf(window_title, "Retina - %s", argv[1]);
      gtk_window_set_title(GTK_WINDOW(window), window_title);
      /* Load the splash screen */
      r_evas_load(argv[1]);
   } else {
      r_evas_load("../img/retina.png");
   }
   
   /* Other initializers */
   r_evas_toolbar_init();
	 r_browser_init();
   
   /* Evas Callback attachments */
   r_cb_init();
   
   /* Show window, loop, return */
   gtk_widget_show(window);
   gtk_main();
   evas_free(e_area);
   return 0;
}

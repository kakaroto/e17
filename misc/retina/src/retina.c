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
Evas_List files = NULL;
Evas_List images = NULL;
Evas_Object e_img;
Evas_Object e_checks;
Evas_Object e_btn1, e_btn2, e_btn3, e_btn4, e_btn5, e_bs;
Evas_Object e_br_bg, e_scr_t, e_scr_b1, e_scr_b2, e_scr_s;
Evas_Object e_t_img[500], e_t_txt[500], e_t_txt2[500], e_t_shd;

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
E_DB_File *db;
char window_title[255] = "Retina - Nothing Loaded...";
char image_title[255];
char *r_images[500];
int tb_status = 0, br_status = 0;
int mouse_button;
int t_y = 40, b_y = 37;
int img_c = 0, cur_sel;
int list_end = 0;
pid_t scanner_pid = 0;

int
main(int argc, char **argv)
{
	int i;
	
   /* Get things started */
   gtk_init(&argc, &argv);
	 r_db_init();
   r_evas_create();
   r_gtk_init();
   r_evas_init();
   
   /* Check for a command line arg, do things appropriately */
   if(argc > 1){
		 for(i = 1; i < argc; i++){
			 r_gen_thumb(argv[i]);
			 r_draw_thumb(argv[i]);
			 r_images[i -1] = strdup(argv[i]);
			 img_c++;
		 }
		 evas_set_color(e_area, e_t_txt[cur_sel], 255, 200, 0, 255);
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

time_t
e_file_modified_time(char *file)
{
	struct stat st;
	if(stat(file, &st) < 0)
		return 0;
	return st.st_mtime;
}

/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"

GtkWidget *filesel;
extern int img_c;

void
r_filesel_hide()
{
	gtk_widget_hide(filesel);
}

void
r_filesel_openfile(GtkWidget *widget, GtkFileSelection *fs)
{
	char *imagefile = NULL;
	
	imagefile = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
	gtk_widget_hide(filesel);
	
	r_gen_thumb(imagefile);
	r_draw_thumb(imagefile);
	printf("fs: %d\n", img_c);
	r_evas_load(imagefile);
}

void
r_file_load()
{
	gtk_widget_show(filesel);
}

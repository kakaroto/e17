#include <gdk_imlib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;

GtkWidget *
create_main_window(void)
{
	GtkWidget *win;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(win),"menu_editor",win);
	GTK_WIDGET_SET_FLAGS(win,GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(win,GTK_CAN_DEFAULT);
	gtk_window_set_title(GTK_WINDOW(win),"E Menu Editor");

	return win;
}

void
on_exit_application(GtkWidget * widget, gpointer user_data)
{

	if (user_data) {
		widget = NULL;
	}
	gtk_exit(0);

}


int
main(int argc, char *argv[])
{
	GtkWidget *main_win;

	gtk_set_locale();
	gtk_init(&argc,&argv);
	gdk_imlib_init();

	tooltips = gtk_tooltips_new();
	accel_group = gtk_accel_group_new();

	gtk_widget_push_visual(gdk_imlib_get_visual());
	gtk_widget_push_colormap(gdk_imlib_get_colormap());

	main_win = create_main_window();
	gtk_widget_show(main_win);
	gtk_signal_connect(GTK_OBJECT(main_win), "destroy",
			GTK_SIGNAL_FUNC(on_exit_application), NULL);
	gtk_signal_connect(GTK_OBJECT(main_win), "delete_event",
			GTK_SIGNAL_FUNC(on_exit_application), NULL);

	gtk_main();

	return 0;
}

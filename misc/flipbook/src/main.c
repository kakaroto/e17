
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;

int main(int argc, char *argv[])
{
	GtkWidget *VA_Flipbook;

	gtk_set_locale();
	gtk_init(&argc, &argv);

	tooltips = gtk_tooltips_new();
	accel_group = gtk_accel_group_new();

	add_pixmap_directory(PACKAGE_DATA_DIR "/pixmaps");
	add_pixmap_directory(PACKAGE_SOURCE_DIR "/pixmaps");

	VA_Flipbook = create_VA_Flipbook();
	gtk_widget_show(VA_Flipbook);
	gtk_signal_connect (GTK_OBJECT (VA_Flipbook), "destroy",
			GTK_SIGNAL_FUNC (on_exit_application), NULL);
	gtk_signal_connect (GTK_OBJECT (VA_Flipbook), "delete_event",
			GTK_SIGNAL_FUNC (on_exit_application), NULL);


	gtk_main();
	return 0;
}

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "viewer.h"

extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;

int main(int argc, char *argv[])
{
	GtkWidget *lister;

	gtk_set_locale();
	gtk_init(&argc, &argv);


}

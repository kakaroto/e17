/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 1999 Brad Grantham, Geoff Harrison, and VA Linux Systems
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "callbacks.h"
#include "interface.h"
#include "init.h"
#include "support.h"

G_LOCK_DEFINE_STATIC (number_loaded);
static volatile int number_loaded = 0;
G_LOCK_DEFINE_STATIC (num_threads);
static volatile int num_threads = 0;

int total_frames;

extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;

typedef struct {
	GtkWidget *widget;
	int what;
} thread_args;

void *loader_thread(void *args);

void *
loader_thread(void *args)
{
	thread_args *data = (thread_args *) args;
	int load_index = 0;
	int thread_num = 0;

	data = NULL;

	G_LOCK(num_threads);
	thread_num = ++num_threads;
	G_UNLOCK(num_threads);

	while(load_index < total_frames) {
		G_LOCK(number_loaded);
		load_index = number_loaded;
		number_loaded++;
		G_UNLOCK(number_loaded);
		if(load_index < total_frames) {
			printf("thread %d: loading number %d\n",thread_num,load_index);
		}
		sleep(0);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	GtkWidget *VA_Flipbook;
	pthread_t one_tid, two_tid;

	g_thread_init(NULL);
	gtk_set_locale();
	gtk_init(&argc, &argv);

	initialize_player();

	tooltips = gtk_tooltips_new();
	accel_group = gtk_accel_group_new();

	total_frames = 10;

	add_pixmap_directory(PACKAGE_DATA_DIR "/pixmaps");
	add_pixmap_directory(PACKAGE_SOURCE_DIR "/pixmaps");

	VA_Flipbook = create_VA_Flipbook();
	gtk_widget_show(VA_Flipbook);
	gtk_signal_connect (GTK_OBJECT (VA_Flipbook), "destroy",
			GTK_SIGNAL_FUNC (on_exit_application), NULL);
	gtk_signal_connect (GTK_OBJECT (VA_Flipbook), "delete_event",
			GTK_SIGNAL_FUNC (on_exit_application), NULL);

	pthread_create(&one_tid, NULL, loader_thread, NULL);
	pthread_create(&two_tid, NULL, loader_thread, NULL);

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();
	return 0;
}

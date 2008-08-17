/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 2000 Brad Grantham, Geoff Harrison, and VA Linux Systems
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

#include <gdk/gdkprivate.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libart_lgpl/libart.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libart_lgpl/art_pixbuf.h>
#include <sys/time.h>

#include "callbacks.h"
#include "interface.h"
#include "init.h"
#include "hooks.h"
#include "support.h"
#include "loadfiles.h"
#include "textload.h"
#include "loader.h"
#include "controls.h"
#include "idle.h"

G_LOCK_DEFINE_STATIC (number_loaded);
static volatile int number_loaded = 0;
G_LOCK_DEFINE_STATIC (num_threads);
static volatile int num_threads = 0;


/* here's the meat of the data segment */

G_LOCK_DEFINE_STATIC (primary_lists);
static volatile int primary_lists = 0;

char **file_list = NULL;
unsigned char **data_set = NULL;
GdkPixbuf **pixmap_set = NULL;

/* *********************************** */


extern GtkWidget *progress_bar;

extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;

typedef struct {
	GtkWidget *widget;
	int what;
} thread_args;

G_LOCK_DEFINE_STATIC (current_index);
static volatile int current_index;

G_LOCK_DEFINE_STATIC (playable);
static volatile int playable;

G_LOCK_DEFINE_STATIC (direction);
static volatile int direction;

void *loader_thread(void *args);
void *monitor_thread(void *args);
void *regulator_thread(void *args);

void *
regulator_thread(void *args) {

	thread_args *data = (thread_args *) args;
	long set_time;
	long timeslots[31];
	int frame_base;
	int lastindex=0;
	int i;

	struct timeval  thisplay;
	struct timezone tz;

	int last_frame;
   
	last_frame= get_total_frames();

	frame_base = get_framerate_base();
	set_time = 1000000 / (long)(frame_base);

	timeslots[0] = set_time;
	for(i=1;i<frame_base;i++) {
		timeslots[i] = timeslots[i-1] + set_time;
	}
	timeslots[frame_base-1] = 1000000;

	/*
	for(i=0;i<frame_base;i++) {
		printf("timeslot[%d]: %ld\n",i,timeslots[i]);
	}
	*/
	
	gettimeofday(&thisplay,&tz);
	data = NULL;
	for(i=0;i<frame_base;i++) {
		if(thisplay.tv_usec <  timeslots[i]) {
			lastindex=i;
			i=frame_base;
		}
	}

	while(1) {
		int tmp;
		int index_advance;

		usleep(1000);
		gettimeofday(&thisplay,&tz);
		if(get_total_frames() == 0) {
			printf("regulator exiting\n");
			pthread_exit(NULL);
		}
		G_LOCK(playable);
		tmp = playable;
		G_UNLOCK(playable);
		if(tmp) {
			index_advance = 0;
			for(i=0;i<frame_base;i++) {
				if(thisplay.tv_usec <  timeslots[i]) {
					if(lastindex != i) {
						index_advance = i - lastindex;
						if(index_advance <= 0) {
							index_advance += ++lastindex;
						}
						lastindex = i;
					}
					i=frame_base;
				}
			}
			if(index_advance > 0) {
				G_LOCK(direction);
				tmp = direction;
				G_UNLOCK(direction);
				G_LOCK(current_index);
				current_index += index_advance;
				if(current_index >= last_frame) {
					current_index -= last_frame;
				} else if(current_index < 0) {
					current_index += last_frame;
				}
				G_UNLOCK(current_index);
			}
		} else {
			for(i=0;i<frame_base;i++) {
				if(thisplay.tv_usec <  timeslots[i]) {
					lastindex=i;
					i=frame_base;
				}
			}
		}

	}

	pthread_exit(NULL);

}

void *
monitor_thread(void *args) {
	thread_args *data = (thread_args *) args;

	int total_frames;
	int byteframe;
	int pixelframe;

	data = NULL;
	total_frames = get_total_frames();

	byteframe = get_width() * get_height() * DefaultDepth((Display *) gdk_display,DefaultScreen((Display *) gdk_display)) / 8;

	pixelframe = get_width() * get_height();
	
	
	while(1) {
		int last_index;
		int cur_dex;

		G_LOCK(current_index);
		last_index = current_index+1;
		G_UNLOCK(current_index);
		sleep(1);
		if(get_total_frames() == 0) {
			printf("monitor exiting\n");
			pthread_exit(NULL);
		}

		G_LOCK(current_index);
		cur_dex = current_index+1;
		G_UNLOCK(current_index);

		if(last_index > 0) {
			int framerate;
			int tmp;
			int length;
			int oldlength;
			char s[255];

			G_LOCK(playable);
			tmp = playable;
			G_UNLOCK(playable);
			if(tmp) {
				double mbsec,mpsec;

				if(cur_dex >= last_index) {
					framerate = cur_dex - last_index;
				} else {
					framerate = cur_dex + total_frames - last_index;
				}
				
				mbsec = (((framerate * byteframe) / 1024.0) / 1024.0);
				mpsec = (((framerate * pixelframe) / 1024.0) / 1024.0);
				sprintf(s,"\n Framerate: %d frames/second\n"
						" Drawrate : %3.2f MB/sec (%3.2f Mpixel/sec)\n"
						" Missed   : %d frames of %d (%d %%"
						")\n", framerate, mbsec, mpsec,
						0,total_frames,100);
				length = strlen(s);
				gdk_threads_enter();
				oldlength = gtk_text_get_length(GTK_TEXT(status_text));
				gtk_text_set_point(GTK_TEXT(status_text),oldlength);
				gtk_text_backward_delete(GTK_TEXT(status_text),oldlength);
				gtk_text_insert(GTK_TEXT(status_text),NULL,NULL,NULL,s,length);
				gdk_threads_leave();
			}
		}

		last_index = cur_dex;

	}

	pthread_exit(NULL);
}

void *
loader_thread(void *args)
{
	thread_args *data = (thread_args *) args;
	int load_index = 0;
	int thread_num = 0;
	int w,h;
	int framerate = 0;

	data = NULL;

	G_LOCK(num_threads);
	thread_num = ++num_threads;
	G_UNLOCK(num_threads);

	w = get_width();
	h = get_height();
	framerate = get_framerate_base();

	while(load_index < get_total_frames()) {
		int total_frames;
		char *filename;
		/* char *data; */

		G_LOCK(number_loaded);
		number_loaded++;
		load_index = number_loaded;
		G_UNLOCK(number_loaded);
		if(load_index <= (total_frames = get_total_frames())) {
			float percentage = 0;
			if(get_total_frames() > 0) {
				int perc_index = 0;

				GdkPixbuf *mypix;

				G_LOCK(primary_lists);
				primary_lists = 1;
				filename = malloc(strlen(file_list[load_index-1]) + 1);
				strcpy(filename,file_list[load_index-1]);
				primary_lists = 0;
				G_UNLOCK(primary_lists);
				mypix = gdk_pixbuf_new_from_file(filename);
				pixmap_set[load_index-1] = mypix;
				G_LOCK(number_loaded);
				perc_index = number_loaded;
				G_UNLOCK(number_loaded);
				percentage = ((perc_index-1) * (10000 / total_frames)) * 0.0001;
				if(perc_index-1 == total_frames) {
					percentage = 1.0;
				}
				gdk_threads_enter();
				gtk_progress_set_percentage(GTK_PROGRESS(progress_bar),
						(percentage < 1) ? percentage : 1.0);
				{
					char s[100];
					int remain = total_frames - perc_index + 1;
					int done = perc_index - 1;

					sprintf(s,"%02d:%02d:%02d:%02d remaining\n"
							"%02d:%02d:%02d:%02d done",
							remain/(framerate * 60 * 60),
							(remain/(framerate * 60)) % 60,
							(remain/framerate) % 60,
							remain % framerate,
							done / (framerate * 60 * 60),
							(done / (framerate * 60)) % 60,
							(done / framerate) % 60,
							done % framerate);
					gtk_label_set_text(GTK_LABEL(percentdone),s);
				}
				gdk_threads_leave();
			} else {
				load_index = total_frames;
			}
		}
	}

	pthread_exit(NULL);
}

GdkPixbuf *get_next_pic(int index) {

	GdkPixbuf *tmp;

	if(index < 0) {
		int tmp;
		G_LOCK(playable);
		tmp = playable;
		G_UNLOCK(playable);
		if(tmp) {
			gtk_widget_hide(pause_pixmap);
			G_LOCK(playable);
			playable = 0;
			G_UNLOCK(playable);
			return NULL;
		} else {
			gtk_widget_hide(play_pixmap);
			pause_pixmap = create_pixmap(play_pixmap, "pause.xpm");
			gtk_container_remove(GTK_CONTAINER(play_button),play_pixmap);
			gtk_widget_show(pause_pixmap);
			gtk_container_add(GTK_CONTAINER(play_button),pause_pixmap);
			G_LOCK(playable);
			playable = 1;
			G_UNLOCK(playable);
			if(fastasicanplay) {
				gtk_idle_add(play_movie,NULL);
			} else {
				gtk_timeout_add(30,play_movie,NULL);
			}
			return NULL;
		}
	}


	if(index > 0) {
		G_LOCK(current_index);
		current_index = index-1;
		G_UNLOCK(current_index);
	} else {
		int tmp;
		G_LOCK(playable);
		tmp = playable;
		G_UNLOCK(playable);
		if(!tmp) {
			return NULL;
		}
	}

	G_LOCK(primary_lists);

	if(pixmap_set) {
		G_LOCK(current_index);
		tmp = pixmap_set[current_index];
		G_UNLOCK(current_index);
	} else {
		tmp = NULL;
	}

	G_UNLOCK(primary_lists);

	return tmp;

}

void launch_monitor(void) {

	pthread_t one_tid,two_tid;

	pthread_create(&one_tid, NULL, monitor_thread, NULL);
	pthread_create(&two_tid, NULL, regulator_thread, NULL);

	return;

}

void load_files(void) {

	pthread_t one_tid, two_tid;

	G_LOCK(number_loaded);
	number_loaded = 0;
	G_UNLOCK(number_loaded);

	if(file_list) {
		if(file_list[0]) {
			get_jpeg_stats(file_list[0]);
			pthread_create(&one_tid, NULL, loader_thread, NULL);
			pthread_create(&two_tid, NULL, loader_thread, NULL);
		}
	}

	return;
}

void load_my_list_of_files(char *file_with_names) {

	char s[1024];
	char *word;
	FILE *names;
	int i=0;

	G_LOCK(primary_lists);
	primary_lists = 1;
	G_LOCK(playable);
	playable = 1;
	G_UNLOCK(playable);

	names = fopen(file_with_names,"r");
	if(file_list)
		free(file_list);
	file_list = malloc(sizeof(char *) + 1);

	while(GetLine(s,sizeof(s),names)) {
		if(IsWhitespace(s))
			continue;
		word = malloc(strlen(s) + 1);
		strcpy(word,s);
		file_list[i] = word;
		i++;
		file_list = realloc(file_list,(sizeof(char *) * i) + 1);
	}

	pixmap_set = malloc((sizeof(GdkPixbuf *) * i) + 1);
	memset(pixmap_set,0,sizeof(GdkPixbuf *) * i);
	set_total_frames(i);
	primary_lists = 0;
	G_UNLOCK(primary_lists);
	set_movie_name(file_with_names);

	return;

}

int get_current_index(void) {

	int temp;

	G_LOCK(current_index);
	temp = current_index;
	G_UNLOCK(current_index);

	return temp;

}

void unload_movie(void) {

	int i,j;

	G_LOCK(primary_lists);
	primary_lists = 1;
	j = get_total_frames();
	if(file_list) {
		for(i=0;i<j;i++) {
			if(file_list[i]) {
				free(file_list[i]);
			}
			if(pixmap_set[i]) {
				gdk_pixbuf_unref(pixmap_set[i]);
			}
		}
		free(file_list);
		file_list = NULL;
		free(pixmap_set);
		pixmap_set = NULL;
	}
	primary_lists = 1;
	G_UNLOCK(primary_lists);

	G_LOCK(number_loaded);
	number_loaded = 0;
	G_UNLOCK(number_loaded);
	set_total_frames(0);
	gtk_progress_set_percentage(GTK_PROGRESS(progress_bar), 0);
	gtk_label_set_text(GTK_LABEL(percentdone),"00:00:00:00 remaining\n"
			"00:00:00:00 done");


}

int main(int argc, char *argv[])
{

	GtkWidget *VA_Flipbook;

	g_thread_init(NULL);
	gtk_set_locale();
	gtk_init(&argc, &argv);

	gdk_rgb_set_verbose(FALSE);

	gdk_rgb_init();

	gtk_widget_set_default_colormap(gdk_rgb_get_cmap());
	gtk_widget_set_default_visual(gdk_rgb_get_visual());

	initialize_player();

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

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	return 0;

}

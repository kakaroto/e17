/*
   Module       : main.c
   Purpose      : GDK/Imlib Quick Image Viewer 
   More         : see eplay README
   Homepage     : http://mandrake.net
   Policy       : GNU GPL
 */

#include <gdk/gdkx.h>
#include <gtk/gtkwidget.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include "eplay.h"
#include "main.h"

int main(int argc, char **argv)
{
	struct timeval tv;			/* Current time */
	GdkColormap *cmap;
	GdkImlibInitParams params;
	params.flags = PARAMS_DITHER;

	gdk_init(&argc, &argv);		/* Initialize GDK */
	gdk_imlib_init_params(&params);		/* Initialize Imlib */

	mod.brightness = mod.contrast = mod.gamma = 256;
	gettimeofday(&tv, NULL);	/* Randomize seed for 'true' random */
	srandom(tv.tv_usec * 1000000 + tv.tv_sec);

	image_idx = options_read(argc, argv);	/* Index of first image */
	images = argc - image_idx;	/* Number of images */
	image_names = &argv[image_idx];

	if (filter)					/* Filter graphic images */
		filter_images(&images, image_names);

	if (!images) {				/* No images to display */
		g_print("\neplay: cannot load any images.\n");
		usage(argv[0], 1);
	}
	image_idx = 0;				/* Display first image first */

	if (to_root == 1 || to_root_t == 1 || to_root_s == 1) {
		params.flags |= PARAMS_VISUALID;
		(GdkVisual *) params.visualid = gdk_window_get_visual(GDK_ROOT_PARENT());
	}
	gdk_imlib_init();
	signal(SIGTERM, finish);
	signal(SIGINT, finish);
	gtk_widget_push_visual(gdk_imlib_get_visual());
	gtk_widget_push_colormap(gdk_imlib_get_colormap());

	cmap = gdk_colormap_get_system();

	if (!gdk_color_alloc(cmap, &text_background_color))
		fprintf(stderr, "eplay: couldn't allocate color (text_background_color)\n");

	if (!gdk_color_alloc(cmap, &color_blue)) {
		fprintf(stderr, "eplay: couldn't allocate color (color_blue),\nusing black...\n");
	}
	color_bg.red = 0;
	color_bg.green = 0;
	color_bg.blue = 0;
	if (!gdk_color_alloc(cmap, &color_bg)) {
		fprintf(stderr, "eplay: couldn't allocate color (color_bg)");
	}
	screen_x = gdk_screen_width();
	screen_y = gdk_screen_height();

	slide = 1;

	p = malloc((images + 1) * sizeof(GdkPixmap *));
	{
		int i=0;
		for(i=0;i<images+1;i++)
			p[i] = NULL;
	}

	eplay_load_image();			/* Load & display the first image */

	/* Setup callbacks */
	gdk_event_handler_set((GdkEventFunc) eplay_handle_event, NULL, NULL);
	g_timeout_add_full(G_PRIORITY_LOW, delay, (GSourceFunc) eplay_handle_timer, &slide, NULL);

	MainLoop = g_main_new(TRUE);	/* Allocate eplay main loop */
	g_main_run(MainLoop);		/* Run the loop */

	/*  main loop will never return */

	return (0);
}

void eplay_exit(int code)
{
	g_main_destroy(MainLoop);
	finish(SIGTERM);			/* deprecated, subject to change */
};


/*
 *    Slideshow
 */

void eplay_handle_timer(gpointer data)
{
	if (*(char *) data || slide) {
		image_idx = (image_idx + 1 + images) % images;
		eplay_load_image();
	}
}


/* 
 *    Handle GDK events 
 */

void eplay_handle_event(GdkEvent * ev)
{
	gboolean exit_slideshow = FALSE;
	switch (ev->type) {
		case GDK_DELETE:
			eplay_exit(0);
			break;

		case GDK_BUTTON_RELEASE:
			eplay_exit(0);
			break;

			/* Use release instead of press (Fixes bug with junk being sent
			   to underlying xterm window on exit) */
		case GDK_KEY_PRESS:
			eplay_exit(0);
			break;
		default:
			break;
	}
	if (exit_slideshow)
		slide = 0;
}

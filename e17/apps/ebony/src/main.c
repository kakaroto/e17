#include <gdk/gdkx.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "util.h"
#include "interface.h"
#include "callbacks.h"
#include "advanced.h"

void
setup_evas(Display *disp, Window win, Visual *vis, Colormap cm, int w, int h)
{
    Evas_Object o;
    int colors[] = { 255, 255, 255, 255 };

    evas = evas_new();
    evas_set_output_method(evas, RENDER_METHOD_ALPHA_SOFTWARE);
    evas_set_output(evas, disp, win, vis, cm);
    evas_set_output_size(evas, w, h);
    evas_set_output_viewport(evas, 0, 0, w, h);
    evas_set_font_cache(evas, ((1024 * 1024) * 1));
    evas_set_image_cache(evas, ((1024 * 1024) * 4)); 
    
    o = evas_add_rectangle(evas);
    evas_move(evas, o, 0, 0);
    evas_resize(evas, o, 999999, 999999);
    evas_set_color(evas, o, 255,255,255,255);
    evas_set_layer(evas, o, -100);
    evas_show(evas, o);

    o = evas_add_line(evas);
    evas_object_set_name(evas, o, "top_line");
    evas_set_color(evas, o, colors[0], colors[1], colors[2], colors[3]);
    evas_show(evas, o);

    o = evas_add_line(evas);
    evas_object_set_name(evas, o, "bottom_line");
    evas_set_color(evas, o, colors[0], colors[1], colors[2], colors[3]);
    evas_show(evas, o);
    
    o = evas_add_line(evas);
    evas_object_set_name(evas, o, "left_line");
    evas_set_color(evas, o, colors[0], colors[1], colors[2], colors[3]);
    evas_show(evas, o);
    
    o = evas_add_line(evas);
    evas_object_set_name(evas, o, "right_line");
    evas_set_color(evas, o, colors[0], colors[1], colors[2], colors[3]);
    evas_show(evas, o);
}

int
main(int argc, char *argv[])
{
    GtkWidget *win, *w;
    int rx, ry, rw, rh, rd;
    int ww, wh;
    char bgfile[PATH_MAX];

    bg = NULL;
    bl = NULL;
    recent_bgs = NULL;

    gtk_init(&argc, &argv);

    win = create_ebony_window();
    
    /* setup the evas stuffs */
    gdk_window_get_geometry(GDK_ROOT_PARENT(), &rx, &ry, &rw, &rh, &rd);
    gtk_widget_realize(GTK_WIDGET(win));

    if(rw > 1400)	/* wider screen */
    {
	ww = rw / 3;
	wh = rh * 0.75;
    }
    else
    {
	ww = rw * 0.75;
	wh = rh * 0.75;
    }
    gtk_widget_set_usize(win, ww, wh);
    w = gtk_object_get_data(GTK_OBJECT(win), "evas");
    
    gtk_widget_realize(w);

    setup_evas(GDK_WINDOW_XDISPLAY(w->window),
		GDK_WINDOW_XWINDOW(w->window),
		GDK_VISUAL_XVISUAL(gtk_widget_get_visual(win)),
		GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(win)),
		w->allocation.width,
		w->allocation.height);
    
    gtk_widget_show(win);
    win_ref = win;
    if(argv[1])
    {
	snprintf(bgfile,PATH_MAX, "%s", argv[1]);
	open_bg_named(bgfile);
    }
    else
    {
	new_bg(NULL, NULL);
    }
    
    gtk_main();

    return(0);
}

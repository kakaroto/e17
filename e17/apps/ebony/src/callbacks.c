#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "advanced.h"
#include "callbacks.h"
#include "util.h"

int idle = 0;

gint
ebony_status_clear(gpointer data)
{
    gtk_statusbar_pop(GTK_STATUSBAR(ebony_status), 1);
    return FALSE;
    UN(data);
}
void
ebony_status_message(gchar *message, gint delay)
{
    gtk_statusbar_push(GTK_STATUSBAR(ebony_status), 1, message);
    gtk_timeout_add(delay, (GtkFunction)ebony_status_clear, ebony_status);
    return;
    UN(delay);
    UN(message);
}
int
redraw(void *data)
{
    evas_render(evas);
    return(FALSE);
}

gboolean
drawing_area_configure_event(GtkWidget *w, GdkEventConfigure *ev, gpointer data)
{
    evas_set_output_viewport(evas, 0, 0, ev->width, ev->height);
    evas_set_output_size(evas, ev->width, ev->height);
    e_bg_resize(bg, ev->width, ev->height);
    if(bl) outline_evas_object(bl->obj);
    DRAW();
    return(FALSE);
}
gboolean
drawing_area_expose_event(GtkWidget *w, GdkEventExpose *ev, gpointer data)
{
    evas_update_rect(evas, ev->area.x, ev->area.y, 
		     ev->area.width, ev->area.height);
    DRAW();
    return(FALSE);
}
void
save_bg(GtkWidget *w, gpointer data)
{
    gchar errstr[1024];
    if(bg)
    {
	fill_background_images(bg); 
	clear_bg_db_keys(bg);
	e_bg_save(bg, bg->file); 

	g_snprintf(errstr, 1024, "Saved background: %s", (char*)bg->file);
	ebony_status_message(errstr, EBONY_STATUS_TO);
    }
    return;
    UN(w);
    UN(data);
}
void
save_as_ok_clicked(GtkWidget *w, gpointer data)
{
    E_Background _bg;
    gchar *file;
    gchar errstr[1024];

    file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
    if(!file) return; 
    

    _bg = bg;
    fill_background_images(bg);
    e_bg_save(bg, (char*)file);
    bg = e_bg_load((char*)file);
    if(bg)
    {
	if(_bg) e_bg_free(_bg);

	e_bg_add_to_evas(bg, evas);
	e_bg_set_layer(bg, 0);
	e_bg_show(bg);
	DRAW();
    
	g_snprintf(errstr, 1024, "Saved background: %s", (char*)file);
	ebony_status_message(errstr, EBONY_STATUS_TO);
    }
    else
    {
	g_snprintf(errstr, 1024, "Unable to save %s", (char*)file);
	ebony_status_message(errstr, EBONY_STATUS_TO);
	bg = _bg;
    }
    gtk_widget_destroy(GTK_WIDGET(data));

    return;
    UN(w);
    UN(data);
}

void
save_as_cancel_clicked(GtkWidget *w, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}

void
save_bg_as(GtkWidget *w, gpointer data)
{
    GtkWidget *fs;

    fs = gtk_file_selection_new("Save Background As File ...");
    gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), "new.bg.db");
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
		"clicked", GTK_SIGNAL_FUNC(save_as_cancel_clicked),
		(gpointer)fs);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
		"clicked", GTK_SIGNAL_FUNC(save_as_ok_clicked),
		(gpointer)fs);
    gtk_widget_show(fs);
    return;
    UN(w);
    UN(data);
}
void
advanced_widgets_show_for_image(void)
{
    GtkWidget *w;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file_frame");
    gtk_widget_show(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_color_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_fill_frame");
    gtk_widget_show(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_w");
    if(w) gtk_widget_set_sensitive(w, TRUE);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_h");
    if(w) gtk_widget_set_sensitive(w, TRUE);
}
void
advanced_widgets_show_for_gradient(void)
{
    GtkWidget *w;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_color_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_fill_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_frame");
    gtk_widget_show(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_w");
    if(w) gtk_widget_set_sensitive(w, FALSE);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_h");
    if(w) gtk_widget_set_sensitive(w, FALSE);
}
void
advanced_widgets_show_for_color(void)
{
    GtkWidget *w;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_color_frame");
    gtk_widget_show(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_fill_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_w");
    if(w) gtk_widget_set_sensitive(w, FALSE);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_h");
    if(w) gtk_widget_set_sensitive(w, FALSE);
}
void
new_bg(GtkWidget *w, void *data)
{
    E_Background_Layer _bl;
    E_Background _bg = NULL;
    
    if(bg) e_bg_free(bg);
    _bg = e_bg_new();
    if(_bg)
    {
	_bl = e_bg_layer_new();
	_bl->type = E_BACKGROUND_TYPE_SOLID;
	_bl->fg.a = _bl->fg.r = _bl->fg.g = _bl->fg.b = 255;
	_bl->size.w = _bl->size.h = 1.0;
	_bg->layers = evas_list_append(_bg->layers, _bl);
	display_bg(_bg);
    }
    return;
    UN(w);
    UN(data);
}
void
open_bg_named(char *name)
{
    E_Background _bg = NULL;
    
    if(!name) return;
     
    _bg = e_bg_load(name);
    if(_bg)
    {
	char buf[256];
	
	snprintf(buf, 256, "Ebony - %s", name);
	gtk_window_set_title(GTK_WINDOW(win_ref), buf);
	e_bg_free(bg);
	display_bg(_bg);

	bg = _bg;
    }
    
    return;
    UN(name);
}
static void
filemenu_load_cancel_clicked(GtkWidget *w, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}

static void
filemenu_load_ok_clicked(GtkWidget *w, gpointer data)
{
    gchar *file;
    E_Background _bg;

    file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
     
    _bg = bg;

    _bg = e_bg_load((char*)file);
    if(_bg) 	
    {
	GList *l;
	int length = 0;
	char *file_on_disk;

	if(bg) e_bg_free(bg);
	display_bg(_bg);
	bg = _bg;
	for(l = recent_bgs; (l) && (length < 5); l = l->next) length++;
	if(length == 4)	/* full list */
	{
	    char *bg_to_pop = NULL;
	    bg_to_pop = g_list_nth_data(recent_bgs, 0);
	    recent_bgs = g_list_remove(recent_bgs, (gpointer)bg_to_pop);
	    free((char*)bg_to_pop);
	}
	file_on_disk = strdup((char*)file);
	recent_bgs = g_list_append(recent_bgs, file_on_disk);
    }
    else
    {
	gchar errstr[1024];

	g_snprintf(errstr, 1024, "Unable to load %s", (char*)file);
	ebony_status_message(errstr, EBONY_STATUS_TO);
    }
    gtk_widget_destroy(GTK_WIDGET(data));

    return;
    UN(w);
    UN(data);
}
void
open_bg(GtkWidget *w, gpointer data)
{
    GtkWidget *fs;

    fs = gtk_file_selection_new("Load Background File");
    gtk_file_selection_complete(GTK_FILE_SELECTION(fs), "*.bg.db");
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
		"clicked", GTK_SIGNAL_FUNC(filemenu_load_cancel_clicked),
		(gpointer)fs);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
		"clicked", GTK_SIGNAL_FUNC(filemenu_load_ok_clicked),
		(gpointer)fs);
    gtk_widget_show(fs);
    return;
    UN(w);
    UN(data);
}
void 
open_bg_cb(GtkWidget *w, gpointer data)
{
    if(!data) return;
    open_bg_named((char*)data);
    return;
    UN(w);
    UN(data);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "advanced.h"
#include "callbacks.h"
#include "util.h"

int idle = 0;

/**
 * ebony_status_clear  - clear the current text in the statusbar
 * @data - ignored
 */
gint
ebony_status_clear(gpointer data)
{
    gtk_statusbar_pop(GTK_STATUSBAR(ebony_status), 1);
    return FALSE;
    UN(data);
}
/** 
 * ebony_status_message - display a message in the statusbar
 * @message - the text to display in the statusbar
 * @delay - the timeout before the text is deleted
 */
void
ebony_status_message(gchar *message, gint delay)
{
    gtk_statusbar_push(GTK_STATUSBAR(ebony_status), 1, message);
    gtk_timeout_add(delay, (GtkFunction)ebony_status_clear, ebony_status);
    return;
    UN(delay);
    UN(message);
}
/**
 * redraw - render the evas
 * @data - ignored
 */
int
redraw(void *data)
{
    evas_render(evas);
    return(FALSE);
}
/**
 * drawing_area_configure_event - resize the bg's evas request
 * @w - ignored
 * @ev - ignored
 * @data - ignored
 */
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
/**
 * drawing_area_expose_event - expose event on the bg's evas
 * @w - ignored
 * @ev - ignored
 * @data - ignored
 */
gboolean
drawing_area_expose_event(GtkWidget *w, GdkEventExpose *ev, gpointer data)
{
    evas_update_rect(evas, ev->area.x, ev->area.y, 
		     ev->area.width, ev->area.height);
    DRAW();
    return(FALSE);
}
/**
 * save_bg - save the current background in context to disk
 * @w - ignored
 * @data - ignored
 */
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
/**
 * save_as_ok_clicked - ok clicked on the save as file selection
 * @w - the ok button
 * @data - the file selection
 */
void
save_as_ok_clicked(GtkWidget *w, gpointer data)
{
    gchar *file;
    gchar errstr[1024];

    file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
    if(!file) return; 

    fill_background_images(bg);
    clear_bg_db_keys(bg);
    e_bg_save(bg, (char*)file);

    open_bg_named((char*)file);
    g_snprintf(errstr, 1024, "Saved background: %s", (char*)file);
    ebony_status_message(errstr, EBONY_STATUS_TO);
	
    gtk_widget_destroy(GTK_WIDGET(data));

    return;
    UN(w);
    UN(data);
}
/**
 * save_as_cancel_clicked - cancel clicked on save as file selection
 * @w - ignored
 * @data - the file selection to destroy
 */
void
save_as_cancel_clicked(GtkWidget *w, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}
/**
 * save_bg_as - save as request, create a file selection
 * @w - ignored
 * @data - ignored
 */
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
/**
 * advanced_widgets_show_for_images - show widgets for an image layer
 * Only certain widgets in the advanced interface are applicable to layers
 * that are of type E_BACKGROUND_TYPE_IMAGE hide the rest so it's less
 * confusing.
 */
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
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_scroll_follow");
    gtk_widget_show(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_w");
    if(w) gtk_widget_set_sensitive(w, TRUE);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_h");
    if(w) gtk_widget_set_sensitive(w, TRUE);
}
/**
 * advanced_widgets_show_for_gradient - show widgets for a gradient layer
 * Only certain widgets in the advanced interface are applicable to layers
 * that are of type E_BACKGROUND_TYPE_GRADIENT hide the rest so it's less
 * confusing.
 */
void
advanced_widgets_show_for_gradient(void)
{
    GtkWidget *w;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_color_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_scroll_follow");
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
/**
 * advanced_widgets_show_for_color - show widgets for a color layer
 * Only certain widgets in the advanced interface are applicable to layers
 * that are of type E_BACKGROUND_TYPE_SOLID hide the rest so it's less
 * confusing.
 */
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
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_scroll_follow");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_frame");
    gtk_widget_hide(w);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_w");
    if(w) gtk_widget_set_sensitive(w, FALSE);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_h");
    if(w) gtk_widget_set_sensitive(w, FALSE);
}
/**
 * new_bg - create a new E_Background and show it in the evas
 * @w - ignored
 * @data - ignored
 */
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
	gtk_window_set_title(GTK_WINDOW(win_ref), "Ebony - New Background");
    }
    return;
    UN(w);
    UN(data);
}

/**
 * add_bg_filename_to_recent_menu - recent menu in realtime
 * @filename - the file to add to the recently load bg menu
 * Add filename to the recent menu so multiple bgs opened in one session are
 * avaiable from the menu list
 */
void
add_bg_filename_to_recent_menu(char *filename)
{
    GtkWidget *mi;
    GtkWidget *menu;
    char *short_name;
    
    short_name = get_shortname_for(filename);
    mi = gtk_menu_item_new_with_label(short_name);

    menu = gtk_object_get_data(GTK_OBJECT(win_ref), "recent_menu");
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_signal_connect(GTK_OBJECT(mi), "activate",
		GTK_SIGNAL_FUNC(open_bg), (gpointer)filename);
    gtk_widget_show(mi);
}
/**
 * handle_recent_bgs_append - append the file to recent bg list
 * @name - the filename on disk
 * If the file name already exists in the list, remove it and append it to
 * the list, if the list is full pop the first element and append the
 * filename to the recent_bgs list
 */
static void
handle_recent_bgs_append(char *name)
{
    GList *l;
    int ok, length;
    char *str;

    ok = 1;
    for(l = recent_bgs; l; l = l->next)
    {
	if(!strcmp((char*)l->data, name))
	{
	    recent_bgs = g_list_remove(recent_bgs, l->data);
	    free((char*)l->data);
	    ok = 0;
	}
    }
    if(!ok)
    {
	char *bg_to_pop = NULL;
	length = 0;
	for(l = recent_bgs; (l) && (length < 5); l = l->next) length++;
	if(length == 4)	
	{
	    bg_to_pop = g_list_nth_data(recent_bgs, 0);
	    recent_bgs = g_list_remove(recent_bgs, (gpointer)bg_to_pop);
	    free((char*)bg_to_pop);
	}
    }
    str = strdup((char*)name);
    recent_bgs = g_list_append(recent_bgs, str);
}
/**
 * open_bg_named - have ebony load the bg named name
 * @name - the filename on disk
 * Handle the freeing of the old bg, and loading of the new bg, plus keep up
 * with the recently used bg stuff
 */
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
	add_bg_filename_to_recent_menu(name);
	handle_recent_bgs_append(name);
	e_bg_free(bg);
	display_bg(_bg);

	bg = _bg;
    }
    else
    {
	gchar errstr[1024];

	g_snprintf(errstr, 1024, "Unable to load %s", name);
	ebony_status_message(errstr, EBONY_STATUS_TO);
    }
    
    return;
    UN(name);
}
/**
 * filemenu_load_cancel_clicked - select a file and click cancel
 * @w - the ok button
 * @data - pointer to the fileselection the user chose from
 */
static void
filemenu_load_cancel_clicked(GtkWidget *w, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}
/**
 * filemenu_load_ok_clicked - select a file and click ok
 * @w - the ok button
 * @data - pointer to the fileselection the user chose from
 */
static void
filemenu_load_ok_clicked(GtkWidget *w, gpointer data)
{
    gchar *file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
    if(file) open_bg_named((char*)file);
    gtk_widget_destroy(GTK_WIDGET(data));

    return;
    UN(w);
    UN(data);
}
/**
 * open_bg_cb - create a file selection to open a background
 * @w - ignored
 * @data - ignored
 */
void
open_bg_cb(GtkWidget *w, gpointer data)
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
/**
 * open_bg - open background file data points to
 * @w - Any widget
 * @data - The char* string for the filename on disk 
 */
void 
open_bg(GtkWidget *w, gpointer data)
{
    if(!data) return;
    open_bg_named((char*)data);
    return;
    UN(w);
    UN(data);
}
/**
 * get_range_value - get the value from the named GtkRange
 * @named - the name of the gtk range
 * Returns the double value of the range
 */
double
get_range_value(char *named)
{
    GtkWidget *w;
    double result = 0.0;
    
    w = gtk_object_get_data(GTK_OBJECT(win_ref), named);
    if(w)
	result = (gtk_range_get_adjustment(GTK_RANGE(w))->value);
    return(result); 
}
/**
 * on_scroll_changed - set the scroll for the image layer
 * @w - the modified range
 * @data - ignored
 */
void
on_scroll_changed(GtkWidget *w, gpointer data)
{
    if(!bg) return;
    if(!bl) return;
    e_bg_set_scroll(bg, (int)get_range_value("hscroll"),
			(int)get_range_value("vscroll"));
    if((bl->scroll.x) || (bl->scroll.y))
	DRAW();

    return;
    UN(w);
    UN(data);
}

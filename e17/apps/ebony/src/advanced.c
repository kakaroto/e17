#include "advanced.h"
#include "util.h"

/**
 * get_spin_value - for a named GtkWidget, attempt to retrieve its data
 * @name - the name of the widget
 * Return the double value of the spinbutton
 */
static double
get_spin_value(char *name)
{
    GtkWidget *w;
    double result = 0.0;
    
    w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
    if(w) result = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
    else fprintf(stderr, "unable to get spin value for %s\n", name);
    
    return(result);
}
/**
 * set_spin_value - for a named GtkWidget, attempt to set its value 
 * @name - the name of the spinbutton
 */
static void
set_spin_value(char *name, double val)
{
    GtkWidget *w;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
    if(w) gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), val);
    else fprintf(stderr, "unable to set spin value for %s\n", name);
}

/**
 * set_toggled_state - for a named GtkWidget, attempt to set its value 
 * @name - the name of the togglebutton 
 * @val - 1 is toggled down, 0 is toggled up
 */
static void
set_toggled_state(char *name, int val)
{
    GtkWidget *w;
    w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
    if(w)
    {
	if(val) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
	else gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), FALSE);
    }
    else
	fprintf(stderr, "unable to set toggled state for %s\n", name);
}
/**
 * get_toggled_state - for a named GtkWidget, attempt to get its value 
 * @name - the name of the togglebutton 
 * Returns 1 is toggled down, 0 is toggled up
 */
static int
get_toggled_state(char *name)
{
    int result = 0;
    GtkWidget *w;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
    if(w)
    {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	    result = 1;
    }
    else
	fprintf(stderr, "unable to get toggled state for %s\n", name);
    return(result);
}

/**
 * get_entry_text - for a named GtkWidget, attempt to get its value 
 * @name - the name of the spinbutton
 * Returns NULL on error, the string on success
 */
static char*
get_entry_text(char *name)
{
    GtkWidget *w;
    char *result = NULL;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
    if(w) result = (char*)gtk_entry_get_text(GTK_ENTRY(w));
    
    return(result);
}
/**
 * set_entry_text - for a named GtkWidget, attempt to get its value 
 * @name - the name of the spinbutton
 * @txt The text to set in the entry
 */
static void
set_entry_text(char *name, char *txt)
{
    GtkWidget *w;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
    if(w) gtk_entry_set_text(GTK_ENTRY(w), txt);
}
/**
 * display_layer_values_for_gradient - display widgets for a gradient layer
 * @_bl - the background layer of type E_BACKGROUND_TYPE_GRADIENT
 * Some controls aren't applicable to gradients, hide them to avoid
 * confusion.
 */
void
display_layer_values_for_gradient(E_Background_Layer _bl)
{
    GtkWidget *w;
    
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_h");
    if(w) gtk_widget_set_sensitive(w, TRUE);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_w");
    if(w) gtk_widget_set_sensitive(w, TRUE);
    if(_bl->file) set_entry_text("image_file", (gchar*)_bl->file);
    else set_entry_text("image_file", "");
    
    set_toggled_state("size_abs_h", _bl->size.absolute.h); 
    set_toggled_state("size_abs_w", _bl->size.absolute.w); 
    set_toggled_state("size_orig_h", 0);
    set_toggled_state("size_orig_w", 0);
    
    set_spin_value("gradient_angle", _bl->gradient.angle);
    return;
    UN(_bl);
}
/**
 * display_layer_values_for_solid - display widgets for a solid color layer
 * @_bl - the background layer of type E_BACKGROUND_TYPE_SOLID
 * Some controls aren't applicable to colors, hide them to avoid
 * confusion.
 */
void
display_layer_values_for_solid(E_Background_Layer _bl)
{
    GtkWidget *w;

    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_h");
    if(w) gtk_widget_set_sensitive(w, TRUE);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_w");
    if(w) gtk_widget_set_sensitive(w, TRUE);
    if(_bl->file) set_entry_text("image_file", (gchar*)_bl->file);
    else set_entry_text("image_file", "");
    
    set_toggled_state("size_abs_h", _bl->size.absolute.h); 
    set_toggled_state("size_abs_w", _bl->size.absolute.w); 
    set_toggled_state("size_orig_h", 0);
    set_toggled_state("size_orig_w", 0);

    return;
    UN(_bl);
}
/**
 * display_layer_values_for_image - display widgets for a image layer
 * @_bl - the background layer of type E_BACKGROUND_TYPE_IMAGE
 * Some controls aren't applicable to image, hide them to avoid
 * confusion.
 */
void
display_layer_values_for_image(E_Background_Layer _bl)
{
    GtkWidget *w;
    
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file");
    if(w)
    {
	if(_bl->file) gtk_entry_set_text(GTK_ENTRY(w), (gchar*)_bl->file);
	else gtk_entry_set_text(GTK_ENTRY(w), "");
    }
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_h");
    if(w) gtk_widget_set_sensitive(w, FALSE);
    w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_w");
    if(w) gtk_widget_set_sensitive(w, FALSE);
    set_toggled_state("size_abs_h", 0); 
    set_toggled_state("size_abs_w", 0); 

    /* setup some stuff */
    set_toggled_state("inline_image", _bl->inlined);
    set_toggled_state("fill_orig_w", _bl->fill.orig.w);
    set_toggled_state("fill_orig_h", _bl->fill.orig.h);
    set_toggled_state("size_orig_h", _bl->size.orig.h); 
    set_toggled_state("size_orig_w", _bl->size.orig.w); 
    
    set_spin_value("fill_w", (double)_bl->fill.w);
    set_spin_value("fill_h", (double)_bl->fill.h);
    set_spin_value("scroll_x", (double)_bl->scroll.x);
    set_spin_value("scroll_y", (double)_bl->scroll.y);
    
    return;
    UN(_bl);
}

/**
 * display_bg - display the bg in the global evas
 * @_bg - the bg to display
 * Sets the image cache to 0, add th ebg to the evas, then select the first
 * layer in the bg
 */
void
display_bg(E_Background _bg)
{
    E_Background_Layer _bl;
    int size;
    
    /* clear the evas cache */
    size = evas_get_image_cache(evas);
    evas_set_image_cache(evas, 0);
    evas_set_image_cache(evas, size);

    e_bg_add_to_evas(_bg, evas);
    e_bg_set_layer(_bg, 0);
    e_bg_show(_bg);
    _bl = e_bg_get_layer_number(_bg, 0);
    display_layer_values(_bl);
    set_spin_value("layer_num", 0);
    update_background(_bg);
    bg = _bg;
}
/**
 * display_layer_values - setup the widgets to have the current bl's values
 * @_bl - the background layer to select
 * Assign global bl to the Background_Layer passed to it
 */
void 
display_layer_values(E_Background_Layer _bl)
{
    GtkWidget *w, *da;
    
    if(!_bl) return;
    
    bl = NULL;
    switch(_bl->type)
    {
	case E_BACKGROUND_TYPE_IMAGE:
	    w = gtk_object_get_data(GTK_OBJECT(win_ref), "type_image");
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
	    display_layer_values_for_image(_bl);
	    break;
	case E_BACKGROUND_TYPE_GRADIENT:
	    w = gtk_object_get_data(GTK_OBJECT(win_ref), "type_gradient");
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
	    display_layer_values_for_gradient(_bl);
	    break;
	case E_BACKGROUND_TYPE_SOLID:
	    w = gtk_object_get_data(GTK_OBJECT(win_ref), "type_color");
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
	    display_layer_values_for_solid(_bl);
	    break;
	default:
	    fprintf(stderr, "Unknown Layer Type Yo: %d\n", _bl->type);
	    return;
    }
    set_spin_value("pos_y", (double)_bl->pos.y);
    set_spin_value("pos_x", (double)_bl->pos.x);
    set_spin_value("abs_y", (double)_bl->abs.y);
    set_spin_value("abs_x", (double)_bl->abs.x);
    set_spin_value("size_w", (double)_bl->size.w);
    set_spin_value("size_h", (double)_bl->size.h);

    /* outline the object */
    if(_bl->obj)
    {
	outline_evas_object(_bl->obj);
	DRAW();
    }
    bl = _bl;
    
    switch(bl->type)
    {
	case E_BACKGROUND_TYPE_SOLID:
	    da = gtk_object_get_data(GTK_OBJECT(win_ref), "color_box");
	    if(da) color_drawing_area_expose(da, NULL, NULL);
	    break;
	case E_BACKGROUND_TYPE_GRADIENT:
	    da = gtk_object_get_data(GTK_OBJECT(win_ref), 
					    "gradient_one_color_box");
	    if(da) gradient_one_color_drawing_area_expose(da, NULL, NULL);
	    da = gtk_object_get_data(GTK_OBJECT(win_ref), 
					    "gradient_two_color_box");
	    if(da) gradient_two_color_drawing_area_expose(da, NULL, NULL);
	    break;
	default:
	    break;
    }
    
    return;
    UN(_bl);
}

/**
 * on_layer_num_changed - handle a layer change request
 * @e - ignored
 * @data - ignored
 */
void
on_layer_num_changed(GtkEditable *e, gpointer data)
{
    E_Background_Layer _bl;
    _bl = e_bg_get_layer_number(bg, (int)get_spin_value("layer_num"));
    if(_bl)
	display_layer_values(_bl);
    else
    {
	_bl = bl;
	bl = NULL;
	set_spin_value("layer_num", get_spin_value("layer_num") - 1);
	bl = _bl;
    }

    return;
    UN(e);
    UN(data);
}
/**
 * on_layer_type_changed - handle a layer's type change request
 * @e - ignored
 * @data - ignored
 */
void 
on_layer_type_changed(GtkWidget *w, gpointer data)
{
    GtkWidget *ti, *tg;
    ti = gtk_object_get_data(GTK_OBJECT(win_ref), "type_image");
    tg = gtk_object_get_data(GTK_OBJECT(win_ref), "type_gradient");
    if(w == ti)
    {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	{
	    if(bl) bl->type = E_BACKGROUND_TYPE_IMAGE;
	    advanced_widgets_show_for_image();
	}
    }
    else if(w == tg)
    {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	{
	    if(bl) bl->type = E_BACKGROUND_TYPE_GRADIENT;
	    advanced_widgets_show_for_gradient();
	}
    }
    else
    {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	{
	    if(bl) bl->type = E_BACKGROUND_TYPE_SOLID;
	    advanced_widgets_show_for_color();
	}
    }

    return;
    UN(w);
    UN(data);
}

void
on_pos_x_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->pos.x = (float)get_spin_value("pos_x");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}

void
on_pos_y_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->pos.y = (float)get_spin_value("pos_y");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}

void
on_scroll_y_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->scroll.y = (float)get_spin_value("scroll_y");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}
void
on_scroll_x_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->scroll.x = (float)get_spin_value("scroll_x");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}
void
on_size_width_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->size.w = (float)get_spin_value("size_w");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}
void
on_size_height_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->size.h = (float)get_spin_value("size_h");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}
void
on_fill_width_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->fill.w = (float)get_spin_value("fill_w");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}
void
on_fill_height_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->fill.h = (float)get_spin_value("fill_h");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}
void 
on_inline_image_toggled(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    bl->inlined = get_toggled_state("inline_image");
    return;
    UN(w);
    UN(data);
}
void 
on_size_orig_width_toggled(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    bl->size.orig.w = get_toggled_state("size_orig_w");
    update_background(bg);
    return;
    UN(w);
    UN(data);
}
void 
on_size_orig_height_toggled(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    bl->size.orig.h = get_toggled_state("size_orig_h");
    update_background(bg);
    return;
    UN(w);
    UN(data);
}
void 
on_fill_orig_width_toggled(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    bl->fill.orig.w = get_toggled_state("fill_orig_w");
    update_background(bg);
    return;
    UN(w);
    UN(data);
}
void 
on_fill_orig_height_toggled(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    bl->fill.orig.h = get_toggled_state("fill_orig_h");
    update_background(bg);
    return;
    UN(w);
    UN(data);
}
void 
on_abs_x_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->abs.x = (int)get_spin_value("abs_x");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}
void 
on_abs_y_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;
    bl->abs.y = (int)get_spin_value("abs_y");
    update_background(bg);
    return;
    UN(e);
    UN(data);
}
void
on_size_abs_w_toggled(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    bl->size.absolute.w = get_toggled_state("size_abs_w");
    update_background(bg);
    return;
    UN(w);
    UN(data);
}
void
on_size_abs_h_toggled(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    bl->size.absolute.h = get_toggled_state("size_abs_h");
    update_background(bg);
    return;
    UN(w);
    UN(data);
}
/**
 * on_layer_add_clicked - add a layer to the bg 
 * @w - ignored
 * @data - ignored
 */
void
on_layer_add_clicked(GtkWidget *w, gpointer data)
{
    E_Background_Layer _bl;
    Evas_List l;
    int size = 0;
    if(!bl) return;
    if(!bg) return;

    _bl = e_bg_layer_new();
    _bl->type = E_BACKGROUND_TYPE_SOLID;
    _bl->size.w = _bl->size.h = 1.0;
    _bl->fg.r = _bl->fg.g = _bl->fg.b = 255;
    _bl->fg.a = 80;
    _bl->obj = evas_add_rectangle(evas);
    evas_set_color(evas, _bl->obj, _bl->fg.r, _bl->fg.g, _bl->fg.b, _bl->fg.a);
    evas_show(evas, _bl->obj);
    bg->layers = evas_list_append(bg->layers, _bl);
    for(l = bg->layers; l; l = l->next) size++;
    bl = NULL;
    set_spin_value("layer_num", size);
    display_layer_values(_bl);
    bl = _bl;
}
/**
 * browse_file_ok_clicked - loading an image layer from a file selection
 * @w - the ok button
 * @data - pointer to the file selection
 */
static void
browse_file_ok_clicked(GtkWidget *w, gpointer data)
{
    gchar *file;

    file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
    
    if(file)
	set_entry_text("image_file", file);

    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}
/**
 * browse_file_cancel_clicked - canceling image layer file selection
 * @w - the cancel button
 * @data - pointer to the file selection
 */
static void
browse_file_cancel_clicked(GtkWidget *w, gpointer data)
{
    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}
/**
 * on_image_file_browse_clicked - create a fiel selection to load an image
 * @w - ignored
 * @data - ignored
 */
void
on_image_file_browse_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *fs;
    char buf[1024];
    
    /* getcwd() ? */
    snprintf(buf, 1024, "%s/", getenv("HOME"));

    fs = gtk_file_selection_new("Add Image File ...");
    gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), buf);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button), 
	    "clicked", GTK_SIGNAL_FUNC(browse_file_cancel_clicked),
	    (gpointer)fs);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button), 
	    "clicked", GTK_SIGNAL_FUNC(browse_file_ok_clicked),
	    (gpointer)fs);
    gtk_widget_show(fs);

    return;
    UN(w);
    UN(data);
}
/**
 * on_image_file_entry_changed - handle a file change in the text entry
 * @w - ignored
 * @data - ignored
 * Unload the image and load with the new filename specified
 */
void
on_image_file_entry_changed(GtkWidget *w, gpointer data)
{
    char *text;
    char filename[PATH_MAX];
    if(!bl) return;
    
    text = get_entry_text("image_file");
    if((text) && (strlen(text)))
    {
	snprintf(filename, PATH_MAX,"%s", (char*)text);
	if(bl->file) free(bl->file);
	bl->file = strdup(filename);
	if(bl->image) 
	{
	    imlib_context_set_image(bl->image);
	    imlib_free_image_and_decache();
	    bl->image = imlib_load_image(bl->file);
	}
	if(bl->obj) evas_del_object(evas, bl->obj);
	bl->obj = evas_add_image_from_file(evas, bl->file);
	evas_show(evas, bl->obj);
	bl->size.w = bl->size.h = 1.0;
	bl->fill.w = bl->fill.h = 1.0;
	bl->inlined = 1;
	update_background(bg);
    }
    return;
    UN(w);
    UN(data);
}
/**
 * on_delete_layer_clicked - delete the current background layer
 * @w - ignored
 * @data - ignored
 */
void
on_delete_layer_clicked(GtkWidget *w, gpointer data)
{
    int l;
    E_Background_Layer _bl;

    bg->layers = evas_list_remove(bg->layers, bl);
    e_bg_layer_free(bg, bl);

    l = (int)get_spin_value("layer_num");
    if(l)
    {
	_bl = e_bg_get_layer_number(bg,(l - 1));
	set_spin_value("layer_num", (l -1));
    }
    else
    {
	/* was the last entry, make a new base */
	if(!bg->layers)
	{
	    _bl = e_bg_layer_new();
	    _bl->type = E_BACKGROUND_TYPE_SOLID;
	    _bl->fg.a = _bl->fg.r = _bl->fg.g = _bl->fg.b = 255;
	    _bl->size.w = _bl->size.h = 1.0;
	    _bl->obj = evas_add_rectangle(evas);
	    evas_set_color(evas, _bl->obj, _bl->fg.r, _bl->fg.g, _bl->fg.b, 
			_bl->fg.a);
	    evas_show(evas, _bl->obj);
	    bg->layers = evas_list_append(bg->layers, _bl);
	}
	/* return the front */
	else
	{
	    _bl = e_bg_get_layer_number(bg,0); 
	}
    }
    
    display_layer_values(_bl);
    update_background(bg);
    return;
    UN(w);
    UN(data);
}
/**
 * on_layer_up_clicked - attempt to move the layer upwards
 * @w - ignored
 * @data - ignored
 * see if the layer can be moved up, do it if it can
 */
void 
on_layer_up_clicked(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    if(move_layer_up(bl))
	set_spin_value("layer_num", (int) get_spin_value("layer_num") + 1);
    return;
    UN(w);
    UN(data);
}

/**
 * on_layer_down_clicked - attempt to move the layer downwards
 * @w - ignored
 * @data - ignored
 * see if the layer can be moved down, do it if it can
 */
void 
on_layer_down_clicked(GtkWidget *w, gpointer data)
{
    if(!bl) return;
    if(move_layer_down(bl))
	set_spin_value("layer_num", (int) get_spin_value("layer_num") -1);
    return;
    UN(w);
    UN(data);
}
/**
 * cs_ok_button_clicked - set the Background color from the color selection
 * @w - the ok button
 * @data - the color selection
 * This is only for the drawing area and section when the layer is a SOLID,
 * gradients have different cbs
 */
void
cs_ok_button_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *da;
    gdouble color[4];
    
    if(!bl) return;
    if(data)
    {
	gtk_color_selection_get_color(
		    GTK_COLOR_SELECTION(
		    GTK_COLOR_SELECTION_DIALOG(data)->colorsel), color);
	bl->fg.r = 255 * color[0];
	bl->fg.g = 255 * color[1];
	bl->fg.b = 255 * color[2];
	bl->fg.a = 255 * color[3];
    
	evas_set_color(evas, bl->obj, bl->fg.r, bl->fg.g, bl->fg.b, bl->fg.a);
	update_background(bg);
    }
    
    da = gtk_object_get_data(GTK_OBJECT(win_ref), "color_box");
    gtk_widget_set_sensitive(da, TRUE);
    color_drawing_area_expose(da, NULL, NULL);

    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}
/**
 * cs_cancel_button_clicked - close the color selection on cancel
 * @w - cancel button
 * @data - the color selection
 */
void
cs_cancel_button_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *da;

    gtk_widget_destroy(GTK_WIDGET(data));
    da = gtk_object_get_data(GTK_OBJECT(win_ref), "color_box");
    gtk_widget_set_sensitive(da, TRUE);
    return;
    UN(w);
    UN(data);
}
/**
 * gradient_one_cs_cancel_button_clicked - close the color selection on cancel
 * @w - cancel button
 * @data - the color selection
 */
void
gradient_one_cs_cancel_button_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *da;

    gtk_widget_destroy(GTK_WIDGET(data));
    da = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_one_color_box");
    gtk_widget_set_sensitive(da, TRUE);
    return;
    UN(w);
    UN(data);
}
/**
 * gradient_two_cs_cancel_button_clicked - close the color selection on cancel
 * @w - cancel button
 * @data - the color selection
 */
void
gradient_two_cs_cancel_button_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *da;

    gtk_widget_destroy(GTK_WIDGET(data));
    da = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_two_color_box");
    gtk_widget_set_sensitive(da, TRUE);
    return;
    UN(w);
    UN(data);
}
/**
 * color_drawing_area_expose - redraw routine for the bg type color
 * @w - the drawing area widget
 * @e - the gdk event expose
 * @data - ignored
 */
gboolean
color_drawing_area_expose(GtkWidget *w, GdkEventExpose *e, gpointer data)
{
    GdkColor fg;
    static GdkGC *gc = NULL; 
    
    if(!w) return(FALSE);
    if(!w->window) return(FALSE);
    if(!bl) return(FALSE);
    if(!gc) gc = gdk_gc_new(w->window);
    
    fg.pixel = gdk_rgb_xpixel_from_rgb((bl->fg.r << 16) | (bl->fg.g << 8) |
				    (bl->fg.b));
     
    gdk_gc_set_foreground(gc, &fg);
    gdk_draw_rectangle(w->window, gc, 1, 0, 0,
		w->allocation.width, w->allocation.height);

    return(TRUE);
    UN(w);
    UN(e);
    UN(data);

}
gboolean
color_drawing_area_mouse_down(GtkWidget *w, GdkEventButton *e, gpointer data)
{
    GtkWidget *cs;
    gdouble color[4], temp;

    cs = gtk_color_selection_dialog_new("Select Color");
    gtk_color_selection_set_opacity(GTK_COLOR_SELECTION(
			GTK_COLOR_SELECTION_DIALOG(cs)->colorsel), TRUE);
    gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(cs)->ok_button),
		    "clicked", GTK_SIGNAL_FUNC(cs_ok_button_clicked), 
		    (gpointer)cs);
    gtk_signal_connect(GTK_OBJECT(
		GTK_COLOR_SELECTION_DIALOG(cs)->cancel_button), "clicked",
			GTK_SIGNAL_FUNC(cs_cancel_button_clicked), 
			(gpointer)cs);
    
    if(bl)
    {
	color[0] = 
	    (temp = ((double)bl->fg.r / (double)255)) == 0 ? 0.001 : temp;
	color[1] = 
	    (temp = ((double)bl->fg.g / (double)255)) == 0 ? 0.001 : temp;
	color[2] = 
	    (temp = ((double)bl->fg.b / (double)255)) == 0 ? 0.001 : temp;
	color[3] =  
	    (temp = ((double)bl->fg.a / (double)255)) == 0 ? 0.001 : temp;
    }
    gtk_color_selection_set_color(GTK_COLOR_SELECTION(
		    GTK_COLOR_SELECTION_DIALOG(cs)->colorsel), color);
    gtk_widget_set_sensitive(w, FALSE);
    gtk_widget_show(cs);
    return(TRUE);
}
void
redraw_gradient_object()
{
    Evas_List l;
    Evas_Gradient og;
    E_Background_Gradient g;

    if(!bl) return;
    if(bl->obj) evas_del_object(evas, bl->obj);

    bl->obj = evas_add_gradient_box(evas);
    og = evas_gradient_new();
    for(l = bl->gradient.colors; l; l = l->next)
    {
	g = (E_Background_Gradient)l->data;
	evas_gradient_add_color(og, g->r, g->g, g->b, g->a, 1);
    }
    evas_set_gradient(evas, bl->obj, og);
    evas_set_angle(evas, bl->obj, bl->gradient.angle);
    evas_gradient_free(og);

    evas_show(evas, bl->obj);
    return;
    UN(g);
}
void
gradient_one_cs_ok_button_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *da;
    E_Background_Gradient g;
    gdouble color[4];
   
    if(!bl) return;
    if(bl->gradient.colors)
    {
	g = (E_Background_Gradient)bl->gradient.colors->data;
    }
    else
    {
	g = (E_Background_Gradient) malloc(sizeof(struct _E_Background_Gradient));
	memset(g, 0, sizeof(struct _E_Background_Gradient));

	bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
    }

    if(data)
    {
	gtk_color_selection_get_color(
		    GTK_COLOR_SELECTION(
		    GTK_COLOR_SELECTION_DIALOG(data)->colorsel), color);
	g->r = 255 * color[0];
	g->g = 255 * color[1];
	g->b = 255 * color[2];
	g->a = 255 * color[3];
    }
    da = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_one_color_box");
    gtk_widget_set_sensitive(da, TRUE);
    gradient_one_color_drawing_area_expose(da, NULL, NULL);
    redraw_gradient_object();
    update_background(bg);

    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}

gboolean
gradient_one_color_drawing_area_expose(GtkWidget *w, 
				    GdkEventExpose *e, gpointer data)
{
    GdkColor fg;
    static GdkGC *gc1 = NULL;
    E_Background_Gradient g = NULL;
   
    if(!w) return(FALSE);
    if(!w->window) return(FALSE);
    if(!bl) return(FALSE);
    if(bl->gradient.colors)
    {
	if(bl->gradient.colors->data) g = bl->gradient.colors->data;
    }
    else
    {
	g = (E_Background_Gradient) 
	    malloc(sizeof(struct _E_Background_Gradient));
	memset(g, 0, sizeof(struct _E_Background_Gradient));
	bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
    }
    if(gc1) gdk_gc_destroy(gc1);
    gc1 = gdk_gc_new(w->window);
    fg.pixel = gdk_rgb_xpixel_from_rgb((g->r << 16) | (g->g << 8) | g->b);
    gdk_gc_set_foreground(gc1, &fg);
    gdk_draw_rectangle(w->window, gc1, 1, 0, 0,
		w->allocation.width, w->allocation.height);

    return(TRUE);
    UN(w);
    UN(e);
    UN(data);
}
gboolean
gradient_two_color_drawing_area_expose(GtkWidget *w, 
				    GdkEventExpose *e, gpointer data)
{
    E_Background_Gradient g = NULL;
    GdkColor fg;
    static GdkGC *gc2 = NULL;
    
    if(!w) return(FALSE);
    if(!w->window) return(FALSE);
    if(!bl) return(FALSE);
    if((bl->gradient.colors) && (bl->gradient.colors->next))
    {
	if(bl->gradient.colors->next->data) g = bl->gradient.colors->next->data;
    }
    else
    {
	g = (E_Background_Gradient) 
	    malloc(sizeof(struct _E_Background_Gradient));
	memset(g, 0, sizeof(struct _E_Background_Gradient));
	bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
    }
    if(gc2) gdk_gc_destroy(gc2);
    gc2 = gdk_gc_new(w->window);
    fg.pixel = gdk_rgb_xpixel_from_rgb((g->r << 16) | (g->g << 8) | g->b);
    gdk_gc_set_foreground(gc2, &fg);
    gdk_draw_rectangle(w->window, gc2, 1, 0, 0,
		w->allocation.width, w->allocation.height);

    return(TRUE);
    UN(w);
    UN(e);
    UN(data);
}
void
gradient_two_cs_ok_button_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *da;
    E_Background_Gradient g;
    gdouble color[4];
   
    if(!bl) return;
    if((bl->gradient.colors) && (bl->gradient.colors->next))
    {
	g = (E_Background_Gradient)bl->gradient.colors->next->data;
    }
    else
    {
	g = (E_Background_Gradient) malloc(sizeof(struct _E_Background_Gradient));
	memset(g, 0, sizeof(struct _E_Background_Gradient));

	bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
    }

    if(data)
    {
	gtk_color_selection_get_color(
		    GTK_COLOR_SELECTION(
		    GTK_COLOR_SELECTION_DIALOG(data)->colorsel), color);
	g->r = 255 * color[0];
	g->g = 255 * color[1];
	g->b = 255 * color[2];
	g->a = 255 * color[3];
    }
    da = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_two_color_box");
    gtk_widget_set_sensitive(da, TRUE);
    gradient_one_color_drawing_area_expose(da, NULL, NULL);
    redraw_gradient_object();
    update_background(bg);

    gtk_widget_destroy(GTK_WIDGET(data));
    return;
    UN(w);
    UN(data);
}
gboolean
gradient_one_color_drawing_area_mouse_down(GtkWidget *w, GdkEventButton *e,
	gpointer data)
{
    GtkWidget *cs;
    gdouble color[4], temp;
    E_Background_Gradient g;

    cs = gtk_color_selection_dialog_new("Select Color");
    gtk_color_selection_set_opacity(GTK_COLOR_SELECTION(
			GTK_COLOR_SELECTION_DIALOG(cs)->colorsel), TRUE);
    gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(cs)->ok_button),
		    "clicked",
		    GTK_SIGNAL_FUNC(gradient_one_cs_ok_button_clicked), 
		    (gpointer)cs);
    gtk_signal_connect(GTK_OBJECT(
		GTK_COLOR_SELECTION_DIALOG(cs)->cancel_button), "clicked",
			GTK_SIGNAL_FUNC(gradient_one_cs_cancel_button_clicked), 
			(gpointer)cs);
    
    if(bl)
    {
	if((bl->gradient.colors) && (bl->gradient.colors))
	{
	    g = (E_Background_Gradient)bl->gradient.colors->data;
	}
	else
	{
	    g = (E_Background_Gradient) malloc(sizeof(struct _E_Background_Gradient));
	    memset(g, 0, sizeof(struct _E_Background_Gradient));

	    bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
	}
	color[0] = 
	    (temp = ((double)g->r / (double)255)) == 0 ? 0.001 : temp;
	color[1] = 
	    (temp = ((double)g->g / (double)255)) == 0 ? 0.001 : temp;
	color[2] = 
	    (temp = ((double)g->b / (double)255)) == 0 ? 0.001 : temp;
	color[3] =  
	    (temp = ((double)g->a / (double)255)) == 0 ? 0.001 : temp;
    }
    gtk_color_selection_set_color(GTK_COLOR_SELECTION(
		    GTK_COLOR_SELECTION_DIALOG(cs)->colorsel), color);
    gtk_widget_set_sensitive(w, FALSE);
    gtk_widget_show(cs);
    return(TRUE);
}
gboolean
gradient_two_color_drawing_area_mouse_down(GtkWidget *w, GdkEventButton *e,
	gpointer data)
{
    GtkWidget *cs;
    gdouble color[4];
    E_Background_Gradient g;

    cs = gtk_color_selection_dialog_new("Select Color");
    gtk_color_selection_set_opacity(GTK_COLOR_SELECTION(
			GTK_COLOR_SELECTION_DIALOG(cs)->colorsel), TRUE);
    gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(cs)->ok_button),
		    "clicked",
		    GTK_SIGNAL_FUNC(gradient_two_cs_ok_button_clicked), 
		    (gpointer)cs);
    gtk_signal_connect(GTK_OBJECT(
		GTK_COLOR_SELECTION_DIALOG(cs)->cancel_button), "clicked",
			GTK_SIGNAL_FUNC(gradient_two_cs_cancel_button_clicked), 
			(gpointer)cs);
    
    if(bl)
    {
	gdouble temp;
	if((bl->gradient.colors) && (bl->gradient.colors->next))
	{
	    g = (E_Background_Gradient)bl->gradient.colors->next->data;
	}
	else
	{
	    g = (E_Background_Gradient) malloc(sizeof(struct _E_Background_Gradient));
	    memset(g, 0, sizeof(struct _E_Background_Gradient));

	    bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
	}
	color[0] = 
	    (temp = ((double)g->r / (double)255)) == 0 ? 0.001 : temp;
	color[1] = 
	    (temp = ((double)g->g / (double)255)) == 0 ? 0.001 : temp;
	color[2] = 
	    (temp = ((double)g->b / (double)255)) == 0 ? 0.001 : temp;
	color[3] =  
	    (temp = ((double)g->a / (double)255)) == 0 ? 0.001 : temp;
    }
    gtk_color_selection_set_color(GTK_COLOR_SELECTION(
		    GTK_COLOR_SELECTION_DIALOG(cs)->colorsel), color);
    gtk_widget_set_sensitive(w, FALSE);
    gtk_widget_show(cs);
    return(TRUE);
}
void 
on_gradient_angle_changed(GtkEditable *e, gpointer data)
{
    if(!bl) return;

    bl->gradient.angle = get_spin_value("gradient_angle");
    evas_set_angle(evas, bl->obj, bl->gradient.angle);
    DRAW();
    return;
    UN(e);
    UN(data);
}

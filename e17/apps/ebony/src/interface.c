#include "interface.h"
#include "callbacks.h"
#include "advanced.h"
#include "util.h"

/** 
 * app_exit - when the app needs to quit
 */
static gint
app_exit(GtkWidget *w, gpointer data)
{
    write_ebony_bg_list_db(recent_bgs); 
    gtk_main_quit();
    return FALSE;
    UN(w);
    UN(data);
}

/**
 * create_ebony_filemenu - Create the Filemenu widget and return it
 * return Completely built menu, AND widget needs to be shown elsewhere
 */
static GtkWidget *
create_ebony_filemenu(void)
{
    GtkWidget *result;
    GtkWidget *menu, *m, *mi;
    GList *l;

    result = gtk_menu_bar_new();

    m = gtk_menu_item_new_with_label("File");
    gtk_menu_bar_append(GTK_MENU_BAR(result), m);
    
    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(m), menu);
    
    mi = gtk_menu_item_new_with_label("New");
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_signal_connect(GTK_OBJECT(mi), "activate",
		GTK_SIGNAL_FUNC(new_bg), NULL);
    gtk_widget_show(mi);
    
    mi = gtk_menu_item_new_with_label("Open");
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_signal_connect(GTK_OBJECT(mi), "activate",
		GTK_SIGNAL_FUNC(open_bg), NULL);
    gtk_widget_show(mi);
    
    mi = gtk_menu_item_new();
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_widget_show(mi);
     
    /* FIXME Parse recent bgs list */
    recent_bgs = parse_ebony_bg_list_db();
    for(l = recent_bgs; l; l = l->next)
    {
	char *txt = (char*)l->data;
	char *short_txt = get_shortname_for(txt);
	mi = gtk_menu_item_new_with_label(short_txt);
	gtk_menu_append(GTK_MENU(menu), mi);
	gtk_signal_connect(GTK_OBJECT(mi), "activate",
		GTK_SIGNAL_FUNC(open_bg_cb), (gpointer)txt);
	gtk_widget_show(mi);
    }

    /*					*/
    mi = gtk_menu_item_new();
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_widget_show(mi);
    
    mi = gtk_menu_item_new_with_label("Save");
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_signal_connect(GTK_OBJECT(mi), "activate",
		GTK_SIGNAL_FUNC(save_bg), NULL);
    gtk_widget_show(mi);
    
    mi = gtk_menu_item_new_with_label("Save As");
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_signal_connect(GTK_OBJECT(mi), "activate",
		GTK_SIGNAL_FUNC(save_bg_as), NULL);
    gtk_widget_show(mi);
    
    mi = gtk_menu_item_new();
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_widget_show(mi);

    mi = gtk_menu_item_new_with_label("Quit");
    gtk_menu_append(GTK_MENU(menu), mi);
    gtk_signal_connect(GTK_OBJECT(mi), "activate",
		GTK_SIGNAL_FUNC(app_exit), NULL);
    gtk_widget_show(mi);
    
    gtk_widget_show(menu);
    gtk_widget_show(m);

    return (result);
}

/**
 * create_ebony_manager_page - create ebg browser interface
 */
static GtkWidget *
create_ebony_manager_page(GtkWidget *win)
{
    GtkWidget *w;

    w = gtk_vbox_new(FALSE, 0);
    
    return(w);
}

/**
 * create_layer_color_frame - create the frame on the advanced tab
 * @win - a pointer to the GtkWidget that will eventually be the "window"
 * Returns - A GtkFrame to be packed into a container
 * All widget settings for the frame and callbacks are assigned here
 * as well as shown
 */
static GtkWidget *
create_layer_color_frame(GtkWidget *win)
{
    GtkWidget *frame, *hbox;
    GtkWidget *color;

    frame = gtk_frame_new("Layer Color");
    
    /* row 1 */
    hbox = gtk_hbox_new(FALSE, 0);

    color = gtk_drawing_area_new();
    gtk_widget_ref(color);
    gtk_object_set_data_full(GTK_OBJECT(win), "color_box", color,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_drawing_area_size(GTK_DRAWING_AREA(color), 80, 20);
    gtk_widget_set_events(color, GDK_BUTTON_RELEASE_MASK | GDK_EXPOSE);
    gtk_signal_connect(GTK_OBJECT(color), "expose_event", 
		    GTK_SIGNAL_FUNC(color_drawing_area_expose), NULL);
    gtk_signal_connect(GTK_OBJECT(color), "button_release_event", 
		    GTK_SIGNAL_FUNC(color_drawing_area_mouse_down), NULL);
    gtk_widget_show(color);
    gtk_box_pack_start(GTK_BOX(hbox), color, TRUE, TRUE, 2);
    
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 4);
    gtk_container_add(GTK_CONTAINER(frame), hbox);

    gtk_widget_show(hbox);

    return(frame);
}
/**
 * create_layer_frame - create the frame for selecting a bg layer to modify
 * @win - a pointer to the GtkWidget that will eventually be the "window"
 * Returns - A GtkFrame to be packed into a container 
 * All widget settings for the frame and callbacks are assigned here
 * as well as shown
 */
static GtkWidget *
create_layer_frame(GtkWidget *win)
{
    GtkWidget *frame, *label, *layer_num, *hbox, *vbox;
    GtkWidget *add, *delete;
    GtkObject *layer_num_adj;
    GtkWidget *layer_up, *layer_down;

    frame = gtk_frame_new("Layer Number");
    vbox = gtk_vbox_new(FALSE, 0);
    
    /* top level */
    hbox = gtk_hbox_new(FALSE, 0);
    label = gtk_label_new("Layer");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
    gtk_widget_show(label);

    layer_num_adj = gtk_adjustment_new(0, 0, 9999, 1, 10, 10);
    layer_num = gtk_spin_button_new(GTK_ADJUSTMENT(layer_num_adj), 1, 0);
    gtk_widget_ref(layer_num);
    gtk_object_set_data_full(GTK_OBJECT(win), "layer_num", layer_num,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(layer_num), "changed",
		    GTK_SIGNAL_FUNC(on_layer_num_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), layer_num, TRUE, TRUE, 1);

    add = gtk_button_new_with_label("Add...");
    gtk_signal_connect(GTK_OBJECT(add), "clicked",
		    GTK_SIGNAL_FUNC(on_layer_add_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), add, TRUE, TRUE, 1);
     
    delete = gtk_button_new_with_label("Delete");
    gtk_signal_connect(GTK_OBJECT(delete), "clicked",
		GTK_SIGNAL_FUNC(on_delete_layer_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), delete, TRUE, TRUE, 1);
    
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 1);
    gtk_widget_show(hbox);

    /* bottom level */
    
    hbox = gtk_hbox_new(FALSE, 0);
    
    layer_down = gtk_button_new_with_label("Move Layer Down");
    gtk_box_pack_start(GTK_BOX(hbox), layer_down, TRUE, TRUE, 2);
    gtk_widget_show(layer_down);
    gtk_signal_connect(GTK_OBJECT(layer_down), "clicked", 
		GTK_SIGNAL_FUNC(on_layer_down_clicked), NULL);
    
    layer_up = gtk_button_new_with_label("Move Layer Up");
    gtk_box_pack_start(GTK_BOX(hbox), layer_up, TRUE, TRUE, 2);
    gtk_widget_show(layer_up);
    gtk_signal_connect(GTK_OBJECT(layer_up), "clicked", 
		GTK_SIGNAL_FUNC(on_layer_up_clicked), NULL);
    
    gtk_widget_show(layer_up);
    gtk_widget_show(layer_down);
    gtk_widget_show(hbox);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 2); 
    /* bottom level */
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 4);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    
    gtk_widget_show(layer_num);
    gtk_widget_show(add);
    gtk_widget_show(delete);
    gtk_widget_show(vbox);
    gtk_widget_show(frame);

    return(frame);
}
/**
 * creat_layer_type_button - Create the frame containing the type radiobtns
 * @win - a pointer to the GtkWidget that will eventually be the "window"
 * Returns - A GtkFrame to be packed into a container 
 * All widget settings for the frame and callbacks are assigned here
 * as well as shown
 */
static GtkWidget *
create_layer_type_buttons(GtkWidget *win)
{
    GtkWidget *frame, *hbox, *b1, *b2, *b3;
   
    frame = gtk_frame_new("Layer Type");

    hbox = gtk_hbox_new(FALSE, 0);
    b1 = gtk_radio_button_new_with_label(NULL, "Image");
    gtk_widget_ref(b1);
    gtk_object_set_data_full(GTK_OBJECT(win), "type_image", b1,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(b1), "toggled",
		GTK_SIGNAL_FUNC(on_layer_type_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), b1, FALSE, FALSE, 1);

    b2 = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(b1),
							"Gradient");
    gtk_widget_ref(b2);
    gtk_object_set_data_full(GTK_OBJECT(win), "type_gradient", b2,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(b2), "toggled",
		GTK_SIGNAL_FUNC(on_layer_type_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), b2, FALSE, FALSE, 1);
    
    b3 = 
	gtk_radio_button_new_with_label_from_widget(
		GTK_RADIO_BUTTON(b2), "Color");
    gtk_widget_ref(b3);
    gtk_object_set_data_full(GTK_OBJECT(win), "type_color", b3,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(b3), "toggled",
		GTK_SIGNAL_FUNC(on_layer_type_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), b3, FALSE, FALSE, 1);
    
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 4);
    gtk_container_add(GTK_CONTAINER(frame), hbox);
    
    gtk_widget_show(b3);
    gtk_widget_show(b2);
    gtk_widget_show(b1);
    gtk_widget_show(hbox);
    gtk_widget_show(frame);

    return(frame);
}
/**
 * creat_layer_image_file_frame - Create the frame containing image path 
 * @win - a pointer to the GtkWidget that will eventually be the "window"
 * Returns - A GtkFrame to be packed into a container 
 * All widget settings for the frame and callbacks are assigned here
 * as well as shown
 */
static GtkWidget *
create_layer_image_file_frame(GtkWidget *win)
{
    GtkWidget *frame, *hbox, *entry, *button, *inline_image;
    GtkWidget *vbox, *hbox2;

    frame = gtk_frame_new("Image File");
    vbox = gtk_vbox_new(FALSE, 0);

    
    /* top */
    hbox = gtk_hbox_new(FALSE, 0);
    
    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 2);
    gtk_signal_connect(GTK_OBJECT(entry), "changed", 
		GTK_SIGNAL_FUNC(on_image_file_entry_changed), NULL);
    gtk_widget_ref(entry);
    gtk_object_set_data_full(GTK_OBJECT(win), "image_file", entry,
			(GtkDestroyNotify)gtk_widget_unref);

    button = gtk_button_new_with_label("Settings");
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 2);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
		    GTK_SIGNAL_FUNC(on_image_file_browse_clicked), NULL);
    gtk_widget_ref(button);
    gtk_object_set_data_full(GTK_OBJECT(win), "image_browse_button", button,
			(GtkDestroyNotify)gtk_widget_unref);
    
    /* pack top part */
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
    
    /* bottom */
    hbox2 = gtk_hbox_new(FALSE, 0);
    inline_image = gtk_check_button_new_with_label("Inline Image");
    gtk_widget_ref(inline_image);
    gtk_object_set_data_full(GTK_OBJECT(win), "inline_image", inline_image,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_box_pack_start(GTK_BOX(hbox2), inline_image, FALSE, FALSE, 2);
    gtk_signal_connect(GTK_OBJECT(inline_image), "toggled",
		    GTK_SIGNAL_FUNC(on_inline_image_toggled), NULL);
    /* pack the bottom */ 
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 2);
    
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 4);
    gtk_container_add(GTK_CONTAINER(frame), vbox);

    gtk_widget_show(inline_image);
    gtk_widget_show(button);
    gtk_widget_show(entry);
    gtk_widget_show(hbox);
    gtk_widget_show(hbox2);
    gtk_widget_show(vbox);
    gtk_widget_show(frame);

    return(frame);
}
/**
 * creat_layer_color_class_frame - Create the frame w/ entry for color class
 * @win - a pointer to the GtkWidget that will eventually be the "window"
 * Returns - A GtkFrame to be packed into a container 
 * All widget settings for the frame and callbacks are assigned here
 * as well as shown
 * FIXME - Should be a Drop Down menu or what ?
 */
static GtkWidget *
create_layer_color_class_frame(GtkWidget *win)
{
    GtkWidget *frame, *entry, *hbox;

    frame = gtk_frame_new("Color Class");
    hbox = gtk_hbox_new(TRUE, 4);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);
    gtk_widget_ref(entry);
    gtk_object_set_data_full(GTK_OBJECT(win), "color_class", entry,
			    (GtkDestroyNotify)gtk_widget_unref);

    gtk_container_set_border_width(GTK_CONTAINER(hbox), 4);
    gtk_container_add(GTK_CONTAINER(frame), hbox);

    gtk_widget_show(entry);
    gtk_widget_show(hbox);
    gtk_widget_show(frame);

    return(frame);
}
static GtkWidget *
create_layer_absolute_position_frame(GtkWidget *win)
{
    GtkWidget *frame, *hbox;
    GtkWidget *label, *spin_button;
    GtkObject *o;

    frame = gtk_frame_new("Absolute Offset From Position");
    hbox = gtk_hbox_new(FALSE, 0);
    
    /* left side */
    label = gtk_label_new("X:");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);

    o = gtk_adjustment_new(0, -999999, 999999, 1, 2.5, 10);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 1, 0);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "abs_x", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_abs_x_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), spin_button, TRUE, TRUE, 0);
    gtk_widget_show(spin_button);
    
    /* right side */    
    label = gtk_label_new("Y:");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    o = gtk_adjustment_new(0, -999999, 999999, 1, 2.5, 10);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 1, 0);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "abs_y", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_abs_y_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), spin_button, TRUE, TRUE, 0);
    gtk_widget_show(spin_button);
    
    /* finish up */
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 3);
    gtk_container_add(GTK_CONTAINER(frame), hbox);

    gtk_widget_show(hbox);
    gtk_widget_show(frame);
    return(frame);
}
/**
 * create_layer_position_frame
 * @win - a pointer to the GtkWidget that will eventually be the "window"
 */
static GtkWidget *
create_layer_position_frame(GtkWidget *win)
{
    GtkWidget *frame, *hbox, *vbox;
    GtkObject *o;
    GtkWidget *label, *spin_button, *absolute_frame;

    frame = gtk_frame_new("Position");
    vbox = gtk_vbox_new(FALSE, 0);

    hbox = gtk_hbox_new(FALSE, 0);
    
    /* left side */
    label = gtk_label_new("X:");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
    gtk_widget_show(label);

    o = gtk_adjustment_new(0.0, 0.0, 1.0, 0.01, 0.1, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 0.01, 2);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "pos_x", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_pos_x_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), spin_button, FALSE, FALSE, 1);
    gtk_widget_show(spin_button);
    
    /* right side */    
    label = gtk_label_new("Y:");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
    gtk_widget_show(label);
    
    o = gtk_adjustment_new(0.0, 0.0, 1.0, 0.01, 0.1, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 0.01, 2);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "pos_y", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_pos_y_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), spin_button, TRUE, TRUE, 1);
    gtk_widget_show(spin_button);
    
    /* pack the top part */
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 2);
    
    /* absolute placement subframe */
    absolute_frame = create_layer_absolute_position_frame(win);
    gtk_container_set_border_width(GTK_CONTAINER(frame), 3);
    gtk_box_pack_start(GTK_BOX(vbox), absolute_frame, FALSE, FALSE, 2);

    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    gtk_container_add(GTK_CONTAINER(frame), vbox);

    gtk_widget_show(hbox);
    gtk_widget_show(vbox);
    gtk_widget_show(frame);

    return(frame);
}
/**
 * create_layer_scroll_frame
 * @win - a pointer to the GtkWidget that will eventually be the "window"
 */
static GtkWidget *
create_layer_scroll_follow_frame(GtkWidget *win)
{
    GtkWidget *frame, *hbox;
    GtkObject *o;
    GtkWidget *label, *spin_button;

    frame = gtk_frame_new("Scroll Follow");
    hbox = gtk_hbox_new(FALSE, 0);
    
    /* left side */
    label = gtk_label_new("X:");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
    gtk_widget_show(label);
    
    o = gtk_adjustment_new(0.0, 0.0, 1.0, 0.01, 0.1, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 0.01, 2);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "scroll_x", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_scroll_x_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), spin_button, TRUE, TRUE, 1);
    
    gtk_widget_show(spin_button);
    
    /* right side */    
    label = gtk_label_new("Y:");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
    gtk_widget_show(label);
    
    o = gtk_adjustment_new(0.0, 0.0, 1.0, 0.01, 0.1, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 0.01, 2);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "scroll_y", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_scroll_y_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), spin_button, TRUE, TRUE, 1);
    gtk_widget_show(spin_button);
    
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 4);
    gtk_container_add(GTK_CONTAINER(frame), hbox);

    gtk_widget_show(hbox);
    gtk_widget_show(frame);

    return(frame);
}
static GtkWidget *
create_layer_size_frame(GtkWidget *win)
{
    GtkWidget *frame, *table;
    GtkWidget *label;
    GtkWidget *spin_button, *check_button;
    GtkObject *o;

    frame = gtk_frame_new("Size");
    table = gtk_table_new(3, 4, FALSE);

    /* left side */
    label = gtk_label_new("Width:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_EXPAND,
			GTK_SHRINK, 2, 2);
    gtk_widget_show(label);
    
    o = gtk_adjustment_new(0.0, 0.0, 10000.0, 0.01, 0.1, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 0.01, 2);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "size_w", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_size_width_changed), NULL);
    gtk_table_attach(GTK_TABLE(table), spin_button, 1,2,0,1, GTK_EXPAND,
			GTK_EXPAND, 2, 2);
    gtk_widget_show(spin_button); 

    check_button = gtk_check_button_new_with_label("Orig Width");
    gtk_widget_ref(check_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "size_orig_w", check_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(check_button), "toggled",
		    GTK_SIGNAL_FUNC(on_size_orig_width_toggled), NULL);
    gtk_table_attach(GTK_TABLE(table), check_button, 0, 2, 1, 2, GTK_FILL,
			GTK_SHRINK, 2, 2);
    gtk_widget_show(check_button);
    
    /* right side */
    label = gtk_label_new("Height:");
    gtk_table_attach(GTK_TABLE(table), label, 2, 3, 0, 1, GTK_EXPAND, 
		    GTK_SHRINK, 2, 2);
    gtk_widget_show(label);
    
    o = gtk_adjustment_new(0.0, 0.0, 10000.0, 0.01, 0.1, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 0.01, 2);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "size_h", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_size_height_changed), NULL);
    gtk_table_attach(GTK_TABLE(table), spin_button, 3,4,0,1, GTK_EXPAND, 
			GTK_EXPAND, 2, 2);
    gtk_widget_show(spin_button); 

    check_button = gtk_check_button_new_with_label("Orig Height");
    gtk_widget_ref(check_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "size_orig_h", check_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(check_button), "toggled",
		    GTK_SIGNAL_FUNC(on_size_orig_height_toggled), NULL);
    gtk_table_attach(GTK_TABLE(table), check_button, 2, 4, 1, 2, GTK_FILL,
		    GTK_SHRINK, 2, 2);
    gtk_widget_show(check_button);
    
    check_button = gtk_check_button_new_with_label("Absolute Width");
    gtk_widget_ref(check_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "size_abs_w", check_button,
			(GtkDestroyNotify)gtk_widget_unref);

    gtk_signal_connect(GTK_OBJECT(check_button), "toggled",
		    GTK_SIGNAL_FUNC(on_size_abs_w_toggled), NULL);
    gtk_table_attach(GTK_TABLE(table), check_button, 0, 2, 2, 3, GTK_FILL,
		    GTK_SHRINK, 2, 2);
    gtk_widget_show(check_button);
    
    check_button = gtk_check_button_new_with_label("Absolute Height");
    gtk_widget_ref(check_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "size_abs_h", check_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(check_button), "toggled",
		    GTK_SIGNAL_FUNC(on_size_abs_h_toggled), NULL);
    gtk_table_attach(GTK_TABLE(table), check_button, 2, 4, 2, 3, GTK_FILL,
		    GTK_SHRINK, 2, 2);
    gtk_widget_show(check_button);

    gtk_container_set_border_width(GTK_CONTAINER(table), 4);
    gtk_container_add(GTK_CONTAINER(frame), table);
    
    gtk_widget_show(table);
    gtk_widget_show(frame);
    return(frame);
}
static GtkWidget *
create_layer_fill_frame(GtkWidget *win)
{
    GtkWidget *frame, *table;
    GtkWidget *label;
    GtkWidget *spin_button, *check_button;
    GtkObject *o;

    frame = gtk_frame_new("Fill");
    table = gtk_table_new(2, 4, FALSE);

    /* left side */
    label = gtk_label_new("Width:");
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_EXPAND,
			GTK_SHRINK, 2, 2);
    gtk_widget_show(label);
    
    o = gtk_adjustment_new(0.0, 0.0, 1.0, 0.01, 0.1, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 0.01, 2);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "fill_w", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_fill_width_changed), NULL);
    gtk_table_attach(GTK_TABLE(table), spin_button, 1,2,0,1, GTK_EXPAND,
			GTK_SHRINK, 2, 2);
    gtk_widget_show(spin_button); 

    check_button = gtk_check_button_new_with_label("Orig Width");
    gtk_widget_ref(check_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "fill_orig_w", check_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(check_button), "toggled",
		    GTK_SIGNAL_FUNC(on_fill_orig_width_toggled), NULL);
    gtk_table_attach(GTK_TABLE(table), check_button, 0, 2, 1, 2, GTK_FILL,
			GTK_SHRINK, 2, 2);
    gtk_widget_show(check_button);
    
    /* right side */
    label = gtk_label_new("Height:");
    gtk_table_attach(GTK_TABLE(table), label, 2, 3, 0, 1, GTK_EXPAND, 
		    GTK_SHRINK, 2, 2);
    gtk_widget_show(label);
    
    o = gtk_adjustment_new(0.0, 0.0, 1.0, 0.01, 0.1, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 0.01, 2);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "fill_h", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_fill_height_changed), NULL);
    gtk_table_attach(GTK_TABLE(table), spin_button, 3,4,0,1, GTK_EXPAND, 
			GTK_SHRINK, 2, 2);
    gtk_widget_show(spin_button); 

    check_button = gtk_check_button_new_with_label("Orig Height");
    gtk_widget_ref(check_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "fill_orig_h", check_button,
			(GtkDestroyNotify)gtk_widget_unref);

    gtk_signal_connect(GTK_OBJECT(check_button), "toggled",
		    GTK_SIGNAL_FUNC(on_fill_orig_height_toggled), NULL);
    gtk_table_attach(GTK_TABLE(table), check_button, 2, 4, 1, 2, GTK_FILL,
		    GTK_SHRINK, 2, 2);
    gtk_widget_show(check_button);

    gtk_container_set_border_width(GTK_CONTAINER(table), 4);
    gtk_container_add(GTK_CONTAINER(frame), table);
    
    gtk_widget_show(table);
    gtk_widget_show(frame);
    return(frame);
}
static GtkWidget *
create_layer_gradient_frame(GtkWidget *win)
{
    GtkWidget *frame, *hbox, *vbox, *label, *spin_button;
    GtkWidget *color;
    GtkObject *o;

    frame = gtk_frame_new("Gradient Settings");
    vbox = gtk_vbox_new(FALSE, 0);
   
    hbox = gtk_hbox_new(FALSE, 0);
    label = gtk_label_new("Base Color");
    gtk_widget_show(label);

    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 2);
    
    
    /* top row */
    
    color = gtk_drawing_area_new();
    gtk_widget_ref(color);
    gtk_object_set_data_full(GTK_OBJECT(win), "gradient_one_color_box", color,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_drawing_area_size(GTK_DRAWING_AREA(color), 80, 20);
    gtk_widget_set_events(color, GDK_BUTTON_RELEASE_MASK | GDK_EXPOSE);
    gtk_signal_connect(GTK_OBJECT(color), "expose_event", 
		    GTK_SIGNAL_FUNC(gradient_one_color_drawing_area_expose),
		    NULL);
    gtk_signal_connect(GTK_OBJECT(color), "button_release_event", 
		    GTK_SIGNAL_FUNC(gradient_one_color_drawing_area_mouse_down),
		    NULL);
    gtk_widget_show(color);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(hbox), color, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 2);
    
    /* row 2 */
    hbox = gtk_hbox_new(FALSE, 0);
    label = gtk_label_new("Destination Color");
    gtk_widget_show(label);

    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 2);
    
    color = gtk_drawing_area_new();
    gtk_widget_ref(color);
    gtk_object_set_data_full(GTK_OBJECT(win), "gradient_two_color_box", color,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_drawing_area_size(GTK_DRAWING_AREA(color), 80, 20);
    gtk_widget_set_events(color, GDK_BUTTON_RELEASE_MASK | GDK_EXPOSE);
    gtk_signal_connect(GTK_OBJECT(color), "expose_event", 
		    GTK_SIGNAL_FUNC(gradient_two_color_drawing_area_expose), 
		    NULL);
    gtk_signal_connect(GTK_OBJECT(color), "button_release_event", 
		    GTK_SIGNAL_FUNC(gradient_two_color_drawing_area_mouse_down),
		    NULL);
    gtk_widget_show(color);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(hbox), color, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 2);
    
    /* row 3 */
    hbox = gtk_hbox_new(FALSE, 0);
    label = gtk_label_new("Angle");
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 2);
    
    o = gtk_adjustment_new(0, 0, 360, 1, 5, 2.5);
    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(o), 1, 0);
    
    gtk_widget_ref(spin_button);
    gtk_object_set_data_full(GTK_OBJECT(win), "gradient_angle", spin_button,
			(GtkDestroyNotify)gtk_widget_unref);
    gtk_signal_connect(GTK_OBJECT(spin_button), "changed",
		    GTK_SIGNAL_FUNC(on_gradient_angle_changed), NULL);
    
    gtk_widget_show(spin_button); 
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(hbox), spin_button, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 2);
     
    
    /* show the big guys */
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 4);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    return(frame);
}
/**
 * create_ebony_controls_page - Create ebg Advanced notebook tab
 * gdk_rgb_init called here 
 */
static GtkWidget *
create_ebony_controls_page(GtkWidget *win)
{
    GtkWidget *w, *vbox;
    GtkWidget *layer_frame, *frame;
    
    /* the main widget we're returning */
    w = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(w), 5);
    
    /* start gdk_rgb for the color selections */
    gdk_rgb_init();


    /* top settings */
    frame = create_layer_frame(win); 
    gtk_box_pack_start(GTK_BOX(w), frame, FALSE, FALSE, 1);
    
    /* layer settings */
    layer_frame = gtk_frame_new("Layer Settings");
    gtk_box_pack_start(GTK_BOX(w), layer_frame, TRUE, TRUE, 1);
    gtk_frame_set_label_align(GTK_FRAME(layer_frame), 0.5, 0.5);
    
    /* Layer manipulations vbox holds all inside of "Layer Settings" */ 
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 3);
    
    /* ROW1 */
    frame = create_layer_type_buttons(win);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);
    /* ROW2 */
    frame = create_layer_image_file_frame(win);    
    gtk_widget_ref(frame);
    gtk_object_set_data_full(GTK_OBJECT(win), "image_file_frame", frame,
			    (GtkDestroyNotify)gtk_widget_unref);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);
    /* GRADIENTROW */
    frame = create_layer_gradient_frame(win);
    gtk_widget_ref(frame);
    gtk_object_set_data_full(GTK_OBJECT(win), "gradient_frame", frame,
			    (GtkDestroyNotify)gtk_widget_unref);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);

    /* ROW3 */
    frame = create_layer_color_frame(win); 
    gtk_widget_ref(frame);
    gtk_object_set_data_full(GTK_OBJECT(win), "layer_color_frame", frame,
			    (GtkDestroyNotify)gtk_widget_unref);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);
    /* ROW4 */
    frame = create_layer_color_class_frame(win);
    gtk_widget_ref(frame);
    gtk_object_set_data_full(GTK_OBJECT(win), "layer_color_class_frame", frame,
			    (GtkDestroyNotify)gtk_widget_unref);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);
    /* ROW 5 */
    frame = create_layer_scroll_follow_frame(win);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);
    /* ROW 6 */ 
    frame = create_layer_position_frame(win);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);
    /* ROW 7 */
    frame = create_layer_size_frame(win);
    gtk_widget_ref(frame);
    gtk_object_set_data_full(GTK_OBJECT(win), "layer_size_frame", frame,
			    (GtkDestroyNotify)gtk_widget_unref);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);
    /* ROW 8 */
    frame = create_layer_fill_frame(win);
    gtk_widget_ref(frame);
    gtk_object_set_data_full(GTK_OBJECT(win), "layer_fill_frame", frame,
			    (GtkDestroyNotify)gtk_widget_unref);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 3);

    /* put the layer settings vbox in the frame */
    gtk_container_add(GTK_CONTAINER(layer_frame), vbox);

    /* show these bad boys */
    gtk_widget_show(vbox);
    gtk_widget_show(layer_frame);
    
    return(w);
}

/**
 * create_ebony_bg_controls - create the right frame and notebook for it
 * @win - a pointer to the GtkWidget that will eventually be the "window"
 * Returns a pointer to the notebook that can be packed and shown
 */
static GtkWidget *
create_ebony_bg_controls(GtkWidget *win)
{
    GtkWidget *result;
    GtkWidget *bgm, *bgc;
    GtkWidget *label;

    result = gtk_notebook_new();
    gtk_widget_ref(result);
    gtk_object_set_data_full(GTK_OBJECT(win), "e_notebook", result,
			(GtkDestroyNotify) gtk_widget_unref);
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(result), GTK_POS_TOP);
    
    label = gtk_label_new("Browse");

    bgm = create_ebony_manager_page(win);
    gtk_notebook_insert_page(GTK_NOTEBOOK(result), bgm, label, 0);
    gtk_widget_show(label);
    gtk_widget_show(bgm);

    label = gtk_label_new("Advanced");
    bgc = create_ebony_controls_page(win);
    gtk_notebook_insert_page(GTK_NOTEBOOK(result), bgc, label, 1);
    gtk_widget_show(label);
    gtk_widget_show(bgc);
    
    return(result);
}

/**
 * create_ebony_main_section - Create the "center" section of the app
 * return - GtkWidget* the hpaned widget that fill the center of the app
 */
static GtkWidget *
create_ebony_main_section(GtkWidget *win)
{
    GtkWidget *result;
    GtkWidget *p1, *da;
    GtkWidget *align;
    GtkWidget *rpd;	/* right pane data */

    result = gtk_hpaned_new();
    p1 = gtk_vpaned_new();
    align = gtk_alignment_new(1.0, 0.0, 0.0, 1.0);
    rpd = create_ebony_bg_controls(win);
    gtk_container_add(GTK_CONTAINER(align), rpd);
    gtk_paned_pack1(GTK_PANED(result), p1, TRUE, FALSE);
    gtk_paned_pack2(GTK_PANED(result), align, FALSE, FALSE);
    
    da = gtk_drawing_area_new();
    gtk_widget_ref(da);
    gtk_object_set_data_full(GTK_OBJECT(win), "evas", da,
			    (GtkDestroyNotify) gtk_widget_unref);
    
    gtk_paned_pack1(GTK_PANED(p1), da, TRUE, FALSE);
    gtk_widget_set_usize(da, 320, 240);
    gtk_widget_set_events(da, GDK_EXPOSURE_MASK | GDK_CONFIGURE);
    gtk_signal_connect(GTK_OBJECT(da), "configure_event",
		    GTK_SIGNAL_FUNC(drawing_area_configure_event), NULL);
    gtk_signal_connect(GTK_OBJECT(da), "expose_event",
		    GTK_SIGNAL_FUNC(drawing_area_expose_event), NULL);
    
    gtk_widget_show(rpd);
    gtk_widget_show(da);
    gtk_widget_show(align);
    gtk_widget_show(p1);

    return (result);
}

/**
 * create_ebony_window - Create the entire app and return the widget
 */
GtkWidget *
create_ebony_window(void)
{
    GtkWidget *result;
    GtkWidget *vbox;
    GtkWidget *menu;
    GtkWidget *hpaned;
    GtkWidget *sb;
    
    result = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(result), "Ebony - ");
    gtk_window_set_wmclass(GTK_WINDOW(result), "Ebony", "Main");
    gtk_object_set_data(GTK_OBJECT(result), "win", result);
    gtk_signal_connect(GTK_OBJECT(result), "delete_event",
		    GTK_SIGNAL_FUNC(app_exit), NULL);

    menu = create_ebony_filemenu();
    hpaned = create_ebony_main_section(result);
    
    vbox = gtk_vbox_new(FALSE, 1);
    sb = gtk_statusbar_new();
    ebony_status = sb;

    gtk_box_pack_start(GTK_BOX(vbox), menu, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hpaned, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), sb, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(result), vbox);
    
    gtk_widget_show(menu);
    gtk_widget_show(hpaned);
    gtk_widget_show(vbox);
    gtk_widget_show(sb);
    
    return(result);
}

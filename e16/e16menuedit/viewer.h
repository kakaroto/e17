GtkWidget * create_main_window(void);
void on_exit_application(GtkWidget * widget, gpointer user_data);
void on_select_submenu_box(GtkWidget *widget, gpointer user_data);
void load_menus_from_disk(void);

GtkTooltips *tooltips;
GtkAccelGroup *accel_group;


GtkWidget * create_main_window(void);
void on_exit_application(GtkWidget * widget, gpointer user_data);
void on_select_submenu_box(GtkWidget *widget, gpointer user_data);
void load_menus_from_disk(void);
void load_new_menu_from_disk(char *file_to_load, GtkCTreeNode *my_parent);
void selection_made(GtkCTree *my_ctree, GList *node, gint column,
		gpointer user_data);

GtkTooltips *tooltips;
GtkAccelGroup *accel_group;


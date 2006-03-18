GtkWidget          *create_list_window(void);
void                on_exit_application(GtkWidget * widget, gpointer user_data);
void                selection_made(GtkWidget * my_clist, gint row, gint column,
				   GdkEventButton * event, gpointer data);
void                on_resort_columns(GtkWidget * widget, gint column,
				      gpointer user_data);
void                e_cb_modifier(GtkWidget * widget, gpointer data);
void                change_action(GtkWidget * my_clist, gint row, gint column,
				  GdkEventButton * event, gpointer data);
void                on_change_params(GtkWidget * widget, gpointer user_data);
void                e_cb_key_change(GtkWidget * widget, gpointer data);
void                on_save_data(GtkWidget * widget, gpointer data);
void                on_delete_row(GtkWidget * widget, gpointer user_data);
void                on_create_row(GtkWidget * widget, gpointer user_data);

GtkTooltips        *tooltips;
GtkAccelGroup      *accel_group;

GtkWidget *create_list_window(void);
void on_exit_application(GtkWidget * widget, gpointer user_data);
void selection_made(GtkWidget *my_clist, gint row, gint column,
	   	GdkEventButton *event, gpointer data);
void on_resort_columns(GtkWidget *widget, gint column, gpointer user_data);
void e_cb_modifier(GtkWidget * widget, gpointer data);
char *atword(char *s, int num);
void change_action(GtkWidget *my_clist, gint row, gint column, 
		GdkEventButton *event, gpointer data);
void on_change_params(GtkWidget *widget, gpointer user_data);

GtkTooltips *tooltips;
GtkAccelGroup *accel_group;


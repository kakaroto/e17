GtkWidget *create_main_window (void);
void on_exit_application (GtkWidget * widget, gpointer user_data);
void load_menus_from_disk (void);
void load_new_menu_from_disk (char *file_to_load, GtkCTreeNode * my_parent);
void selection_made (GtkCTree * my_ctree, GList * node,
		     gint column, gpointer user_data);

GtkTooltips *tooltips;
GtkAccelGroup *accel_group;
void save_menus (GtkWidget * widget, gpointer user_data);
gint write_menu (GNode * node, gchar * filename);
void write_menu_title (GNode * node, FILE * fp);
gint write_menu_entry (GNode * node, FILE * fp);
gboolean tree_to_gnode (GtkCTree * ctree,
			guint depth,
			GNode * gnode, GtkCTreeNode * cnode, gpointer data);
void quit_cb (GtkWidget * widget, gpointer user_data);
struct entry_data
{
  gchar *desc;
  gchar *icon;
  gchar *params;
};
void delete_entry (GtkWidget * widget, gpointer user_data);
void insert_entry (GtkWidget * widget, gpointer user_data);
void entries_to_ctree (GtkWidget * widget, gpointer user_data);
void destroy_node_data (GNode * node);
void save_menus_quit (GtkWidget * widget, gpointer user_data);
void real_save_menus (gint exit);
void cb_icon_browse (GtkWidget * widget, gpointer user_data);
void cb_icon_browse_ok (GtkWidget * widget, gpointer user_data);
void cb_icon_browse_cancel (GtkWidget * widget, gpointer user_data);
void cb_exec_browse (GtkWidget * widget, gpointer user_data);
void cb_exec_browse_ok (GtkWidget * widget, gpointer user_data);
void cb_exec_browse_cancel (GtkWidget * widget, gpointer user_data);
void status_message (gchar * message, gint delay);
gint status_clear (gpointer user_data);

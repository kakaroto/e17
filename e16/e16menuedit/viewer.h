GtkWidget          *create_main_window(void);
void                on_exit_application(GtkWidget * widget, gpointer user_data);
void                on_select_submenu_box(GtkWidget * widget,

					  gpointer user_data);
void                load_menus_from_disk(void);
void                load_new_menu_from_disk(char *file_to_load,

					    GtkCTreeNode * my_parent);
void                selection_made(GtkCTree * my_ctree, GList * node,
				   gint column, gpointer user_data);

GtkTooltips        *tooltips;
GtkAccelGroup      *accel_group;
void                save_menus(GtkWidget * widget, gpointer user_data);
gint                write_menu(GNode * node, gchar * filename);
void                write_menu_title(GNode * node, FILE * fp);
gint write_menu_entry(GNode * node, FILE * fp);
gboolean            tree_to_gnode(GtkCTree * ctree,
				  guint depth,
				  GNode * gnode, GtkCTreeNode * cnode,

				  gpointer data);
void                quit_cb(GtkWidget * widget, gpointer user_data);
struct entry_data
{
   gchar              *desc;
   gchar              *icon;
   gchar              *params;
};

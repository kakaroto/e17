#include <gdk_imlib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "viewer.h"
#include "menus.h"
#include "file.h"

extern GtkTooltips *tooltips;
extern GtkAccelGroup *accel_group;
GtkWidget *win;
GtkWidget *txt_description;
GtkWidget *txt_icon;
GtkWidget *txt_exec;
GtkWidget *ctree;
GtkWidget *btn_browse;
GtkWidget *btn_browse2;
GtkWidget *lbl_params;
GtkWidget *statusbar;

static void on_resort_columns(GtkWidget *widget, gint column, gpointer user_data);

void
load_new_menu_from_disk (char *file_to_load, GtkCTreeNode * my_parent)
{

  FILE *menufile;
  char buf[1024];
  char first = 1;
  char s[4096];

  if (!file_to_load)
    return;
  if (file_to_load[0] != '/')
    sprintf (buf, "%s/.enlightenment/%s", homedir (getuid ()), file_to_load);
  else
    sprintf (buf, "%s", file_to_load);

  menufile = fopen (buf, "r");
  if (!menufile)
    return;

  gtk_clist_freeze (GTK_CLIST (ctree));

  while (fgets (s, 4096, menufile))
    {
      s[strlen (s) - 1] = 0;
      if ((s[0] && s[0] != '#'))
	{
	  if (first)
	    first = 0;
	  else
	    {
	      char *txt = NULL, *icon = NULL, *act = NULL, *params = NULL;
	      gchar *text[3];

	      GtkCTreeNode *current;

	      txt = field (s, 0);
	      icon = field (s, 1);
	      act = field (s, 2);
	      params = field (s, 3);

	      text[0] = txt;
	      if (!icon)
		icon = g_strdup ("");
	      text[1] = icon;
	      if (!params)
		params = g_strdup ("");
	      text[2] = params;

	      /* printf("subitem: %s, %s, %s, %s\n",txt,icon,act,params); */
	      current = gtk_ctree_insert_node (GTK_CTREE (ctree), my_parent,
					       NULL, text, 5, NULL, NULL,
					       NULL, NULL, FALSE, FALSE);
	      if (!strcasecmp (act, "menu"))
		load_new_menu_from_disk (params, current);

	      if (txt)
		g_free (txt);
	      if (icon)
		g_free (icon);
	      if (act)
		g_free (act);
	      if (params)
		g_free (params);
	    }
	}
    }

  fclose (menufile);

  gtk_clist_thaw (GTK_CLIST (ctree));

  return;
}

void
load_menus_from_disk (void)
{

  FILE *menufile;
  char buf[1024];
  char first = 1;
  char s[4096];
  GtkCTreeNode *parent = NULL;

  /* currently hardcoded, but not a big issue to change later */
  sprintf (buf, "%s/.enlightenment/file.menu", homedir (getuid ()));
  menufile = fopen (buf, "r");
  if (!menufile)
    {
      printf ("hmm. looks like you have some \"issues\" as you don't have\n"
	      "a %s file.  Sucks to be you\n", buf);
      gtk_exit (1);
    }

  while (fgets (s, 4096, menufile))
    {
      s[strlen (s) - 1] = 0;
      if ((s[0] && s[0] != '#'))
	{
	  if (first)
	    {
	      gchar *text[3];
	      char *txt = NULL;
	      char *txt2 = NULL;
	      char *txt3 = NULL;

	      txt = field (s, 0);
	      text[0] = txt;
	      txt2 = g_strdup ("");
	      text[1] = txt2;
	      txt3 = g_strdup (buf);
	      text[2] = txt3;

	      parent = gtk_ctree_insert_node (GTK_CTREE (ctree), NULL, NULL,
					      text, 5, NULL, NULL, NULL,
					      NULL, FALSE, TRUE);
	      /* printf("mainitem: %s, %s, %s, %s\n",txt,txt2,txt3,txt4); */

	      if (txt)
		g_free (txt);
	      if (txt2)
		g_free (txt2);
	      if (txt3)
		g_free (txt3);

	      first = 0;
	    }
	  else
	    {
	      char *txt = NULL, *icon = NULL, *act = NULL, *params = NULL;
	      gchar *text[3];
	      GtkCTreeNode *current;

	      txt = field (s, 0);
	      icon = field (s, 1);
	      act = field (s, 2);
	      params = field (s, 3);

	      text[0] = txt;
	      if (!icon)
		icon = g_strdup ("");
	      text[1] = icon;
	      if (!params)
		params = g_strdup ("");
	      text[2] = params;

	      /* printf("subitem: %s, %s, %s, %s\n",txt,icon,act,params); */
	      current =
		gtk_ctree_insert_node (GTK_CTREE (ctree), parent, NULL, text,
				       5, NULL, NULL, NULL, NULL, FALSE,
				       FALSE);

	      if (!strcasecmp (act, "menu"))
		load_new_menu_from_disk (params, current);

	      if (txt)
		g_free (txt);
	      if (icon)
		g_free (icon);
	      if (act)
		g_free (act);
	      if (params)
		g_free (params);

	    }
	}
    }

  fclose (menufile);

  return;
}

void
selection_made (GtkCTree * my_ctree, GList * node, gint column,
		gpointer user_data)
{
  static int first = 1;
  gchar *col1 = NULL;
  gchar *col2 = NULL;
  gchar *col3 = NULL;
  gchar *source = NULL;
  GtkCTreeNode *last_node = NULL;

  if (first)
    {
      gtk_widget_set_sensitive (GTK_WIDGET (txt_exec), TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (txt_icon), TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (txt_description), TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (btn_browse), TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET (btn_browse2), TRUE);
      first = 0;
    }
  last_node = GTK_CTREE_NODE ((GTK_CLIST (ctree)->selection)->data);
  gtk_ctree_node_get_text (GTK_CTREE (ctree), GTK_CTREE_NODE (last_node), 0,
			   &col1);
  gtk_ctree_node_get_text (GTK_CTREE (ctree), GTK_CTREE_NODE (last_node), 1,
			   &col2);
  gtk_ctree_node_get_text (GTK_CTREE (ctree), GTK_CTREE_NODE (last_node), 2,
			   &col3);
  gtk_ctree_get_node_info (GTK_CTREE (ctree), GTK_CTREE_NODE (last_node),
			   &source, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  gtk_entry_set_text (GTK_ENTRY (txt_description), source);
  gtk_entry_set_text (GTK_ENTRY (txt_icon), col2);
  gtk_entry_set_text (GTK_ENTRY (txt_exec), col3);
  if (GTK_CTREE_ROW (last_node)->children)
    gtk_label_set_text (GTK_LABEL (lbl_params), "Submenu");
  else
    gtk_label_set_text (GTK_LABEL (lbl_params), "Executes");

  return;
  user_data = NULL;
  my_ctree = NULL;
  column = 0;
  node = NULL;
}

GtkWidget *
create_main_window (void)
{
  GtkWidget *bigvbox;
  GtkWidget *menubar;
  GtkWidget *panes;
  GtkWidget *scrollybit;
  GtkWidget *vbox;
  GtkWidget *vbox2;
  GtkWidget *table;
  GtkWidget *frames;
  GtkWidget *button;
  GtkWidget *entry;
  GtkWidget *label;
  GtkWidget *alignment;
  GtkWidget *hbox;
  GtkWidget *menu;
  GtkWidget *menuitem;


  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GTK_WIDGET_SET_FLAGS (win, GTK_CAN_FOCUS);
  GTK_WIDGET_SET_FLAGS (win, GTK_CAN_DEFAULT);
  gtk_window_set_policy (GTK_WINDOW (win), TRUE, TRUE, FALSE);
  gtk_window_set_title (GTK_WINDOW (win), "E Menu Editor");
  gtk_window_set_wmclass (GTK_WINDOW (win), "e16menuedit", "e16menuedit");
  gtk_widget_set_usize (win, 660, 300);

  bigvbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (bigvbox);
  gtk_container_add (GTK_CONTAINER (win), bigvbox);

  menubar = gtk_menu_bar_new ();
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (bigvbox), menubar, FALSE, FALSE, 0);

  menu = CreateBarSubMenu (menubar, "File");
  menuitem =
    CreateMenuItem (menu, "Save", "", "Save Current Data", save_menus,
		    "save data");
  menuitem =
    CreateMenuItem (menu, "Save & Quit", "",
		    "Save Current Data & Quit Application", save_menus_quit,
		    "save quit");
  menuitem =
    CreateMenuItem (menu, "Quit", "", "Quit Without Saving", quit_cb,
		    "quit program");

  /* I hate right aligned menus =) It just puts stuff out of the way */
  menu = CreateBarSubMenu (menubar, "Help");
  menuitem = CreateMenuItem (menu, "About", "", "About E Menu Editor",
			     NULL, "about");
  menuitem = CreateMenuItem (menu, "Documentation", "",
			     "Read the Menu Editor Documentation", NULL,
			     "read docs");

  panes = gtk_hpaned_new ();
  gtk_widget_show (panes);
  gtk_paned_set_gutter_size (GTK_PANED (panes), 10);
  gtk_box_pack_start (GTK_BOX (bigvbox), panes, TRUE, TRUE, 0);

  statusbar = gtk_statusbar_new ();
  gtk_widget_show (statusbar);
  gtk_box_pack_start (GTK_BOX (bigvbox), statusbar, FALSE, FALSE, 0);

  scrollybit = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrollybit);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollybit),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_paned_pack1 (GTK_PANED (panes), scrollybit, TRUE, TRUE);

  ctree = gtk_ctree_new (3, 0);
  gtk_ctree_set_line_style (GTK_CTREE (ctree), GTK_CTREE_LINES_DOTTED);
  gtk_clist_set_column_auto_resize (GTK_CLIST (ctree), 0, TRUE);
  gtk_clist_set_column_title (GTK_CLIST (ctree), 0, "Description");
  gtk_clist_set_column_title (GTK_CLIST (ctree), 1, "Icon");
  gtk_clist_set_column_title (GTK_CLIST (ctree), 2, "Params");
  gtk_clist_column_titles_show (GTK_CLIST (ctree));
  gtk_container_add (GTK_CONTAINER (scrollybit), ctree);
  gtk_clist_set_reorderable (GTK_CLIST (ctree), TRUE);
  gtk_signal_connect (GTK_OBJECT (ctree), "tree-select-row",
		      GTK_SIGNAL_FUNC (selection_made), NULL);
  gtk_signal_connect(GTK_OBJECT(ctree), "click_column",
            GTK_SIGNAL_FUNC(on_resort_columns), NULL);

  gtk_widget_show (ctree);

  vbox = gtk_vbox_new (FALSE, 3);

  gtk_widget_show (vbox);
  gtk_paned_pack2 (GTK_PANED (panes), vbox, FALSE, FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 2);

  frames = gtk_frame_new ("Edit Menu Item Properties");
  gtk_container_set_border_width (GTK_CONTAINER (frames), 2);
  gtk_widget_show (frames);
  gtk_box_pack_start (GTK_BOX (vbox), frames, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 3);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frames), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);

  table = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table);
  gtk_table_set_row_spacings (GTK_TABLE (table), 3);
  gtk_table_set_col_spacings (GTK_TABLE (table), 3);
  gtk_box_pack_start (GTK_BOX (vbox2), table, FALSE, FALSE, 2);

  alignment = gtk_alignment_new (1.0, 0.5, 0, 0);
  gtk_widget_show (alignment);
  label = gtk_label_new ("Description:");
  gtk_widget_show (label);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
  gtk_container_add (GTK_CONTAINER (alignment), label);
  gtk_table_attach (GTK_TABLE (table), alignment, 0, 1, 0, 1,
		    GTK_FILL, (GtkAttachOptions) (0), 0, 0);

  alignment = gtk_alignment_new (1.0, 0.5, 0, 0);
  gtk_widget_show (alignment);
  label = gtk_label_new ("Icon:");
  gtk_widget_show (label);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
  gtk_container_add (GTK_CONTAINER (alignment), label);
  gtk_table_attach (GTK_TABLE (table), alignment, 0, 1, 1, 2,
		    GTK_FILL, (GtkAttachOptions) (0), 0, 0);

  alignment = gtk_alignment_new (1.0, 0.5, 0, 0);
  gtk_widget_show (alignment);
  lbl_params = gtk_label_new ("Executes:");
  gtk_widget_show (lbl_params);
  gtk_label_set_justify (GTK_LABEL (lbl_params), GTK_JUSTIFY_RIGHT);
  gtk_container_add (GTK_CONTAINER (alignment), lbl_params);
  gtk_table_attach (GTK_TABLE (table), alignment, 0, 1, 2, 3,
		    GTK_FILL, (GtkAttachOptions) (0), 0, 0);

  txt_description = entry = gtk_entry_new_with_max_length (200);
  gtk_widget_show (entry);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 3, 0, 1,
		    GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);
  gtk_signal_connect_after (GTK_OBJECT (txt_description), "key_press_event",
			    GTK_SIGNAL_FUNC (entries_to_ctree), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (txt_description), FALSE);

  txt_icon = entry = gtk_entry_new_with_max_length (200);
  gtk_widget_show (entry);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2,
		    GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);
  gtk_signal_connect_after (GTK_OBJECT (txt_icon), "key_press_event",
			    GTK_SIGNAL_FUNC (entries_to_ctree), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (txt_icon), FALSE);

  btn_browse = gtk_button_new_with_label ("Browse");
  gtk_widget_show (btn_browse);
  gtk_table_attach (GTK_TABLE (table), btn_browse, 2, 3, 1, 2,
		    (GtkAttachOptions) 0, (GtkAttachOptions) (0), 0, 0);
  gtk_signal_connect (GTK_OBJECT (btn_browse), "clicked",
		      GTK_SIGNAL_FUNC (cb_icon_browse), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (btn_browse), FALSE);

  txt_exec = entry = gtk_entry_new_with_max_length (200);
  gtk_widget_show (entry);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3,
		    GTK_EXPAND | GTK_FILL, (GtkAttachOptions) (0), 0, 0);
  gtk_signal_connect_after (GTK_OBJECT (txt_exec), "key_press_event",
			    GTK_SIGNAL_FUNC (entries_to_ctree), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (txt_exec), FALSE);

  btn_browse2 = gtk_button_new_with_label ("Browse");
  gtk_widget_show (btn_browse2);
  gtk_table_attach (GTK_TABLE (table), btn_browse2, 2, 3, 2, 3,
		    (GtkAttachOptions) 0, (GtkAttachOptions) (0), 0, 0);
  gtk_signal_connect (GTK_OBJECT (btn_browse2), "clicked",
		      GTK_SIGNAL_FUNC (cb_exec_browse), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (btn_browse2), FALSE);

  hbox = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 2);

  button = gtk_button_new_with_label (" Insert Menu Entry ");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (insert_entry), NULL);

  button = gtk_button_new_with_label (" Delete Menu Entry ");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (delete_entry), NULL);

  button = gtk_button_new_with_label (" Save ");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (save_menus), NULL);

  button = gtk_button_new_with_label (" Quit ");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (quit_cb), NULL);

  return win;
}

/* Stick the contents of the entries into the tree */
void
entries_to_ctree (GtkWidget * widget, gpointer user_data)
{
  GtkCTreeNode *node;

  node = GTK_CLIST (ctree)->selection->data;

  if (!node)
    return;

  gtk_ctree_node_set_text (GTK_CTREE (ctree), node, 0,
			   gtk_entry_get_text (GTK_ENTRY (txt_description)));
  gtk_ctree_node_set_text (GTK_CTREE (ctree), node, 1,
			   gtk_entry_get_text (GTK_ENTRY (txt_icon)));
  gtk_ctree_node_set_text (GTK_CTREE (ctree), node, 2,
			   gtk_entry_get_text (GTK_ENTRY (txt_exec)));

  return;
  widget = NULL;
  user_data = NULL;
}

void
insert_entry (GtkWidget * widget, gpointer user_data)
{
  GtkCTreeNode *newparent = NULL;
  GtkCTreeNode *newnode = NULL;
  GtkCTreeNode *newp = NULL, *news = NULL;
  gchar *text[3];

  text[0] = g_strdup ("New Entry");
  text[1] = g_strdup ("");
  text[2] = g_strdup ("");

  if (GTK_CLIST (ctree)->selection)
    {
      newparent = GTK_CLIST (ctree)->selection->data;

      /* Move the entry to the position just below the selected node. */
      newp = GTK_CTREE_ROW (newparent)->parent;
      news = GTK_CTREE_ROW (newparent)->sibling;
      if (news == newnode)
	news = NULL;
    }

  if (newp == NULL)
    newp = gtk_ctree_node_nth (GTK_CTREE (ctree), 0);

  newnode =
    gtk_ctree_insert_node (GTK_CTREE (ctree),
			   newp, news, text, 0, NULL, NULL,
			   NULL, NULL, FALSE, TRUE);

  g_free (text[0]);
  g_free (text[1]);
  g_free (text[2]);

  gtk_ctree_select (GTK_CTREE (ctree), newnode);

  return;
  widget = NULL;
  user_data = NULL;
}

void
delete_entry (GtkWidget * widget, gpointer user_data)
{
  if (GTK_CLIST (ctree)->selection)
    {
      GtkCTreeNode *node;
      GtkCTreeNode *next;
      GtkCTreeNode *parent;
      node = GTK_CTREE_NODE (GTK_CLIST (ctree)->selection->data);
      next = GTK_CTREE_ROW (node)->sibling;
      parent = GTK_CTREE_ROW (node)->parent;
      gtk_ctree_remove_node (GTK_CTREE (ctree), node);
      if (!next && parent)
	{
	  /* Why does this have to be so painful? */
	  next = GTK_CTREE_ROW (parent)->children;
	  if (next)
	    while (GTK_CTREE_ROW (next)->sibling)
	      next = GTK_CTREE_ROW (next)->sibling;
	}
      if (next)
	gtk_ctree_select (GTK_CTREE (ctree), next);
    }
  return;
  widget = NULL;
  user_data = NULL;
}

void
quit_cb (GtkWidget * widget, gpointer user_data)
{
  gtk_exit (0);
  return;
  widget = NULL;
  user_data = NULL;
}

void
save_menus (GtkWidget * widget, gpointer user_data)
{
  real_save_menus (0);
  return;
  widget = NULL;
  user_data = NULL;
}

void
save_menus_quit (GtkWidget * widget, gpointer user_data)
{
  real_save_menus (1);
  return;
  widget = NULL;
  user_data = NULL;
}

void
real_save_menus (gint exit)
{
  GNode *node;
  gchar *buf;
  gint retval = 0;

  buf =
    g_strjoin ("/", homedir (getuid ()), ".enlightenment", "file.menu", NULL);

  node =
    gtk_ctree_export_to_gnode (GTK_CTREE (ctree), NULL, NULL,
			       gtk_ctree_node_nth (GTK_CTREE (ctree), 0),
			       tree_to_gnode, NULL);

  retval = write_menu (node, buf);

  g_free (buf);

  if (node)
    {
      destroy_node_data (node);
      g_node_destroy (node);
    }

  if (retval)
    status_message ("Couldn't save all menu entries.", 3000);
  else
    {
      status_message ("Menus saved successfully", 3000);
      if (exit)
	gtk_exit (0);
    }
}

/* recursive */
void
destroy_node_data (GNode * node)
{
  GNode *parent;
  parent = node;
  while (1)
    {
      if (parent->children)
	destroy_node_data (parent->children);
      if (parent->next)
	parent = parent->next;
      else
	break;
    }
  if (node->data)
    {
      struct entry_data *data;
      data = node->data;

      if (data->desc)
	g_free (data->desc);
      if (data->icon)
	g_free (data->icon);
      if (data->params)
	g_free (data->params);
      g_free (data);
    }
}

/* Get those annoyingly painful CTreeNodes into a useable format */
gboolean
tree_to_gnode (GtkCTree * ctree,
	       guint depth, GNode * gnode, GtkCTreeNode * cnode,
	       gpointer data)
{
  struct entry_data *edata;
  gchar *col1, *col2, *col3;
  gtk_ctree_node_get_text (GTK_CTREE (ctree), GTK_CTREE_NODE (cnode), 1,
			   &col2);
  gtk_ctree_node_get_text (GTK_CTREE (ctree), GTK_CTREE_NODE (cnode), 2,
			   &col3);
  gtk_ctree_get_node_info (GTK_CTREE (ctree), GTK_CTREE_NODE (cnode),
			   &col1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if ((col1 == NULL) || col1[0] == '\0')
    {
      printf ("e16menuedit ERROR\n");
      printf
	("Entry with description ->%s<-, icon ->%s<-, and parameters ->%s<-\n",
	 col1, col2, col3);
      printf ("You can't have a description-less entry!\n"
	      "That just won't do. I'm omitting this entry.\n");
      return FALSE;
    }
  else if ((col3 == NULL) || (col3[0] == '\0'))
    {
      printf ("e16menuedit ERROR\n");
      printf
	("Entry with description ->%s<-, icon ->%s<-, and parameters ->%s<-\n",
	 col1, col2, col3);
      printf ("You can't have an entry with no parameters!\n"
	      "If it's a submenu, you *must* specify a file to "
	      "store the submenu in.\nI'm omitting this entry.\n");
      return FALSE;
    }
  edata = g_malloc (sizeof (struct entry_data));
  edata->desc = g_strdup (col1);
  edata->icon = g_strdup (col2);
  edata->params = g_strdup (col3);
  gnode->data = edata;
  return TRUE;
  depth = 0;
  data = NULL;
}

/* Next two functions are co-recursing */
gint
write_menu (GNode * node, gchar * file)
{
  GNode *ptr;
  FILE *fp = NULL;
  gchar *realfile;
  if (!(node && file))
    {
      printf ("either node or file is null\n");
      return 1;
    }

  if (file[0] != '/')
    {
      /* Tarnation! A relative path */
      realfile =
	g_strjoin ("/", homedir (getuid ()), ".enlightenment", file, NULL);
    }
  else
    realfile = g_strdup (file);

  if ((fp = fopen (realfile, "w")) == NULL)
    {
      printf ("failed to create file %s\n", file);
      return 1;
    }

  g_free (realfile);
  write_menu_title (node, fp);
  node = node->children;
  for (ptr = node; ptr; ptr = ptr->next)
    if (write_menu_entry (ptr, fp))
      {
	printf ("error writing entry\n");
	return 1;
      }
  fclose (fp);

  return 0;
}

gint
write_menu_entry (GNode * node, FILE * fp)
{
  struct entry_data *dat;

  dat = (struct entry_data *) node->data;
  if (G_NODE_IS_LEAF (node))
    {
      /* It's an entry */
      fprintf (fp, "\"%s\"\t%s\texec\t\"%s\"\n",
	       dat->desc[0] == '\0' ? "NULL" : dat->desc,
	       dat->icon[0] == '\0' ? "NULL" : dat->icon,
	       dat->params[0] == '\0' ? "" : dat->params);
    }
  else
    {
      /* It's a submenu */
      fprintf (fp, "\"%s\"\t%s\tmenu\t\"%s\"\n",
	       dat->desc[0] == '\0' ? "NULL" : dat->desc,
	       dat->icon[0] == '\0' ? "NULL" : dat->icon,
	       dat->params[0] == '\0' ? "" : dat->params);
      if (write_menu (node, dat->params))
	{
	  printf ("error writing menu\n");
	  return 1;
	}
    }
  return 0;
}

void
write_menu_title (GNode * node, FILE * fp)
{
  fprintf (fp, "\"%s\"\n", ((struct entry_data *) (node->data))->desc);
}

void
on_exit_application (GtkWidget * widget, gpointer user_data)
{
  gtk_exit (0);
  return;
  user_data = NULL;
  widget = NULL;
}

int
main (int argc, char *argv[])
{
  GtkWidget *main_win;
  gtk_set_locale ();
  gtk_init (&argc, &argv);
  tooltips = gtk_tooltips_new ();
  accel_group = gtk_accel_group_new ();
  main_win = create_main_window ();
  gtk_signal_connect (GTK_OBJECT (main_win), "destroy",
		      GTK_SIGNAL_FUNC (on_exit_application), NULL);
  gtk_signal_connect (GTK_OBJECT (main_win), "delete_event",
		      GTK_SIGNAL_FUNC (on_exit_application), NULL);
  load_menus_from_disk ();
  gtk_ctree_collapse_recursive (GTK_CTREE (ctree),
				gtk_ctree_node_nth (GTK_CTREE (ctree), 0));
  gtk_ctree_expand (GTK_CTREE (ctree),
		    gtk_ctree_node_nth (GTK_CTREE (ctree), 0));
  gtk_widget_show (main_win);
  gtk_main ();
  return 0;
}

void
cb_icon_browse (GtkWidget * widget, gpointer user_data)
{
  GtkWidget *fs;
  gchar *file;

  fs = gtk_file_selection_new ("Select icon file");

  file = gtk_entry_get_text (GTK_ENTRY (txt_icon));

  gtk_file_selection_set_filename (GTK_FILE_SELECTION (fs), file);

  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (fs)->cancel_button),
		      "clicked", GTK_SIGNAL_FUNC (cb_icon_browse_cancel),
		      (gpointer) fs);

  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (fs)->ok_button),
		      "clicked", GTK_SIGNAL_FUNC (cb_icon_browse_ok),
		      (gpointer) fs);

  gtk_widget_show (fs);
  return;
  widget = NULL;
  user_data = NULL;
}

void
cb_icon_browse_cancel (GtkWidget * widget, gpointer user_data)
{
  gtk_widget_destroy (GTK_WIDGET (user_data));
  return;
  widget = NULL;
  user_data = NULL;
}

void
cb_icon_browse_ok (GtkWidget * widget, gpointer user_data)
{
  gchar *file;
  file = gtk_file_selection_get_filename (GTK_FILE_SELECTION (user_data));

  gtk_entry_set_text (GTK_ENTRY (txt_icon), file);

  entries_to_ctree(GTK_WIDGET (txt_icon), NULL);

  gtk_widget_destroy (GTK_WIDGET (user_data));

  return;
  widget = NULL;
  user_data = NULL;
}

void
cb_exec_browse (GtkWidget * widget, gpointer user_data)
{
  GtkWidget *fs;
  gchar *file;

  fs = gtk_file_selection_new ("Select executable file");

  file = gtk_entry_get_text (GTK_ENTRY (txt_exec));

  gtk_file_selection_set_filename (GTK_FILE_SELECTION (fs), file);

  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (fs)->cancel_button),
		      "clicked", GTK_SIGNAL_FUNC (cb_exec_browse_cancel),
		      (gpointer) fs);

  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (fs)->ok_button),
		      "clicked", GTK_SIGNAL_FUNC (cb_exec_browse_ok),
		      (gpointer) fs);

  gtk_widget_show (fs);
  return;
  widget = NULL;
  user_data = NULL;

}

void
cb_exec_browse_ok (GtkWidget * widget, gpointer user_data)
{
  gchar *file;
  file = gtk_file_selection_get_filename (GTK_FILE_SELECTION (user_data));

  gtk_entry_set_text (GTK_ENTRY (txt_exec), file);
  entries_to_ctree (GTK_WIDGET (txt_exec), NULL);

  gtk_widget_destroy (GTK_WIDGET (user_data));

  return;
  widget = NULL;
  user_data = NULL;

}

void
cb_exec_browse_cancel (GtkWidget * widget, gpointer user_data)
{
  gtk_widget_destroy (GTK_WIDGET (user_data));
  return;
  widget = NULL;
  user_data = NULL;
}

void
status_message (gchar * message, gint delay)
{
  gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, message);
  gtk_timeout_add (delay, (GtkFunction) status_clear, statusbar);
}

gint
status_clear (gpointer user_data)
{
  gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
  return FALSE;
  user_data = NULL;
}

void
on_resort_columns(GtkWidget *widget, gint column, gpointer user_data)
{
    static int order=0;
    static int last_col=0;
    GtkCList *clist;

    clist = GTK_CLIST(ctree);

    if(user_data) {
        widget = NULL;
    } 
    gtk_clist_set_sort_column(GTK_CLIST(clist),column);
    if(last_col == column) {
        if(order) {
            order=0;
            gtk_clist_set_sort_type(GTK_CLIST(clist),GTK_SORT_DESCENDING);
        } else {
            order=1;
            gtk_clist_set_sort_type(GTK_CLIST(clist),GTK_SORT_ASCENDING);
        }
    } else {
        order=1;
        gtk_clist_set_sort_type(GTK_CLIST(clist),GTK_SORT_ASCENDING);
        last_col = column;
    }

    gtk_clist_sort(GTK_CLIST(clist));

    return;

}

/**************************************************************************
 * menu_interface.c
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 12, 2001
 * Notebook tab, and callback function definitions for the menu interface
 * in ebindings
 *************************************************************************/
#include "menu_interface.h"

static struct
{
   GtkWidget *d, *i, *e;
}
m_entry;

/* action driven functions */
static void menu_action_button_clicked(GtkWidget *, gpointer);
static void menu_type_button_clicked(GtkWidget *, gpointer);
static void menu_entry_value_changed(GtkWidget *, gpointer);
static void menu_change_icon_button_cb(GtkWidget *, gpointer);

/* callback for parsing */
static void populate_menu_ctree_with_menu_data(GtkWidget * tree);

/* callbacks for saving */
static void menu_ctree_dump_and_save(GtkWidget *);
static void gnode_to_menu_item_ewd_list(Ewd_List *, GNode *);
static void write_emenu_from_ctree(GtkCTree *);
static void menu_ctree_recursive_check_for_submenus(GtkCTree *, GtkCTreeNode *,
                                                    gpointer);

/* menu ctree row callbacks */
static void menu_ctree_select_row_cb(GtkCTree *, GList *, gint, gpointer);
static void menu_ctree_insert_new_row_relative_to_selected(GtkWidget *);
static void menu_ctree_delete_row_selected(GtkWidget *);

/* add_menu_notebook
 * @param w: pointer to the window widget this note belongs to
 * @param note: The notebook to attach this entry to
 * @param sheet: the number in the notebook this sheet belongs
 * description:  Creates and adds everything in the menu tab.  Sets
 * 	approriate callbacks etc
 */
void
add_menu_notebook(GtkWidget * w, GtkWidget * note, int sheet)
{
   GtkWidget *tab_label;
   GtkWidget *hpaned;
   GtkWidget *hbox;
   GtkWidget *hbox2;
   GtkWidget *vbox;
   GtkWidget *ctree;
   GtkWidget *scroller;
   GtkWidget *frame;
   GtkWidget *table;
   GtkWidget *ichange;
   GtkWidget *dlabel, *ilabel, *elabel;
   GtkWidget *dentry, *ientry, *eentry;
   GtkWidget *execb, *scriptb, *separatorb;
   GtkWidget *saveb, *newb, *delb;

   gchar *fields[] = {
      "Description", "Icon", "Params", "Menu Type"
   };

   tab_label = gtk_label_new("Menus");

   hpaned = gtk_hpaned_new();

   gtk_notebook_insert_page(GTK_NOTEBOOK(note), hpaned, tab_label, sheet);

   scroller = gtk_scrolled_window_new(NULL, NULL);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   gtk_paned_pack1(GTK_PANED(hpaned), scroller, TRUE, FALSE);

   ctree = gtk_ctree_new_with_titles(4, 0, fields);
   gtk_clist_set_column_auto_resize(GTK_CLIST(ctree), 0, TRUE);
   gtk_clist_set_column_auto_resize(GTK_CLIST(ctree), 3, TRUE);
   gtk_clist_set_column_visibility(GTK_CLIST(ctree), 1, FALSE);
   gtk_clist_set_column_visibility(GTK_CLIST(ctree), 2, FALSE);
   gtk_ctree_set_reorderable(GTK_CTREE(ctree), TRUE);
   gtk_widget_ref(ctree);
   gtk_widget_set_name(ctree, "menu_ctree");
   gtk_container_add(GTK_CONTAINER(scroller), ctree);

   vbox = gtk_vbox_new(FALSE, 0);
   gtk_paned_pack2(GTK_PANED(hpaned), vbox, FALSE, FALSE);
   /* why does it freakin grow !?!?!?! */

   frame = gtk_frame_new("Edit Menu Item Properties");
   gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 4);

   table = gtk_table_new(4, 5, FALSE);
   gtk_container_add(GTK_CONTAINER(frame), table);

   dlabel = gtk_label_new("Description: ");
   gtk_table_attach(GTK_TABLE(table), dlabel, 0, 1, 0, 1, GTK_FILL, 0, 2, 2);

   ilabel = gtk_label_new("Icon: ");
   gtk_table_attach(GTK_TABLE(table), ilabel, 0, 1, 1, 2, GTK_FILL, 0, 2, 2);

   elabel = gtk_label_new("Executes: ");
   gtk_table_attach(GTK_TABLE(table), elabel, 0, 1, 2, 3, GTK_FILL, 0, 2, 2);

   dentry = gtk_entry_new();
   gtk_widget_ref(dentry);
   gtk_widget_set_name(dentry, "desc_entry");
   gtk_table_attach(GTK_TABLE(table), dentry, 1, 3, 0, 1, GTK_FILL, 0, 2, 2);
   gtk_signal_connect(GTK_OBJECT(dentry), "changed",
                      GTK_SIGNAL_FUNC(menu_entry_value_changed), ctree);

   ientry = gtk_entry_new();
   gtk_widget_ref(ientry);
   gtk_widget_set_name(ientry, "icon_entry");
   gtk_table_attach(GTK_TABLE(table), ientry, 1, 2, 1, 2, GTK_FILL, 0, 2, 2);
   gtk_signal_connect(GTK_OBJECT(ientry), "changed",
                      GTK_SIGNAL_FUNC(menu_entry_value_changed), ctree);

   ichange = gtk_button_new_with_label("Change");
   gtk_widget_ref(ichange);
   gtk_widget_set_name(ichange, "icon_change_button");
   gtk_widget_ref(ichange);
   gtk_object_set_data_full(GTK_OBJECT(w), "icon_change_button", ichange,
                            (GtkDestroyNotify) gtk_widget_unref);
   gtk_signal_connect(GTK_OBJECT(ichange), "clicked",
                      GTK_SIGNAL_FUNC(menu_change_icon_button_cb), w);
   gtk_table_attach(GTK_TABLE(table), ichange, 2, 3, 1, 2, GTK_FILL, 0, 2, 2);

   eentry = gtk_entry_new();
   gtk_widget_ref(eentry);
   gtk_widget_set_name(eentry, "exec_entry");
   gtk_table_attach(GTK_TABLE(table), eentry, 1, 3, 2, 3, GTK_FILL, 0, 2, 2);
   gtk_signal_connect(GTK_OBJECT(eentry), "changed",
                      GTK_SIGNAL_FUNC(menu_entry_value_changed), ctree);

   /* setup the 3 menu entry containers to be passed to the clist callback */
   m_entry.d = dentry;
   m_entry.i = ientry;
   m_entry.e = eentry;
   gtk_signal_connect(GTK_OBJECT(ctree), "tree_select_row",
                      GTK_SIGNAL_FUNC(menu_ctree_select_row_cb), NULL);
   /* conatiner is setup */

   hbox = gtk_hbox_new(FALSE, 0);
   gtk_table_attach(GTK_TABLE(table), hbox, 0, 3, 3, 4, GTK_FILL, 0, 2, 2);

   execb = gtk_button_new_with_label("Executable");
   gtk_widget_ref(execb);
   gtk_widget_set_name(execb, "exec_button");
   gtk_box_pack_start(GTK_BOX(hbox), execb, TRUE, TRUE, 2);
   gtk_signal_connect(GTK_OBJECT(execb), "clicked",
                      GTK_SIGNAL_FUNC(menu_type_button_clicked), ctree);

   scriptb = gtk_button_new_with_label("Script");
   gtk_widget_ref(scriptb);
   gtk_widget_set_name(scriptb, "script_button");
   gtk_box_pack_start(GTK_BOX(hbox), scriptb, TRUE, TRUE, 2);
   gtk_signal_connect(GTK_OBJECT(scriptb), "clicked",
                      GTK_SIGNAL_FUNC(menu_type_button_clicked), ctree);

   separatorb = gtk_button_new_with_label("Separator");
   gtk_widget_ref(separatorb);
   gtk_widget_set_name(separatorb, "separator_button");
   gtk_box_pack_start(GTK_BOX(hbox), separatorb, TRUE, TRUE, 2);
   gtk_signal_connect(GTK_OBJECT(separatorb), "clicked",
                      GTK_SIGNAL_FUNC(menu_type_button_clicked), ctree);

   hbox2 = gtk_hbox_new(FALSE, 0);
   gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 2);

   newb = gtk_button_new_with_label("New Item");
   gtk_widget_ref(newb);
   gtk_widget_set_name(newb, "new_menu_item_button");
   gtk_signal_connect(GTK_OBJECT(newb), "clicked",
                      GTK_SIGNAL_FUNC(menu_action_button_clicked), ctree);
   gtk_box_pack_start(GTK_BOX(hbox2), newb, TRUE, TRUE, 2);

   delb = gtk_button_new_with_label("Delete Item");
   gtk_widget_ref(delb);
   gtk_widget_set_name(delb, "delete_menu_item_button");
   gtk_signal_connect(GTK_OBJECT(delb), "clicked",
                      GTK_SIGNAL_FUNC(menu_action_button_clicked), ctree);
   gtk_box_pack_start(GTK_BOX(hbox2), delb, TRUE, TRUE, 2);

   saveb = gtk_button_new_with_label("Save");
   gtk_widget_ref(saveb);
   gtk_widget_set_name(saveb, "save_menu_item_button");
   gtk_signal_connect(GTK_OBJECT(saveb), "clicked",
                      GTK_SIGNAL_FUNC(menu_action_button_clicked), ctree);
   gtk_box_pack_start(GTK_BOX(hbox2), saveb, TRUE, TRUE, 2);

   populate_menu_ctree_with_menu_data(ctree);

   gtk_widget_show(newb);
   gtk_widget_show(delb);
   gtk_widget_show(saveb);
   gtk_widget_show(hbox2);
   gtk_widget_show(hbox);
   gtk_widget_show(scriptb);
   gtk_widget_show(separatorb);
   gtk_widget_show(execb);
   gtk_widget_show(eentry);
   gtk_widget_show(dentry);
   gtk_widget_show(ichange);
   gtk_widget_show(ientry);
   gtk_widget_show(elabel);
   gtk_widget_show(dlabel);
   gtk_widget_show(ilabel);
   gtk_widget_show(table);
   gtk_widget_show(frame);
   gtk_widget_show(vbox);
   gtk_widget_show(scroller);
   gtk_widget_show(ctree);
   gtk_widget_show(tab_label);
   gtk_widget_show(hpaned);
}
static void
icon_selection_fs_ok(GtkWidget * w, gpointer data)
{
   gchar *file;

   file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));

   gtk_entry_set_text(GTK_ENTRY(m_entry.i), file);

   gtk_widget_destroy(GTK_WIDGET(data));

   return;
   UN(w);
}
static void
icon_selection_fs_cancel(GtkWidget * w, gpointer data)
{
   gtk_widget_destroy(GTK_WIDGET(data));
   return;
   UN(w);
}
static void
menu_change_icon_button_cb(GtkWidget * w, gpointer data)
{
   GtkWidget *fs;
   gchar *file;

   fs = gtk_file_selection_new("Selection icon file");
   file = gtk_entry_get_text(GTK_ENTRY(m_entry.i));

   if ((!strcmp(file, "")) || (!file))
      file = g_strdup(DEFAULT_ICON_PIXMAP_DIR);

   gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), file);

   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
                      "clicked", GTK_SIGNAL_FUNC(icon_selection_fs_cancel),
                      (gpointer) fs);

   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button), "clicked",
                      GTK_SIGNAL_FUNC(icon_selection_fs_ok), (gpointer) fs);

   gtk_widget_show(fs);
   return;
   UN(w);
   UN(data);
}

/* menu_ctree_select_row_cb: 
 * description - When a row in the menu ctree is selected this function gets
 * called.  Setup all the necessary changes so the text entries reflect the
 * currently selected row's info.  Also enable/disable entries depending on
 * the type of entry that is currently selected.  It keeps users from have
 * an executable and submenu
 */
void
menu_ctree_select_row_cb(GtkCTree * tree, GList * node, gint column,
                         gpointer data)
{
   GtkCTreeNode *c;

   /* the selected row */
   gchar *fields[4] = {
      "", "", "", ""
   };

   c = GTK_CTREE_NODE((GTK_CLIST(tree)->selection)->data);
   gtk_ctree_get_node_info(GTK_CTREE(tree), GTK_CTREE_NODE(c), &fields[0],
                           NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   gtk_ctree_node_get_text(GTK_CTREE(tree), GTK_CTREE_NODE(c), 1, &fields[1]);
   gtk_ctree_node_get_text(GTK_CTREE(tree), GTK_CTREE_NODE(c), 2, &fields[2]);
   gtk_ctree_node_get_text(GTK_CTREE(tree), GTK_CTREE_NODE(c), 3, &fields[3]);

   GTK_ENTRY_SET_TEXT(m_entry.d, fields[0]);
   GTK_ENTRY_SET_TEXT(m_entry.i, fields[1]);
   GTK_ENTRY_SET_TEXT(m_entry.e, fields[2]);

   if (GTK_CTREE_ROW(c)->children)
   {
      gtk_ctree_node_set_text(GTK_CTREE(tree), GTK_CTREE_NODE(c), 3,
                              "Submenu");
      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.d), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.i), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.e), FALSE);
      GTK_ENTRY_SET_TEXT(m_entry.e, "");
   }
   else if (!(strcmp(fields[3], "Separator")))
   {
      gtk_ctree_node_set_text(GTK_CTREE(tree), GTK_CTREE_NODE(c), 0,
                              "-(SEPARATOR)-");
      GTK_ENTRY_SET_TEXT(m_entry.i, "");
      GTK_ENTRY_SET_TEXT(m_entry.e, "");
      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.d), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.i), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.e), FALSE);
   }
   else
   {
      if (strcmp(fields[3], "Script"))
         gtk_ctree_node_set_text(GTK_CTREE(tree), GTK_CTREE_NODE(c), 3,
                                 "Executable");
      /* if it's not a script * submenu formerly with no children left
         changes state to an * executable */

      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.d), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.i), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(m_entry.e), TRUE);
   }
   return;
   UN(node);
   UN(column);
   UN(data);
}

/* menu_action_button_clicked:
 *	Description: handle clicked events for 1 of 3 action buttons on the menu
 *	tab.
 * @param w: Button widget that was clicked
 * @param data:is the ctree from the menu tab =) */
void
menu_action_button_clicked(GtkWidget * w, gpointer data)
{
   gchar *wname;

   wname = gtk_widget_get_name(GTK_WIDGET(w));
   /* get the widgets name to identify it, DO NOT free it */

   if (!strcmp(wname, "new_menu_item_button"))
   {
      menu_ctree_insert_new_row_relative_to_selected(data);
   }
   else if (!strcmp(wname, "delete_menu_item_button"))
   {
      menu_ctree_delete_row_selected(data);
   }
   else if (!strcmp(wname, "save_menu_item_button"))
   {
      menu_ctree_dump_and_save(data);
   }
   else
   {
      fprintf(stderr, "Unknown action button with name: %s\n", wname);
   }
}
void
menu_type_button_clicked(GtkWidget * w, gpointer data)
{
   /* 
    * @param data: is a pointer to the ctree used on the menu tab
    * @param w: the button type clicked from the menu tab that has received a 
    *                      "clicked" event 
    * description: do some checking, update currently selected row who's cell
    * needs to be changed to correspond with the change in the menu item's
    * type descriptor which is obtained by the button
    */
   gchar *button_name = NULL;

   /* button_name: the name of the button widget that is set when the
      button is originally setup */
   GtkCTreeNode *node = NULL, *node_data = NULL;

   if (!data)
      return;
   if (!w)
      return;

   node = GTK_CTREE_NODE(GTK_CLIST(data)->selection);

   if (node)
   {
      button_name = gtk_widget_get_name(w);
      node_data = GTK_CTREE_NODE((GTK_CLIST(data)->selection)->data);

      if (!strcmp(button_name, "exec_button"))
      {
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 3,
                                 "Executable");

      }
      else if (!strcmp(button_name, "script_button"))
      {
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 3,
                                 "Script");

      }
      else if (!strcmp(button_name, "separator_button"))
      {
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 0,
                                 "-(SEPARATOR)-");
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 1,
                                 "");
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 2,
                                 "");
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 3,
                                 "Separator");
      }
      else
      {
         fprintf(stderr, "Unknown menu type button with name: %s\n",
                 button_name);
      }
      /* 
       * emit a select signal, so valid fields get set uneditable or
       * editable based on menu type 
       */
      gtk_ctree_select(GTK_CTREE(data), node_data);
   }
}

void
menu_entry_value_changed(GtkWidget * w, gpointer data)
{
   /* 
    * data: is a pointer to the ctree used on the menu tab
    * w: the text entry widget on the menu tab that has received a "change"
    *              event 
    * description: do some checking, update currently selected row who's cell
    * needs to be changed to correspond with the changed text entry widget 
    *
    */
   gchar *new_val = NULL, *wname = NULL;
   GtkCTreeNode *node = NULL, *node_data = NULL;

   new_val = gtk_entry_get_text(GTK_ENTRY(w));
   /* 
    * get the text from the changed entry so i can modify the clist to
    * match it, also new_val is not freed because it is a poitner to the
    * text in the GTK_ENTRY
    */
   node = GTK_CTREE_NODE(GTK_CLIST(data)->selection);
   /* 
    * Paranoid Checking, make sure something in the clist is the selected
    * node 
    */
   if (node)
   {
      node_data = GTK_CTREE_NODE((GTK_CLIST(data)->selection)->data);
      /* 
       * - Get the address of the container that holds the selected row
       * - then match up the widget name with the specified column =) 
       */
      wname = gtk_widget_get_name(w);
      if (!strcmp(wname, "desc_entry"))
      {
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 0,
                                 new_val);
      }
      else if (!strcmp(wname, "icon_entry"))
      {
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 1,
                                 new_val);
      }
      else if (!strcmp(wname, "exec_entry"))
      {
         gtk_ctree_node_set_text(GTK_CTREE(data), GTK_CTREE_NODE(node_data), 2,
                                 new_val);
      }
      else
      {
         fprintf(stderr, "ERROR: Unknown entry type changed with name: %s\n",
                 wname);
      }

   }
   else
   {
      fprintf(stderr, "ERROR: No ctree node currently selected.\n");
   }
}

/* menu_ctree_insert_new_row_relative:
 * - When a new entry request is received, the new entry is inserted
 * 		after the currently selected now in the ctree
 * @param w: a pointer to the ctree on the menu tab
 */
static void
menu_ctree_insert_new_row_relative_to_selected(GtkWidget * w)
{
   GtkCTreeNode *newparent, *newnode, *news, *newp;

   newparent = newnode = news = newp = NULL;

   if ((w) && (GTK_CLIST(GTK_CTREE(w))->selection))
   {
      gchar *fields[4];
      int i;

      fields[0] = g_strdup("New Entry");
      fields[1] = g_strdup("");
      fields[2] = g_strdup("");
      fields[3] = g_strdup("Executable");
      newparent = GTK_CLIST(GTK_CTREE(w))->selection->data;
      newp = GTK_CTREE_ROW(newparent)->parent;
      news = GTK_CTREE_ROW(newparent)->sibling;

      if (!newp)
         newp = gtk_ctree_node_nth(GTK_CTREE(w), 0);

      newnode =
         gtk_ctree_insert_node(GTK_CTREE(w), newp, news, fields, 0, NULL, NULL,
                               NULL, NULL, FALSE, FALSE);
      gtk_ctree_select(GTK_CTREE(w), newnode);

      for (i = 0; i < 4; i++)
         g_free(fields[i]);
   }
   else
   {
      fprintf(stderr,
              "ERROR: Attempted to insert a new menu item, but"
              " nothing is currently selected in the menu ctree");
      return;
   }

}

/* menu_ctree_delete_row_selected
 * - Removes the currently selected row from the ctree
 * @param w: a pointer to the menu ctree on the menu tab
 */
static void
menu_ctree_delete_row_selected(GtkWidget * w)
{
   if ((w) && (GTK_CLIST(GTK_CTREE(w))->selection))
   {
      GtkCTreeNode *node, *next, *parent;

      node = GTK_CTREE_NODE(GTK_CLIST(GTK_CTREE(w))->selection->data);
      next = GTK_CTREE_ROW(node)->sibling;
      parent = GTK_CTREE_ROW(node)->parent;
      gtk_ctree_remove_node(GTK_CTREE(w), node);

      if (!next && parent)
      {
         next = GTK_CTREE_ROW(parent)->children;
         if (next)
            while (GTK_CTREE_ROW(next)->sibling)
               next = GTK_CTREE_ROW(next)->sibling;
      }
      if (next)
         gtk_ctree_select(GTK_CTREE(w), next);
   }
   else
   {
      fprintf(stderr,
              "ERROR: Attempted to delete a menu item,\n\tbut"
              " nothing is currently selected in the menu ctree\n");
      return;
   }

}

/* ctree_to_emenu_item_gnode:
 *  When a save request is sent, the data must be dumped from the ctree to a
 *  GNode, and (currently) export the GNode to an Ewl_List.  Writing
 *  straight from the GNode is completely possible, but I want to eventually
 *  move ebindings to ewl, and ewd has lots of nice things =)
 * description: Dumps data from a row in a ctree to ->data of a GNode
 * 		Function is never explicity called by us, gtk uses it.
 */
static gint
ctree_to_emenu_item_gnode(GtkCTree * tree, guint depth, GNode * node,
                          GtkCTreeNode * cnode, gpointer data)
{
   emenu_item *e = NULL;
   char *fields[4] = {
      "", "", "", ""
   };

   e = emenu_item_new();
   gtk_ctree_node_get_text(GTK_CTREE(tree), GTK_CTREE_NODE(cnode), 1,
                           &fields[1]);
   gtk_ctree_node_get_text(GTK_CTREE(tree), GTK_CTREE_NODE(cnode), 2,
                           &fields[2]);
   gtk_ctree_node_get_text(GTK_CTREE(tree), GTK_CTREE_NODE(cnode), 3,
                           &fields[3]);
   gtk_ctree_get_node_info(GTK_CTREE(tree), GTK_CTREE_NODE(cnode), &fields[0],
                           NULL, NULL, NULL, NULL, NULL, NULL, NULL);

   e->text = strdup(fields[0]);
   e->icon = strdup(fields[1]);
   e->exec = strdup(fields[2]);

   if (!strcmp(fields[3], ""))
   {
      /* shouldn't ever happen =) */
   }
   else if (!strcmp(fields[3], "Separator"))
   {
      e->type = E_MENU_SEPARATOR;
      IF_FREE(e->text);
      IF_FREE(e->icon);
      IF_FREE(e->exec);
   }
   else if (!strcmp(fields[3], "Submenu"))
   {
      e->type = E_MENU_SUBMENU;
      IF_FREE(e->exec);
   }
   else
   {
      if (e->text[0] == '\0')
      {
         IF_FREE(e->text);
         IF_FREE(e->exec);
         e->text = strdup("");
         e->exec = strdup("");
         /* now we have a nice spacer in the menu */
      }
      else if (e->exec[0] == '\0')
      {
         fprintf(stderr,
                 "ERROR: Tried inserting an entry with no "
                 "executable defined\nIts description is %s.\n"
                 "I'm omitting this entry\n", e->text);
         IF_FREE(e->text);
         /* make sure this won't get written ever */
      }
      else if (!strcmp(fields[3], "Script"))
      {
         e->type = E_MENU_SCRIPT;
      }
      else
      {
         /* executable if it makes it here */
         e->type = E_MENU_EXECUTABLE;
      }
   }
   node->data = e;
   return TRUE;
   UN(depth);
   UN(data);
}

/* gnode_to_menu_item_ewl_list:
 * description: Takes a pointer to an already initialized ewd_list.
 * Extracts the node pointer from the GNode, and populates the ewd list.  
 * 
 * @param l: Ewd_List that has been created with ewd_list_new()
 * @param node: GNode that is the result of a gtk_ctree_export_to_gnode
 */
static void
gnode_to_menu_item_ewd_list(Ewd_List * l, GNode * node)
{
   emenu_item *e = NULL;
   GNode *ptr = NULL;

   for (ptr = node; ptr; ptr = ptr->next)
   {
      e = (emenu_item *) ptr->data;
      ewd_list_append(l, e);
      if (ptr->children)
         gnode_to_menu_item_ewd_list(e->children, ptr->children);
   }
}

/* write_emenu_from_ctree:
 * 	description: takes the tree passed to it, and writes that tree as the
 * 	users menu db.
 */
static void
write_emenu_from_ctree(GtkCTree * tree)
{
   GNode *items;
   Ewd_List *l;
   int written = 0;

   items =
      gtk_ctree_export_to_gnode(GTK_CTREE(tree), NULL, NULL,
                                gtk_ctree_node_nth(GTK_CTREE(tree), 0),
                                ctree_to_emenu_item_gnode, NULL);

   if (!items)
   {
      fprintf(stderr, "Error dumping CTree to a GNode menu_interface.c\n");
      return;
   }
   items = items->children;
   l = ewd_list_new();
   ewd_list_set_free_cb(l, _emenu_item_free);

   gnode_to_menu_item_ewd_list(l, items);
   g_node_destroy(items);

   written = write_user_menu_db_with_ewd_list(l);
   if (written)
      status_bar_message("Error Saving your Menus", 3000);
   else
      status_bar_message("Menus Saved Successfully ...", 3000);

   /* see util.c for this function */

   ewd_list_destroy(l);
}

/* - checks to make sure we don't apply filter rules to the root of the ctree.
 * - never explicitly called.
 */
static void
menu_ctree_recursive_check_for_submenus(GtkCTree * tree, GtkCTreeNode * node,
                                        gpointer data)
{
   gchar *description = NULL;
   char buf[1024];

   if (!node || !tree)
      return;

   sprintf(buf, "%s/.e/behavior/apps_menu.db", getenv("HOME"));

   if (GTK_CTREE_ROW(node)->children)
   {
      gtk_ctree_get_node_info(GTK_CTREE(tree), GTK_CTREE_NODE(node),
                              &description, NULL, NULL, NULL, NULL, NULL, NULL,
                              NULL);

      /* be sure it's not the root !!! =) */
      if (strcmp(buf, description) || (!description))
      {
         gtk_ctree_node_set_text(GTK_CTREE(tree), GTK_CTREE_NODE(node), 2, "");
         gtk_ctree_node_set_text(GTK_CTREE(tree), GTK_CTREE_NODE(node), 3,
                                 "Submenu");
      }
   }
   return;
   UN(data);
}

/* menu_ctree_dump_and_save:
 *  - description: when a user clicks save, it calls this routine 
 * @param w: pointer the menu ctree on the menu tab 
 */
static void
menu_ctree_dump_and_save(GtkWidget * w)
{
   if (!w)
      return;
   gtk_ctree_pre_recursive(GTK_CTREE(w), gtk_ctree_node_nth(GTK_CTREE(w), 0),
                           menu_ctree_recursive_check_for_submenus, NULL);
   /* check for submenus cases to apply the Submenu tag if ncessary */
   write_emenu_from_ctree(GTK_CTREE(w));
}

/* enables recursive calls on the Ewd_List to populate a ctree with a
 * pre-established parent
 * @param l: a Ewl_List populated with the info from apps_menu.db
 * @param tree: pointer to the tree we're populating
 * @param parent: parent node in the tree to attach new ones to
 */
static void
populate_menu_ctree_recursive(Ewd_List * l, GtkWidget * tree,
                              GtkCTreeNode * parent)
{
   emenu_item *e;
   GtkCTreeNode *node;
   gchar *fields[4] = {
      "", "", "", ""
   };
   int i;

   if (!l)
   {
      fprintf(stderr, "No list supplied for menu ctree dump\n");
      return;
   }
   else if (!tree)
      return;
   else if (!parent)
      return;

   ewd_list_goto_first(l);
   while ((e = (emenu_item *) ewd_list_next(l)))
   {

      if ((e->text) && (strlen(e->text) > 0))
         fields[0] = g_strdup(e->text);
      else
         fields[0] = g_strdup("");

      if ((e->icon) && (strlen(e->icon) > 0))
         fields[1] = g_strdup(e->icon);
      else
         fields[1] = g_strdup("");

      if ((e->exec) && (strlen(e->exec) > 0))
         fields[2] = g_strdup(e->exec);
      else
         fields[2] = g_strdup("");

      switch (e->type)
      {
        case E_MENU_SCRIPT:
           fields[3] = g_strdup("Script");
           break;
        case E_MENU_SUBMENU:
           fields[3] = g_strdup("Submenu");
           break;
        case E_MENU_SEPARATOR:
           fields[3] = g_strdup("Separator");
           g_free(fields[0]);
           fields[0] = g_strdup("-(SEPARATOR)-");
           break;
        case E_MENU_EXECUTABLE:
           fields[3] = g_strdup("Executable");
           break;
        default:
           break;
      }
      node =
         gtk_ctree_insert_node(GTK_CTREE(tree), parent, NULL, fields, 5, NULL,
                               NULL, NULL, NULL, FALSE, FALSE);

      for (i = 0; i < 4; i++)
      {
         g_free(fields[i]);
         fields[i] = NULL;
      }

      if (e->type == E_MENU_SUBMENU)
         populate_menu_ctree_recursive(e->children, tree, node);
   }
}

/* populate_menu_ctree_with_menu_data
 * @param tree: a pointer to the ctree we're going to populate with the
 * 		user's menu db info
 * FIXME: a reloading of the users db would require the ctree to be
 * 		freed/cleared.  I don' tknow how to do this yet
 */
static void
populate_menu_ctree_with_menu_data(GtkWidget * tree)
{
   Ewd_List *l;
   GtkCTreeNode *root;
   char buf[1024];
   gchar *fields[4] = {
      "", "", "", ""
   };

   l = ewd_list_new();
   ewd_list_set_free_cb(l, _emenu_item_free);

   read_user_menu_db_into_ewd_list(l);
   ewd_list_goto_first(l);

   sprintf(buf, "%s/.e/behavior/apps_menu.db", getenv("HOME"));

   fields[0] = g_strdup(buf);

   root =
      gtk_ctree_insert_node(GTK_CTREE(tree), NULL, NULL, fields, 5, NULL, NULL,
                            NULL, NULL, FALSE, TRUE);
   gtk_ctree_node_set_selectable(GTK_CTREE(tree), GTK_CTREE_NODE(root), FALSE);

   populate_menu_ctree_recursive(l, tree, root);

   if (GTK_CTREE_ROW(root)->children)
   {
      gtk_ctree_select(GTK_CTREE(tree), GTK_CTREE_ROW(root)->children);
      /* select the first child of the root */
   }

   ewd_list_destroy(l);
   /* clean up */
}

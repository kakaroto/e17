#include "ee2.h"

enum {
   TARGET_URI_LIST,
   TARGET_TEXT_PLAIN
};

static GtkTargetEntry file_drop_types[] = {
   { "text/uri-list", 0, TARGET_URI_LIST }
};

GList *parse_uri_list(gchar *uri_list)
{
  GList *list = NULL;
  char *p = uri_list;
  /* Where's perl when you need it? */
  while (*p) {
    int len = strcspn(p, "\n\r");
    if (!strncmp(p, "file:", 5))
      list = g_list_append(list, g_strndup(p + 5, len - 5));
    p = p + len + strspn(p + len + 1, "\n\r") + 1;
  }
  return list;
}

void dnd_get_data(GtkWidget *widget, GdkDragContext *context,
                       gint x, gint y, GtkSelectionData *sel, guint info,
                       guint time, gpointer data)
{
   if (info == TARGET_URI_LIST) {
     GList *list = parse_uri_list(sel->data),
           *p = list;
     if (p) {
      gtk_clist_clear(GTK_CLIST(BrClist));
      while (p) {
        AddList(p->data);
        p = p->next;
      }
      LoadImage(list->data);
      DrawImage(im, 0, 0);
      prev_draw(im, area2->allocation.width, area2->allocation.height);
      g_list_foreach(list, (GFunc)g_free, NULL);
      g_list_free(list);
    }
  }
}

void dnd_init(GtkWidget *view)
{
  gtk_drag_dest_set(view, GTK_DEST_DEFAULT_MOTION | 
                    GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP, 
                    file_drop_types, 1, GDK_ACTION_COPY);
  gtk_signal_connect(GTK_OBJECT(view), "drag_data_received", 
                     dnd_get_data, NULL);
}

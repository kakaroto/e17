#include "manager.h"
#include "util.h"
#include "advanced.h"

extern int current_row;


void
manager_entry_changed(GtkWidget *w, gpointer data)
{


    fprintf(stderr, "I am manager_entry_changed\n");    
    return;
    UN(w);
    UN(data);
}

void
manager_add_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *fs;

    fs = gtk_file_selection_new("Add Background File");
    gtk_file_selection_complete(GTK_FILE_SELECTION(fs), "*.bg.db");
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
		"clicked", GTK_SIGNAL_FUNC(manager_fs_cancel_clicked),
		(gpointer)fs);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
		"clicked", GTK_SIGNAL_FUNC(manager_fs_ok_clicked),
		(gpointer)fs);
    gtk_widget_show(fs);
    return;
    UN(w);
    UN(data);
}

void
manager_remove_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *clist;
    extern int current_row;
    
    clist = gtk_object_get_data(GTK_OBJECT(win_ref), "bg_list");
    if(clist) gtk_clist_remove(GTK_CLIST(clist), current_row);

    /* handle removes relative to currentp osition */
    if((current_row--))
    {
	bg_list_count--;
	gtk_clist_select_row(GTK_CLIST(clist), current_row, 0);
    }
    /* otherwise just select the first one */
    else if(bg_list_count--)
	gtk_clist_select_row(GTK_CLIST(clist), 0, 0);
    /* nuke the bg cause it's the last one */
    else 
    { 
	bg_list_count = 0;
	e_bg_free(bg); 
	DRAW(); 
    }

    return;
    UN(w);
    UN(data);
}

void
bg_list_item_selected(GtkWidget *w, gint row, gint column,
			GdkEventButton *ev, gpointer data)
{
    E_Background _bg;
    gchar *filename;
    
    if(current_row == row) return; /* don't reload what's already there */
    current_row = row;
    gtk_clist_get_text(GTK_CLIST(w), row, 1, &filename);
    if(filename)
    {
	_bg = bg;
	
	bg = e_bg_load(filename);
	if(bg) 	
	{
	    char buf[256];
	    
	    if(_bg) e_bg_free(_bg);
	   
	    snprintf(buf, 256, "Ebony - %s", filename);
	    gtk_window_set_title(GTK_WINDOW(win_ref), buf);
	    display_bg(bg);
	}
	else
	{
	    bg = _bg;
	}
    }
    else
    {
	gchar errstr[1024];

	g_snprintf(errstr, 1024, "Unable to load %s", (char*)filename);
	ebony_status_message(errstr, EBONY_STATUS_TO);
    }

    return;
    UN(w);
    UN(row);
    UN(column);
    UN(data);
}

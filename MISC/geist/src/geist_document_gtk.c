/* geist_document_gtk.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "geist_document_gtk.h"
#include "geist_document_xml.h"
gboolean file_save_ok_cb(GtkWidget * widget, gpointer * data);
gboolean file_load_ok_cb(GtkWidget * widget, gpointer * data);

void
geist_document_render_to_window(geist_document * doc)
{
   Window xwin;

   D_ENTER(3);
   gtk_widget_realize(doc->darea);
   xwin = GDK_WINDOW_XWINDOW(doc->darea->window);

   XSetWindowBackgroundPixmap(disp, xwin, doc->pmap);

   XClearWindow(disp, xwin);

   D_RETURN_(3);
}

void
geist_document_render_to_window_partial(geist_document * doc, int x, int y,
                                        int w, int h)
{
   Window xwin;

   D_ENTER(3);

   xwin = GDK_WINDOW_XWINDOW(doc->darea->window);

   XSetWindowBackgroundPixmap(disp, xwin, doc->pmap);

   XClearArea(disp, xwin, x, y, w, h, False);

   D_RETURN_(3);
}

void
geist_document_load(void)
{
   GtkWidget *fs;

   D_ENTER(3);

   fs = gtk_file_selection_new("Load document...");
   gtk_signal_connect(GTK_OBJECT(fs), "destroy",
                      (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT(fs));

   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
                      "clicked", (GtkSignalFunc) file_load_ok_cb, fs);

   gtk_signal_connect_object(GTK_OBJECT
                             (GTK_FILE_SELECTION(fs)->cancel_button),
                             "clicked", (GtkSignalFunc) gtk_widget_destroy,
                             GTK_OBJECT(fs));

   gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), "test.xml");
   gtk_widget_show(fs);

   D_RETURN_(3);
}

gboolean file_load_ok_cb(GtkWidget * widget, gpointer * data)
{
   char *filename;
   geist_document *doc;
   int err;
   GtkWidget *doc_win;

   D_ENTER(3);

   filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
   if (!filename)
      D_RETURN(3, TRUE);

   doc = geist_document_load_xml(filename, &err);
   if (!doc)
   {
      weprintf("failed to load file %s\n", filename);
      gtk_widget_destroy(GTK_WIDGET(data));
      D_RETURN(3, TRUE);
   }
   else
      D(2, ("file %s loaded okay\n", filename));
   doc_win = geist_gtk_new_document_window(doc);
   geist_document_render_full(doc, 1);
   gtk_widget_show(doc_win);
   gtk_widget_destroy(GTK_WIDGET(data));

   D_RETURN(3, TRUE);
}


void
geist_document_save_as(geist_document * doc)
{
   GtkWidget *fs;

   D_ENTER(3);

   fs = gtk_file_selection_new("Save document as...");
   gtk_signal_connect(GTK_OBJECT(fs), "destroy",
                      (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT(fs));

   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
                      "clicked", (GtkSignalFunc) file_save_ok_cb, fs);

   gtk_signal_connect_object(GTK_OBJECT
                             (GTK_FILE_SELECTION(fs)->cancel_button),
                             "clicked", (GtkSignalFunc) gtk_widget_destroy,
                             GTK_OBJECT(fs));

   gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs),
                                   doc->filename ? doc->
                                   filename : "test.xml");
   gtk_object_set_data(GTK_OBJECT(fs), "doc", current_doc);
   gtk_widget_show(fs);

   D_RETURN_(3);
}

gboolean file_save_ok_cb(GtkWidget * widget, gpointer * data)
{
   char *filename;
   geist_document *doc;

   D_ENTER(3);

   filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
   if (!filename)
      D_RETURN(3, TRUE);

   doc = gtk_object_get_data(GTK_OBJECT(data), "doc");
   if (doc)
   {
      if (doc->filename)
         efree(doc->filename);
      doc->filename = estrdup(filename);
      geist_document_save(doc, filename);
   }
   gtk_widget_destroy(GTK_WIDGET(data));

   D_RETURN(3, TRUE);
}

void geist_document_resize_gtk(geist_document *doc, int w,int h)
{
   D_ENTER(3);

   gtk_widget_set_usize(doc->darea, w, h);
   gtk_widget_set_usize(doc->scrollwin, w, h);

   D_RETURN_(3);
}

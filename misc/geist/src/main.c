
/*  main.c  */

#include "geist.h"
#include "geist_document.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_document_gtk.h"
#include "geist_text.h"
#include "geist_rect.h"
#include "geist_line.h"
#include "geist_layer.h"
#include "geist_document_xml.h"
#include "geist_gtk_menu.h"
#include "geist_interface.h"

int call_level = 0;
geist_list *doc_list = NULL;

int
main(int argc, char *argv[])
{
   GtkWidget *obj_win, *doc_win;
   geist_document *doc;
   int err;

   opt.debug_level = 1;
   D_ENTER(3);

   gtk_init(&argc, &argv);

   obj_win = geist_create_object_list();
   gtk_widget_show(obj_win);
   imlib_init(obj_win);

   gtk_clist_freeze(GTK_CLIST(obj_list));

   if (argc > 1)
      doc = geist_document_load_xml(argv[1], &err);
   else
      doc = geist_document_load_xml("test.xml", &err);
   if (!doc)
      weprintf("failed to load xml file\n");
   else
   {
      doc_win = geist_gtk_new_document_window(doc);
      geist_document_render_full(doc, 1);
      gtk_widget_show(doc_win);
   }

   gtk_clist_thaw(GTK_CLIST(obj_list));
   gtk_main();
   D_RETURN(3, 0);
}

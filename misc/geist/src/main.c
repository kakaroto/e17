
/*  main.c  */

#include "geist.h"
#include "geist_document.h"
#include "geist_object.h"
#include "geist_image.h"
#include "geist_document_gtk.h"
#include "geist_text.h"
#include "geist_rect.h"
#include "geist_layer.h"
#include "geist_document_xml.h"
#include "geist_gtk_menu.h"
#include "geist_interface.h"

int call_level = 0;
geist_list *doc_list = NULL;

int
main(int argc, char *argv[])
{
   GtkWidget *nbook, *obj_win;
   geist_document *doc;

   opt.debug_level = 5;
   D_ENTER(3);

   gtk_init(&argc, &argv);

   mainwin = geist_create_main_window();

   imlib_init(mainwin);

   obj_win = geist_create_object_list();

   nbook = gtk_object_get_data(GTK_OBJECT(mainwin), "notebook");
   doc = geist_gtk_new_document_page(nbook, 500, 500, "New document");

   doc->bg_fill->r = 155;
   doc->bg_fill->g = 216;
   doc->bg_fill->b = 237;
   doc->bg_fill->a = 255;

   gtk_clist_freeze(GTK_CLIST(obj_list));

   geist_document_add_object(doc,
                             geist_image_new_from_file(0, 0,
                                                       PREFIX
                                                       "/share/geist/images/laet.jpg"));
   geist_document_add_object(doc,
                             geist_text_new_with_text(0, 405, "20thcent", 16,
                                                      "Some pr0n - I have to.",
                                                      255, 100, 100, 255));
   geist_document_add_object(doc,
                             geist_image_new_from_file(220, 140,
                                                       PREFIX
                                                       "/share/geist/images/elogo.png"));
   geist_document_add_object(doc,
                             geist_image_new_from_file(175, 125,
                                                       PREFIX
                                                       "/share/geist/images/bulb.png"));
   geist_document_add_object(doc,
                             geist_image_new_from_file(375, 145,
                                                       PREFIX
                                                       "/share/geist/images/bulb.png"));
   geist_document_add_object(doc,
                             geist_image_new_from_file(415, 200,
                                                       PREFIX
                                                       "/share/geist/images/mail.png"));
   geist_document_add_object(doc,
                             geist_image_new_from_file(445, 305,
                                                       PREFIX
                                                       "/share/geist/images/mush.png"));
   geist_document_add_object(doc,
                             geist_image_new_from_file(315, 405,
                                                       PREFIX
                                                       "/share/geist/images/paper.png"));
   geist_document_add_object(doc,
                             geist_rect_new_of_size(325, 5, 100, 100, 255,
                                                    100, 250, 120));
   geist_document_add_object(doc,
                             geist_text_new_with_text(275, 15, "20thcent", 20,
                                                      "So this is geist...",
                                                      128, 255, 100, 100));
   geist_document_add_object(doc,
                             geist_image_new_from_file(103, 171,
                                                       PREFIX
                                                       "/share/geist/images/globe.png"));
   geist_document_add_object(doc,
                             geist_image_new_from_file(83, 171,
                                                       PREFIX
                                                       "/share/geist/images/globe.png"));
   geist_document_add_object(doc,
                             geist_rect_new_of_size(25, 175, 200, 300, 50,
                                                    255, 255, 0));

   geist_document_render_full(doc);

   doc = geist_gtk_new_document_page(nbook, 400, 300, "Second document");
   geist_document_add_object(doc,
                             geist_text_new_with_text(15, 15, "20thcent", 20,
                                                      "Wow! 2 documents? Neato ;-)",
                                                      255, 0, 100, 255));
   geist_document_render_full(doc);

   gtk_clist_thaw(GTK_CLIST(obj_list));
   gtk_main();
   D_RETURN(3, 0);
}

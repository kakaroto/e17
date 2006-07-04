#include "etk_test.h"

/* Creates the window for the text view test */
void etk_test_text_view_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *text_view;
   
   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }	
  
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Text View Test");
   etk_widget_size_request_set(ETK_WIDGET(win), 400, 300);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   text_view = etk_text_view_new();
   etk_textblock_text_set(ETK_TEXT_VIEW(text_view)->textblock,
      "<p align=\"center\"><b><u><font size=18>Etk Textblock</font></u></b></p> \n"
      "<b><u><font size=12>Supported tags:</font></u></b>\n"
      "<p left_margin=30>"
         "<b>&lt;b&gt;:</b> <b>Bold</b>\n"
         "<b>&lt;i&gt;:</b> <i>Italic</i>\n"
         "<b>&lt;u&gt;:</b> <u>Underline</u>\n"
         "<b>&lt;font&gt;:</b>\n"
      "</p>"
      "<p left_margin=60>"
            "<i>face:</i> Defines the face of the font\n"
            "<i>size:</i> Defines the size of the font\n"
            "<i>color:</i> Defines the color of the font\n"
      "</p>"
      "<p left_margin=30>"
         "<b>&lt;style&gt;:</b>\n"
      "</p>"
      "<p left_margin=60>"
            "<i>effect:</i> Defines the effect to apply on the text\n"
            "<i>color1:</i> Defines the first color of the effect\n"
            "<i>color2:</i> Defines the second color of the effect\n"
      "</p>",
      ETK_TRUE);
   
   etk_box_pack_start(ETK_BOX(vbox), text_view, ETK_TRUE, ETK_TRUE, 0);
   
   etk_widget_show_all(win);
}

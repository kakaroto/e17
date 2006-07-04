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
   etk_widget_size_request_set(win, 150, 150);
   etk_window_resize(ETK_WINDOW(win), 400, 300);
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
      "</p>"
      "<p left_margin=60>"
            "<i>type:</i> Whether the text is single or double underlined\n"
            "<i>color1:</i> The color of the first underline\n"
            "<i>color2:</i> The color of the second underline\n"
      "</p>"
      "<p left_margin=30>"
         "<b>&lt;s&gt;:</b> <s>Strikethrough</s>\n"
      "</p>"
      "<p left_margin=60>"
            "<i>color:</i> The color of the strikethrough\n"
      "</p>"
      "<p left_margin=30>"
         "<b>&lt;font&gt;:</b>\n"
      "</p>"
      "<p left_margin=60>"
            "<i>face:</i> The face of the font\n"
            "<i>size:</i> The size of the font\n"
            "<i>color:</i> The color of the font\n"
      "</p>"
      "<p left_margin=30>"
         "<b>&lt;style&gt;:</b>\n"
      "</p>"
      "<p left_margin=60>"
            "<i>effect:</i> The type of effect to apply on the text\n"
            "<i>color1:</i> The first color of the effect\n"
            "<i>color2:</i> The second color of the effect\n"
      "</p>",
      ETK_TRUE);
   
   etk_box_pack_start(ETK_BOX(vbox), text_view, ETK_TRUE, ETK_TRUE, 0);
   
   printf("%s\n", etk_string_get(etk_textblock_text_get(ETK_TEXT_VIEW(text_view)->textblock, ETK_FALSE)));
   
   etk_widget_show_all(win);
}

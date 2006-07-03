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
   /*etk_textblock_text_set(ETK_TEXT_VIEW(text_view)->textblock,
      "Test1 <b>Bold <i>Bold_Italic </b>Italic </i>Test2",
      ETK_TRUE);*/
   etk_textblock_text_set(ETK_TEXT_VIEW(text_view)->textblock,
      "<p align=\"center\">Title</p>"
      "<b>Bold <i>Bold-Italic </b>Italic </i>: Overlapping tags are supported!\n"
      "A paragraph here <font color=#f00>red <b>text</b></font> and stuff."
      "And escaping &lt; and &gt; as well as &amp; as <b>normal.</b>"
      "If you want a newline use &lt;br&gt;\nwoo a new line!"
      "<p align=\"right\">Right</p>"
      "<style effect=\"outline\" color1=#fff color2=#000>aligned</style> "
      "<style effect=\"shadow\" color1=#fff8>text</style> "
      "<style effect=\"soft_shadow\" color1=#0002>should</style> "
      "<style effect=\"glow\" color1=#fe87 color2=#f214 >go here</style> "
      "<u>right hand</u> "
      "And "
      "<u type=\"double\">now we need</u> "
      "to test some <font color=#f00 size=8>COLOR</font>\n",
      ETK_TRUE);
   
   
   /*etk_textblock_printf(ETK_TEXT_VIEW(text_view)->textblock);*/
   printf("\n");
   printf("%s\n", etk_string_get(etk_textblock_text_get(ETK_TEXT_VIEW(text_view)->textblock, ETK_TRUE)));
   /*printf("\n");
   printf("%s\n", etk_string_get(etk_textblock_text_get(ETK_TEXT_VIEW(text_view)->textblock, ETK_FALSE)));*/
   
   etk_box_pack_start(ETK_BOX(vbox), text_view, ETK_TRUE, ETK_TRUE, 0);
   
   etk_widget_show_all(win);
}

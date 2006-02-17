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
   etk_window_title_set(ETK_WINDOW(win), _("Etk Text View Test"));
   etk_widget_size_request_set(ETK_WIDGET(win), 400, 300);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   text_view = etk_text_view_new();
   /* TODO: */
   etk_textblock_text_set(ETK_TEXT_VIEW(text_view)->textblock,
      "<center><h1>Title</h1></center><br>"
      "<p><tab>A pragraph here <red>red text</red> and stuff.</p>"
      "<p>And escaping &lt; and &gt; as well as &amp; as <h1>normal.</h1></p>"
      "<p>If you want a newline use &lt;br&gt;<br>woo a new line!</p>"
      "<right>Right "
      "<style=outline color=#fff outline_color=#000>aligned</> "
      "<style=shadow shadow_color=#fff8>text</> "
      "<style=soft_shadow shadow_color=#0002>should</> "
      "<style=glow color=#fff glow2_color=#fe87 glow_color=#f214 >go here</> "
      "<style=far_shadow shadow_color=#0005>as it is</> "
      "<style=outline_shadow color=#fff outline_color=#8228 shadow_color=#005>within</> "
      "<style=outline_soft_shadow color=#fff outline_color=#8228 shadow_color=#0002>right tags</> "
      "<style=far_soft_shadow color=#fff shadow_color=#0002>to make it align to the</> "
      "<underline=on underline_color=#00f>right hand</> "
      "<backing=on backing_color=#fff8>side </><backing_color=#ff08>of</><backing_color=#0f08> </>"
      "<strikethrough=on strikethrough_color=#f0f8>the textblock</>.</right>"
      "<p>And "
      "<underline=double underline_color=#f00 underline2_color=#00f>now we need</> "
      "to test some <color=#f00 font_size=8>C</><color=#0f0 font_size=10>O</>"
      "<color=#00f font_size=12>L</><color=#fff font_size=14>O</>"
      "<color=#ff0 font_size=16>R</><color=#0ff font_size=18> Bla Rai</>"
      "<color=#f0f font_size=20> Stuff</>.</p>"
      "<blockquote>"
      "Heizölrückstoßabdämpfung fløde pingüino kilómetros cœur déçu l'âme "
      "plutôt naïve Louÿs rêva crapaüter Íosa Úrmhac Óighe pór Éava Ádhaim"
      "</blockquote>"
   );
   etk_box_pack_start(ETK_BOX(vbox), text_view, ETK_TRUE, ETK_TRUE, 0);
   
   etk_widget_show_all(win);
}

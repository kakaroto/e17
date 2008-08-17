/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "etk_test.h"
#include <stdlib.h>
#include <string.h>

typedef struct _IM_Button_Type
{
   Etk_Stock_Id stock_id;
   char *open_tag;
   char *close_tag;
} IM_Button_Type;

static void _etk_test_text_view_tag_window_create(void *data);
static void _etk_test_text_view_im_window_create(void *data);
static Etk_Bool _etk_test_im_editor_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);

static IM_Button_Type _im_buttons[] =
{
   { ETK_STOCK_FORMAT_TEXT_BOLD, "<b>", "</b>" },
   { ETK_STOCK_FORMAT_TEXT_ITALIC, "<i>", "</i>" },
   { ETK_STOCK_FORMAT_TEXT_UNDERLINE, "<u>", "</u>" }
};
static char *_im_buddies[] =
{
   "<b><font color=#16569e>Jack B:</font></b> ",
   "<b><font color=#609028>Chloe O:</font></b> ",
   "<b><font color=#a82f2f>David P:</font></b> "
};
static int _num_im_buttons = sizeof(_im_buttons) / sizeof(_im_buttons[0]);
static int _num_im_buddies = sizeof(_im_buddies) / sizeof(_im_buddies[0]);
static int _num_messages = 0;

/* Creates the window for the text view test */
void etk_test_text_view_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *button;

   if (win)
   {
      etk_widget_show(win);
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Text View Test");
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   button = etk_button_new_with_label("Tag Presentation");
   etk_signal_connect_swapped_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_test_text_view_tag_window_create), NULL);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_EXPAND, 0);

   button = etk_button_new_with_label("Instant Messenger");
   etk_signal_connect_swapped_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_test_text_view_im_window_create), NULL);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_EXPAND, 0);

   etk_widget_show_all(win);
}

/**************************
 *
 * Tag Presentation
 *
 **************************/

/* Creates the window for the tag presentation */
static void _etk_test_text_view_tag_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *text_view;
   Etk_Widget *scrolled_view;

   if (win)
   {
      etk_widget_show(win);
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Text View Test");
   etk_widget_size_request_set(win, 150, 150);
   etk_window_resize(ETK_WINDOW(win), 400, 300);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   text_view = etk_text_view_new();
   etk_textblock_text_set(ETK_TEXT_VIEW(text_view)->textblock,
      "<p align=\"center\"><b><u><font size=18>Etk Textblock</font></u></b></p>\n"
      "<b><u><font size=12>Supported tags:</font></u></b>"
      "<p left_margin=30>"
         "<b>&lt;b&gt;:</b> <b>Bold</b>\n"
         "<b>&lt;i&gt;:</b> <i>Italic</i>\n"
         "<b>&lt;u&gt;:</b> <u>Underline</u>"
      "</p>"
      "<p left_margin=60>"
            "<i>type:</i> Whether the text is single or double underlined\n"
            "<i>color1:</i> The color of the first underline\n"
            "<i>color2:</i> The color of the second underline"
      "</p>"
      "<p left_margin=30>"
         "<b>&lt;s&gt;:</b> <s>Strikethrough</s>"
      "</p>"
      "<p left_margin=60>"
            "<i>color:</i> The color of the strikethrough"
      "</p>"
      "<p left_margin=30>"
         "<b>&lt;font&gt;:</b>"
      "</p>"
      "<p left_margin=60>"
            "<i>face:</i> The face of the font\n"
            "<i>size:</i> The size of the font\n"
            "<i>color:</i> The color of the font"
      "</p>"
      "<p left_margin=30>"
         "<b>&lt;style&gt;:</b>"
      "</p>"
      "<p left_margin=60>"
            "<i>effect:</i> The type of effect to apply on the text\n"
            "<i>color1:</i> The first color of the effect\n"
            "<i>color2:</i> The second color of the effect"
      "</p>"
      "<p left_margin=30>"
         "<b>&lt;p&gt;:</b>"
      "</p>"
      "<p left_margin=60>"
            "<i>align:</i> The horizontal alignment of the text of the paragraph\n"
            "<i>wrap:</i> The way the text is wrapped\n"
            "<i>left_margin:</i> The left margin of the paragraph, in pixels\n"
            "<i>right_margin:</i> The right margin of the paragraph, in pixels"
      "</p>",
      ETK_TRUE);

   scrolled_view = etk_scrolled_view_new();

   etk_bin_child_set( ETK_BIN(scrolled_view), text_view );

   etk_box_append(ETK_BOX(vbox), scrolled_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   etk_widget_show_all(win);
}

/**************************
 *
 * Instant Messenger
 *
 **************************/

/* Creates the window of the instant messenger test app */
static void _etk_test_text_view_im_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vpaned;
   Etk_Widget *vbox, *hbox;
   Etk_Widget *button;
   Etk_Widget *message_view, *editor_view;
   Etk_Textblock *tb;
   int i;

   if (win)
   {
      etk_widget_show(win);
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Text View Test: Instant Messenger");
   etk_window_resize(ETK_WINDOW(win), 300, 300);
   etk_container_border_width_set(ETK_CONTAINER(win), 3);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vpaned = etk_vpaned_new();
   etk_container_add(ETK_CONTAINER(win), vpaned);

   message_view = etk_text_view_new();
   etk_widget_size_request_set(message_view, 200, 100);
   etk_paned_child1_set(ETK_PANED(vpaned), message_view, ETK_TRUE);
   tb = etk_text_view_textblock_get(ETK_TEXT_VIEW(message_view));
   etk_textblock_text_set(tb,
      "<i>Connected with David P. and Chloe O.</i>\n\n"
      "<b><font color=#a82f2f>David P:</font></b> Hello Jack!\n"
      "<b><font color=#a82f2f>David P:</font></b> How are you my friend?\n",
      ETK_TRUE);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_paned_child2_set(ETK_PANED(vpaned), vbox, ETK_FALSE);


   /* TODO: We must use a toolbar here instead */
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   for (i = 0; i < _num_im_buttons; i++)
   {
      button = etk_button_new_from_stock(_im_buttons[i].stock_id);
      etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   }


   editor_view = etk_text_view_new();
   etk_widget_size_request_set(editor_view, 200, 80);
   etk_box_append(ETK_BOX(vbox), editor_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(editor_view), ETK_CALLBACK(_etk_test_im_editor_key_down_cb), message_view);

   etk_widget_show_all(win);
}

/* Called when a key is pressed when the editor text view is focused */
static Etk_Bool _etk_test_im_editor_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Textblock *message_tb, *editor_tb;
   Etk_Textblock_Iter *iter, *cursor;
   Etk_String *message;
   int buddy_id;

   if (!(message_tb = etk_text_view_textblock_get(ETK_TEXT_VIEW(data))))
      return ETK_FALSE;
   if (!(editor_tb = etk_text_view_textblock_get(ETK_TEXT_VIEW(object))))
      return ETK_FALSE;

   message = etk_textblock_text_get(editor_tb, ETK_TRUE);
   if ((strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "KP_Enter") == 0))
   {
      iter = etk_textblock_iter_new(message_tb);
      etk_textblock_iter_forward_end(iter);

      if (event->modifiers & ETK_MODIFIER_SHIFT)
      {
         cursor = etk_text_view_cursor_get(ETK_TEXT_VIEW(object));
         etk_textblock_insert(editor_tb, cursor, "\n", -1);
      }
      else
      {
         if (etk_string_length_get(message) > 0)
         {

            buddy_id = _num_messages % _num_im_buddies;
            etk_textblock_insert_markup(message_tb, iter, _im_buddies[buddy_id], -1);
            etk_textblock_insert_markup(message_tb, iter, etk_string_get(message), -1);
            etk_textblock_insert(message_tb, iter, "\n", -1);

            etk_textblock_clear(editor_tb);
            etk_object_destroy(ETK_OBJECT(message));
            _num_messages++;
         }
      }

      etk_textblock_iter_free(iter);
      return ETK_TRUE;
   }

   return ETK_FALSE;
}

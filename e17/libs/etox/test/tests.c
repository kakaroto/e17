#include "Etox_test.h"

void
test_basic (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  char msg[] =
     "This series of tests will exercise the most basic\n"
     "functions of etox, reading and displaying text with\n"
     "basic formatting.\n"
     "\n"
     "In addition, prepending, appending and inserting text\n"
     "in an existing etox will be tested\n"
     "\n"
     "Click Next to begin the tests.";

  /* Setup message etox */
  /* Clip rectangle for bounding where the message text is drawn */
  clip_msg = evas_add_rectangle (evas);
  evas_show (evas, clip_msg);
  evas_set_color (evas, clip_msg, 255, 0, 255, 255);
  evas_move (evas, clip_msg, 40, 40);
  evas_resize (evas, clip_msg, 520, 140);
  /* Create message etox */
  e_msg = etox_new_all (evas, 40, 40, 520, 140, 255, ETOX_ALIGN_LEFT);
  etox_context_set_align (e_msg, ETOX_ALIGN_LEFT);
  etox_context_set_font (e_msg, "sinon", 14);
  etox_context_set_style (e_msg, "plain");
  etox_context_set_color (e_msg, 255, 255, 255, 255);
  etox_set_clip (e_msg, clip_msg);
  etox_set_alpha (e_msg, 255);
  etox_set_layer (e_msg, 1000);
  etox_set_text (e_msg, msg);
  etox_show (e_msg);

  /* Setup test etox */
  /* Setup test etox background */
  o_bg_etox = evas_add_rectangle (evas);
  evas_move (evas, o_bg_etox, 40, 200);
  evas_resize (evas, o_bg_etox, 520, 260);
  evas_set_color (evas, o_bg_etox, 0, 100, 100, 100);
  evas_set_layer (evas, o_bg_etox, 100);
  evas_show (evas, o_bg_etox);
  /* Clip rectangle for bounding where the test text is drawn */
  clip_test = evas_add_rectangle (evas);
  evas_set_color (evas, clip_test, 255, 0, 255, 255);
  evas_move (evas, clip_test, 40, 200);
  evas_resize (evas, clip_test, 520, 260);
  evas_show (evas, clip_test);
  /* Create test etox */
  e_test = etox_new_all (evas, 40, 200, 520, 260, 255, ETOX_ALIGN_CENTER);
  etox_context_set_align (e_test, ETOX_ALIGN_CENTER);
  etox_context_set_font (e_test, "sinon", 14);
  etox_context_set_style (e_test, "plain");
  etox_context_set_color (e_test, 255, 255, 255, 255);
  etox_set_clip (e_test, clip_test);
  etox_set_alpha (e_test, 255);
  etox_set_layer (e_test, 1000);

  /* Create "Next" button */
  button_next_new_all (evas);

  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_IN, mouse_in, NULL);
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_OUT, mouse_out, NULL);
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_DOWN,
        test_basic_get, e_msg);
}

void
test_basic_get (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  char msg[] = 
     "The sample text shown below was retrieved from\n"
     "the text in this message etox.\n"
     "\n"
     "The function used was etox_get_text.";

  char *string;

  /* Change message */
  etox_set_text (_data, msg);

  /* Recreate "Next" button */
  button_next_new(_e);
   
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_IN, mouse_in, NULL);
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_OUT, mouse_out, NULL);
  evas_callback_add (_e, o_txt_next_box, CALLBACK_MOUSE_DOWN,
        test_basic_set, _data);
 
  /* Set test text and show test etox */
  string = etox_get_text (e_msg);
  etox_set_text (e_test, string);
  etox_show (e_test);
}

void
test_basic_set (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  char msg[] = 
    "The text in an etox can be changed on the fly.\n" 
    "The sample paragraph shown below replaced the\n"
    "text shown in the previous test.\n"
    "\n"
    "The function used was etox_set_text.";

  char string[] = 
     "\n"
    "A file that big?\n"
    "It might be very useful.\n"
    "But now it is gone.\n"
    "\n";

  /* Change message */
  etox_set_text (_data, msg);

  /* Recreate "Next" button */
  button_next_new(_e);
 
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_IN, mouse_in, NULL);
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_OUT, mouse_out, NULL);
  evas_callback_add (_e, o_txt_next_box, CALLBACK_MOUSE_DOWN,
        test_basic_append, _data);
 
  /* Change test etox */
  etox_set_text (e_test, string);
}

void
test_basic_append (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  char msg[] = 
    "Text can be appended to the text already existent\n"
    "in the etox.\n"
    "\n"
    "The second paragraph shown below was appended\n"
    "at the end of the etox through etox_append_text.\n";

  char string[] = 
    "The Tao that is seen\n"
    "Is not the true Tao\n"
    "Until you bring fresh toner.\n"
    "\n";

  /* Change message */
  etox_set_text (_data, msg);

  /* Recreate "Next" button */
  button_next_new(_e);

  /* Change test etox */
  etox_append_text (e_test, string);
}

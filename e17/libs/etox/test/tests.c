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
     "in an existing etox will be tested.\n"
     "\n"
     "Click Next to begin the tests.";

  etox_set_text (e_msg, msg);

  /* Create "Next" button */
  button_next_new_all (evas);

  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_IN, button_mouse_in, NULL);
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_OUT, button_mouse_out, NULL);
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
   
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_IN, button_mouse_in, NULL);
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_OUT, button_mouse_out, NULL);
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
 
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_IN, button_mouse_in, NULL);
  evas_callback_add (evas, o_txt_next_box, CALLBACK_MOUSE_OUT, button_mouse_out, NULL);
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

void
test_style (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  printf ("Style button clicked");
}

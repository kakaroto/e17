#include "Etox_test.h"

void
test_basic (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{

  char msg[] = "This test is exercising the most basic\n"
    "functions of etox, reading and\n"
    "displaying text with basic formatting.";

  char string[] = "\n"
    "This text should test most of the basic\n"
    "text display characteristics of etox.\n"
    "including multi-line text, \ttabs,\n"
    "and eventually text styles and inline formatting.\n"
    "\n"
    "The real test will be when there are huge\n"
    "strings of text that wrap around obstacles.\n"
    "At this point each line only contains 1 bit,\n"
    "but when wrapping obstacles, multiple bits\n"
    "will be needed to represent the lines.";

  etox_set_text (e_msg, msg);
  etox_show (e_msg);

  /* Test etox background */
  o_bg_etox = evas_add_rectangle (evas);
  evas_move (evas, o_bg_etox, 40, 200);
  evas_resize (evas, o_bg_etox, 520, 260);
  evas_set_color (evas, o_bg_etox, 0, 100, 100, 100);
  evas_set_layer (evas, o_bg_etox, 100);
  evas_show (evas, o_bg_etox);

  /* Clip rectangle for bounding where the test text is drawn */
  clip_test = evas_add_rectangle (evas);
  evas_show (evas, clip_test);
  evas_set_color (evas, clip_test, 255, 0, 255, 255);
  evas_move (evas, clip_test, 40, 200);
  evas_resize (evas, clip_test, 520, 260);

  /* Test etox */
  e_test = etox_new_all (evas, 40, 200, 520, 260, 255, ETOX_ALIGN_CENTER);
  etox_set_text (e_test, string);
  etox_context_set_align (e_test, ETOX_ALIGN_CENTER);
  etox_context_set_style (e_test, "raised");
  etox_context_set_color (e_test, 128, 255, 255, 255);
  etox_set_clip (e_test, clip_test);
  etox_set_alpha (e_test, 128);
  etox_set_layer (e_test, 1000);

  etox_show (e_test);
}

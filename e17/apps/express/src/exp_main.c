#include "Express.h"

static void exp_fake_buddies(Exp *exp);
  
int
main (int argc, char **argv)
{
  Exp *exp;

  ecore_init();
  ecore_evas_init();
  edje_init();

  ecore_app_args_set(argc, (const char **)argv);
  
  exp = (Exp *)calloc(1, sizeof(Exp));
  exp->ee = ecore_evas_software_x11_new(0,0,0,0,255,255);
  exp->evas = ecore_evas_get(exp->ee);
  ecore_evas_data_set(exp->ee, "exp", exp);

  exp_gui_setup(exp);

  ecore_evas_show(exp->ee);

  exp_fake_buddies(exp);

  ecore_main_loop_begin();

  edje_shutdown();
  ecore_evas_shutdown();
  ecore_shutdown();
  return 0;
}



static void
exp_fake_buddies(Exp *exp)
{
  Exp_Buddy *b;
  int i;

  char *names[] = {
    "tokyo",
    "rephorm",
    "godiva",
    "meowma"
  };

  for (i = 0; i < 4; i++)
  {
    Evas_Object *o;
    int w, h;

    b = exp_buddy_new(exp);
    exp_buddy_name_set(b, names[i]);

    o = evas_object_image_add(exp->evas);
    if (i%2)evas_object_image_file_set(o, PACKAGE_DATA_DIR"/icons/gryffon.png", "");
    else evas_object_image_file_set(o, PACKAGE_DATA_DIR"/icons/eagle.png", "");
    /* hack in a 'user' */
    if (i == 1) exp->user = b;
    evas_object_image_size_get(o, &w, &h);
    exp_buddy_icon_data_set(b, evas_object_image_data_get(o, 0), w, h);
    evas_object_del(o);
  }
}



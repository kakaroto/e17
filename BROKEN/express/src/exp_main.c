/*
 * vim:ts=4:cino=t0
 */

#include "Express.h"
#include "exp_eb.h"

static void exp_fake_buddies(Exp *exp);
  
int
main (int argc, char **argv)
{
  Exp *exp;

  if (!ecore_init())
  {
	printf("Unable to init ecore\n");
	return 1;
  }
  if (!ecore_evas_init())
  {
	printf("Unable to init ecore evas\n");
	return 1;
  }
  if (!edje_init())
  {
	printf("Unable to init edje\n");
	return 1;
  }

  ecore_app_args_set(argc, (const char **)argv);
  
  exp = (Exp *)calloc(1, sizeof(Exp));

  if (!exp_eb_init("~/.everybuddy"))
  {
	printf("Unable to setup Everybuddy subsystem\n");
	return 1;
  }

  if (!exp_services_init())
  {
    printf("Unable to setup services handler\n");
    return 1;
  }

  if (!exp_gui_init(exp))
  {
    printf("Unable to setup GUI subsystem\n");
  }

  ecore_evas_show(exp->ee);

  exp_fake_buddies(exp);

  if (!exp_eb_connect(exp, "localhost"))
  {
    printf("Unable to connect to Everybody\n");
    /* XXX may want to exit here in the future ... */
  }

  ecore_main_loop_begin();

  exp_services_shutdown();
  exp_eb_shutdown();
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



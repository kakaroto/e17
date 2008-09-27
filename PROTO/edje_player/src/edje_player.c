#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <popt.h>

#include <Ecore.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "option_pool.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// defines

// function prototypes
void resize_cb(Ecore_Evas *ee);
void print_help (char *app_name);
void list_engines ();
void list_groups (const char *edje_file);
void parse_options (int argc, char **argv);
void show_version ();

// variables
Ecore_Evas *ee;
Evas *evas;

Evas_Object *o_bg;
Evas_Object *o_edje;

enum
{
  ARG_GROUP = 1,
  ARG_ENGINE,
  ARG_BORDERLESS,
  ARG_STICKY,
  ARG_SHAPED,
  ARG_ALPHA,
  ARG_LIST_ENGINES,
  ARG_LIST_GROUPS,
  ARG_VERSION
};

void resize_cb(Ecore_Evas *ee)
{
  Evas_Coord x, y, w, h;

  ecore_evas_geometry_get (ee, &x, &y, &w, &h);

  if (w < h)
  {
    evas_object_resize(o_bg, w, w);
    evas_object_resize(o_edje, w, w);
  }
  else
  {
    evas_object_resize(o_bg, h, h);
    evas_object_resize(o_edje, h, h);
  }

}

void print_help (char *app_name)
{
  printf("Usage: %s --help\n", app_name);
  exit(-1);
}

int main(int argc, char **argv)
{
  int edje_w = 240;
  int edje_h = 240;
  bool edje_load = false;
  
  option_pool_constructor ();
  
  switch (argc)
  {
    case 1:
      print_help (argv[0]);
      break;
    default:
      parse_options (argc, argv);
  }

  ecore_init();
  ecore_app_args_set(argc, (const char **)argv);
  ecore_evas_init();
  edje_init();
  
  ee = ecore_evas_new (option_pool.engine, 0, 0, edje_w, edje_h, NULL);

  if (ee)
  {
    printf ("Using engine %s to create a canvas...\n", option_pool.engine);
  }
  else
  {
    fprintf (stderr, "Engine %s failed to create a canvas\n", option_pool.engine);
    fprintf (stderr, "Exiting...\n");
    exit (1);
  }
  
  if (option_pool.list_groups_flag)
  {
    list_groups (option_pool.file);
    exit (0);
  }

  ecore_evas_title_set (ee, "Edje Player");

  if (option_pool.borderless)
  {
    ecore_evas_borderless_set (ee, 1);
  }

  if (option_pool.sticky)
  {
    ecore_evas_sticky_set (ee, 1);
  }
  
  if (option_pool.alpha || option_pool.shaped) 
  {
    if (option_pool.alpha)
    {
       ecore_evas_alpha_set(ee, 1);
    }
    else if (option_pool.shaped)
    {
       ecore_evas_shaped_set(ee, 1);
    }
    
    // set alpha transparent background if window has alpha support or is shaped...
    evas_object_color_set(o_bg, 0, 0, 0, 0);
  } 
  else
  {
    // ...and set black background in other case
    evas_object_color_set(o_bg, 0, 0, 0, 255);
  }
  
  evas = ecore_evas_get(ee);
  evas_image_cache_set(evas, 8192 * 1024);
  evas_font_cache_set(evas, 512 * 1024);

  o_bg = evas_object_rectangle_add(evas);
  evas_object_move(o_bg, 0, 0);
  evas_object_resize(o_bg, edje_w, edje_h);
  evas_object_color_set(o_bg, 0, 0, 0, 255);
  
  evas_object_focus_set(o_edje, 1);

  evas_object_show(o_bg);
  evas_object_show(o_edje);

  o_edje = edje_object_add (evas);
  
  edje_load = edje_object_file_set(o_edje, option_pool.file, option_pool.group);
  if (!edje_load)
  {
    fprintf (stderr, "The Edje file '%s' and group '%s' couldn't be set!\n", option_pool.file, option_pool.group);
    exit (1);
  }
  
  evas_object_move(o_edje, 0, 0);
  evas_object_resize(o_edje, edje_w, edje_h);

  evas_object_show(o_edje);

  ecore_evas_callback_resize_set(ee, resize_cb);
  ecore_evas_show(ee);

  ecore_main_loop_begin();

  edje_shutdown();
  ecore_evas_shutdown();
  ecore_shutdown();
  
  option_pool_destructor ();

  return 0;
}

void list_engines ()
{
  Evas_List *engines_list;
  Evas_List *l; // list element
 
  engines_list = ecore_evas_engines_get ();
  for (l = engines_list; l; l = evas_list_next(l))
  {
    const char *engine = (const char*) l->data;
    
    printf ("%s : supported\n", engine);
  }
}

void list_groups (const char *edje_file)
{
  Evas_List *group_list;
  Evas_List *l; // list element

  group_list = edje_file_collection_list (edje_file);

  printf("Available groups in 'edje_file':\n");
  for (l = group_list; l; l = evas_list_next(l))
  {
    printf("%s\n", (const char*) l->data);
  }

  edje_file_collection_list_free (group_list);
}

void parse_options (int argc, char **argv)
{
  poptContext context;
  int option;
  char *option_group_tmp = NULL;
  char *option_engine_tmp = NULL;
  const char *option_leftover_tmp = NULL;

  struct poptOption options[] =
    {
      {"group", 'g', POPT_ARG_STRING, &option_group_tmp, ARG_GROUP,
        "The Edje group to display (default: 'main').",
       NULL},
      {"engine", 'e', POPT_ARG_STRING, &option_engine_tmp, ARG_ENGINE,
       "The Evas engine type (use environment variable $ECORE_EVAS_ENGINE if not set).",
       NULL},
      {"borderless", 'b', POPT_ARG_NONE, NULL, ARG_BORDERLESS,
       "Display window without border.",
       NULL},
      {"sticky", 's', POPT_ARG_NONE, NULL, ARG_STICKY,
       "Display window sticky.",
       NULL},
      {"shaped", 'p', POPT_ARG_NONE, NULL, ARG_SHAPED,
       "Display window shaped.",
       NULL},
      {"alpha", 'a', POPT_ARG_NONE, NULL, ARG_ALPHA,
       "Display window with alpha channel (needs composite support!).",
       NULL},
      {"list-engines", 'l', POPT_ARG_NONE, NULL, ARG_LIST_ENGINES,
       "List all available engines.",
       NULL},
      {"list-groups", 'r', POPT_ARG_NONE, NULL, ARG_LIST_GROUPS,
       "List all available groups in the Edje file.",
       NULL},
      {"version", 'v', POPT_ARG_NONE, NULL, ARG_VERSION, "show version", NULL},
      POPT_AUTOHELP {NULL, '\0', 0, NULL, 0}
    };

  context = poptGetContext ("popt1", argc, (const char **) argv, options, 0);

  /* start option handling */
  while ((option = poptGetNextOpt (context)) > 0)
  {  
    switch (option)
    {
    case ARG_GROUP:
      option_pool.group = strdup (option_group_tmp);
      break;
    case ARG_ENGINE:
      option_pool.engine = strdup (option_engine_tmp);
      break;
    case ARG_BORDERLESS:
      option_pool.borderless = true;
      break;
    case ARG_STICKY:
      option_pool.sticky = true;
      break;
    case ARG_SHAPED:
      option_pool.shaped = true;
      break;
    case ARG_ALPHA:
      option_pool.alpha = true;
      break;
    case ARG_LIST_ENGINES:
      list_engines ();
      exit (0);
      break;
    case ARG_LIST_GROUPS:
      option_pool.list_groups_flag = true;
      break;
    case ARG_VERSION:
      show_version ();
      exit (0);
      break;
    }
  }

  if (!option_pool.group)
  {
    // set 'main' as default group if nothing else is set
    option_pool.group = strdup ("main");
  }
  
  option_leftover_tmp = poptGetArg (context);

  if (option_leftover_tmp)
  {
    option_pool.file = strdup (option_leftover_tmp);
  }
  else
  {
    printf ("You need to give a .edj file on command line!\n");
    exit (1);
  }
  
  poptFreeContext (context);
}

void show_version ()
{
  printf ("Package name: ");
  printf ("%s %s\n", PACKAGE, VERSION);
  printf ("Build information: ");
  printf ("%s %s\n", __DATE__, __TIME__);
}

/*char *e_strdup_printf (const char *format, ...)
{
  va_list ap;
  char *target = NULL;
  char tmp[1024];
  
  va_start (ap, format);

  target strdup (
    
  vsprintf(target, format, ap);
}*/

/*void dbg_printf (const gchar *format, ...)
{
  va_list ap;

  va_start (ap, format);

  printf ("(%s, %d): ", __FILE__, __LINE__);
  g_vprintf(format, ap);
}*/


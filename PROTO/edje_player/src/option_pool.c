#include <stdlib.h>
#include <popt.h>
#include <string.h>
#include "option_pool.h"

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

void option_pool_constructor (struct option_pool_t *option_pool)
{
  option_pool->engine = NULL;
  option_pool->group = NULL;
  option_pool->file = NULL;
  option_pool->borderless = false;
  option_pool->sticky = false;
  option_pool->shaped = false;
  option_pool->alpha = false;
  option_pool->list_groups_flag = false;
  option_pool->list_engines = false;
}

void option_pool_destructor (struct option_pool_t *option_pool)
{
  free (option_pool->group);
  free (option_pool->file);
  free (option_pool->engine);
}

void option_pool_parse (struct option_pool_t *option_pool, int argc, char **argv)
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
      option_pool->group = strdup (option_group_tmp);
      break;
    case ARG_ENGINE:
      option_pool->engine = strdup (option_engine_tmp);
      break;
    case ARG_BORDERLESS:
      option_pool->borderless = true;
      break;
    case ARG_STICKY:
      option_pool->sticky = true;
      break;
    case ARG_SHAPED:
      option_pool->shaped = true;
      break;
    case ARG_ALPHA:
      option_pool->alpha = true;
      break;
    case ARG_LIST_ENGINES:
      option_pool->list_engines = true;
      break;
    case ARG_LIST_GROUPS:
      option_pool->list_groups_flag = true;
      break;
    case ARG_VERSION:
      option_pool->show_version = true;
      break;
    }
  }

  if (!option_pool->group)
  {
    // set 'main' as default group if nothing else is set
    option_pool->group = strdup ("main");
  }
  
  option_leftover_tmp = poptGetArg (context);

  if (option_leftover_tmp)
  {
    option_pool->file = strdup (option_leftover_tmp);
  }
  else
  {
    printf ("You need to give a .edj file on command line!\n");
    exit (1);
  }
  
  poptFreeContext (context);
}

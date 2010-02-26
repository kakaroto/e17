#include <stdlib.h>
#include <stdio.h>

#include <Ecore.h>
#include <Ecore_X.h>

#include "elicit.h"
#include "config.h"

void
parse_options(int argc, char **argv, Elicit *el)
{
}

int
main(int argc, char **argv)
{
  Elicit *el;

  if (!elicit_libs_init())
    return 1;
  
  ecore_app_args_set(argc, (const char **)argv);

  el = elicit_new();
  if (!el)
  {
    fprintf(stderr, "[Elicit] Out of memory?\n");
    elicit_libs_shutdown();
    return 1;
  }

  elicit_config_load(el);
  parse_options(argc, argv, el);
  elicit_theme_set(el, el->conf.theme);
  elicit_show(el);

  ecore_main_loop_begin();

  elicit_config_save(el);
  if (el) elicit_free(el);
  elicit_libs_shutdown();

  return 0;
}

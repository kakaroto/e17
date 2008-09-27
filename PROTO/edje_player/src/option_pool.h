#ifndef OPTION_POOL_H
#define OPTION_POOL_H

#include "stdbool.h"

struct option_pool_t
{
  char *engine;
  char *group;
  char *file;
  bool borderless;
  bool sticky;
  bool shaped;
  bool alpha;
  bool list_groups_flag;
  bool list_engines;
  bool show_version;
};

void option_pool_constructor (struct option_pool_t *option_pool);
void option_pool_destructor (struct option_pool_t *option_pool);
void option_pool_parse (struct option_pool_t *option_pool, int argc, char **argv);

#endif // OPTION_POOL_H

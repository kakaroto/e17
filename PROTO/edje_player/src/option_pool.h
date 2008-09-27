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
} option_pool;

void option_pool_constructor ();
void option_pool_destructor ();

#endif // OPTION_POOL_H

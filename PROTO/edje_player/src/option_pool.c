#include <stdlib.h>
#include "option_pool.h"

void option_pool_constructor ()
{
  option_pool.engine = NULL;
  option_pool.group = NULL;
  option_pool.file = NULL;
  option_pool.borderless = false;
  option_pool.sticky = false;
  option_pool.shaped = false;
  option_pool.alpha = false;
  option_pool.list_groups_flag = false;
}

void option_pool_destructor ()
{
  free (option_pool.group);
  free (option_pool.file);
  free (option_pool.engine);
}

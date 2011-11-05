#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* Project */
#include "../include/elementaryxx/Gen.h"
#include "localUtil.h"

/* STD */
#include <cassert>

using namespace std;

namespace Elmxx {

void Gen::clear ()
{
  elm_gen_clear (o);
}

void Gen::setAlwaysSelectMode (bool alwaysSelect)
{
  elm_gen_always_select_mode_set (o, alwaysSelect);
}

bool Gen::getAlwaysSelectMode ()
{
  return elm_gen_always_select_mode_get (o);
}

void Gen::setNoSelectMode (bool noSelect)
{
  elm_gen_no_select_mode_set (o, noSelect);
}

bool Gen::getNoSelectMode ()
{
  return elm_gen_no_select_mode_get (o);
}

void Gen::setBounce (bool hBounce, bool vBounce)
{
  elm_gen_bounce_set (o, hBounce, vBounce);
}

void Gen::getBounce (bool &hBounceOut, bool &vBounceOut)
{
  Eina_Bool h, v;
  elm_gen_bounce_get (o, &h, &v);
  hBounceOut = h;
  vBounceOut = v;
}

} // end namespace Elmxx

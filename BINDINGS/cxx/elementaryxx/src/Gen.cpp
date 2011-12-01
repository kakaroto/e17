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

Gen::Gen () {}

Gen::~Gen () {}  
  
void Gen::clear ()
{
  elm_genlist_clear (o);
}

void Gen::setAlwaysSelectMode (bool alwaysSelect)
{
  elm_genlist_always_select_mode_set (o, alwaysSelect);
}

bool Gen::getAlwaysSelectMode ()
{
  return elm_genlist_always_select_mode_get (o);
}

void Gen::setNoSelectMode (bool noSelect)
{
  elm_genlist_no_select_mode_set (o, noSelect);
}

bool Gen::getNoSelectMode ()
{
  return elm_genlist_no_select_mode_get (o);
}

void Gen::setBounce (bool hBounce, bool vBounce)
{
  elm_genlist_bounce_set (o, hBounce, vBounce);
}

void Gen::getBounce (bool &hBounceOut, bool &vBounceOut)
{
  Eina_Bool h, v;
  elm_genlist_bounce_get (o, &h, &v);
  hBounceOut = h;
  vBounceOut = v;
}

} // end namespace Elmxx

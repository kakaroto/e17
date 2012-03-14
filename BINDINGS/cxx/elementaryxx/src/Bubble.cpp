#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Bubble.h"

using namespace std;

namespace Elmxx {

Bubble::Bubble (Evasxx::Object &parent)
{
  o = elm_bubble_add (parent.obj ());
  
  elmInit ();
}

Bubble::~Bubble () {}

Bubble *Bubble::factory (Evasxx::Object &parent)
{
  return new Bubble (parent);
}

} // end namespace Elmxx

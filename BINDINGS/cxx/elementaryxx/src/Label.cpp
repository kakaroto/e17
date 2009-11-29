#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Label.h"

using namespace std;

namespace Elmxx {

Label::Label (Evasxx::Object &parent)
{
  o = elm_label_add (parent.obj ());
  
  elmInit ();
}

Label::~Label () {}

Label *Label::factory (Evasxx::Object &parent)
{
  return new Label (parent);
}

void Label::setLabel (const std::string &label)
{
  elm_label_label_set (o, label.c_str ());
}

} // end namespace Elmxx

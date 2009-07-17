#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmLabel.h"

using namespace std;

namespace efl {

ElmLabel::ElmLabel (EvasObject &parent)
{
  o = elm_label_add (parent.obj ());
  
  elmInit ();
}

ElmLabel::~ElmLabel () {}

ElmLabel *ElmLabel::factory (EvasObject &parent)
{
  return new ElmLabel (parent);
}

void ElmLabel::setLabel (const std::string &label)
{
  elm_label_label_set (o, label.c_str ());
}

} // end namespace efl

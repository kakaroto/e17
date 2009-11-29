#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Icon.h"

using namespace std;

namespace Elmxx {

Icon::Icon (Evasxx::Object &parent)
{
  o = elm_icon_add (parent.obj ());
  
  elmInit ();
}

Icon::~Icon () {}

Icon *Icon::factory (Evasxx::Object &parent)
{
  return new Icon (parent);
}

bool Icon::setFile (const std::string &file)
{
  return elm_icon_file_set (o, file.c_str (), NULL);
}

bool Icon::setFile (const std::string &file, const std::string &group)
{
  return elm_icon_file_set (o, file.c_str (), group.c_str ());
}

void Icon::setStandard (const std::string &name)
{
  elm_icon_standard_set (o, name.c_str ());
}
void Icon::setSmooth (bool smooth)
{
  elm_icon_smooth_set (o, smooth);
}
void Icon::setNoScale (bool noScale)
{
  elm_icon_no_scale_set (o, noScale);
}
void Icon::setScale (bool scaleUp, bool scaleDown)
{
  elm_icon_scale_set (o, scaleUp, scaleDown);
}
void Icon::setFillOutside (bool fillOutside)
{
  elm_icon_fill_outside_set (o, fillOutside);
}
void Icon::setPrescale (int size)
{
  elm_icon_prescale_set (o, size);
}
} // end namespace Elmxx

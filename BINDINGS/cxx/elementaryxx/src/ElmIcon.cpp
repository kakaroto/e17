#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmIcon.h"

using namespace std;

namespace efl {

ElmIcon::ElmIcon (EvasObject &parent)
{
  o = elm_icon_add (parent.obj ());
  
  elmInit ();
}

ElmIcon::~ElmIcon () {}

ElmIcon *ElmIcon::factory (EvasObject &parent)
{
  return new ElmIcon (parent);
}

bool ElmIcon::setFile (const std::string &file)
{
  return elm_icon_file_set (o, file.c_str (), NULL);
}

bool ElmIcon::setFile (const std::string &file, const std::string &group)
{
  return elm_icon_file_set (o, file.c_str (), group.c_str ());
}

void ElmIcon::setStandard (const std::string &name)
{
  elm_icon_standard_set (o, name.c_str ());
}
void ElmIcon::setSmooth (bool smooth)
{
  elm_icon_smooth_set (o, smooth);
}
void ElmIcon::setNoScale (bool noScale)
{
  elm_icon_no_scale_set (o, noScale);
}
void ElmIcon::setScale (bool scaleUp, bool scaleDown)
{
  elm_icon_scale_set (o, scaleUp, scaleDown);
}
void ElmIcon::setFillOutside (bool fillOutside)
{
  elm_icon_fill_outside_set (o, fillOutside);
}
void ElmIcon::setPrescale (int size)
{
  elm_icon_prescale_set (o, size);
}
} // end namespace efl

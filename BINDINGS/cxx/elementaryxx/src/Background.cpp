#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Background.h"

using namespace std;

namespace Elmxx {

Background::Background (Evasxx::Object &parent)
{
  o = elm_bg_add (parent.obj ());
  
  elmInit ();
}

Background::~Background () {}

Background *Background::factory (Evasxx::Object &parent)
{
  return new Background (parent);
}

void Background::setFile (const std::string &file)
{
  elm_bg_file_set (o, file.empty() ? NULL : file.c_str (), NULL); 
}

void Background::setFile (const std::string &file, const std::string &group)
{
  elm_bg_file_set (o, file.empty() ? NULL : file.c_str (), group.empty() ? NULL : group.c_str ());
}

void Background::setOption(Elm_Bg_Option option)
{
  elm_bg_option_set(o, option);
}

Elm_Bg_Option Background::getOption()
{
  return elm_bg_option_get(o);
}

void Background::setColor(const Eflxx::Color c)
{
  elm_bg_color_set(o, c.red(), c.green(), c.blue());
}

Eflxx::Color Background::getColor()
{
  int r,g,b;
  elm_bg_color_get(o, &r, &g, &b);
  Eflxx::Color rgb (r,g,b);
  return rgb;
}

void Background::setLoadSize (Eflxx::Size s)
{
  elm_bg_load_size_set(o, s.width(), s.height());
}

} // end namespace Elmxx

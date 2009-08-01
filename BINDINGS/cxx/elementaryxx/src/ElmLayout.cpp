#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmLayout.h"

using namespace std;

namespace efl {

ElmLayout::ElmLayout (EvasObject &parent)
{
  o = elm_layout_add (parent.obj ());
  
  elmInit ();
}

ElmLayout::~ElmLayout () {}

ElmLayout *ElmLayout::factory (EvasObject &parent)
{
  return new ElmLayout (parent);
}

bool ElmLayout::setFile (const std::string &file)
{
  return elm_layout_file_set (o, file.c_str (), NULL);
}

bool ElmLayout::setFile (const std::string &file, const std::string &group)
{
  return elm_layout_file_set (o, file.c_str (), group.c_str ());
}

void ElmLayout::setContent (const std::string &swallow, const EvasObject &content)
{
  elm_layout_content_set (o, swallow.c_str (), content.obj ());
}

} // end namespace efl

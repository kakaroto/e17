#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Layout.h"

using namespace std;

namespace Elmxx {

Layout::Layout (Evasxx::Object &parent)
{
  o = elm_layout_add (parent.obj ());
  
  elmInit ();
}

Layout::~Layout () {}

Layout *Layout::factory (Evasxx::Object &parent)
{
  return new Layout (parent);
}

bool Layout::setFile (const std::string &file)
{
  return elm_layout_file_set (o, file.c_str (), NULL);
}

bool Layout::setFile (const std::string &file, const std::string &group)
{
  return elm_layout_file_set (o, file.c_str (), group.c_str ());
}

void Layout::setContent (const std::string &swallow, const Evasxx::Object &content)
{
  elm_layout_content_set (o, swallow.c_str (), content.obj ());
}

} // end namespace Elmxx

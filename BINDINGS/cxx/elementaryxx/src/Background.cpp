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
  elm_bg_file_set (o, file.c_str (), NULL); 
}

void Background::setFile (const std::string &file, const std::string &group)
{
  elm_bg_file_set (o, file.c_str (), group.c_str ());
}

} // end namespace Elmxx

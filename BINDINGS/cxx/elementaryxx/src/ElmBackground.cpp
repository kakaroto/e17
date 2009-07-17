#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmBackground.h"

using namespace std;

namespace efl {

ElmBackground::ElmBackground (EvasObject &parent)
{
  o = elm_bg_add (parent.obj ());
  
  elmInit ();
}

ElmBackground::~ElmBackground () {}

ElmBackground *ElmBackground::factory (EvasObject &parent)
{
  return new ElmBackground (parent);
}

void ElmBackground::setFile (const std::string &file)
{
  elm_bg_file_set (o, file.c_str (), NULL); 
}

void ElmBackground::setFile (const std::string &file, const std::string &group)
{
  elm_bg_file_set (o, file.c_str (), group.c_str ());
}

} // end namespace efl

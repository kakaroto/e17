#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Frame.h"

using namespace std;

namespace Elmxx {

Frame::Frame (Evasxx::Object &parent)
{
  o = elm_frame_add (parent.obj ());
  
  elmInit ();
}

Frame::~Frame () {}

Frame *Frame::factory (Evasxx::Object &parent)
{
  return new Frame (parent);
}

void Frame::setLabel (const std::string &label)
{
  elm_object_text_set (o, label.c_str ());
}

void Frame::setContent (const Evasxx::Object &content)
{
  elm_frame_content_set (o, content.obj ());
}

} // end namespace Elmxx

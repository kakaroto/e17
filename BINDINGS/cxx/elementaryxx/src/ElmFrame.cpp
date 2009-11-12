#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmFrame.h"

using namespace std;

namespace efl {

ElmFrame::ElmFrame (EvasObject &parent)
{
  o = elm_frame_add (parent.obj ());
  
  elmInit ();
}

ElmFrame::~ElmFrame () {}

ElmFrame *ElmFrame::factory (EvasObject &parent)
{
  return new ElmFrame (parent);
}

void ElmFrame::setLabel (const std::string &label)
{
  elm_frame_label_set (o, label.c_str ());
}

void ElmFrame::setContent (const EvasObject &content)
{
  elm_frame_content_set (o, content.obj ());
}

} // end namespace efl

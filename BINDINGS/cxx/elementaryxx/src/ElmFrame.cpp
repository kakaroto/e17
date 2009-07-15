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

ElmFrame::~ElmFrame ()
{
  if (mFree)
  {
    evas_object_del (o);
  }
}

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

void ElmFrame::setStyle (const std::string &style)
{
  elm_frame_style_set (o, style.c_str ());
}

} // end namespace efl

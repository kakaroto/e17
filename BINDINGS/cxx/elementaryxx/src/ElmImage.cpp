#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmImage.h"

using namespace std;

namespace efl {

ElmImage::ElmImage (EvasObject &parent)
{
  o = elm_image_add (parent.obj ());
  
  elmInit ();
}

ElmImage::~ElmImage ()
{
  if (mFree)
  {
    evas_object_del (o);
  }
}

ElmImage *ElmImage::factory (EvasObject &parent)
{
  return new ElmImage (parent);
}

bool ElmImage::setFile (const std::string &file, const std::string &group)
{
  return elm_image_file_set (o, file.c_str (), group.c_str ());
}

void ElmImage::setSmooth (bool smooth)
{
  elm_image_smooth_set (o, smooth);
}

void ElmImage::setNoScale (bool noScale)
{
  elm_image_no_scale_set (o, noScale);
}

void ElmImage::setScale (bool scaleUp, bool scaleDown)
{
  elm_image_scale_set (o, scaleUp, scaleDown);
}

void ElmImage::setFillOutside (bool fillOutside)
{
  elm_image_fill_outside_set (o, fillOutside);
}

void ElmImage::setPrescale (int size)
{
  elm_image_prescale_set (o, size);
}

void ElmImage::setOrient (Elm_Image_Orient orient)
{
  elm_image_orient_set (o, orient);
}

} // end namespace efl

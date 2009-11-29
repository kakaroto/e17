#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Image.h"

using namespace std;

namespace Elmxx {

Image::Image (Evasxx::Object &parent)
{
  o = elm_image_add (parent.obj ());
  
  elmInit ();
}

Image::~Image () {}

Image *Image::factory (Evasxx::Object &parent)
{
  return new Image (parent);
}

bool Image::setFile (const std::string &file, const std::string &group)
{
  return elm_image_file_set (o, file.c_str (), group.c_str ());
}

void Image::setSmooth (bool smooth)
{
  elm_image_smooth_set (o, smooth);
}

void Image::setNoScale (bool noScale)
{
  elm_image_no_scale_set (o, noScale);
}

void Image::setScale (bool scaleUp, bool scaleDown)
{
  elm_image_scale_set (o, scaleUp, scaleDown);
}

void Image::setFillOutside (bool fillOutside)
{
  elm_image_fill_outside_set (o, fillOutside);
}

void Image::setPrescale (int size)
{
  elm_image_prescale_set (o, size);
}

void Image::setOrient (Elm_Image_Orient orient)
{
  elm_image_orient_set (o, orient);
}

} // end namespace Elmxx

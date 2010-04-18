/* Project */
#include "../include/edjexx/ImageStateEdit.h"

namespace Edjexx {

ImageStateEdit::ImageStateEdit (Edit &edit, const std::string &part, const std::string &state, double value) :
  StateEdit (edit, part, state, value)
{
}
  
std::string ImageStateEdit::getImage ()
{
  const char *cimage = edje_edit_state_image_get (mEdit->obj (), mPart.c_str (), mState.c_str (), mValue);  
  string image (cimage);
  edje_edit_string_free(cimage);
  return image;
}

void ImageStateEdit::setImage (const std::string &image)
{
  edje_edit_state_image_set (mEdit->obj (), mPart.c_str (), mState.c_str (), mValue, image.c_str ());
}

} // end namespace Edjexx

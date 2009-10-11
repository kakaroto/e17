#ifndef ELM_IMAGE_H
#define ELM_IMAGE_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*!
 * smart callbacks called:
 * "clicked" - the user clicked the image
 */
class ElmImage : public ElmObject
{
public:
  static ElmImage *factory (EvasObject &parent);

  bool setFile (const std::string &file, const std::string &group);
  
  void setSmooth (bool smooth);
  
  void setNoScale (bool noScale);
  
  void setScale (bool scaleUp, bool scaleDown);
  
  void setFillOutside (bool fillOutside);
  
  void setPrescale (int size);
  
  void setOrient (Elm_Image_Orient orient);

private:
  ElmImage (); // forbid standard constructor
  ElmImage (const ElmImage&); // forbid copy constructor
  ElmImage (EvasObject &parent); // private construction -> use factory ()
  ~ElmImage (); // forbid direct delete -> use ElmObject::destroy()
};

} // end namespace efl

#endif // ELM_IMAGE_H

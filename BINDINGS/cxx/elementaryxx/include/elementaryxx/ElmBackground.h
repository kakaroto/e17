#ifndef ELM_BACKGROUND_H
#define ELM_BACKGROUND_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

class ElmBackground : public ElmWidget
{
public:
  virtual ~ElmBackground ();

  static ElmBackground *factory (EvasObject &parent);
  
  void setFile (const std::string &file);
  
  /*!
   * FIXME: do bool return here? 
   * @see ElmImage::setFile
   */
  void setFile (const std::string &file, const std::string &group);
  
private:
  ElmBackground (); // forbid standard constructor
  ElmBackground (const ElmBackground&); // forbid copy constructor
  ElmBackground (EvasObject &parent); // private construction -> use factory ()
};

} // end namespace efl

#endif // ELM_BACKGROUND_H

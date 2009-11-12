#ifndef ELM_FRAME_H
#define ELM_FRAME_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

 /*! 
  * available styles:
  * default
  * pad_small
  * pad_medium
  * pad_large
  * pad_huge
  * outdent_top
  * outdent_bottom
  */
class ElmFrame : public ElmObject
{
public:
  static ElmFrame *factory (EvasObject &parent);

  void setLabel (const std::string &label);

  void setContent (const EvasObject &content);
  
private:
  ElmFrame (); // forbid standard constructor
  ElmFrame (const ElmFrame&); // forbid copy constructor
  ElmFrame (EvasObject &parent); // private construction -> use factory ()
  ~ElmFrame (); // forbid direct delete -> use ElmObject::destroy()
};

} // end namespace efl

#endif // ELM_FRAME_H

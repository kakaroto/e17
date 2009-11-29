#ifndef ELMXX_FRAME_H
#define ELMXX_FRAME_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

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
class Frame : public Object
{
public:
  static Frame *factory (Evasxx::Object &parent);

  void setLabel (const std::string &label);

  void setContent (const Evasxx::Object &content);
  
private:
  Frame (); // forbid standard constructor
  Frame (const Frame&); // forbid copy constructor
  Frame (Evasxx::Object &parent); // private construction -> use factory ()
  ~Frame (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_FRAME_H

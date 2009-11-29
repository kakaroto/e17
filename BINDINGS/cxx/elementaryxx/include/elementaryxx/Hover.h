#ifndef ELMXX_HOVER_H
#define ELMXX_HOVER_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*! smart callbacks called:
 * "clicked" - the user clicked the empty space in the hover to dismiss
 *
 * available styles: 
 * default
 * popout
 * hoversel_vertical
 */
class Hover : public Object
{
public:
  static Hover *factory (Evasxx::Object &parent);

  void setTarget (const Evasxx::Object &target);

  void setParent (const Evasxx::Object &parent);

  void setContent (const std::string &swallow, const Evasxx::Object &content);

  const string getBestContentLocation (Elm_Hover_Axis prefAxis) const;

private:
  Hover (); // forbid standard constructor
  Hover (const Hover&); // forbid copy constructor
  Hover (Evasxx::Object &parent); // private construction -> use factory ()
  ~Hover (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_HOVER_H

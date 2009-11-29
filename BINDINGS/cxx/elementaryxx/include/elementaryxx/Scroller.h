#ifndef ELMXX_SCROLLER_H
#define ELMXX_SCROLLER_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 * "edge_left"
 * "edge_right"
 * "edge_top"
 * "edge_bottom"
 * "scroll"
 */
class Scroller : public Object
{
public:
  static Scroller *factory (Evasxx::Object &parent);

  void setContent (Evasxx::Object &child);
  
  void limitMinContent (bool width, bool height);
  
  void showRegion (const Eflxx::Rect &rect);
  
  void setPolicy (Elm_Scroller_Policy policyH, Elm_Scroller_Policy policyV); // TODO: is H=height and V=vertical?
  
  const Eflxx::Rect getRegion () const;
  
  const Eflxx::Size getChildSize () const;
  
  void setBounce (bool hBounce, bool vBounce); // TODO: is H=height and V=vertical?
  
private:
  Scroller (); // forbid standard constructor
  Scroller (const Scroller&); // forbid copy constructor
  Scroller (Evasxx::Object &parent); // private construction -> use factory ()
  ~Scroller (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_SCROLLER_H

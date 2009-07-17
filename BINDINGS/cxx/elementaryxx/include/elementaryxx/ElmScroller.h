#ifndef ELM_SCROLLER_H
#define ELM_SCROLLER_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

/*!
 * smart callbacks called:
 * "edge_left"
 * "edge_right"
 * "edge_top"
 * "edge_bottom"
 * "scroll"
 */
class ElmScroller : public ElmWidget
{
public:
  static ElmScroller *factory (EvasObject &parent);

  void setContent (EvasObject &child);
  
  void limitMinContent (bool width, bool height);
  
  void showRegion (const Rect &rect);
  
  void setPolicy (Elm_Scroller_Policy policyH, Elm_Scroller_Policy policyV); // TODO: is H=height and V=vertical?
  
  const Rect getRegion () const;
  
  const Size getChildSize () const;
  
  void setBounce (bool hBounce, bool vBounce); // TODO: is H=height and V=vertical?
  
private:
  ElmScroller (); // forbid standard constructor
  ElmScroller (const ElmScroller&); // forbid copy constructor
  ElmScroller (EvasObject &parent); // private construction -> use factory ()
  ~ElmScroller (); // forbid direct delete -> use ElmWidget::destroy()
};

} // end namespace efl

#endif // ELM_SCROLLER_H

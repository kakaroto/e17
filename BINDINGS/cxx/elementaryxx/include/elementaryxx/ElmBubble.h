#ifndef ELM_BUBBLE_H
#define ELM_BUBBLE_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

class ElmBubble : public ElmWidget
{
public:
  static ElmBubble *factory (EvasObject &parent);

  void setLabel (const std::string &label);
  
  void setInfo (const std::string &info);
  
  void setContent (const EvasObject &content);
  
  void setIcon (const EvasObject &icon);
  
  void setCorner (const std::string &corner);
  
private:
  ElmBubble (); // forbid standard constructor
  ElmBubble (const ElmBubble&); // forbid copy constructor
  ElmBubble (EvasObject &parent); // private construction -> use factory ()
  ~ElmBubble (); // forbid direct delete -> use ElmWidget::destroy()
};

} // end namespace efl

#endif // ELM_BUBBLE_H

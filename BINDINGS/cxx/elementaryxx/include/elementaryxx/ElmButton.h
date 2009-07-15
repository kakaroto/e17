#ifndef ELM_BUTTON_H
#define ELM_BUTTON_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

/*!
 * smart callbacks called:
 * "clicked" - the user clicked the button
 */
class ElmButton : public ElmWidget
{
public:
  virtual ~ElmButton ();
  
  static ElmButton *factory (EvasObject &parent);

  void setLabel (const std::string &label);
  
  void setIcon (const EvasObject &icon);
  
  /*! 
   * available styles: 
   * default 
   * hoversel_vertical 
   * hoversel_vertical_entry
   */
  void setStyle (const std::string &style);
  
private:
  ElmButton (); // forbid standard constructor
  ElmButton (const ElmButton&); // forbid copy constructor
  ElmButton (EvasObject &parent); // private construction -> use factory ()
};

} // end namespace efl

#endif // ELM_BUTTON_H

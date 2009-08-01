#ifndef ELM_HOVER_H
#define ELM_HOVER_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

/*! smart callbacks called:
 * "clicked" - the user clicked the empty space in the hover to dismiss
 */
class ElmHover : public ElmWidget
{
public:
  static ElmHover *factory (EvasObject &parent);

  void setTarget (const EvasObject &target);

  void setParent (const EvasObject &parent);

  void setContent (const std::string &swallow, const EvasObject &content);

  /*! 
   * available styles: 
   * default
   * popout
   * hoversel_vertical
   */
  void setStyle (const std::string &style);

  const string getBestContentLocation (Elm_Hover_Axis prefAxis) const;

private:
  ElmHover (); // forbid standard constructor
  ElmHover (const ElmHover&); // forbid copy constructor
  ElmHover (EvasObject &parent); // private construction -> use factory ()
  ~ElmHover (); // forbid direct delete -> use ElmWidget::destroy()
};

} // end namespace efl

#endif // ELM_HOVER_H

#ifndef ELM_ICON_H
#define ELM_ICON_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

/*!
 * smart callbacks called:
 * "clicked" - the user clicked the icon
 */
class ElmIcon : public ElmWidget
{
public:
  virtual ~ElmIcon ();
  
  static ElmIcon *factory (EvasObject &parent);

  bool setFile (const std::string &file);
  
  bool setFile (const std::string &file, const std::string &group);
  
  void setStandard (const std::string &name);
  
  void setSmooth (bool smooth);

  void setNoScale (bool noScale);

  void setScale (bool scaleUp, bool scaleDown);

  void setFillOutside (bool fillOutside);

  void setPrescale (int size);

private:
  ElmIcon (); // forbid standard constructor
  ElmIcon (const ElmIcon&); // forbid copy constructor
  ElmIcon (EvasObject &parent); // private construction -> use factory ()
};

} // end namespace efl

#endif // ELM_ICON_H

#ifndef ELM_ICON_H
#define ELM_ICON_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*!
 * smart callbacks called:
 * "clicked" - the user clicked the icon
 */
class ElmIcon : public ElmObject
{
public:
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
  ~ElmIcon (); // forbid direct delete -> use ElmObject::destroy()
};

} // end namespace efl

#endif // ELM_ICON_H

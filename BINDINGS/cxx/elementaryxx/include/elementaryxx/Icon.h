#ifndef ELMXX_ICON_H
#define ELMXX_ICON_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 * "clicked" - the user clicked the icon
 */
class Icon : public Object
{
public:
  static Icon *factory (Evasxx::Object &parent);

  bool setFile (const std::string &file);
  
  bool setFile (const std::string &file, const std::string &group);
  
  void setStandard (const std::string &name);
  
  void setSmooth (bool smooth);

  void setNoScale (bool noScale);

  void setScale (bool scaleUp, bool scaleDown);

  void setFillOutside (bool fillOutside);

  void setPrescale (int size);

private:
  Icon (); // forbid standard constructor
  Icon (const Icon&); // forbid copy constructor
  Icon (Evasxx::Object &parent); // private construction -> use factory ()
  ~Icon (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_ICON_H

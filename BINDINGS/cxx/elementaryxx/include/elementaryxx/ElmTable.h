#ifndef ELM_TABLE_H
#define ELM_TABLE_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

class ElmTable : public ElmWidget
{
public:
  virtual ~ElmTable ();

  static ElmTable *factory (EvasObject &parent);
  
  void setHomogenous (bool homogenous);

  void pack (const EvasObject &subobj, const Rect &rect);

private:
  ElmTable (); // forbid standard constructor
  ElmTable (const ElmTable&); // forbid copy constructor
  ElmTable (EvasObject &parent); // private construction -> use factory ()
};

} // end namespace efl

#endif // ELM_TABLE_H

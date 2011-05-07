#ifndef ELMXX_TABLE_H
#define ELMXX_TABLE_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Table : public Object
{
public:
  static Table *factory (Evasxx::Object &parent);
  
  void setHomogeneous (bool homogeneous);

  void setHomogenous (bool homogenous);

  void pack (const Evasxx::Object &subobj, const Eflxx::Rect &rect);

private:
  Table (); // forbid standard constructor
  Table (const Table&); // forbid copy constructor
  Table (Evasxx::Object &parent); // private construction -> use factory ()
  ~Table (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_TABLE_H

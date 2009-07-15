#ifndef ELM_WIDGET_H
#define ELM_WIDGET_H

/* STL */
#include <string>

/* EFL */

/* ELFxx */
#include <evasxx/EvasSmart.h>

namespace efl {

class ElmWidget : public EvasSmart
{
public:
  virtual ~ElmWidget ();

protected:
  ElmWidget (); // allow only construction for child classes
  
  void elmInit ();
  
private:
  ElmWidget (const ElmWidget&); // forbid copy constructor
  
  void freeSignalHandler ();
};

} // end namespace efl

#endif // ELM_WIDGET_H

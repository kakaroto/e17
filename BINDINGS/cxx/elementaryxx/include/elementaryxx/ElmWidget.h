#ifndef ELM_WIDGET_H
#define ELM_WIDGET_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include <evasxx/EvasSmart.h>

namespace efl {

class ElmWidget : public EvasSmart
{
public:

protected:
  ElmWidget (); // allow only construction for child classes
  virtual ~ElmWidget (); // forbid direct delete -> use destroy()
  
  void elmInit ();

  void destroy ();
  
private:
  ElmWidget (const ElmWidget&); // forbid copy constructor

  
  void freeSignalHandler ();
};

} // end namespace efl

#endif // ELM_WIDGET_H

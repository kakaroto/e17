#ifndef ELMXX_GEN_H
#define ELMXX_GEN_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Gen : public Object
{
public:  
  virtual void clear ();

  virtual void setAlwaysSelectMode (bool alwaysSelect);
  
  virtual bool getAlwaysSelectMode ();
  
  virtual void setNoSelectMode (bool noSelect);
  
  virtual bool getNoSelectMode ();

  virtual void setBounce (bool hBounce, bool vBounce);
  
  virtual void getBounce (bool &hBounceOut, bool &vBounceOut);

protected:
  Gen (); // allow only construction for child classes
  ~Gen (); // forbid direct delete -> use Object::destroy()

private:
  Gen (const Gen&); // forbid copy constructor

};

} // end namespace Elmxx

#endif // ELMXX_GEN_H

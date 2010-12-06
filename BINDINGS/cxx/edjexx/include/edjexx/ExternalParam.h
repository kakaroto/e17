#ifndef EDJEXX_EXTERNAL_PARAM_H
#define EDJEXX_EXTERNAL_PARAM_H

/* STD */
#include <string>

/* EFL */
#include <Edje.h>

/* local */
#include "Part.h"

namespace Edjexx {
  
class ExternalParam
{
  friend class Part;
  
public:    
  ExternalParam (const std::string &name, int i);
  ExternalParam (const std::string &name, bool b);
  ExternalParam (const std::string &name, double d);
  ExternalParam (const std::string &name, const char *c);
  ExternalParam (const std::string &name, const std::string &s);

protected:
  Edje_External_Param *getRaw ();

private:  
  Edje_External_Param mParam;
  std::string mName;
  std::string mStr;
};

} // end namespace Edjexx

#endif // EDJEXX_EXTERNAL_PARAM_H

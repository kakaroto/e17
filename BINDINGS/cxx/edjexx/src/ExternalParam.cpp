#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/edjexx/ExternalParam.h"

namespace Edjexx {
  
ExternalParam::ExternalParam (const std::string &name, int i) :
  mName (name)
{
  mParam.name = mName.c_str ();
  mParam.d = 0.0;
  mParam.s = NULL;
  mParam.i = i;
  mParam.type = EDJE_EXTERNAL_PARAM_TYPE_INT;
}

ExternalParam::ExternalParam (const std::string &name, bool b) :
  mName (name)
{
  mParam.name = mName.c_str ();
  mParam.d = 0.0;
  mParam.s = NULL;
  mParam.i = b;
  mParam.type = EDJE_EXTERNAL_PARAM_TYPE_BOOL;
}

ExternalParam::ExternalParam (const std::string &name, double d) :
  mName (name)
{
  mParam.name = mName.c_str ();
  mParam.d = d;
  mParam.s = NULL;
  mParam.i = 0;
  mParam.type = EDJE_EXTERNAL_PARAM_TYPE_DOUBLE;
}

ExternalParam::ExternalParam (const std::string &name, const char *c) :
  mName (name), mStr (c)
{
  mParam.name = mName.c_str ();
  mParam.d = 0.0;
  mParam.s = mStr.c_str ();
  mParam.i = 0;
  mParam.type = EDJE_EXTERNAL_PARAM_TYPE_STRING;
}

ExternalParam::ExternalParam (const std::string &name, const std::string &s) :
  mName (name), mStr (s)
{
  mParam.name = mName.c_str ();
  mParam.d = 0.0;
  mParam.s = mStr.c_str ();
  mParam.i = 0;
  mParam.type = EDJE_EXTERNAL_PARAM_TYPE_STRING;
}

Edje_External_Param *ExternalParam::getRaw ()
{
  return &mParam;
}

} // end namespace Edjexx

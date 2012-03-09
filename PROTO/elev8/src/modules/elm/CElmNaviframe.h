#ifndef C_ELM_NAVIFRAME_H
#define C_ELM_NAVIFRAME_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmNaviframe : public CEvasObject {
   FACTORY(CElmNaviframe)

protected:
   CPropHandler<CElmNaviframe> prop_handler;

public:
   CElmNaviframe(CEvasObject *parent, Local<Object> obj);

   static Handle<Value> pop(const Arguments& args);

   static Handle<Value> push(const Arguments& args);
};

#endif

#ifndef C_ELM_NAVIFRAME_H
#define C_ELM_NAVIFRAME_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmNaviframe : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;
   Persistent<Array> stack;

protected:
   CElmNaviframe(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmNaviframe();

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> pop(const Arguments& args);
   Handle<Value> push(const Arguments& args);
   Handle<Value> promote(const Arguments& args);

   friend Handle<Value> CElmObject::New<CElmNaviframe>(const Arguments &args);
};

}

#endif

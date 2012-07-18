#ifndef C_ELM_CONFORM_H
#define C_ELM_CONFORM_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

using namespace v8;

class CElmConform : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmConform(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();
   virtual ~CElmConform();

   struct {
      Persistent<Value> content;
   } cached;

public:
   static void Initialize (Handle<Object> target);

   void content_set(Handle<Value> val);
   Handle<Value> content_get() const;

   friend Handle<Value> CElmObject::New<CElmConform>(const Arguments& args);
};

}

#endif

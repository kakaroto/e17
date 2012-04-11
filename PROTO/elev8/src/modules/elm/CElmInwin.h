#ifndef C_ELM_INWIN_H
#define C_ELM_INWIN_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmInwin : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmInwin(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();
   virtual ~CElmInwin();

   struct {
      Persistent<Value> content;
   } cached;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> activate(const Arguments&);

   virtual Handle<Value> content_get() const;
   virtual void content_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmInwin>(const Arguments& args);
};

}

#endif

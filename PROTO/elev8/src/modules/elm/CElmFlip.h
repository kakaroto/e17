#ifndef C_ELM_FLIP_H
#define C_ELM_FLIP_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmFlip : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmFlip(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

   struct {
      Persistent<Value> front;
      Persistent<Value> back;
   } cached;

public:
   static void Initialize(Handle<Object> target);

   static Handle<Value> do_flip(const Arguments& args);
   Handle<Value> flip(const Arguments& args);

   Handle<Value> front_get() const;
   void front_set(Handle<Value> obj);

   Handle<Value> back_get() const;
   void back_set(Handle<Value> obj);

   friend Handle<Value> CElmObject::New<CElmFlip>(const Arguments& args);
};

}

#endif // C_ELM_FLIP_H

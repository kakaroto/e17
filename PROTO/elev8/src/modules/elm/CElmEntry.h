#ifndef C_ELM_ENTRY_H
#define C_ELM_ENTRY_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmEntry : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmEntry(Local<Object> _jsObject, CElmObject *parent);

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> val);

   Handle<Value> password_get() const;
   void password_set(Handle<Value> value);

   Handle<Value> editable_get() const;
   void editable_set(Handle<Value> value);

   Handle<Value> line_wrap_get() const;
   void line_wrap_set(Handle<Value> value);

   Handle<Value> scrollable_get() const;
   void scrollable_set(Handle<Value> value);

   Handle<Value> single_line_get() const;
   void single_line_set(Handle<Value> value);

   friend Handle<Value> CElmObject::New<CElmEntry>(const Arguments& args);
};

}

#endif

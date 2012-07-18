#ifndef C_ELM_GRID_H
#define C_ELM_GRID_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmGrid : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmGrid(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   virtual Handle<Value> Pack(Handle<Value>, Handle<Value>);
   virtual Handle<Value> Unpack(Handle<Value>);

   void size_set(Handle<Value> val);
   Handle<Value> size_get() const;

   friend Handle<Value> CElmObject::New<CElmGrid>(const Arguments& args);
};

}

#endif

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
   virtual void DidRealiseElement(Local<Value>);

   virtual Handle<Value> Pack(Handle<Value>);
   virtual Handle<Value> Unpack(Handle<Value>);

   static Handle<Value> add(const Arguments& args);
   static Handle<Value> clear(const Arguments& args);

   void pack(Handle<Object> obj);
   Handle<Value> pack(const Arguments&);

   void size_set(Handle<Value> val);
   Handle<Value> size_get() const;

   friend Handle<Value> CElmObject::New<CElmGrid>(const Arguments& args);
};

}

#endif

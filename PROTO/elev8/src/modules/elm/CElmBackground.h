#ifndef C_ELM_BACKGROUND_H
#define C_ELM_BACKGROUND_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmBackground : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmBackground(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

   static Handle<Value> New(const Arguments& args);
   static void Delete(Persistent<Value>, void *parameter);

public:
   static void Initialize(Handle<Object> target);

   void Setimage(Handle<Value> val);
   Handle<Value> Getimage(void) const;

   Handle<Value> Getred() const;
   void Setred(Handle<Value> val);

   Handle<Value> Getgreen() const;
   void Setgreen(Handle<Value> val);

   Handle<Value> Getblue() const;
   void Setblue(Handle<Value> val);
};

}

#endif // C_ELM_BACKGROUND_H

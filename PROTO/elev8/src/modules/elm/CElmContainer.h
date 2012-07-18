#ifndef C_ELM_CONTAINER_H
#define C_ELM_CONTAINER_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmContainer : public CElmObject {
private:
   CElmObject *parent;
   static Persistent<FunctionTemplate> tmpl;

   Persistent<Object> contents;

   void init();
protected:
   CElmContainer(Local<Object> _jsObject, CElmObject*);
   CElmContainer(Local<Object> _jsObject, Evas_Object *child);
   ~CElmContainer();

   static Handle<FunctionTemplate> GetTemplate();
public:
   static void Initialize(Handle<Object> target);

   Handle<Value> content_get(const Arguments &args);
   Handle<Value> content_set(const Arguments &args);

   friend Handle<Value> CElmObject::New<CElmContainer>(const Arguments& args);
};

}

#endif // C_ELM_CONTAINER_H

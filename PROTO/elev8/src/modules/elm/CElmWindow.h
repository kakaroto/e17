#ifndef _CELM_WINDOW_H
#define _CELM_WINDOW_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmWindow : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmWindow(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();
   static void quit(void *, Evas_Object *, void *);

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> title_get() const;
   void title_set(Handle<Value> val);

   Handle<Value> conformant_get() const;
   void conformant_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmWindow>(const Arguments& args);
};

}

#endif

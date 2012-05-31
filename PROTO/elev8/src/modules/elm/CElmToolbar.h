#ifndef C_ELM_TOOLBAR_H
#define C_ELM_TOOLBAR_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmToolbar : public CElmObject {
private:
   class Item {
   public:
      Persistent<Value> data;
      Persistent<Value> callback;

      Item(Handle<Value> _data, Handle<Value> _callback)
         : data(Persistent<Value>::New(_data))
         , callback(Persistent<Value>::New(_callback)) {}
      ~Item()
        {
           data.Dispose();
           callback.Dispose();
        }
   };

   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmToolbar(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);
   Handle<Value> append(const Arguments& args);
   static void OnSelect(void *data, Evas_Object *, void *);
   friend Handle<Value> CElmObject::New<CElmToolbar>(const Arguments& args);
};

}

#endif

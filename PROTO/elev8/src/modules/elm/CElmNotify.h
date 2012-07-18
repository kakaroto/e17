#ifndef C_ELM_NOTIFY_H
#define C_ELM_NOTIFY_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmNotify : public CElmObject {
private:
  static Persistent<FunctionTemplate> tmpl;

protected:
   CElmNotify(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmNotify();

   struct {
      Persistent<Value> content;
   } cached;

   static Handle<FunctionTemplate> GetTemplate();

   Persistent<Value> notify_parent;
public:
   static void  Initialize(Handle<Object> target);

   Handle<Value> content_get() const;
   void content_set(Handle<Value> val);

   Handle<Value> orient_get() const;
   void orient_set(Handle<Value> val);

   Handle<Value> timeout_get() const;
   void timeout_set(Handle<Value> val);

   Handle<Value> allow_events_get() const;
   void allow_events_set(Handle<Value> val);

   Handle<Value> parent_get() const;
   void parent_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmNotify>(const Arguments& args);
};

}

#endif

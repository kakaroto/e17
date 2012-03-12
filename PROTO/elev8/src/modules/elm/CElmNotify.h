#ifndef C_ELM_NOTIFY_H
#define C_ELM_NOTIFY_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmNotify : public CEvasObject {
   FACTORY(CElmNotify)

protected:
   CPropHandler<CElmNotify> prop_handler;
   CEvasObject *content;

public:
   CElmNotify(CEvasObject *parent, Local<Object> obj);

   virtual Handle<Value> content_get() const;

   virtual void content_set(Handle<Value> val);

   virtual Handle<Value> orient_get() const;

   virtual void orient_set(Handle<Value> val);

   virtual Handle<Value> timeout_get() const;

   virtual void timeout_set(Handle<Value> val);

   virtual Handle<Value> allow_events_get() const;

   virtual void allow_events_set(Handle<Value> val);
};

#endif

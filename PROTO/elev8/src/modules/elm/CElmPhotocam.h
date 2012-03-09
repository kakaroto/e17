#ifndef C_ELM_PHOTOCAM_H
#define C_ELM_PHOTOCAM_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmPhotocam : public CEvasObject {
   FACTORY(CElmPhotocam)

protected:
   CPropHandler<CElmPhotocam> prop_handler;

public:
   CElmPhotocam(CEvasObject *parent, Local<Object> obj);

   virtual void file_set(Handle<Value> val);

   virtual Handle<Value> file_get(void) const;

   virtual Handle<Value> zoom_get() const;

   virtual void zoom_set(Handle<Value> value);

   virtual Handle<Value> zoom_mode_get() const;

   virtual void zoom_mode_set(Handle<Value> value);

   virtual void bounce_set(Handle<Value> val);

   virtual Handle<Value> bounce_get() const;

   void paused_set(Handle<Value> val);

   virtual Handle<Value> paused_get() const;
};

#endif

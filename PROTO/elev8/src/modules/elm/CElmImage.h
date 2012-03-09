#ifndef C_ELM_IMAGE_H
#define C_ELM_IMAGE_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmImage : public CEvasObject {
   FACTORY(CElmImage)

protected:
   CPropHandler<CElmImage> prop_handler;

public:
   CElmImage(CEvasObject *parent, Local<Object> obj);

   virtual void resize_set(Handle<Value> val);

   virtual void file_set(Handle<Value> val);

   virtual Handle<Value> file_get(void) const;

   virtual Handle<Value> smooth_get() const;

   virtual void smooth_set(Handle<Value> val);

   virtual Handle<Value> no_scale_get() const;

   virtual void no_scale_set(Handle<Value> val);

   virtual Handle<Value> fill_outside_get() const;

   virtual void fill_outside_set(Handle<Value> val);

   virtual Handle<Value> editable_get() const;

   virtual void editable_set(Handle<Value> val);

   virtual Handle<Value> aspect_fixed_get() const;

   virtual void aspect_fixed_set(Handle<Value> val);

   virtual Handle<Value> prescale_get() const;

   virtual void prescale_set(Handle<Value> val);

   virtual Handle<Value> orient_get() const;

   virtual void orient_set(Handle<Value> val);
};

#endif

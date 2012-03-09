#ifndef C_ELM_SPINNER_H
#define C_ELM_SPINNER_H

#include <v8.h>
#include "CEvasObject.h"

class CElmSpinner : public CEvasObject {
   FACTORY(CElmSpinner)
protected:
   CPropHandler<CElmSpinner> prop_handler;

public:
   CElmSpinner(CEvasObject *parent, Local<Object> obj);

   virtual Handle<Value> label_format_get() const;
   virtual void label_format_set(Handle<Value> val);
   virtual Handle<Value> step_get() const;
   virtual void step_set(Handle<Value> val);
   virtual Handle<Value> min_get() const;
   virtual void min_set(Handle<Value> value);
   virtual Handle<Value> max_get() const;
   virtual void max_set(Handle<Value> value);
   virtual Handle<Value> style_get() const;
   virtual void style_set(Handle<Value> val);
   virtual Handle<Value> editable_get() const;
   virtual void editable_set(Handle<Value> val);
   virtual Handle<Value> disabled_get() const;
   virtual void disabled_set(Handle<Value> val);
   virtual Handle<Value> special_value_get() const;
   virtual void special_value_set(Handle<Value> val);
};

#endif // C_ELM_SPINNER_H

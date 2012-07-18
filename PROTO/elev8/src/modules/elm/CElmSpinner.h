#ifndef C_ELM_SPINNER_H
#define C_ELM_SPINNER_H

#include "elm.h"
#include "CElmLayout.h"
#include "CElmObject.h"

namespace elm {

class CElmSpinner : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmSpinner(Local<Object> _jsObject, CElmObject *parent);

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> val);

   Handle<Value> label_format_get() const;
   void label_format_set(Handle<Value> val);

   Handle<Value> step_get() const;
   void step_set(Handle<Value> val);

   Handle<Value> min_get() const;
   void min_set(Handle<Value> value);

   Handle<Value> max_get() const;
   void max_set(Handle<Value> value);

   Handle<Value> editable_get() const;
   void editable_set(Handle<Value> val);

   Handle<Value> disabled_get() const;
   void disabled_set(Handle<Value> val);

   Handle<Value> special_value_get() const;
   void special_value_set(Handle<Value> val);

   Handle<Value> wrap_get() const;
   void wrap_set(Handle<Value> val);

   Handle<Value> round_get() const;
   void round_set(Handle<Value> val);

   Handle<Value> base_get() const;
   void base_set(Handle<Value> val);

   Handle<Value> interval_get() const;
   void interval_set(Handle<Value> val);

   Handle<Value> value_get() const;
   void value_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmSpinner>(const Arguments& args);
};

}
#endif

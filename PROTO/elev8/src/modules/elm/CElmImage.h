#ifndef C_ELM_IMAGE_H
#define C_ELM_IMAGE_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmImage : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmImage(Local<Object> _jsObject, CElmObject *parent);

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> file_get(void) const;
   void file_set(Handle<Value> val);

   Handle<Value> smooth_get() const;
   void smooth_set(Handle<Value> val);

   Handle<Value> no_scale_get() const;
   void no_scale_set(Handle<Value> val);

   Handle<Value> fill_outside_get() const;
   void fill_outside_set(Handle<Value> val);

   Handle<Value> editable_get() const;
   void editable_set(Handle<Value> val);

   Handle<Value> aspect_fixed_get() const;
   void aspect_fixed_set(Handle<Value> val);

   Handle<Value> prescale_get() const;
   void prescale_set(Handle<Value> val);

   Handle<Value> orient_get() const;
   void orient_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmImage>(const Arguments& args);
};

}
#endif

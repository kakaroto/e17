#ifndef C_ELM_PROGRESS_BAR_H
#define C_ELM_PROGRESS_BAR_H

#include "elm.h"
#include "CElmLayout.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmProgressBar : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmProgressBar(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();
   virtual ~CElmProgressBar();

   struct {
      Persistent<Value> icon;
   } cached;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> pulse(const Arguments& args);

   Handle<Value> icon_get() const;
   void icon_set(Handle<Value> value);

   Handle<Value> inverted_get() const;
   void inverted_set(Handle<Value> value);

   Handle<Value> horizontal_get() const;
   void horizontal_set(Handle<Value> value);

   Handle<Value> units_get() const;
   void units_set(Handle<Value> value);

   Handle<Value> span_get() const;
   void span_set(Handle<Value> value);

   Handle<Value> pulser_get() const;
   void pulser_set(Handle<Value> value);

   Handle<Value> value_get() const;
   void value_set(Handle<Value> value);

   friend Handle<Value> CElmObject::New<CElmProgressBar>(const Arguments& args);
};

}

#endif

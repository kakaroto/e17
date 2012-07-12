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

   Handle<Value> autodel_get() const;
   void autodel_set(Handle<Value> val);

   Handle<Value> borderless_get() const;
   void borderless_set(Handle<Value> val);

   Handle<Value> shaped_get() const;
   void shaped_set(Handle<Value> val);

   Handle<Value> alpha_get() const;
   void alpha_set(Handle<Value> val);

   Handle<Value> override_get() const;
   void override_set(Handle<Value> val);

   Handle<Value> fullscreen_get() const;
   void fullscreen_set(Handle<Value> val);

   Handle<Value> maximized_get() const;
   void maximized_set(Handle<Value> val);

   Handle<Value> iconified_get() const;
   void iconified_set(Handle<Value> val);

   Handle<Value> withdrawn_get() const;
   void withdrawn_set(Handle<Value> val);

   Handle<Value> urgent_get() const;
   void urgent_set(Handle<Value> val);

   Handle<Value> demand_attention_get() const;
   void demand_attention_set(Handle<Value> val);

   Handle<Value> modal_get() const;
   void modal_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmWindow>(const Arguments& args);
};

}

#endif

#ifndef C_ELM_COLOR_SELECTOR_H
#define C_ELM_COLOR_SELECTOR_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

class CElmColorSelector : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmColorSelector(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmColorSelector();

   struct {
      Persistent<Value> change;
   } cb;

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   void OnChange(void *);
   static void OnChangeWrapper(void *, Evas_Object *, void *);

   void on_change_set(Handle<Value> val);
   Handle<Value> on_change_get(void) const;

   Handle<Value> red_get() const;
   void red_set(Handle<Value> val);

   Handle<Value> green_get() const;
   void green_set(Handle<Value> val);

   Handle<Value> blue_get() const;
   void blue_set(Handle<Value> val);

   Handle<Value> alpha_get() const;
   void alpha_set(Handle<Value> val);

   Handle<Value> palette_name_get() const;
   void palette_name_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmColorSelector>(const Arguments& args);
};

}
#endif

#ifndef C_ELM_SLIDER_H
#define C_ELM_SLIDER_H

#include "elm.h"
#include "CElmLayout.h"
#include "CElmObject.h"

namespace elm {

class CElmSlider : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmSlider(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmSlider();

   struct {
      Persistent<Value> icon;
      Persistent<Value> end;
   } cached;

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

   void OnLangChange(void *);
   static void OnLangChangeWrapper(void *, Evas_Object *, void *);

   void on_lang_change_set(Handle<Value> val);
   Handle<Value> on_lang_change_get(void) const;

   void units_set(Handle<Value> value);
   Handle<Value> units_get() const;

   void indicator_set(Handle<Value> value);
   Handle<Value> indicator_get() const;

   Handle<Value> span_get() const;
   void span_set(Handle<Value> value);

   Handle<Value> icon_get() const;
   void icon_set(Handle<Value> value);

   Handle<Value> end_get() const;
   void end_set(Handle<Value> value);

   Handle<Value> value_get() const;
   void value_set(Handle<Value> value);

   Handle<Value> min_get() const;
   void min_set(Handle<Value> value);

   Handle<Value> max_get() const;
   void max_set(Handle<Value> value);

   Handle<Value> inverted_get() const;
   void inverted_set(Handle<Value> value);

   Handle<Value> horizontal_get() const;
   void horizontal_set(Handle<Value> value);

   Handle<Value> indicator_show_get() const;
   void indicator_show_set(Handle<Value> value);

   friend Handle<Value> CElmObject::New<CElmSlider>(const Arguments& args);
};

}
#endif

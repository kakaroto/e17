#ifndef C_ELM_SLIDER_H
#define C_ELM_SLIDER_H

#include <v8.h>
#include "CEvasObject.h"

class CElmSlider : public CEvasObject {
   FACTORY(CElmSlider)
protected:
   Persistent<Value> the_icon;
   Persistent<Value> the_end_object;
   Persistent<Value> on_changed_val;
   CPropHandler<CElmSlider> prop_handler;

public:
   CElmSlider(CEvasObject *parent, Local<Object> obj);
   virtual ~CElmSlider();
   virtual void units_set(Handle<Value> value);
   virtual Handle<Value> units_get() const;
   virtual void indicator_set(Handle<Value> value);
   virtual Handle<Value> indicator_get() const;
   virtual Handle<Value> span_get() const;
   virtual void span_set(Handle<Value> value);
   virtual Handle<Value> icon_get() const;
   virtual void icon_set(Handle<Value> value);
   virtual Handle<Value> end_get() const;
   virtual void end_set(Handle<Value> value);
   virtual Handle<Value> value_get() const;
   virtual void value_set(Handle<Value> value);
   virtual Handle<Value> min_get() const;
   virtual void min_set(Handle<Value> value);
   virtual Handle<Value> max_get() const;
   virtual void max_set(Handle<Value> value);
   virtual Handle<Value> inverted_get() const;
   virtual void inverted_set(Handle<Value> value);
   virtual Handle<Value> horizontal_get() const;
   virtual void horizontal_set(Handle<Value> value);
   static void eo_on_changed(void *data, Evas_Object *, void *event_info);
   virtual void on_changed(void *);
   virtual void on_changed_set(Handle<Value> val);
   virtual Handle<Value> on_changed_get(void) const;
};

#endif // C_ELM_SLIDER_H

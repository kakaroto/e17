#ifndef C_ELM_TOGGLE_H
#define C_ELM_TOGGLE_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmToggle : public CEvasObject {
   FACTORY(CElmToggle)

protected:
   CPropHandler<CElmToggle> prop_handler;

   /* the on_changed function */
   Persistent<Value> on_changed_val;
   Persistent<Value> the_icon;

public:
   CElmToggle(CEvasObject *parent, Local<Object> obj);

   static void eo_on_changed(void *data, Evas_Object *, void *event_info);

   virtual void on_changed(void *);

   virtual void on_changed_set(Handle<Value> val);

   virtual Handle<Value> on_changed_get(void) const;

   virtual void onlabel_set(Handle<Value> val);

   virtual Handle<Value> onlabel_get(void) const;

   virtual void offlabel_set(Handle<Value> val);

   virtual Handle<Value> offlabel_get(void) const;

   virtual Handle<Value> icon_get() const;

   virtual void icon_set(Handle<Value> value);

   void state_set(Handle<Value> val);

   virtual Handle<Value> state_get() const;
};

#endif

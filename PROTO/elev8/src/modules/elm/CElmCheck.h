#ifndef C_ELM_CHECK_H
#define C_ELM_CHECK_H

#include <v8.h>
#include "CEvasObject.h"

class CElmCheck : public CEvasObject {
   FACTORY(CElmCheck)

protected:
   CPropHandler<CElmCheck> prop_handler;

   Persistent<Value> on_changed_val;
   Persistent<Value> the_icon;

   static void eo_on_changed(void *data, Evas_Object *, void *event_info);
   virtual void on_changed(void *);

public:
   virtual void on_changed_set(Handle<Value> val);
   virtual Handle<Value> on_changed_get(void) const;
   CElmCheck(CEvasObject *parent, Local<Object> obj); 

   virtual ~CElmCheck();

   virtual void state_set(Handle<Value> value);

   virtual Handle<Value> state_get() const;

   virtual Handle<Value> icon_get() const;

   virtual void icon_set(Handle<Value> value);

   virtual void onlabel_set(Handle<Value> val);

   virtual Handle<Value> onlabel_get(void) const;

   virtual void offlabel_set(Handle<Value> val);

   virtual Handle<Value> offlabel_get(void) const;
};

#endif

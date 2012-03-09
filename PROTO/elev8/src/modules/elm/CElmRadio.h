#ifndef C_ELM_RADIO_H
#define C_ELM_RADIO_H

#include <v8.h>
#include "CEvasObject.h"

class CElmRadio : public CEvasObject {
   FACTORY(CElmRadio)
protected:
   CPropHandler<CElmRadio> prop_handler;
   Persistent<Value> the_icon;
   Persistent<Value> the_group;

public:
   CElmRadio(CEvasObject *parent, Local<Object> obj);
   virtual ~CElmRadio();
   virtual Handle<Value> icon_get() const;
   virtual void icon_set(Handle<Value> value);
   virtual Handle<Value> group_get() const;
   virtual void group_set(Handle<Value> value);
   virtual Handle<Value> value_get() const;
   virtual void value_set(Handle<Value> value);
};

#endif // C_ELM_RADIO_H

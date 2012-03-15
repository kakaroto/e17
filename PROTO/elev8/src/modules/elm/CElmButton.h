#ifndef C_ELM_BUTTON_H
#define C_ELM_BUTTON_H

#include <v8.h>
#include "CEvasObject.h"

class CElmButton : public CEvasObject {
   FACTORY(CElmButton)
protected:
   Persistent<Value> the_icon;
   CPropHandler<CElmButton> prop_handler;

   CElmButton(CEvasObject *parent, Local<Object> obj);
public:
   virtual ~CElmButton();

   virtual Handle<Value> icon_get() const;
   virtual void icon_set(Handle<Value> value);
};

#endif // C_ELM_BUTTON_H

#ifndef C_ELM_ICON_H
#define C_ELM_ICON_H

#include <v8.h>
#include "CEvasObject.h"

class CElmIcon : public CEvasObject {
   FACTORY(CElmIcon)
public:
   CPropHandler<CElmIcon> prop_handler;
public:
   CElmIcon(CEvasObject *parent, Local<Object> obj);

   virtual void scale_up_set(Handle<Value> val);
   virtual Handle<Value> scale_up_get() const;
   virtual void scale_down_set(Handle<Value> val);
   virtual Handle<Value> prescale_get() const;
   virtual void prescale_set(Handle<Value> val);
   virtual Handle<Value> scale_down_get() const;
   virtual void image_set(Handle<Value> val);
   virtual Handle<Value> image_get(void) const;
};

#endif // C_ELM_ICON_H

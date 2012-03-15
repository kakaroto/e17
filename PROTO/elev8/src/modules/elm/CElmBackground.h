#ifndef C_ELM_BACKGROUND_H
#define C_ELM_BACKGROUND_H

#include <v8.h>
#include "CEvasObject.h"

class CElmBackground : public CEvasObject {
   FACTORY(CElmBackground)

protected:
   CPropHandler<CElmBackground> prop_handler;

public:
   explicit CElmBackground(CEvasObject *parent, Local<Object> obj);

   virtual void image_set(Handle<Value> val);
   virtual Handle<Value> image_get(void) const;

   virtual Handle<Value> red_get() const;
   virtual void red_set(Handle<Value> val);

   virtual Handle<Value> green_get() const;
   virtual void green_set(Handle<Value> val);

   virtual Handle<Value> blue_get() const;
   virtual void blue_set(Handle<Value> val);
};

#endif // C_ELM_BACKGROUND_H

#ifndef C_ELM_FLIP_H
#define C_ELM_FLIP_H

#include <v8.h>
#include "CEvasObject.h"

class CElmFlip : public CEvasObject {
   FACTORY(CElmFlip)
protected:
   CPropHandler<CElmFlip> prop_handler;

   CElmFlip(CEvasObject *parent, Local<Object> obj);
   static Handle<Value> do_flip(const Arguments& args);
   virtual void flip(Elm_Flip_Mode mode);
public:
   virtual Handle<Value> front_get() const;
   virtual void front_set(Handle<Value> obj);

   virtual Handle<Value> back_get() const;
   virtual void back_set(Handle<Value> obj);
};

#endif // C_ELM_FLIP_H

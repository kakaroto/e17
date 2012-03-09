#ifndef C_ELM_FLIP_H
#define C_ELM_FLIP_H

#include <v8.h>
#include "CEvasObject.h"

class CElmFlip : public CEvasObject {
   FACTORY(CElmFlip)
public:
   static Handle<Value> do_flip(const Arguments& args);
   virtual void flip(Elm_Flip_Mode mode);
   CElmFlip(CEvasObject *parent, Local<Object> obj);
};

#endif // C_ELM_FLIP_H

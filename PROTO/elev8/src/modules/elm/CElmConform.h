#ifndef C_ELM_CONFORM_H
#define C_ELM_CONFORM_H

#include <v8.h>
#include "CEvasObject.h"

class CElmConform : public CEvasObject {
   FACTORY(CElmConform)
protected:
   CPropHandler<CElmConform> prop_handler;

   CElmConform(CEvasObject *parent, Local<Object> obj);
public:
   virtual ~CElmConform();

   virtual void content_set(Handle<Value> val);
   virtual Handle<Value> content_get() const;
};

#endif // C_ELM_CONFORM_H

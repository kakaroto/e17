#ifndef C_ELM_LABEL_H
#define C_ELM_LABEL_H

#include <v8.h>
#include "CEvasObject.h"

class CElmLabel : public CEvasObject {
   FACTORY(CElmLabel)
protected:
   CPropHandler<CElmLabel> prop_handler;

public:
   CElmLabel(CEvasObject *parent, Local<Object> obj);
   virtual void wrap_set(Handle<Value> wrap);
   virtual Handle<Value> wrap_get() const;
};

#endif // C_ELM_LABEL_H

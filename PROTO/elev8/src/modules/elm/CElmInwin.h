#ifndef C_ELM_INWIN_H
#define C_ELM_INWIN_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmInwin : public CEvasObject {
   FACTORY(CElmInwin)

protected:
   CPropHandler<CElmInwin> prop_handler;
   CEvasObject *content;

public:
   CElmInwin(CEvasObject *parent, Local<Object> obj);

   virtual Handle<Value> activate_get() const;

   virtual void activate_set(Handle<Value> val);
};

#endif

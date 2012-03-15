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

   CElmInwin(CEvasObject *parent, Local<Object> obj);

   static Handle<Value> activate(const Arguments&);

public:
   virtual Handle<Value> content_get() const;
   virtual void content_set(Handle<Value> val);
};

#endif

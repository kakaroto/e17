#ifndef C_ELM_PANE_H
#define C_ELM_PANE_H

#include <v8.h>
#include "CEvasObject.h"

class CElmPane : public CEvasObject {
   FACTORY(CElmPane)
protected:
   CPropHandler<CElmPane> prop_handler;

   CElmPane(CEvasObject *parent, Local<Object> obj);
public:
   virtual Handle<Value> horizontal_get() const;
   virtual void horizontal_set(Handle<Value> val);
};

#endif


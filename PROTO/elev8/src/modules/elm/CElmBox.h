#ifndef C_ELM_BOX_H
#define C_ELM_BOX_H

#include <v8.h>
#include "CEvasObject.h"

class CElmBox : public CEvasObject {
   FACTORY(CElmBox)
protected:
   virtual void add_child(CEvasObject *child);
   virtual CEvasObject *get_child(Handle<Value> name);

   CPropHandler<CElmBox> prop_handler;

public:
   CElmBox(CEvasObject *parent, Local<Object> obj);
   void horizontal_set(Handle<Value> val);
   virtual Handle<Value> horizontal_get() const;
   void homogeneous_set(Handle<Value> val);
   virtual Handle<Value> homogeneous_get() const;
};

#endif // C_ELM_BOX_H

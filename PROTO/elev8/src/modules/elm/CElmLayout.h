#ifndef C_ELM_LAYOUT_H
#define C_ELM_LAYOUT_H

#include <v8.h>
#include "CEvasObject.h"

class CElmLayout : public CEvasObject {
   FACTORY(CElmLayout)
protected:
   Persistent<Object> the_contents;
   CPropHandler<CElmLayout> prop_handler;
public:
   CElmLayout(CEvasObject *parent, Local<Object> obj);
   virtual Handle<Value> contents_get() const;
   virtual void contents_set(Handle<Value> val);
   virtual Handle<Value> file_get() const;
   virtual void file_set(Handle<Value> val);
   virtual Handle<Value> theme_get() const;
   virtual void theme_set(Handle<Value> val);
};

#endif // C_ELM_LAYOUT_H

#ifndef C_ELM_ENTRY_H
#define C_ELM_ENTRY_H

#include <v8.h>
#include "CEvasObject.h"

class CElmEntry : public CEvasObject {
   FACTORY(CElmEntry)
protected:
   CPropHandler<CElmEntry> prop_handler;
public:
   CElmEntry(CEvasObject *parent, Local<Object> obj);

   virtual Handle<Value> password_get() const;
   virtual void password_set(Handle<Value> value);
   virtual Handle<Value> editable_get() const;
   virtual void editable_set(Handle<Value> value);
   virtual Handle<Value> line_wrap_get() const;
   virtual void line_wrap_set(Handle<Value> value);
   virtual Handle<Value> scrollable_get() const;
   virtual void scrollable_set(Handle<Value> value);
   virtual Handle<Value> single_line_get() const;
   virtual void single_line_set(Handle<Value> value);
};

#endif // C_ELM_ENTRY_H

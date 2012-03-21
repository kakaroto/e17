#ifndef C_ELM_BASIC_WINDOW_H
#define C_ELM_BASIC_WINDOW_H

#include <v8.h>

#include "CEvasObject.h"

class CElmBasicWindow : public CEvasObject {
protected:
   Persistent<Value> win_name;
   Persistent<Value> win_type;

public:
   CElmBasicWindow(CEvasObject *parent, Local<Object> obj, Local<String> name, Local<Number> type);

   static Handle<Value> add(const Arguments& args);

   virtual Handle<Value> type_get(void) const;

   virtual Handle<Value> label_get() const;
   virtual void label_set(Handle<Value>);
   static void on_delete(void *, Evas_Object *, void *);
   virtual void resize_set(Handle<Value>);
};

#endif // C_ELM_BASIC_WINDOW_H

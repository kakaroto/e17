#ifndef C_ELM_PANE_H
#define C_ELM_PANE_H

#include "elm.h"
#include "CElmContainer.h"
#include "CElmObject.h"

namespace elm {

class CElmPane : public CElmContainer {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmPane(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmPane();

   struct {
      Persistent<Value> left;
      Persistent<Value> right;
   } cached;

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> horizontal_get() const;
   void horizontal_set(Handle<Value> val);

   Handle<Value> left_get() const;
   void left_set(Handle<Value> val);

   Handle<Value> right_get() const;
   void right_set(Handle<Value> val);

   Handle<Value> fixed_get() const;
   void fixed_set(Handle<Value> val);

   Handle<Value> size_left_content_get() const;
   void size_left_content_set(Handle<Value> val);

   Handle<Value> size_right_content_get() const;
   void size_right_content_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmPane>(const Arguments& args);
};

}

#endif


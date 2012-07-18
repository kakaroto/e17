#ifndef C_ELM_BUBBLE_H
#define C_ELM_BUBBLE_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

using namespace v8;

class CElmBubble : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmBubble(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmBubble();

   static Handle<FunctionTemplate> GetTemplate();

   struct {
      Persistent<Value> content;
   } cached;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> text_part_get() const;
   void text_part_set(Handle<Value> val);

   Handle<Value> corner_get() const;
   void corner_set(Handle<Value> val);

   Handle<Value> content_get() const;
   void content_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmBubble>(const Arguments& args);
};

}

#endif

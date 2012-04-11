#ifndef C_ELM_SCROLLER_H
#define C_ELM_SCROLLER_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmScroller : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmScroller(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmScroller();

   struct {
      Persistent<Value> content;
   } cached;

   static Handle<FunctionTemplate> GetTemplate();
   static Elm_Scroller_Policy policy_from_string(Handle<Value> val);
   static Local<Value> string_from_policy(Elm_Scroller_Policy policy);

public:
   static void Initialize(Handle<Object> val);

   void bounce_set(Handle<Value> val);
   Handle<Value> bounce_get() const;

   void policy_set(Handle<Value> val);
   Handle<Value> policy_get() const;

   void content_set(Handle<Value> val);
   Handle<Value> content_get() const;

   friend Handle<Value> CElmObject::New<CElmScroller>(const Arguments& args);
};

}

#endif // C_ELM_SCROLLER_H

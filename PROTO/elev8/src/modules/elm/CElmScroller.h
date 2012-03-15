#ifndef C_ELM_SCROLLER_H
#define C_ELM_SCROLLER_H

#include <v8.h>
#include "CEvasObject.h"

class CElmScroller : public CEvasObject {
   FACTORY(CElmScroller)
private:
   CPropHandler<CElmScroller> prop_handler;

   static Elm_Scroller_Policy policy_from_string(Handle<Value> val);
   static Local<Value> string_from_policy(Elm_Scroller_Policy policy);

   CElmScroller(CEvasObject *parent, Local<Object> obj);
public:
   virtual void bounce_set(Handle<Value> val);
   virtual Handle<Value> bounce_get() const;

   virtual void policy_set(Handle<Value> val);
   virtual Handle<Value> policy_get() const;

   virtual void content_set(Handle<Value> val);
   virtual Handle<Value> content_get() const;
};

#endif // C_ELM_SCROLLER_H

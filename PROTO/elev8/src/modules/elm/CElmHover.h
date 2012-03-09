#ifndef C_ELM_HOVER_H
#define C_ELM_HOVER_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmHover : public CEvasObject {
   FACTORY(CElmHover)

protected:
   CPropHandler<CElmHover> prop_handler;

   Persistent<Value> target;
   Persistent<Value> parent;

   /* the contents of Hover */
   Persistent<Value> top;
   Persistent<Value> top_left;
   Persistent<Value> top_right;
   Persistent<Value> bottom;
   Persistent<Value> bottom_left;
   Persistent<Value> bottom_right;
   Persistent<Value> left;
   Persistent<Value> right;
   Persistent<Value> middle;

public:
   CElmHover(CEvasObject *parent, Local<Object> obj);

   virtual void content_set(const char *swallow,Handle<Value> val);

   virtual Handle<Value> content_get(const char *swallow) const;

   void top_set(Handle<Value> val);

   virtual Handle<Value> top_get() const;

   void top_left_set(Handle<Value> val);

   virtual Handle<Value> top_left_get() const;

   void top_right_set(Handle<Value> val);

   virtual Handle<Value> top_right_get() const;

   void bottom_set(Handle<Value> val);

   virtual Handle<Value> bottom_get() const;

   void bottom_left_set(Handle<Value> val);

   virtual Handle<Value> bottom_left_get() const;

   void bottom_right_set(Handle<Value> val);

   virtual Handle<Value> bottom_right_get() const;

   void left_set(Handle<Value> val);

   virtual Handle<Value> left_get() const;

   void right_set(Handle<Value> val);

   virtual Handle<Value> right_get() const;

   void middle_set(Handle<Value> val);

   virtual Handle<Value> middle_get() const;
};

#endif

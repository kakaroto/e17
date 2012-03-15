#ifndef C_ELM_HOVER_H
#define C_ELM_HOVER_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmHover : public CEvasObject {
   FACTORY(CElmHover)

protected:
   CPropHandler<CElmHover> prop_handler;

   enum Position {
      TOP,
      TOP_LEFT,
      TOP_RIGHT,
      BOTTOM,
      BOTTOM_LEFT,
      BOTTOM_RIGHT,
      LEFT,
      RIGHT,
      MIDDLE,
      N_POSITIONS
   };
   static const char *position_as_string[];

   Persistent<Value> target;
   Persistent<Value> parent;

   Persistent<Value> positions[N_POSITIONS];

   virtual void content_set(CElmHover::Position, Handle<Value> val);

   CElmHover(CEvasObject *parent, Local<Object> obj);
public:
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

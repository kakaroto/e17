#ifndef C_ELM_HOVER_H
#define C_ELM_HOVER_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

using namespace v8;

class CElmHover : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmHover(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmHover();

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

   struct {
      Persistent<Value> content[N_POSITIONS];
   } cached;

   static Handle<FunctionTemplate> GetTemplate();

   void content_set(CElmHover::Position, Handle<Value> val);

   Persistent<Value> hover_parent;
   Persistent<Value> target;
public:
   static void Initialize(Handle<Object> val);

   void top_set(Handle<Value> val);
   Handle<Value> top_get() const;

   void top_left_set(Handle<Value> val);
   Handle<Value> top_left_get() const;

   void top_right_set(Handle<Value> val);
   Handle<Value> top_right_get() const;

   void bottom_set(Handle<Value> val);
   Handle<Value> bottom_get() const;

   void bottom_left_set(Handle<Value> val);
   Handle<Value> bottom_left_get() const;

   void bottom_right_set(Handle<Value> val);
   Handle<Value> bottom_right_get() const;

   void left_set(Handle<Value> val);
   Handle<Value> left_get() const;

   void right_set(Handle<Value> val);
   Handle<Value> right_get() const;

   void middle_set(Handle<Value> val);
   Handle<Value> middle_get() const;

   void target_set(Handle<Value> val);
   Handle<Value> target_get() const;

   void parent_set(Handle<Value> val);
   Handle<Value> parent_get() const;

   Handle<Value> dismiss(const Arguments&);

   Handle<Value> best_content_location_get(const Arguments& args);

   friend Handle<Value> CElmObject::New<CElmHover>(const Arguments &args);
};

}

#endif

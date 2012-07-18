#ifndef C_ELM_LAYOUT_H
#define C_ELM_LAYOUT_H

#include "elm.h"
#include "CElmObject.h"
#include "CElmContainer.h"

namespace elm {

using namespace v8;

class CElmLayout : public CElmContainer {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmLayout(Local<Object> _jsObject, CElmObject *parent);
   CElmLayout(Local<Object> _jsObject, Evas_Object *child);
   ~CElmLayout();

   static Handle<FunctionTemplate> GetTemplate();

   Persistent<Value> fileused;
   Persistent<Value> chosentheme;
   Persistent<Value> part_cursor;
   Persistent<Value> cursor_style;
   Persistent<Value> cursor_engine;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> file_get() const;
   void file_set(Handle<Value> val);

   Handle<Value> theme_get() const;
   void theme_set(Handle<Value> val);

   Handle<Value> part_cursor_get() const;
   void part_cursor_set(Handle<Value> val);

   Handle<Value> cursor_style_get() const;
   void cursor_style_set(Handle<Value> val);

   Handle<Value> cursor_engine_get() const;
   void cursor_engine_set(Handle<Value> val);

   Handle<Value> part_cursor_unset(const Arguments&);

   Handle<Value> sizing_eval(const Arguments&);

   Handle<Value> box_remove_all(const Arguments& args);

   Handle<Value> signal_emit(const Arguments& args);

   Handle<Value> table_clear(const Arguments& args);

   friend Handle<Value> CElmObject::New<CElmLayout>(const Arguments& args);
};

}
#endif // C_ELM_LAYOUT_H

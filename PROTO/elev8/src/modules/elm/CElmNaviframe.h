#ifndef C_ELM_NAVIFRAME_H
#define C_ELM_NAVIFRAME_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmNaviframe : public CElmObject {
private:
   bool title_visible;
   static Persistent<FunctionTemplate> tmpl;
   Persistent<Array> stack;

protected:
   CElmNaviframe(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmNaviframe();

   static Handle<FunctionTemplate> GetTemplate();

   void title_visible_eval();

   Persistent<Value> item_style;
public:
   static void Initialize(Handle<Object> target);

   Handle<Value> pop(const Arguments& args);
   Handle<Value> push(const Arguments& args);
   Handle<Value> promote(const Arguments& args);
   Handle<Value> item_promote(const Arguments& args);
   Handle<Value> pop_to(const Arguments& args);

   void title_visible_set(Handle<Value> val);
   Handle<Value> title_visible_get() const;

   void event_enabled_set(Handle<Value> val);
   Handle<Value> event_enabled_get() const;

   void prev_btn_auto_pushed_set(Handle<Value> val);
   Handle<Value> prev_btn_auto_pushed_get() const;

   void item_style_set(Handle<Value> val);
   Handle<Value> item_style_get() const;

   Handle<Value> items_get() const;
   Handle<Value> top_item_get() const;
   Handle<Value> bottom_item_get() const;

   friend Handle<Value> CElmObject::New<CElmNaviframe>(const Arguments &args);
};

}

#endif

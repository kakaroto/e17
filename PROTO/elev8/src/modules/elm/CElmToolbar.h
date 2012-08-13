#ifndef C_ELM_TOOLBAR_H
#define C_ELM_TOOLBAR_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmToolbar : public CElmObject {
private:
   class Item {
   public:
      Persistent<Object> self;
      Persistent<Value> data;
      Persistent<Value> callback;

      Item(Handle<Object> _self, Handle<Value> _data, Handle<Value> _callback)
         : self(Persistent<Object>::New(_self))
         , data(Persistent<Value>::New(_data))
         , callback(Persistent<Value>::New(_callback)) {}
      ~Item()
        {
           data.Dispose();
           callback.Dispose();
           self.Dispose();
        }
   };

   static Persistent<FunctionTemplate> tmpl;

   void AddItemStates(Local<Object>, Elm_Object_Item*, Handle<Value>);

protected:
   CElmToolbar(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   virtual Handle<Value> Pack(Handle<Value>, Handle<Value>);
   virtual Handle<Value> Unpack(Handle<Value>);

   static void OnSelect(void *data, Evas_Object *, void *);

   Handle<Value> icon_size_get() const;
   void icon_size_set(Handle<Value> value);

   Handle<Value> icon_order_lookup_get() const;
   void icon_order_lookup_set(Handle<Value> value);

   Handle<Value> homogeneous_get() const;
   void homogeneous_set(Handle<Value> value);

   Handle<Value> item_align_get() const;
   void item_align_set(Handle<Value> value);

   Handle<Value> shrink_mode_get() const;
   void shrink_mode_set(Handle<Value> value);

   Handle<Value> horizontal_get() const;
   void horizontal_set(Handle<Value> value);

   Handle<Value> standard_priority_get() const;
   void standard_priority_set(Handle<Value> value);

   Handle<Value> select_mode_get() const;
   void select_mode_set(Handle<Value> value);

   Handle<Value> items_count_get() const;

   Handle<Value> item_state_set(const Arguments& args);
   Handle<Value> item_state_get(const Arguments& args);

   friend Handle<Value> CElmObject::New<CElmToolbar>(const Arguments& args);
};

}

#endif

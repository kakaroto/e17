#ifndef C_ELM_TOOLBAR_H
#define C_ELM_TOOLBAR_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmToolbar : public CElmObject {
private:
   class State {
   public:
      Persistent<Object> self;
      Elm_Toolbar_Item_State *state;
      State(Local<Object> item, Local<Value> value);
      Handle<Object> ToObject() { return self; };
      static void OnSelect(void *data, Evas_Object *, void *);
   };

   class Item {
   public:
      Elm_Object_Item *object_item;
      Persistent<Object> self;

      Item(Handle<Value> value, Handle<Object> parent);
      ~Item();

      Handle<Object> ToObject() { return self; };
      static void OnSelect(void *data, Evas_Object *, void *);

      static Item *Unwrap(Handle<Value> value);
      static Item *Unwrap(const AccessorInfo &info);

      static Handle<Value> GetIcon(Local<String>, const AccessorInfo &info);
      static void SetIcon(Local<String>, Local<Value>, const AccessorInfo &info);

      static Handle<Value> GetLabel(Local<String>, const AccessorInfo &info);
      static void SetLabel(Local<String>, Local<Value>, const AccessorInfo &info);

      static Handle<Value> GetPriority(Local<String>, const AccessorInfo &info);
      static void SetPriority(Local<String>, Local<Value>, const AccessorInfo &info);

      static Handle<Value> GetSeparator(Local<String>, const AccessorInfo &info);
      static void SetSeparator(Local<String>, Local<Value>, const AccessorInfo &info);

      static Handle<Value> GetEnable(Local<String>, const AccessorInfo &info);
      static void SetEnable(Local<String>, Local<Value>, const AccessorInfo &info);

      static Handle<Value> GetElement(Local<String>, const AccessorInfo &info);
      static void SetElement(Local<String>, Local<Value>, const AccessorInfo &info);

      static Handle<Value> GetStates(Local<String>, const AccessorInfo &info);
      static void SetStates(Local<String>, Local<Value>, const AccessorInfo &info);

      static Handle<Value> GetState(Local<String>, const AccessorInfo &info);
      static void SetState(Local<String>, Local<Value>, const AccessorInfo &info);

      static Handle<Value> StatesGetter(Local<String> attr, const AccessorInfo& info);
      static Handle<Value> StatesSetter(Local<String> attr, Local<Value> val,
                                      const AccessorInfo& info);
   };

   static Persistent<FunctionTemplate> tmpl;

   void AddItemStates(Local<Object>, Elm_Object_Item*);
   Elm_Object_Item *CreateItem(Handle<Value> next,
                     Handle<Value> icon, Handle<Value> label,
                     Handle<Value> callback, Handle<Value> data);
   Elm_Object_Item *CreateItem(Handle<Value> next);

protected:
   CElmToolbar(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   virtual Handle<Value> Pack(Handle<Value>, Handle<Value>);
   virtual Handle<Value> Unpack(Handle<Value>);

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

   friend Handle<Value> CElmObject::New<CElmToolbar>(const Arguments& args);
};

}

#endif

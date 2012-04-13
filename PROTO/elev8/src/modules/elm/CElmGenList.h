#ifndef C_ELM_GEN_LIST_H
#define C_ELM_GEN_LIST_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmGenList : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmGenList(Local<Object> _jsObject, CElmObject *parent);

   struct {
      Persistent<Object> classes;
   } cached;

   class ItemClass;
   struct Item {
      ItemClass *klass;
      Persistent<Value> data;
      Persistent<Value> select_callback;

      Item(ItemClass *k, Handle<Value> d, Handle<Value> s)
         : klass(k)
         , data(Persistent<Value>::New(d))
         , select_callback(Persistent<Value>::New(s)) {}
      ~Item()
      {
         data.Dispose();
         select_callback.Dispose();
      }
      static void OnSelect(void *data, Evas_Object *, void *)
      {
         Item *item = static_cast<Item *>(data);
         if (!item->select_callback->IsFunction())
            return;

         HandleScope scope;
         Local<Function> callback(Function::Cast(*item->select_callback));
         Handle<Value> args[1] = { item->data };
         callback->Call(item->data->ToObject(), 1, args);
      }
   };

   class ItemClass {
   private:
      CElmGenList *genlist;
      Elm_Gen_Item_Class klass;
      Persistent<String> name;
      Persistent<Object> description;
      struct {
         Persistent<Value> text;
         Persistent<Value> content;
         Persistent<Value> state;
         Persistent<Value> del;
      } js;

      static char *GetTextWrapper(void *item, Evas_Object *, const char *part)
      {
         Item *list_item = static_cast<Item *>(item);
         if (list_item->klass->js.text->IsFunction())
            return list_item->klass->GetText(list_item->data, part);
         return NULL;
      }
      static Evas_Object *GetContentWrapper(void *item, Evas_Object *, const char *part)
      {
         Item *list_item = static_cast<Item *>(item);
         if (list_item->klass->js.content->IsFunction())
            return list_item->klass->GetContent(list_item->data, part);
         return NULL;
      }
      static Eina_Bool GetStateWrapper(void *item, Evas_Object *, const char *part)
      {
         Item *list_item = static_cast<Item *>(item);
         if (list_item->klass->js.state->IsFunction())
            return list_item->klass->GetState(list_item->data, part);
         return EINA_FALSE;
      }
      static void DeleteItemWrapper(void *item, Evas_Object *)
      {
         Item *list_item = static_cast<Item *>(item);
         if (list_item->klass->js.del->IsFunction())
            list_item->klass->DeleteItem(list_item->data);
         delete list_item;
      }

   public:
      ItemClass(CElmGenList *g, Handle<String> n, Handle<Object> d)
         : genlist(g)
         , name(Persistent<String>::New(n))
         , description(Persistent<Object>::New(d))
      {
         klass.item_style = "default";
         klass.func.text_get = ItemClass::GetTextWrapper;
         klass.func.content_get = ItemClass::GetContentWrapper;
         klass.func.state_get = ItemClass::GetStateWrapper;
         klass.func.del = ItemClass::DeleteItemWrapper;

         js.text = Persistent<Value>::New(d->Get(String::NewSymbol("text")));
         js.content = Persistent<Value>::New(d->Get(String::NewSymbol("content")));
         js.state = Persistent<Value>::New(d->Get(String::NewSymbol("state")));
         js.del = Persistent<Value>::New(d->Get(String::NewSymbol("del")));
      }
      ~ItemClass()
      {
         name.Dispose();
         description.Dispose();
         js.text.Dispose();
         js.content.Dispose();
         js.state.Dispose();
         js.del.Dispose();
      }
      Elm_Gen_Item_Class *GetElmClass()
      {
         return &klass;
      }
      char *GetText(Handle<Value> data, const char *part)
      {
         HandleScope scope;
         Local<Function> callback(Function::Cast(*js.text));
         Local<Object> temp = Object::New();
         temp->Set(String::NewSymbol("data"), data);
         temp->Set(String::NewSymbol("part"), String::New(part));
         Handle<Value> args[1] = { temp };
         Local<Value> text = callback->Call(temp, 1, args);
         return strdup(*String::Utf8Value(text->ToString()));
      }
      Evas_Object *GetContent(Handle<Value> data, const char *part)
      {
         HandleScope scope;
         Local<Function> callback(Function::Cast(*js.content));
         Local<Object> temp = Object::New();
         temp->Set(String::NewSymbol("data"), data);
         temp->Set(String::NewSymbol("part"), String::New(part));
         Handle<Value> args[1] = { temp };
         Local<Value> retval = callback->Call(temp, 1, args);

         if (!retval->IsObject())
            return NULL;

         /* FIXME: This might leak. Investigate. */
         Local<Value> content = Realise(retval->ToObject(), genlist->GetJSObject());
         return content.IsEmpty() ? NULL : GetEvasObjectFromJavascript(content);
      }
      Eina_Bool GetState(Handle<Value> data, const char *part)
      {
         HandleScope scope;
         Handle<Function> callback(Function::Cast(*js.state));
         Local<Object> temp = Object::New();
         temp->Set(String::NewSymbol("data"), data);
         temp->Set(String::NewSymbol("part"), String::New(part));
         Handle<Value> args[1] = { temp };
         return callback->Call(temp, 1, args)->BooleanValue();
      }
      void DeleteItem(Handle<Value> data)
      {
         HandleScope scope;
         Handle<Function> callback(Function::Cast(*js.del));
         Local<Object> temp = Object::New();
         temp->Set(String::NewSymbol("data"), data);
         Handle<Value> args[1] = { temp };
         callback->Call(temp, 1, args);
      }
   };

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> clear(const Arguments& args);
   Handle<Value> append(const Arguments& args);

   Handle<Value> multi_select_get() const;
   void multi_select_set(Handle<Value> value);

   Handle<Value> reorder_mode_get() const;
   void reorder_mode_set(Handle<Value> value);

   Handle<Value> mode_get() const;
   void mode_set(Handle<Value> value);

   Handle<Value> select_mode_get() const;
   void select_mode_set(Handle<Value> value);

   Handle<Value> block_count_get() const;
   void block_count_set(Handle<Value> value);

   Handle<Value>longpress_timeout_get() const;
   void longpress_timeout_set(Handle<Value> value);

   void vertical_bounce_set(Handle<Value> val);
   Handle<Value> vertical_bounce_get() const;

   void horizontal_bounce_set(Handle<Value> val);
   Handle<Value> horizontal_bounce_get() const;

   Handle<Value> highlight_mode_get() const;
   void highlight_mode_set(Handle<Value> value);

   Handle<Value> tree_effect_get() const;
   void tree_effect_set(Handle<Value> value);

   Handle<Value> decorate_mode_get() const;
   void decorate_mode_set(Handle<Value> value);

   Handle<Value> homogeneous_get() const;
   void homogeneous_set(Handle<Value> value);

   Handle<Value> classes_get() const;
   void classes_set(Handle<Value> value);

   friend Handle<Value> CElmObject::New<CElmGenList>(const Arguments&);
};

}

#endif


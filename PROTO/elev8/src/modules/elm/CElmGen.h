#ifndef C_ELM_GEN
#define C_ELM_GEN

#include "elm.h"
#include "CElmObject.h"
#include <v8.h>

namespace elm {
namespace gen {

using namespace v8;

template <class T> class ItemClass;

template <class T>
struct Item {
   ItemClass<T> *klass;
   Persistent<Value> data;
   Persistent<Value> select_callback;
   Elm_Object_Item *object_item;

   Item(ItemClass<T> *k, Handle<Value> d, Handle<Value> s)
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
      Item<T> *item = static_cast<Item<T> *>(data);
      if (!item->select_callback->IsFunction())
         return;
      HandleScope scope;
      Local<Function> callback(Function::Cast(*item->select_callback));
      Handle<Value> args[1] = { item->data };
      callback->Call(item->data->ToObject(), 1, args);
   }

   void Destroy()
   {
      if (object_item)
         elm_object_item_del(object_item);
   }
};

template <class T>
class ItemClass {
private:
   T *gen;
   Persistent<String> name;
   Persistent<Object> description;
   Elm_Gen_Item_Class klass;
   struct {
      Persistent<Value> text;
      Persistent<Value> content;
      Persistent<Value> state;
      Persistent<Value> del;
   } js;

   static char *GetTextWrapper(void *item, Evas_Object *, const char *part)
   {
      Item<T> *list_item = static_cast<Item<T> *>(item);
      if (list_item->klass->js.text->IsFunction())
         return list_item->klass->GetText(list_item->data, part);
      return NULL;
   }

   static Evas_Object *GetContentWrapper(void *item, Evas_Object *, const char *part)
   {
      Item<T> *list_item = static_cast<Item<T> *>(item);
      if (list_item->klass->js.content->IsFunction())
         return list_item->klass->GetContent(list_item->data, part);
      return NULL;
   }

   static Eina_Bool GetStateWrapper(void *item, Evas_Object *, const char *part)
   {
      Item<T> *list_item = static_cast<Item<T> *>(item);
      if (list_item->klass->js.state->IsFunction())
         return list_item->klass->GetState(list_item->data, part);
      return EINA_FALSE;
   }

   static void DeleteItemWrapper(void *item, Evas_Object *)
   {
      Item<T> *list_item = static_cast<Item<T> *>(item);
      if (list_item->klass->js.del->IsFunction())
         list_item->klass->DeleteItem(list_item->data);
      delete list_item;
   }

public:
   ItemClass(T *g, Handle<String> n, Handle<Object> d)
      : gen(g)
      , name(Persistent<String>::New(n))
      , description(Persistent<Object>::New(d))
   {
      Handle<Value> item_style = d->Get(String::NewSymbol("style"));
      if (!item_style->IsString())
         klass.item_style = eina_stringshare_add("default");
      else
         klass.item_style = eina_stringshare_add(*String::Utf8Value(item_style->ToString()));

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
      eina_stringshare_del(klass.item_style);
   }

   Elm_Gen_Item_Class *GetElmClass() { return &klass; }

   char *GetText(Handle<Value> data, const char *part)
   {
      HandleScope scope;
      Local<Function> callback(Function::Cast(*js.text));
      Local<Object> temp = Object::New();
      temp->Set(String::NewSymbol("data"), data);
      temp->Set(String::NewSymbol("part"), String::New(part));
      Handle<Value> args[1] = { temp };
      Local<Value> text = callback->Call(temp, 1, args);
      return (text.IsEmpty() || text->IsUndefined() || text->IsNull())
            ? NULL : strdup(*String::Utf8Value(text->ToString()));

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
      if (retval.IsEmpty() || !retval->IsObject())
         return NULL;

      /* FIXME: This might leak. Investigate. */
      Local<Value> content = CElmObject::Realise(retval->ToObject(), gen->GetJSObject());
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

}
}

#endif

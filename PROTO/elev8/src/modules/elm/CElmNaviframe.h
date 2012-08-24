#ifndef C_ELM_NAVIFRAME_H
#define C_ELM_NAVIFRAME_H

#include "elm.h"
#include "CElmLayout.h"
#include "CElmObject.h"

namespace elm {

class CElmNaviframe : public CElmLayout {
private:

   class Item {
   public:
      static struct Strings {
         Persistent<String> parent;
         Persistent<String> items;
         Persistent<String> item;
      } str;
      Persistent<Object> jsObject;
      Elm_Object_Item *object_item;

      Item(Elm_Object_Item *_object_item, Local<Object> obj, Handle<Value> parent)
        {
           HandleScope scope;
           static Persistent<ObjectTemplate> tmpl;

           if (tmpl.IsEmpty())
             {
                Local<FunctionTemplate> klass = FunctionTemplate::New();
                klass->SetClassName(String::NewSymbol("NaviframeItem"));

                tmpl = Persistent<ObjectTemplate>::New(klass->InstanceTemplate());
                tmpl->SetNamedPropertyHandler(ElementGet, ElementSet, NULL, ElementDel);

                str.parent = Persistent<String>::New(String::NewSymbol("naviframe::parent"));
                str.items = Persistent<String>::New(String::NewSymbol("naviframe::items"));
                str.item = Persistent<String>::New(String::NewSymbol("naviframe::item"));
             }

           object_item = _object_item;
           elm_object_item_data_set(object_item, this);
           elm_object_item_del_cb_set(object_item, Delete);

           jsObject = Persistent<Object>::New(tmpl->NewInstance());
           jsObject->SetHiddenValue(str.parent, parent);
           jsObject->SetHiddenValue(str.item, External::Wrap(this));
           jsObject->SetHiddenValue(str.items, obj);

           Local<Array> props = obj->GetOwnPropertyNames();
           for (unsigned int i = 0; i < props->Length(); i++)
             {
                Local<Value> key = props->Get(i);
                jsObject->Set(key, obj->Get(key));
             }
        }

      ~Item()
        {
           Local<Function> callback
              (Function::Cast(*jsObject->Get(String::NewSymbol("on_delete"))));

           if (callback->IsFunction())
             callback->Call(jsObject, 0, NULL);

           jsObject->DeleteHiddenValue(str.item);
           jsObject.Dispose();
        }

      Handle<Object> ToObject()
        {
           return jsObject;
        }

      static void Delete(void *data, Evas_Object *, void *)
        {
           delete static_cast<Item *>(data);
        }

      static Handle<Value> ElementSet(Local<String> attr, Local<Value> val,
                                      const AccessorInfo& info)
        {
           HandleScope scope;

           Item *item = Unwrap(info.This());
           Handle<Object> obj = item->ToObject();
           Handle<Value> value = val;

           String::Utf8Value part(attr);
           Handle<Value> parent = obj->GetHiddenValue(str.parent);

           if (!strcmp(*part, "title"))
             {
                elm_object_item_part_text_set(item->object_item, "default",
                                              *String::Utf8Value(val));
             }
           else if (!strcmp(*part, "subtitle"))
             {
                elm_object_item_part_text_set(item->object_item, *part,
                                              *String::Utf8Value(val));
             }
           else if (!strcmp(*part, "content"))
             {
                value = Realise(value, parent);
                if (value->IsUndefined())
                  elm_object_item_part_content_unset(item->object_item, *part);
                else
                  elm_object_item_part_content_set(item->object_item, "default",
                                                   GetEvasObjectFromJavascript(value));
             }
           else if (!strcmp(*part, "prev_btn") ||
                    !strcmp(*part, "next_btn") ||
                    !strcmp(*part, "icon"))
             {
                value = Realise(value, parent);
                if (value->IsUndefined())
                  elm_object_item_part_content_unset(item->object_item, *part);
                else
                  elm_object_item_part_content_set(item->object_item, *part,
                                                   GetEvasObjectFromJavascript(value));
             }
           else if (!strcmp(*part, "style"))
             {
                elm_naviframe_item_style_set(item->object_item,
                                             *String::Utf8Value(value));
             }

           Local<Value> items = obj->GetHiddenValue(str.items);
           info.This()->Delete(attr);
           items->ToObject()->Set(attr, value);
           return val;
        }

      static Handle<Value> ElementGet(Local<String> attr, const AccessorInfo& info)
        {
           HandleScope scope;
           Item *item = Unwrap(info.This());
           Handle<Object> obj = item->ToObject();
           Local<Value> items = obj->GetHiddenValue(str.items);
           return scope.Close(items->ToObject()->Get(attr));
        }

      static Handle<Boolean> ElementDel(Local<String> attr, const AccessorInfo& info)
        {
           HandleScope scope;
           Item *item = Unwrap(info.This());
           Handle<Object> obj = item->ToObject();
           Local<Object> items = obj->GetHiddenValue(str.items)->ToObject();

           Local<Value> jsItem = items->Get(attr);
           if (CElmObject::HasInstance(jsItem))
             delete GetObjectFromJavascript(jsItem);

           items->Delete(attr);
           return scope.Close(Boolean::New(true));
        }

      static Item *Unwrap(Handle<Value> value)
        {
           if (!value->IsObject())
             return NULL;
           value = value->ToObject()->GetHiddenValue(str.item);
           if (value.IsEmpty())
             return NULL;
           return static_cast<Item *>(External::Unwrap(value));
        }
   };

   bool title_visible;
   static Persistent<FunctionTemplate> tmpl;
   Persistent<Array> stack;

protected:
   CElmNaviframe(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmNaviframe();

   static Handle<FunctionTemplate> GetTemplate();

   void title_visible_eval();

public:
   static void Initialize(Handle<Object> target);

   virtual Handle<Value> Pack(Handle<Value>, Handle<Value>);
   virtual Handle<Value> Unpack(Handle<Value>);

   Handle<Value> pop(const Arguments& args);
   Handle<Value> promote(const Arguments& args);
   Handle<Value> item_promote(const Arguments& args);
   Handle<Value> pop_to(const Arguments& args);

   void title_visible_set(Handle<Value> val);
   Handle<Value> title_visible_get() const;

   void event_enabled_set(Handle<Value> val);
   Handle<Value> event_enabled_get() const;

   void prev_btn_auto_pushed_set(Handle<Value> val);
   Handle<Value> prev_btn_auto_pushed_get() const;

   void content_preserve_on_pop_set(Handle<Value> val);
   Handle<Value> content_preserve_on_pop_get() const;

   Handle<Value> items_get() const;
   Handle<Value> top_item_get() const;
   Handle<Value> bottom_item_get() const;

   friend Handle<Value> CElmObject::New<CElmNaviframe>(const Arguments &args);
};

}

#endif

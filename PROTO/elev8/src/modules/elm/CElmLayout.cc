#include "CElmLayout.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmLayout, file);
GENERATE_PROPERTY_CALLBACKS(CElmLayout, theme);
GENERATE_PROPERTY_CALLBACKS(CElmLayout, part_cursor);
GENERATE_PROPERTY_CALLBACKS(CElmLayout, cursor_style);
GENERATE_PROPERTY_CALLBACKS(CElmLayout, cursor_engine);
GENERATE_METHOD_CALLBACKS(CElmLayout, part_cursor_unset);
GENERATE_METHOD_CALLBACKS(CElmLayout, sizing_eval);
GENERATE_METHOD_CALLBACKS(CElmLayout, box_remove_all);
GENERATE_METHOD_CALLBACKS(CElmLayout, signal_emit);
GENERATE_METHOD_CALLBACKS(CElmLayout, table_clear);

GENERATE_TEMPLATE_FULL(CElmContainer, CElmLayout,
                  PROPERTY(file),
                  PROPERTY(theme),
                  PROPERTY(part_cursor),
                  PROPERTY(cursor_style),
                  PROPERTY(cursor_engine),
                  METHOD(part_cursor_unset),
                  METHOD(sizing_eval),
                  METHOD(box_remove_all),
                  METHOD(signal_emit),
                  METHOD(table_clear));

CElmLayout::CElmLayout(Local<Object> _jsObject, CElmObject *_parent)
     : CElmContainer(_jsObject, elm_layout_add(_parent->GetEvasObject()))
{
}

CElmLayout::CElmLayout(Local<Object> _jsObject, Evas_Object *child)
     : CElmContainer(_jsObject, child)
{
}

CElmLayout::~CElmLayout()
{
   fileused.Dispose();
   chosentheme.Dispose();
   part_cursor.Dispose();
   cursor_style.Dispose();
   cursor_engine.Dispose();
}

Handle<Value> CElmLayout::file_get() const
{
   return fileused;
}

void CElmLayout::file_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   Eina_Bool status;
   Local<Object> file = val->ToObject();

   status = elm_layout_file_set(eo,
                 *String::Utf8Value(file->Get(0)),
                 *String::Utf8Value(file->Get(1)));

   if(!status)
     return;

   fileused.Dispose();
   fileused = Persistent<Value>::New(val);
}

Handle<Value> CElmLayout::theme_get() const
{
   return chosentheme;
}

void CElmLayout::theme_set(Handle <Value> val)
{
   if (!val->IsArray())
     return;

   Eina_Bool status;
   Local<Object> theme = val->ToObject();

   status = elm_layout_theme_set(eo,
                 *String::Utf8Value(theme->Get(0)),
                 *String::Utf8Value(theme->Get(1)),
                 *String::Utf8Value(theme->Get(2)));
   if(!status)
     return;

   chosentheme.Dispose();
   chosentheme = Persistent<Value>::New(val);
}

Handle<Value> CElmLayout::contents_get() const
{
   HandleScope scope;

   return scope.Close(the_contents);
}

void CElmLayout::contents_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Handle<Object> contents = val->ToObject();
   Handle<Array> properties = contents->GetPropertyNames();
   for (unsigned int i = 0; i <properties->Length(); i++)
     {
        Handle<Value> element = properties->Get(Integer::New(i));
        CEvasObject *child = make_or_get(this, contents->Get(element->ToString()));
        if (child)
          {
             elm_object_part_content_set(eo, *String::Utf8Value(element), child->get());
             the_contents->Set(element, child->get_object());
          }
     }
}

}

#include "elm.h"
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
   Local<Object> obj = val->ToObject();
   Local<Value> name = obj->Get(String::NewSymbol("name"));
   Local<Value> group = obj->Get(String::NewSymbol("group"));

   if (!name->IsString() || !group->IsString())
     return;

   if (!elm_layout_file_set(eo, *String::Utf8Value(name), *String::Utf8Value(group)))
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
   Local<Object> obj = val->ToObject();
   Local<Value> klass = obj->Get(String::NewSymbol("class"));
   Local<Value> group = obj->Get(String::NewSymbol("group"));
   Local<Value> style = obj->Get(String::NewSymbol("style"));

   Eina_Bool success = elm_layout_theme_set(eo,
                                            *String::Utf8Value(klass),
                                            *String::Utf8Value(group),
                                            *String::Utf8Value(style));
   if (!success)
        return;

   chosentheme.Dispose();
   chosentheme = Persistent<Value>::New(val);
}

Handle<Value> CElmLayout::part_cursor_get() const
{
   if (part_cursor.IsEmpty())
     return Undefined();

   Local<Object> cursor = part_cursor->ToObject();
   return String::New(elm_layout_part_cursor_get(eo,
                           *String::Utf8Value(cursor->Get(0))));
}

void CElmLayout::part_cursor_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   Eina_Bool status;
   Local<Object> cursor = val->ToObject();
   status = elm_layout_part_cursor_set(eo,
                 *String::Utf8Value(cursor->Get(0)),
                 *String::Utf8Value(cursor->Get(1)));

   if(!status)
     return;

   part_cursor.Dispose();
   part_cursor = Persistent<Value>::New(val);
}

Handle<Value> CElmLayout::cursor_style_get() const
{
   if (cursor_style.IsEmpty())
     return Undefined();

   Local<Object> cursor = cursor_style->ToObject();
   return String::New(elm_layout_part_cursor_style_get(eo,
                           *String::Utf8Value(cursor->Get(0))));
}

void CElmLayout::cursor_style_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   Eina_Bool status;
   Local<Object> cursor = val->ToObject();
   status = elm_layout_part_cursor_style_set(eo,
                 *String::Utf8Value(cursor->Get(0)),
                 *String::Utf8Value(cursor->Get(1)));

   if(!status)
     return;

   cursor_style.Dispose();
   cursor_style = Persistent<Value>::New(val);
}

Handle<Value> CElmLayout::cursor_engine_get() const
{
   if (cursor_engine.IsEmpty())
     return Undefined();

   Local<Object> cursor = cursor_engine->ToObject();
   return Boolean::New(elm_layout_part_cursor_engine_only_get(eo,
                           *String::Utf8Value(cursor->Get(0))));
}

void CElmLayout::cursor_engine_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   Eina_Bool status;
   Local<Object> cursor = val->ToObject();
   status = elm_layout_part_cursor_engine_only_set(eo,
                 *String::Utf8Value(cursor->Get(0)),
                 cursor->Get(1)->BooleanValue());

   if(!status)
     return;

   cursor_engine.Dispose();
   cursor_engine = Persistent<Value>::New(val);
}

Handle<Value> CElmLayout::part_cursor_unset(const Arguments&)
{
   Local<Object> cursor = part_cursor->ToObject();
   return Boolean::New(elm_layout_part_cursor_unset(eo,
                           *String::Utf8Value(cursor->Get(0))));
}

Handle<Value> CElmLayout::sizing_eval(const Arguments&)
{
   elm_layout_sizing_eval(eo);
   return Undefined();
}

Handle<Value> CElmLayout::box_remove_all(const Arguments& args)
{
   if((args[0]->IsUndefined()) && (args[1]->IsBoolean()))
     return Boolean::New(elm_layout_box_remove_all(eo,
                              *String::Utf8Value(args[0]),
                              args[1]->BooleanValue()));
   return Undefined();
}

Handle<Value> CElmLayout::signal_emit(const Arguments& args)
{
   if((!args[0]->IsUndefined()) && (!args[1]->IsUndefined()))
     elm_layout_signal_emit(eo, *String::Utf8Value(args[0]), *String::Utf8Value(args[1]));

   return Undefined();
}

Handle<Value> CElmLayout::table_clear(const Arguments& args)
{
   if((args[0]->IsUndefined()) && (args[1]->IsBoolean()))
     return Boolean::New(elm_layout_table_clear(eo,
                              *String::Utf8Value(args[0]),
                              args[1]->BooleanValue()));
   return Undefined();
}

void CElmLayout::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Layout"), GetTemplate()->GetFunction());
}

}

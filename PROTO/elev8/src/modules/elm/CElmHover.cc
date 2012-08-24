#include "elm.h"
#include "CElmHover.h"

namespace elm {

using namespace v8;

const char *CElmHover::position_as_string[] = {
   "top", "top_left", "top_right",
   "bottom", "bottom_left", "bottom_right",
   "left", "right", "middle"
};

GENERATE_PROPERTY_CALLBACKS(CElmHover, top);
GENERATE_PROPERTY_CALLBACKS(CElmHover, top_left);
GENERATE_PROPERTY_CALLBACKS(CElmHover, top_right);
GENERATE_PROPERTY_CALLBACKS(CElmHover, bottom);
GENERATE_PROPERTY_CALLBACKS(CElmHover, bottom_left);
GENERATE_PROPERTY_CALLBACKS(CElmHover, bottom_right);
GENERATE_PROPERTY_CALLBACKS(CElmHover, left);
GENERATE_PROPERTY_CALLBACKS(CElmHover, right);
GENERATE_PROPERTY_CALLBACKS(CElmHover, middle);
GENERATE_PROPERTY_CALLBACKS(CElmHover, target);
GENERATE_PROPERTY_CALLBACKS(CElmHover, parent);
GENERATE_METHOD_CALLBACKS(CElmHover, dismiss);
GENERATE_METHOD_CALLBACKS(CElmHover, best_content_location_get);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmHover,
                  PROPERTY(top),
                  PROPERTY(top_left),
                  PROPERTY(top_right),
                  PROPERTY(bottom),
                  PROPERTY(bottom_left),
                  PROPERTY(bottom_right),
                  PROPERTY(left),
                  PROPERTY(right),
                  PROPERTY(middle),
                  PROPERTY(target),
                  PROPERTY(parent),
                  METHOD(dismiss),
                  METHOD(best_content_location_get));

CElmHover::CElmHover(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_hover_add(_parent->GetEvasObject()))
{
}

CElmHover::~CElmHover()
{
   for (unsigned i = 0; i < N_POSITIONS; i++)
      cached.content[i].Dispose();

   target.Dispose();
   hover_parent.Dispose();
}

void CElmHover::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Hover"), GetTemplate()->GetFunction());
}

void CElmHover::content_set(Position pos, Handle<Value> val)
{
   if (!val->IsObject())
     return;

   cached.content[pos].Dispose();
   cached.content[pos] = Persistent<Value>::New(Realise(val, jsObject));
   elm_object_part_content_set(eo, position_as_string[pos], 
                               GetEvasObjectFromJavascript(cached.content[pos]));
}

void CElmHover::top_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(TOP, val);
}

Handle<Value> CElmHover::top_get() const
{
   return cached.content[TOP];
}

void CElmHover::top_left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(TOP_LEFT, val);
}

Handle<Value> CElmHover::top_left_get() const
{
   return cached.content[TOP_LEFT];
}

void CElmHover::top_right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(TOP_RIGHT, val);
}

Handle<Value> CElmHover::top_right_get() const
{
   return cached.content[TOP_RIGHT];
}

void CElmHover::bottom_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(BOTTOM, val);
}

Handle<Value> CElmHover::bottom_get() const
{
   return cached.content[BOTTOM];
}

void CElmHover::bottom_left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(BOTTOM_LEFT, val);
}

Handle<Value> CElmHover::bottom_left_get() const
{
   return cached.content[BOTTOM_LEFT];
}

void CElmHover::bottom_right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(BOTTOM_RIGHT, val);
}

Handle<Value> CElmHover::bottom_right_get() const
{
   return cached.content[BOTTOM_RIGHT];
}

void CElmHover::left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(LEFT, val);
}

Handle<Value> CElmHover::left_get() const
{
   return cached.content[LEFT];
}

void CElmHover::right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(RIGHT, val);
}

Handle<Value> CElmHover::right_get() const
{
   return cached.content[RIGHT];
}

void CElmHover::middle_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(MIDDLE, val);
}

Handle<Value> CElmHover::middle_get() const
{
   return cached.content[MIDDLE];
}

Handle<Value> CElmHover::dismiss(const Arguments&)
{
  elm_hover_dismiss(eo);
  return Undefined();
}

Handle<Value> CElmHover::best_content_location_get(const Arguments& args)
{
  if (!args[0]->IsString())
    return Undefined();

  String::Utf8Value pref_axis(args[0]->ToString());
  const char* best_content = elm_hover_best_content_location_get(eo, ELM_HOVER_AXIS_NONE);

  if (!strcmp(*pref_axis, "horizontal"))
    best_content = elm_hover_best_content_location_get(eo, ELM_HOVER_AXIS_HORIZONTAL);
  else if (!strcmp(*pref_axis, "vertical"))
    best_content = elm_hover_best_content_location_get(eo, ELM_HOVER_AXIS_VERTICAL);
  else if (!strcmp(*pref_axis, "both"))
    best_content = elm_hover_best_content_location_get(eo, ELM_HOVER_AXIS_BOTH);

  return best_content ? String::New(best_content) :  Undefined();
}

Handle<Value> CElmHover::target_get() const
{
  return target;
}

void CElmHover::target_set(Handle<Value> val)
{
  if (val->IsObject())
    {
      elm_hover_target_set(eo, GetEvasObjectFromJavascript(val));

      target.Dispose();
      target = Persistent<Value>::New(val);
    }
}

Handle<Value> CElmHover::parent_get() const
{
  return hover_parent;
}

void CElmHover::parent_set(Handle<Value> val)
{
  if (val->IsObject())
    {
      elm_hover_parent_set(eo, GetEvasObjectFromJavascript(val));

      hover_parent.Dispose();
      hover_parent = Persistent<Value>::New(val);
    }
}

}

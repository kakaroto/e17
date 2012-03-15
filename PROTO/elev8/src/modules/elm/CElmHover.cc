#include "CElmHover.h"

const char *CElmHover::position_as_string[] = {
   "top", "top_left", "top_right",
   "bottom", "bottom_left", "bottom_right",
   "left", "right", "middle"
};

CElmHover::CElmHover(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_hover_add(parent->top_widget_get());
   construct(eo, obj);
}

void CElmHover::content_set(Position pos, Handle<Value> val)
{
   if (!val->IsObject())
     return;

   CEvasObject *content = make_or_get(this,val);

   elm_object_part_content_set(eo, CElmHover::position_as_string[pos], content->get());

   positions[pos].Dispose();
   positions[pos] = Persistent<Value>::New(content->get_object());
}

void CElmHover::top_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(TOP, val);
}

Handle<Value> CElmHover::top_get() const
{
   return positions[TOP];
}

void CElmHover::top_left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(TOP_LEFT, val);
}

Handle<Value> CElmHover::top_left_get() const
{
   return positions[TOP_LEFT];
}

void CElmHover::top_right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(TOP_RIGHT, val);
}

Handle<Value> CElmHover::top_right_get() const
{
   return positions[TOP_RIGHT];
}

void CElmHover::bottom_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(BOTTOM, val);
}

Handle<Value> CElmHover::bottom_get() const
{
   return positions[BOTTOM];
}

void CElmHover::bottom_left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(BOTTOM_LEFT, val);
}

Handle<Value> CElmHover::bottom_left_get() const
{
   return positions[BOTTOM_LEFT];
}

void CElmHover::bottom_right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(BOTTOM_RIGHT, val);
}

Handle<Value> CElmHover::bottom_right_get() const
{
   return positions[BOTTOM_RIGHT];
}

void CElmHover::left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(LEFT, val);
}

Handle<Value> CElmHover::left_get() const
{
   return positions[LEFT];
}

void CElmHover::right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(RIGHT, val);
}

Handle<Value> CElmHover::right_get() const
{
   return positions[RIGHT];
}

void CElmHover::middle_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set(MIDDLE, val);
}

Handle<Value> CElmHover::middle_get() const
{
   return positions[MIDDLE];
}

PROPERTIES_OF(CElmHover) = {
  PROP_HANDLER(CElmHover, top),
  PROP_HANDLER(CElmHover, top_left),
  PROP_HANDLER(CElmHover, top_right),
  PROP_HANDLER(CElmHover, bottom),
  PROP_HANDLER(CElmHover, bottom_left),
  PROP_HANDLER(CElmHover, bottom_right),
  PROP_HANDLER(CElmHover, left),
  PROP_HANDLER(CElmHover, right),
  PROP_HANDLER(CElmHover, middle),
  { NULL }
};

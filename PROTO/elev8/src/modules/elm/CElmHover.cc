#include "CElmHover.h"

CElmHover::CElmHover(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_hover_add(parent->top_widget_get());
   construct(eo, obj);
}

void CElmHover::content_set(const char *swallow,Handle<Value> val)
{
   if (val->IsObject())
     {

        CEvasObject *content = make_or_get(this,val);

        elm_object_part_content_set(eo, swallow, content->get());

        if (!strcmp(swallow, "top"))
          {
             top.Dispose();
             top = Persistent<Value>::New(content->get_object());
          }
        if (!strcmp(swallow, "top_left"))
          {
             top_left.Dispose();
             top_left = Persistent<Value>::New(content->get_object());
          }
        if (!strcmp(swallow, "top_right"))
          {
             top_right.Dispose();
             top_right = Persistent<Value>::New(content->get_object());
          }
        if (!strcmp(swallow, "bottom"))
          {
             bottom.Dispose();
             bottom = Persistent<Value>::New(content->get_object());
          }
        if (!strcmp(swallow, "bottom_left"))
          {
             bottom_left.Dispose();
             bottom_left = Persistent<Value>::New(content->get_object());
          }
        if (!strcmp(swallow, "bottom_right"))
          {
             bottom_right.Dispose();
             bottom_right = Persistent<Value>::New(content->get_object());
          }
        if (!strcmp(swallow, "left"))
          {
             left.Dispose();
             left = Persistent<Value>::New(content->get_object());
          }
        if (!strcmp(swallow, "right"))
          {
             right.Dispose();
             right = Persistent<Value>::New(content->get_object());
          }
        if (!strcmp(swallow, "middle"))
          {
             middle.Dispose();
             middle = Persistent<Value>::New(content->get_object());
          }
     }
}

Handle<Value> CElmHover::content_get(const char *swallow) const
{
   if (!strcmp(swallow, "top"))
     return top;
   if (!strcmp(swallow, "top_left"))
     return top_left;
   if (!strcmp(swallow, "top_right"))
     return top_right;
   if (!strcmp(swallow, "bottom"))
     return bottom;
   if (!strcmp(swallow, "bottom_left"))
     return bottom_left;
   if (!strcmp(swallow, "bottom_right"))
     return bottom_right;
   if (!strcmp(swallow, "left"))
     return left;
   if (!strcmp(swallow, "right"))
     return right;
   if (!strcmp(swallow, "middle"))
     return middle;

   return Null();
}

void CElmHover::top_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("top", val);
}

Handle<Value> CElmHover::top_get() const
{
   return content_get("top");
}

void CElmHover::top_left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("top_left", val);
}

Handle<Value> CElmHover::top_left_get() const
{
   return content_get("top_left");
}

void CElmHover::top_right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("top_right", val);
}

Handle<Value> CElmHover::top_right_get() const
{
   return content_get("top_right");
}

void CElmHover::bottom_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("bottom", val);
}

Handle<Value> CElmHover::bottom_get() const
{
   return content_get("bottom");
}

void CElmHover::bottom_left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("bottom_left", val);
}

Handle<Value> CElmHover::bottom_left_get() const
{
   return content_get("bottom_left");
}

void CElmHover::bottom_right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("bottom_right", val);
}

Handle<Value> CElmHover::bottom_right_get() const
{
   return content_get("bottom_right");
}

void CElmHover::left_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("left", val);
}

Handle<Value> CElmHover::left_get() const
{
   return content_get("left");
}

void CElmHover::right_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("right", val);
}

Handle<Value> CElmHover::right_get() const
{
   return content_get("right");
}

void CElmHover::middle_set(Handle<Value> val)
{
   if (val->IsObject())
     content_set("middle", val);
}

Handle<Value> CElmHover::middle_get() const
{
   return content_get("middle");
}

template<> CEvasObject::CPropHandler<CElmHover>::property_list
CEvasObject::CPropHandler<CElmHover>::list[] = {
  PROP_HANDLER(CElmHover, top),
  PROP_HANDLER(CElmHover, top_left),
  PROP_HANDLER(CElmHover, top_right),
  PROP_HANDLER(CElmHover, bottom),
  PROP_HANDLER(CElmHover, bottom_left),
  PROP_HANDLER(CElmHover, bottom_right),
  PROP_HANDLER(CElmHover, left),
  PROP_HANDLER(CElmHover, right),
  PROP_HANDLER(CElmHover, middle),
  { NULL, NULL, NULL },
};

#include "CElmInwin.h"

CElmInwin::CElmInwin(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_win_inwin_add(parent->top_widget_get());
   construct(eo, obj);
   content = make_or_get(this, obj->Get(String::New("content")));
   if (content)
     {
        elm_win_inwin_content_set(eo, content->get());
     }
}

Handle<Value> CElmInwin::activate_get() const
{
   return Null();
}

void CElmInwin::activate_set(Handle<Value> val)
{
   ELM_INF("Actiavted.");
   if (val->IsBoolean())
     elm_win_inwin_activate(eo);
}

PROPERTIES_OF(CElmInwin) = {
     PROP_HANDLER(CElmInwin, activate),
     { NULL }
};

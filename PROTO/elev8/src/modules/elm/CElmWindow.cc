#include "elm.h"
#include "CElmWindow.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmWindow, title);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, conformant);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, autodel);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, borderless);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, shaped);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, alpha);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, override);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, fullscreen);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, maximized);

GENERATE_TEMPLATE(CElmWindow,
                  PROPERTY(title),
                  PROPERTY(conformant),
                  PROPERTY(autodel),
                  PROPERTY(borderless),
                  PROPERTY(shaped),
                  PROPERTY(alpha),
                  PROPERTY(override),
                  PROPERTY(fullscreen),
                  PROPERTY(maximized));

// Getters and Settters

Handle<Value> CElmWindow::title_get() const
{
   const char *s = elm_win_title_get(eo);
   return s ? String::New(s) : Undefined();
}

void CElmWindow::title_set(Handle<Value> val)
{
   if (val->IsString())
     elm_win_title_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmWindow::conformant_get() const
{
   return Boolean::New(elm_win_conformant_get(eo));
}

void CElmWindow::conformant_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_conformant_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::autodel_get() const
{
   return Boolean::New(elm_win_autodel_get(eo));
}

void CElmWindow::autodel_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_autodel_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::borderless_get() const
{
   return Boolean::New(elm_win_borderless_get(eo));
}

void CElmWindow::borderless_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_borderless_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::shaped_get() const
{
   return Boolean::New(elm_win_shaped_get(eo));
}

void CElmWindow::shaped_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_shaped_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::alpha_get() const
{
   return Boolean::New(elm_win_alpha_get(eo));
}

void CElmWindow::alpha_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_alpha_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::override_get() const
{
   return Boolean::New(elm_win_override_get(eo));
}

void CElmWindow::override_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_override_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::fullscreen_get() const
{
   return Boolean::New(elm_win_fullscreen_get(eo));
}

void CElmWindow::fullscreen_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_fullscreen_set(eo, val->BooleanValue());
}

Handle<Value> CElmWindow::maximized_get() const
{
   return Boolean::New(elm_win_maximized_get(eo));
}

void CElmWindow::maximized_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_win_maximized_set(eo, val->BooleanValue());
}

//---------------------

CElmWindow::CElmWindow(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_win_add(parent ? parent->GetEvasObject() : NULL, "main", ELM_WIN_BASIC))
{
   evas_object_focus_set(eo, 1);
   evas_object_smart_callback_add(eo, "delete,request", &quit, NULL);
   evas_object_show(eo);
}

void CElmWindow::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Window"), GetTemplate()->GetFunction());
}

void CElmWindow::quit(void *, Evas_Object *, void *)
{
   //TODO: check if his window has parent
   ecore_main_loop_quit();
}

}

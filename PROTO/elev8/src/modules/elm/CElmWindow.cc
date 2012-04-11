#include "elm.h"
#include "CElmWindow.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmWindow, title);
GENERATE_PROPERTY_CALLBACKS(CElmWindow, conformant);

GENERATE_TEMPLATE(CElmWindow,
                  PROPERTY(title),
                  PROPERTY(conformant));

// Getters and Settters

Handle<Value> CElmWindow::title_get() const
{
   return String::New(elm_win_title_get(eo));
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
   if (val->IsString())
     elm_win_title_set(eo, *String::Utf8Value(val));
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

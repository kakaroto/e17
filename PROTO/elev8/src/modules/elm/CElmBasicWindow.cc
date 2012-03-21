#include "CElmBasicWindow.h"

CElmBasicWindow::CElmBasicWindow(CEvasObject *parent, Local<Object> obj,
                                 Local<String> name, Local<Number> type)
   : CEvasObject()
{
   eo = elm_win_add(parent ? parent->get() : NULL,
                    *String::Utf8Value(name),
                    (Elm_Win_Type) (type->Value()));
   construct(eo, obj);

   /*
    * Create elements and attach to parent so children can see siblings
    * that have already been created.  Useful to find radio button groups.
    */
   Handle<Object> elements = Object::New();
   get_object()->Set(String::New("elements"), elements);
   realize_objects(obj->Get(String::New("elements")), elements);

   evas_object_focus_set(eo, 1);
   evas_object_smart_callback_add(eo, "delete,request", &on_delete, NULL);

   win_name = Persistent<Value>::New(name);
   win_type = Persistent<Value>::New(type);

   get_object()->Set(String::New("add"), FunctionTemplate::New(add)->GetFunction());
}

Handle<Value> CElmBasicWindow::add(const Arguments& args) 
{
   CEvasObject *self = eo_from_info(args.This());
   CEvasObject *obj = make_or_get(self, args[0]->ToObject());
   return obj->get_object();
}

Handle<Value> CElmBasicWindow::type_get(void) const
{
   return String::New("main");
}

Handle<Value> CElmBasicWindow::label_get() const
{
   return String::New(elm_win_title_get(eo));
}

void CElmBasicWindow::label_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        elm_win_title_set(eo, *str);
     }
}

Handle<Value> CElmBasicWindow::conformant_get() const
{
   return Boolean::New(elm_win_conformant_get(eo));
}

void CElmBasicWindow::conformant_set(Handle<Value> conformant)
{
   if (conformant->IsBoolean())
     {
        elm_win_conformant_set(eo, conformant->BooleanValue());
     }
}

void CElmBasicWindow::on_delete(void *, Evas_Object *, void *)
{
   elm_exit();
}

void CElmBasicWindow::resize_set(Handle<Value>)
{
   ELM_ERR("warning: resize=true ignored on main window");
}

PROPERTIES_OF(CElmBasicWindow) = {
   PROP_HANDLER(CElmBasicWindow, conformant),
   { NULL }
};

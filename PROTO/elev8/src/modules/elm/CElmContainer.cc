#include "elm.h"
#include "CElmContainer.h"

namespace elm {

using namespace v8;

GENERATE_METHOD_CALLBACKS(CElmContainer, content_get);
GENERATE_METHOD_CALLBACKS(CElmContainer, content_set);

GENERATE_TEMPLATE_FULL(CElmObject, CElmContainer,
                       METHOD(content_get),
                       METHOD(content_set));

void CElmContainer::init()
{
   contents = Persistent<Object>::New(Object::New());
}

CElmContainer::CElmContainer(Local<Object> _jsObject, Evas_Object *child)
   : CElmObject(_jsObject, child)
{
   init();
}

CElmContainer::CElmContainer(Local<Object> _jsObject, CElmObject* )
   : CElmObject(_jsObject, NULL)
{
   init();
}

CElmContainer::~CElmContainer()
{
   contents.Dispose();
}

Handle<Value> CElmContainer::content_set(const Arguments &args)
{
   if (!args[0]->IsString())
     return Undefined();

   if (args[1]->IsUndefined() || args[1]->IsNull())
     {
        contents->Delete(args[0]->ToString());
        elm_object_part_content_unset(eo, *String::Utf8Value(args[0]));
     }
   else if (args[1]->IsObject())
     {
        contents->Set(args[0]->ToString(), Realise(args[1], jsObject));
        elm_object_part_content_set(eo, *String::Utf8Value(args[0]),
                                    GetEvasObjectFromJavascript(
                                      contents->Get(args[0]->ToString())));
     }

   return Undefined();
}

Handle<Value> CElmContainer::content_get(const Arguments &args)
{
   if (args[0]->IsString())
     return contents->Get(args[0]->ToString());

   return Undefined();
}

}

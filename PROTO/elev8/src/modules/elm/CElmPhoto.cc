#include "CElmPhoto.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmPhoto, size);
GENERATE_PROPERTY_CALLBACKS(CElmPhoto, fill);

GENERATE_TEMPLATE(CElmPhoto,
                  PROPERTY(size),
                  PROPERTY(fill));

CElmPhoto::CElmPhoto(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_photo_add(parent->GetEvasObject()))
{
}

void CElmPhoto::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Photo"), GetTemplate()->GetFunction());
}

Handle <Value> CElmPhoto::image_get() const
{
   //No getter available
   return Undefined();
}

void CElmPhoto::image_set(Handle <Value> val)
{
   if (val->IsString())
     return;

   String::Utf8Value str(val);

   if (0 > access(*str, R_OK))
     ELM_ERR("warning: can't read image file %s", *str);

   if (elm_photo_file_set(eo, *str))
     ELM_ERR("Unable to set the image");
}

Handle <Value> CElmPhoto::size_get() const
{
   //No getter available
   return Undefined();
}

void CElmPhoto::size_set(Handle <Value> val)
{
   if (val->IsNumber())
     elm_photo_size_set(eo, val->ToInt32()->Value());
}

Handle <Value> CElmPhoto::fill_get() const
{
   //No getter available
   return Undefined();
}

void CElmPhoto::fill_set(Handle <Value> val)
{
   if (val->IsBoolean())
     elm_photo_fill_inside_set(eo, val->BooleanValue());
}

}

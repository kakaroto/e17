#include "CElmPhoto.h"

CElmPhoto::CElmPhoto(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
    , prop_handler(property_list_base)
{
    eo = elm_photo_add(parent->top_widget_get());
    construct(eo, obj);
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

    Eina_Bool retval = elm_photo_file_set(eo, *str);
    if (retval == EINA_FALSE)
        ELM_ERR("Unable to set the image");
}

Handle <Value> CElmPhoto::size_get() const
{
    //No getter available
    return Undefined();
}

void CElmPhoto::size_set(Handle <Value> val)
{
    if (val->IsNumber()) {
        int size = val->ToInt32()->Value();
        elm_photo_size_set(eo, size);
    }
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

PROPERTIES_OF(CElmPhoto) = {
    PROP_HANDLER(CElmPhoto, size),
    PROP_HANDLER(CElmPhoto, fill),
    { NULL }
};

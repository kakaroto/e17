#include "CElmFlip.h"

CElmFlip::CElmFlip(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
{
    CEvasObject *front, *back;

    eo = elm_flip_add(parent->get());
    construct(eo, obj);

    /* realize front and back */
    front = make_or_get(this, obj->Get(String::New("front")));
    elm_object_part_content_set(eo, "front", front->get());

    back = make_or_get(this, obj->Get(String::New("back")));
    elm_object_part_content_set(eo, "back", back->get());

    get_object()->Set(String::New("flip"),
              FunctionTemplate::New(do_flip)->GetFunction());
}

Handle <Value> CElmFlip::do_flip(const Arguments & args)
{
    CEvasObject *self = eo_from_info(args.This());
    CElmFlip *flipper = static_cast <CElmFlip *>(self);
    flipper->flip(ELM_FLIP_ROTATE_Y_CENTER_AXIS);
    return Undefined();
}

void CElmFlip::flip(Elm_Flip_Mode mode)
{
    elm_flip_go(eo, mode);
}

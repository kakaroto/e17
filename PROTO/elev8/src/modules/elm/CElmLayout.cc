#include "CElmLayout.h"

CElmLayout::CElmLayout(CEvasObject * parent, Local <Object> obj)
  : CEvasObject()
  , prop_handler(property_list_base)
{
    the_contents = Persistent <Object>::New(Object::New());
    eo = elm_layout_add(parent->top_widget_get());
    construct(eo, obj);
}

Handle <Value> CElmLayout::contents_get() const
{
    return the_contents;
}

void CElmLayout::contents_set(Handle <Value> val)
{
    if (val->IsObject()) {
        Handle <Object> contents = val->ToObject();
        Handle <Array> properties = contents->GetPropertyNames();

        for (unsigned int i = 0; i <properties->Length(); i++) {
            Handle <Value> element = properties->Get(Integer::New(i));

            CEvasObject *child = make_or_get(this, contents->Get(element->ToString()));
            if (!child)
                continue;

            String::Utf8Value elementName(element);
            elm_object_part_content_set(eo, *elementName,
                            child->get());

            the_contents->Set(element, child->get_object());
        }
    }
}

Handle <Value> CElmLayout::file_get() const
{
    // FIXME: implement
    return Undefined();
}

void CElmLayout::file_set(Handle <Value> val)
{
    if (val->IsObject()) {
        Local <Object> obj = val->ToObject();
        Local <Value> fileParam = obj->Get(String::New("name"));
        Local <Value> groupParam = obj->Get(String::New("group"));

        String::Utf8Value fileName(fileParam);
        String::Utf8Value groupName(groupParam);

        elm_layout_file_set(eo, *fileName, *groupName);
    }
}

Handle <Value> CElmLayout::theme_get() const
{
    // FIXME: implement
    return Undefined();
}

void CElmLayout::theme_set(Handle <Value> val)
{
    if (val->IsObject()) {
        Local <Object> obj = val->ToObject();
        Local <Value> classParam = obj->Get(String::New("class"));
        Local <Value> groupParam = obj->Get(String::New("group"));
        Local <Value> styleParam = obj->Get(String::New("style"));

        String::Utf8Value className(classParam);
        String::Utf8Value groupName(groupParam);
        String::Utf8Value styleName(styleParam);

        elm_layout_theme_set(eo, *className, *groupName, *styleName);
    }
}

PROPERTIES_OF(CElmLayout) = {
    PROP_HANDLER(CElmLayout, file),
    PROP_HANDLER(CElmLayout, theme),
    PROP_HANDLER(CElmLayout, contents),
    { NULL }
};

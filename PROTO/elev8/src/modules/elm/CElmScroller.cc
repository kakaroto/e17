#include "CElmScroller.h"

CElmScroller::CElmScroller(CEvasObject * parent, Local <Object> obj)
    : CEvasObject()
    , prop_handler(property_list_base)
{
    CEvasObject *content;
    eo = elm_scroller_add(parent->top_widget_get());
    construct(eo, obj);
    content = make_or_get(this, obj->Get(String::New("content")));
    if (!content) {
        ELM_ERR("scroller has no content");
    } else {
        // FIXME: filter the object list copied in construct for more efficiency
        get_object()->Set(String::New("content"),
                  content->get_object());
        elm_object_content_set(eo, content->get());
    }
}

void CElmScroller::bounce_set(Handle <Value> val)
{
    bool x_bounce = false, y_bounce = false;

    if (get_xy_from_object(val, x_bounce, y_bounce))
        elm_scroller_bounce_set(eo, x_bounce, y_bounce);
}

Handle <Value> CElmScroller::bounce_get() const
{
    Eina_Bool x, y;
    elm_scroller_bounce_get(eo, &x, &y);
    Local <Object> obj = Object::New();
    obj->Set(String::New("x"), Boolean::New(x));
    obj->Set(String::New("y"), Boolean::New(y));
    return obj;
}

Elm_Scroller_Policy CElmScroller::policy_from_string(Handle <Value> val)
{
    String::Utf8Value str(val);
    Elm_Scroller_Policy policy = ELM_SCROLLER_POLICY_AUTO;

    if (!strcmp(*str, "auto"))
        policy = ELM_SCROLLER_POLICY_AUTO;
    else if (!strcmp(*str, "on"))
        policy = ELM_SCROLLER_POLICY_ON;
    else if (!strcmp(*str, "off"))
        policy = ELM_SCROLLER_POLICY_OFF;
    else if (!strcmp(*str, "last"))
        policy = ELM_SCROLLER_POLICY_LAST;
    else
        ELM_ERR("unknown scroller policy %s", *str);

    return policy;
}

Local <Value> CElmScroller::string_from_policy(Elm_Scroller_Policy policy)
{
    switch (policy) {
    case ELM_SCROLLER_POLICY_AUTO:
        return String::New("auto");
    case ELM_SCROLLER_POLICY_ON:
        return String::New("on");
    case ELM_SCROLLER_POLICY_OFF:
        return String::New("off");
    case ELM_SCROLLER_POLICY_LAST:
        return String::New("last");
    default:
        return String::New("unknown");
    }
}

void CElmScroller::policy_set(Handle <Value> val)
{
    Local <Value> x_val, y_val;

    if (get_xy_from_object(val, x_val, y_val)) {
        Elm_Scroller_Policy x_policy, y_policy;
        x_policy = policy_from_string(x_val);
        y_policy = policy_from_string(y_val);
        elm_scroller_policy_set(eo, x_policy, y_policy);
    }
}

Handle <Value> CElmScroller::policy_get() const
{
    Elm_Scroller_Policy x_policy, y_policy;
    elm_scroller_policy_get(eo, &x_policy, &y_policy);
    Local <Object> obj = Object::New();

    obj->Set(String::New("x"), string_from_policy(x_policy));
    obj->Set(String::New("y"), string_from_policy(y_policy));

    return obj;
}

template<> CEvasObject::CPropHandler<CElmScroller>::property_list
CEvasObject::CPropHandler<CElmScroller>::list[] = {
     PROP_HANDLER(CElmScroller, bounce),
     PROP_HANDLER(CElmScroller, policy),
};

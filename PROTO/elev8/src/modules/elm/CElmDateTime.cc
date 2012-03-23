#include "CElmDateTime.h"

CElmDateTime::CElmDateTime(CEvasObject *parent, Local<Object> obj) :
   CEvasObject(),
   prop_handler(property_list_base)
{
   eo = elm_datetime_add(parent->top_widget_get());
   construct(eo, obj);
}

Handle<Value> CElmDateTime::format_get() const
{
   return (String::New(elm_datetime_format_get(eo)));
}

void CElmDateTime::format_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        elm_datetime_format_set(eo, *str);
     }
}

Local<Object> CElmDateTime::fill_tm(struct tm *timevar) const
{
   Local<Object> obj = Object::New();
   obj->Set(String::New("sec"), Number::New(timevar->tm_sec));
   obj->Set(String::New("min"), Number::New(timevar->tm_min));
   obj->Set(String::New("hour"), Number::New(timevar->tm_hour));
   obj->Set(String::New("dayofmonth"), Number::New(timevar->tm_mday));
   obj->Set(String::New("month"), Number::New(timevar->tm_mon));
   obj->Set(String::New("year"), Number::New(timevar->tm_year));
   obj->Set(String::New("dayofweek"), Number::New(timevar->tm_wday));
   obj->Set(String::New("daysinyear"), Number::New(timevar->tm_yday));
   obj->Set(String::New("dst"), Number::New(timevar->tm_isdst));
   return obj;
}

struct tm * CElmDateTime::get_tm(Handle<Value> val) const
{
   if (val->IsObject())
     {
        tm *var = new tm();
        Local<Object> obj = val->ToObject();
        Handle<Value> temp = obj->Get(String::New("sec"));
        var->tm_sec = temp->NumberValue();
        temp = obj->Get(String::New("min"));
        var->tm_min = temp->NumberValue();
        temp = obj->Get(String::New("hour"));
        var->tm_hour = temp->NumberValue();
        temp = obj->Get(String::New("dayofmonth"));
        var->tm_mday = temp->NumberValue();
        temp = obj->Get(String::New("month"));
        var->tm_mon = temp->NumberValue();
        temp = obj->Get(String::New("year"));
        var->tm_year = temp->NumberValue();
        temp = obj->Get(String::New("dayofweek"));
        var->tm_wday = temp->NumberValue();
        temp = obj->Get(String::New("daysinyear"));
        var->tm_yday = temp->NumberValue();
        temp = obj->Get(String::New("dst"));
        var->tm_isdst = temp->NumberValue();
        return var;
     }
   return NULL;
}

bool CElmDateTime::get_min_max_from_object(Handle<Value> val, int &min_out, int &max_out) const
{
   HandleScope handle_scope;
   if (!val->IsObject())
     return false;
   Local<Object> obj = val->ToObject();
   Local<Value> min = obj->Get(String::New("min"));
   Local<Value> max = obj->Get(String::New("max"));
   if (!min->IsNumber() || !max->IsNumber())
     return false;
   min_out = min->NumberValue();
   max_out = max->NumberValue();
   return true;
}

void CElmDateTime::field_limit_set(Handle<Value> val)
{
   int min, max;
   Local<Object> obj = val->ToObject();
   Handle<Value> temp = obj->Get(String::New("ampm"));
   if (temp->IsObject())
     {
        if (get_min_max_from_object(temp, min, max))
          elm_datetime_field_limit_set(eo, ELM_DATETIME_AMPM, min, max);
     }

   temp = obj->Get(String::New("mon"));
   if (temp->IsObject())
     {
        if (get_min_max_from_object(temp, min, max))
          elm_datetime_field_limit_set(eo, ELM_DATETIME_MONTH, min, max);
     }

   temp = obj->Get(String::New("year"));
   if (temp->IsObject())
     {
        if (get_min_max_from_object(temp, min, max))
          elm_datetime_field_limit_set(eo, ELM_DATETIME_YEAR, min, max);
     }

   temp = obj->Get(String::New("date"));
   if (temp->IsObject())
     {
        if (get_min_max_from_object(temp, min, max))
          elm_datetime_field_limit_set(eo, ELM_DATETIME_DATE, min, max);
     }

   temp = obj->Get(String::New("hour"));
   if (temp->IsObject())
     {
        if (get_min_max_from_object(temp, min, max))
          elm_datetime_field_limit_set(eo, ELM_DATETIME_HOUR, min, max);
     }

   temp = obj->Get(String::New("min"));
   if (temp->IsObject())
     {
        if (get_min_max_from_object(temp, min, max))
          elm_datetime_field_limit_set(eo, ELM_DATETIME_MINUTE, min, max);
     }
}

Local<Object> CElmDateTime::get_field_limits(Elm_Datetime_Field_Type type) const
{
   int min = 0, max = 0;
   elm_datetime_field_limit_get(eo, type, &min, &max);
   Local<Object> obj = Object::New();
   obj->Set(String::New("min"), Number::New(min));
   obj->Set(String::New("max"), Number::New(max));
   return obj;
}

Handle<Value> CElmDateTime::field_limit_get() const
{
   Local<Object> obj = Object::New();
   obj->Set(String::New("ampm"), get_field_limits(ELM_DATETIME_AMPM));
   obj->Set(String::New("min"), get_field_limits(ELM_DATETIME_MINUTE));
   obj->Set(String::New("hour"), get_field_limits(ELM_DATETIME_HOUR));
   obj->Set(String::New("date"), get_field_limits(ELM_DATETIME_DATE));
   obj->Set(String::New("month"), get_field_limits(ELM_DATETIME_MONTH));
   obj->Set(String::New("year"), get_field_limits(ELM_DATETIME_YEAR));
   return obj;
}

void CElmDateTime::field_visible_set(Handle<Value> val)
{
   Local<Object> obj = val->ToObject();
   Local<Value> temp = obj->Get(String::New("ampm"));
   if (temp->IsBoolean())
     {
        elm_datetime_field_visible_set(eo, ELM_DATETIME_AMPM, temp->BooleanValue());
     }

   temp = obj->Get(String::New("mon"));
   if (temp->IsBoolean())
     {
        elm_datetime_field_visible_set(eo, ELM_DATETIME_MONTH, temp->BooleanValue());
     }


   temp = obj->Get(String::New("year"));
   if (temp->IsBoolean())
     {
        elm_datetime_field_visible_set(eo, ELM_DATETIME_YEAR, temp->BooleanValue());
     }


   temp = obj->Get(String::New("date"));
   if (temp->IsBoolean())
     {
        elm_datetime_field_visible_set(eo, ELM_DATETIME_DATE, temp->BooleanValue());
     }


   temp = obj->Get(String::New("hour"));
   if (temp->IsBoolean())
     {
        elm_datetime_field_visible_set(eo, ELM_DATETIME_HOUR, temp->BooleanValue());
     }

   temp = obj->Get(String::New("minute"));
   if (temp->IsBoolean())
     {
        elm_datetime_field_visible_set(eo, ELM_DATETIME_MINUTE, temp->BooleanValue());
     }
}

Handle<Value> CElmDateTime::field_visible_get() const
{
   Local<Object> obj = Object::New();
   obj->Set(String::New("ampm"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_AMPM)));
   obj->Set(String::New("minute"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_MINUTE)));
   obj->Set(String::New("hour"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_HOUR)));
   obj->Set(String::New("date"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_DATE)));
   obj->Set(String::New("month"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_MONTH)));
   obj->Set(String::New("year"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_YEAR)));
   return obj;
}

Handle<Value> CElmDateTime::value_max_get() const
{
   struct tm timevar;
   elm_datetime_value_max_get(eo, &timevar);
   return fill_tm(&timevar);
}

void CElmDateTime::value_max_set(Handle<Value> timevar)
{
   struct tm *retval = get_tm(timevar);

   if (retval)
     elm_datetime_value_max_set(eo, retval);
}

Handle<Value> CElmDateTime::value_min_get() const
{
   struct tm timevar;
   elm_datetime_value_min_get(eo, &timevar);
   return fill_tm(&timevar);
}

void CElmDateTime::value_min_set(Handle<Value> timevar)
{
   struct tm *retval = get_tm(timevar);

   if (retval)
     elm_datetime_value_min_set(eo, retval);
}

Handle<Value> CElmDateTime::value_get() const
{
   struct tm timevar;
   elm_datetime_value_get(eo, &timevar);
   return fill_tm(&timevar);
}

void CElmDateTime::value_set(Handle<Value> timevar)
{
   struct tm *retval = get_tm(timevar);

   if (retval)
     elm_datetime_value_set(eo, retval);
}

void CElmDateTime::on_changed(void *)
{
   HandleScope handle_scope;
   Handle<Object> obj = get_object();
   Handle<Value> val = on_changed_val;
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[1] = { obj };
   fn->Call(obj, 1, args);
}

void CElmDateTime::eo_on_changed(void *data, Evas_Object *, void *event_info)
{
   CElmDateTime *changed = static_cast<CElmDateTime*>(data);
   changed->on_changed(event_info);
}

void CElmDateTime::on_changed_set(Handle<Value> val)
{
   on_changed_val.Dispose();
   on_changed_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
   else
     evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
}

Handle<Value> CElmDateTime::on_changed_get(void) const
{
   return on_changed_val;
}

void CElmDateTime::on_lang_changed(void *)
{
   HandleScope handle_scope;
   Handle<Object> obj = get_object();
   Handle<Value> val = on_lang_changed_val;
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[1] = { obj };
   fn->Call(obj, 1, args);
}

void CElmDateTime::eo_on_lang_changed(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmDateTime*>(data)->on_lang_changed(event_info);
}

void CElmDateTime::on_lang_changed_set(Handle<Value> val)
{
   on_lang_changed_val.Dispose();

   on_lang_changed_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "language,changed", &eo_on_lang_changed, this);
   else
     evas_object_smart_callback_del(eo, "language,changed", &eo_on_lang_changed);
}

Handle<Value> CElmDateTime::on_lang_changed_get(void) const
{
   return on_lang_changed_val;
}


PROPERTIES_OF(CElmDateTime) = {
   PROP_HANDLER(CElmDateTime, format),
   PROP_HANDLER(CElmDateTime, value_max),
   PROP_HANDLER(CElmDateTime, value_min),
   PROP_HANDLER(CElmDateTime, value),
   PROP_HANDLER(CElmDateTime, field_limit),
   PROP_HANDLER(CElmDateTime, field_visible),
   { NULL },
};

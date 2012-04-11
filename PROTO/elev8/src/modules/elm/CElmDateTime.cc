#include "elm.h"
#include "CElmDateTime.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmDateTime, format);
GENERATE_PROPERTY_CALLBACKS(CElmDateTime, value_max);
GENERATE_PROPERTY_CALLBACKS(CElmDateTime, value_min);
GENERATE_PROPERTY_CALLBACKS(CElmDateTime, value);
GENERATE_PROPERTY_CALLBACKS(CElmDateTime, field_limit);
GENERATE_PROPERTY_CALLBACKS(CElmDateTime, field_visible);
GENERATE_PROPERTY_CALLBACKS(CElmDateTime, on_change);
GENERATE_PROPERTY_CALLBACKS(CElmDateTime, on_lang_change);

GENERATE_TEMPLATE(CElmDateTime,
                  PROPERTY(format),
                  PROPERTY(value_max),
                  PROPERTY(value_min),
                  PROPERTY(value),
                  PROPERTY(field_limit),
                  PROPERTY(field_visible),
                  PROPERTY(on_change),
                  PROPERTY(on_lang_change));

CElmDateTime::CElmDateTime(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_datetime_add(parent->GetEvasObject()))
{
}

void CElmDateTime::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("DateTime"), GetTemplate()->GetFunction());
}

CElmDateTime::~CElmDateTime()
{
   on_change_set(Undefined());
   on_lang_change_set(Undefined());
}

void CElmDateTime::format_set(Handle<Value> val)
{
   if (val->IsString())
     elm_datetime_format_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmDateTime::format_get() const
{
   return (String::NewSymbol(elm_datetime_format_get(eo)));
}

Handle<Value> CElmDateTime::value_max_get() const
{
   struct tm timeval;
   elm_datetime_value_max_get(eo, &timeval);
   return TimeGet(&timeval);
}

void CElmDateTime::value_max_set(Handle<Value> timevar)
{
   struct tm retval;
   if (TimeSet(&retval, timevar))
     elm_datetime_value_max_set(eo, &retval);
}

Handle<Value> CElmDateTime::value_min_get() const
{
   struct tm timeval;
   elm_datetime_value_min_get(eo, &timeval);
   return TimeGet(&timeval);
}

void CElmDateTime::value_min_set(Handle<Value> timevar)
{
   struct tm timeval;
   if (TimeSet(&timeval, timevar))
     elm_datetime_value_min_set(eo, &timeval);
}

Handle<Value> CElmDateTime::value_get() const
{
   struct tm timeval;
   elm_datetime_value_get(eo, &timeval);
   return TimeGet(&timeval);
}

void CElmDateTime::value_set(Handle<Value> timevar)
{
   struct tm timeval;
   if (TimeSet(&timeval, timevar))
     elm_datetime_value_set(eo, &timeval);
}

void CElmDateTime::field_limit_set(Handle<Value> val)
{
   int min, max;
   Local<Object> obj = val->ToObject();
   Handle<Value> temp = obj->Get(String::NewSymbol("ampm"));

   if (temp->IsObject() && GetMinMaxFromObject(temp, min, max))
     elm_datetime_field_limit_set(eo, ELM_DATETIME_AMPM, min, max);

   temp = obj->Get(String::NewSymbol("month"));
   if (temp->IsObject() && GetMinMaxFromObject(temp, min, max))
     elm_datetime_field_limit_set(eo, ELM_DATETIME_MONTH, min, max);

   temp = obj->Get(String::NewSymbol("year"));
   if (temp->IsObject() && GetMinMaxFromObject(temp, min, max))
     elm_datetime_field_limit_set(eo, ELM_DATETIME_YEAR, min - 1900, max - 1900);

   temp = obj->Get(String::NewSymbol("date"));
   if (temp->IsObject() && GetMinMaxFromObject(temp, min, max))
     elm_datetime_field_limit_set(eo, ELM_DATETIME_DATE, min, max);

   temp = obj->Get(String::NewSymbol("hour"));
   if (temp->IsObject() && GetMinMaxFromObject(temp, min, max))
     elm_datetime_field_limit_set(eo, ELM_DATETIME_HOUR, min, max);

   temp = obj->Get(String::NewSymbol("min"));
   if (temp->IsObject() && GetMinMaxFromObject(temp, min, max))
     elm_datetime_field_limit_set(eo, ELM_DATETIME_MINUTE, min, max);
}

Handle<Value> CElmDateTime::field_limit_get() const
{
   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("ampm"), GetFieldLimits(ELM_DATETIME_AMPM));
   obj->Set(String::NewSymbol("min"), GetFieldLimits(ELM_DATETIME_MINUTE));
   obj->Set(String::NewSymbol("hour"), GetFieldLimits(ELM_DATETIME_HOUR));
   obj->Set(String::NewSymbol("date"), GetFieldLimits(ELM_DATETIME_DATE));
   obj->Set(String::NewSymbol("month"), GetFieldLimits(ELM_DATETIME_MONTH));
   obj->Set(String::NewSymbol("year"), GetFieldLimits(ELM_DATETIME_YEAR));
   return obj;
}

Handle<Value> CElmDateTime::field_visible_get() const
{
   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("ampm"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_AMPM)));
   obj->Set(String::NewSymbol("minute"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_MINUTE)));
   obj->Set(String::NewSymbol("hour"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_HOUR)));
   obj->Set(String::NewSymbol("date"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_DATE)));
   obj->Set(String::NewSymbol("month"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_MONTH)));
   obj->Set(String::NewSymbol("year"), Boolean::New(elm_datetime_field_visible_get(eo, ELM_DATETIME_YEAR)));
   return obj;
}

void CElmDateTime::field_visible_set(Handle<Value> val)
{
   Local<Object> obj = val->ToObject();
   Local<Value> temp = obj->Get(String::NewSymbol("ampm"));

   if (temp->IsBoolean())
     elm_datetime_field_visible_set(eo, ELM_DATETIME_AMPM, temp->BooleanValue());

   temp = obj->Get(String::NewSymbol("month"));
   if (temp->IsBoolean())
     elm_datetime_field_visible_set(eo, ELM_DATETIME_MONTH, temp->BooleanValue());

   temp = obj->Get(String::NewSymbol("year"));
   if (temp->IsBoolean())
     elm_datetime_field_visible_set(eo, ELM_DATETIME_YEAR, temp->BooleanValue());

   temp = obj->Get(String::NewSymbol("date"));
   if (temp->IsBoolean())
     elm_datetime_field_visible_set(eo, ELM_DATETIME_DATE, temp->BooleanValue());

   temp = obj->Get(String::NewSymbol("hour"));
   if (temp->IsBoolean())
     elm_datetime_field_visible_set(eo, ELM_DATETIME_HOUR, temp->BooleanValue());

   temp = obj->Get(String::NewSymbol("minute"));
   if (temp->IsBoolean())
     elm_datetime_field_visible_set(eo, ELM_DATETIME_MINUTE, temp->BooleanValue());

}

void CElmDateTime::OnChange(void *)
{
   HandleScope scope;
   Local<Function> callback(Function::Cast(*cb.change));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmDateTime::OnChangeWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmDateTime*>(data)->OnChange(event_info);
}

void CElmDateTime::on_change_set(Handle<Value> val)
{
   if (!cb.change.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "changed", &OnChangeWrapper);
        cb.change.Dispose();
        cb.change.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.change = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "changed", &OnChangeWrapper, this);
}

Handle<Value> CElmDateTime::on_change_get(void) const
{
   return cb.change;
}

void CElmDateTime::OnLangChange(void *)
{
   HandleScope scope;
   Local<Function> callback(Function::Cast(*cb.lang_change));
   Handle<Value> args[1] = { jsObject };
   callback->Call(jsObject, 1, args);
}

void CElmDateTime::OnLangChangeWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmDateTime*>(data)->OnLangChange(event_info);
}

void CElmDateTime::on_lang_change_set(Handle<Value> val)
{
   if (cb.change.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "language,changed", &OnLangChangeWrapper);
        cb.change.Dispose();
        cb.change.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.change = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "language,changed", &OnLangChangeWrapper, this);
}

Handle<Value> CElmDateTime::on_lang_change_get(void) const
{
   return cb.lang_change;
}

bool CElmDateTime::TimeSet(struct tm *time, Handle<Value> val) const
{
   if (!val->IsObject())
     return false;

   Local<Object> obj = val->ToObject();

   Handle<Value> temp = obj->Get(String::NewSymbol("sec"));
   time->tm_sec = temp->Int32Value();
   temp = obj->Get(String::NewSymbol("min"));
   time->tm_min = temp->Int32Value();
   temp = obj->Get(String::NewSymbol("hour"));
   time->tm_hour = temp->Int32Value();
   temp = obj->Get(String::NewSymbol("dayofmonth"));
   time->tm_mday = temp->Int32Value();
   temp = obj->Get(String::NewSymbol("month"));
   time->tm_mon = temp->Int32Value();
   temp = obj->Get(String::NewSymbol("year"));
   time->tm_year = temp->Int32Value() - 1900;
   temp = obj->Get(String::NewSymbol("dayofweek"));
   time->tm_wday = temp->Int32Value();
   temp = obj->Get(String::NewSymbol("daysinyear"));
   time->tm_yday = temp->Int32Value();
   temp = obj->Get(String::NewSymbol("dst"));
   time->tm_isdst = temp->Int32Value();

   return true;
}

Local<Object> CElmDateTime::TimeGet(struct tm *time) const
{
   Local<Object> obj = Object::New();

   obj->Set(String::NewSymbol("sec"), Int32::New(time->tm_sec));
   obj->Set(String::NewSymbol("min"), Int32::New(time->tm_min));
   obj->Set(String::NewSymbol("hour"), Int32::New(time->tm_hour));
   obj->Set(String::NewSymbol("dayofmonth"), Int32::New(time->tm_mday));
   obj->Set(String::NewSymbol("month"), Int32::New(time->tm_mon));
   obj->Set(String::NewSymbol("year"), Int32::New(time->tm_year + 1900));
   obj->Set(String::NewSymbol("dayofweek"), Int32::New(time->tm_wday));
   obj->Set(String::NewSymbol("daysinyear"), Int32::New(time->tm_yday));
   obj->Set(String::NewSymbol("dst"), Int32::New(time->tm_isdst));

   return obj;
}

Local<Object> CElmDateTime::GetFieldLimits(Elm_Datetime_Field_Type type) const
{
   int min, max;
   Local<Object> obj = Object::New();

   elm_datetime_field_limit_get(eo, type, &min, &max);
   if (type == ELM_DATETIME_YEAR)
     {
        min += 1900;
        max += 1900;
     }

   obj->Set(String::NewSymbol("min"), Int32::New(min));
   obj->Set(String::NewSymbol("max"), Int32::New(max));

   return obj;
}

bool CElmDateTime::GetMinMaxFromObject(Handle<Value> val, int &min_out, int &max_out) const
{
   if (!val->IsObject())
     return false;

   Local<Object> obj = val->ToObject();
   Local<Value> min = obj->Get(String::NewSymbol("min"));
   Local<Value> max = obj->Get(String::NewSymbol("max"));

   if (!min->IsNumber() || !max->IsNumber())
     return false;

   min_out = min->Int32Value();
   max_out = max->Int32Value();

   return true;
}

}

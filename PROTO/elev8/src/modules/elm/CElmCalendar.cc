#include "elm.h"
#include "CElmCalendar.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmCalendar, weekday_names);
GENERATE_PROPERTY_CALLBACKS(CElmCalendar, min_year);
GENERATE_PROPERTY_CALLBACKS(CElmCalendar, max_year);
GENERATE_PROPERTY_CALLBACKS(CElmCalendar, select_mode);
GENERATE_PROPERTY_CALLBACKS(CElmCalendar, day);
GENERATE_PROPERTY_CALLBACKS(CElmCalendar, month);
GENERATE_PROPERTY_CALLBACKS(CElmCalendar, year);
GENERATE_PROPERTY_CALLBACKS(CElmCalendar, interval);
GENERATE_PROPERTY_CALLBACKS(CElmCalendar, marks);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmCalendar,
                  PROPERTY(weekday_names),
                  PROPERTY(min_year),
                  PROPERTY(max_year),
                  PROPERTY(select_mode),
                  PROPERTY(day),
                  PROPERTY(month),
                  PROPERTY(year),
                  PROPERTY(interval),
                  PROPERTY(marks));

CElmCalendar::CElmCalendar(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_calendar_add(_parent->GetEvasObject()))
{
}

CElmCalendar::~CElmCalendar()
{
   cached.marks.Dispose();
   cb.change.Dispose();
}

void CElmCalendar::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Calendar"), GetTemplate()->GetFunction());
}

void CElmCalendar::marks_set(Handle<Value> val)
{
   Local<Object> marks = val->ToObject();
   Local<Array> props = marks->GetOwnPropertyNames();

   /* iterate through elements and instantiate them */
   for (unsigned int i = 0; i < props->Length(); i++)
     {
        struct tm mark_time;

        Local<Value> x = props->Get(i);
        Local<Value> item = marks->Get(x->ToString());

        if (!item->IsObject())
          {
             ELM_ERR("mark %s is not an object", *String::Utf8Value(x->ToString()));
             continue;
          }

        Local<Object> mark = item->ToObject();

        Local<Value> type = mark->Get(String::NewSymbol("type"));
        String::Utf8Value mark_type(type);

        Local<Value> day = mark->Get(String::NewSymbol("day"));
        mark_time.tm_mday = day->ToNumber()->Value();

        Local<Value> month = mark->Get(String::NewSymbol("month"));
        mark_time.tm_mon = month->ToNumber()->Value() - 1;

        Local<Value> year = mark->Get(String::NewSymbol("year"));
        mark_time.tm_year = year->ToNumber()->Value() - 1900;

        Local<Value> repeat = mark->Get(String::NewSymbol("repeat"));
        String::Utf8Value mark_repeat(repeat);

        Elm_Calendar_Mark_Repeat_Type intRepeat;

        if (!strcmp(*mark_repeat, "annually"))
          intRepeat = ELM_CALENDAR_ANNUALLY;
        else if (!strcmp(*mark_repeat, "monthly"))
          intRepeat = ELM_CALENDAR_MONTHLY;
        else if (!strcmp(*mark_repeat, "weekly"))
          intRepeat = ELM_CALENDAR_WEEKLY;
        else if (!strcmp(*mark_repeat, "daily"))
          intRepeat = ELM_CALENDAR_DAILY;
        else
          intRepeat = ELM_CALENDAR_UNIQUE;

        elm_calendar_mark_add(eo, *mark_type, &mark_time, intRepeat);
     }

   elm_calendar_marks_draw(eo);
   cached.marks = Persistent<Object>::New(marks);
}

Handle<Value> CElmCalendar::marks_get() const
{
   return cached.marks;
}

void CElmCalendar::OnChange()
{
   HandleScope scope;
   Handle<Function> callback(Function::Cast(*cb.change));
   Handle<Value> arguments[1] = { jsObject };
   callback->Call(jsObject, 1, arguments);
}

void CElmCalendar::OnChangeWrapper(void *data, Evas_Object *, void *)
{
   static_cast<CElmCalendar*>(data)->OnChange();
}

void CElmCalendar::on_change_set(Handle<Value> val)
{
   cb.change.Dispose();
   cb.change = Persistent<Value>::New(val);

   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "changed", &OnChangeWrapper, this);
   else
     evas_object_smart_callback_del(eo, "changed", &OnChangeWrapper);
}

Handle<Value> CElmCalendar::on_change_get(void) const
{
   return cb.change;
}

Handle<Value> CElmCalendar::weekday_names_get(void) const
{
   Handle<Array> obj = Array::New(7);
   const char **weekdays = elm_calendar_weekdays_names_get(eo);
   for (int i = 0; i < 7; i++)
     obj->Set(i, String::New(weekdays[i]));

   return obj;
}

void CElmCalendar::weekday_names_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   char *weekdays[7];
   Local<Object> weekdaysArray = val->ToObject();

   for (int i = 0; i < 7 ; i++)
     {
        Local<Value> weekday = weekdaysArray->Get(i);
        weekdays[i] = (weekday->IsString() || weekday->IsNumber()) ?
             strdup(*String::Utf8Value(weekday)) : NULL; }

   elm_calendar_weekdays_names_set(eo, (const char**) weekdays);

   for (int i = 0; i < 7; i++)
     free(weekdays[i]);
}

Handle<Value> CElmCalendar::min_year_get(void) const
{
   int year_min;
   elm_calendar_min_max_year_get(eo, &year_min, NULL);
   return Number::New(year_min);
}

void CElmCalendar::min_year_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int year_max;
   elm_calendar_min_max_year_get(eo, NULL, &year_max);
   elm_calendar_min_max_year_set(eo, val->ToNumber()->Value(), year_max);
}

Handle<Value> CElmCalendar::max_year_get(void) const
{
   int year_max;
   elm_calendar_min_max_year_get(eo, NULL, &year_max);
   return Number::New(year_max);
}

void CElmCalendar::max_year_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   int year_min;
   elm_calendar_min_max_year_get(eo, &year_min, NULL);
   elm_calendar_min_max_year_set(eo, year_min, val->ToNumber()->Value());
}

Handle<Value> CElmCalendar::select_mode_get(void) const
{
   return Number::New(elm_calendar_select_mode_get(eo));
}

void CElmCalendar::select_mode_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_calendar_select_mode_set(eo, (Elm_Calendar_Select_Mode)val->ToNumber()->Value());
}

Handle<Value> CElmCalendar::day_get(void) const
{
   struct tm selected_time;
   elm_calendar_selected_time_get(eo,&selected_time);

   return Number::New(selected_time.tm_mday);
}

void CElmCalendar::day_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   struct tm selected_time;
   elm_calendar_selected_time_get(eo, &selected_time);
   selected_time.tm_mday = val->ToNumber()->Value();
   elm_calendar_selected_time_set(eo, &selected_time);
}

Handle<Value> CElmCalendar::month_get(void) const
{
   struct tm selected_time;
   elm_calendar_selected_time_get(eo, &selected_time);
   return Number::New(selected_time.tm_mon);
}

void CElmCalendar::month_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   struct tm selected_time;
   elm_calendar_selected_time_get(eo, &selected_time);
   selected_time.tm_mon = val->ToNumber()->Value();

   //tm_mon is zero based - but hide that from user.
   //let them give a normal number
   selected_time.tm_mon = selected_time.tm_mon - 1;
   elm_calendar_selected_time_set(eo, &selected_time);
}

Handle<Value> CElmCalendar::year_get(void) const
{
   struct tm selected_time;
   elm_calendar_selected_time_get(eo,&selected_time);
   return Number::New(selected_time.tm_year);
}

void CElmCalendar::year_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;

   struct tm selected_time;
   elm_calendar_selected_time_get(eo, &selected_time);
   selected_time.tm_year = val->ToNumber()->Value();

   //tm_year is years since 1900 - but hide that from user.
   //let them give a normal year
   selected_time.tm_year = selected_time.tm_year - 1900;
   elm_calendar_selected_time_set(eo, &selected_time);
}

Handle<Value> CElmCalendar::interval_get(void) const
{
   return Number::New(elm_calendar_interval_get(eo));
}

void CElmCalendar::interval_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_calendar_interval_set(eo, val->ToNumber()->Value());
}

}

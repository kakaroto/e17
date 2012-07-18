#ifndef C_ELM_CALENDAR_H
#define C_ELM_CALENDAR_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

using namespace v8;

class CElmCalendar : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmCalendar(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmCalendar();

   static Handle<FunctionTemplate> GetTemplate();

   struct {
      Persistent<Value> change;
   } cb;

   struct {
      Persistent<Object> marks;
   } cached;

   static void OnChangeWrapper(void *data, Evas_Object *obj, void *event_info);

public:
   static void Initialize(Handle<Object> target);
   void OnChange();

   void on_change_set(Handle<Value> val);
   Handle<Value> on_change_get() const;

   Handle<Value> weekday_names_get() const;
   void weekday_names_set(Handle<Value> val);

   Handle<Value> min_year_get() const;
   void min_year_set(Handle<Value> val);

   Handle<Value> max_year_get() const;
   void max_year_set(Handle<Value> val);

   Handle<Value> select_mode_get() const;
   void select_mode_set(Handle<Value> val);

   Handle<Value> day_get() const;
   void day_set(Handle<Value> val);

   Handle<Value> month_get() const;
   void month_set(Handle<Value> val);

   Handle<Value> year_get() const;
   void year_set(Handle<Value> val);

   Handle<Value> interval_get() const;
   void interval_set(Handle<Value> val);

   Handle<Value> marks_get() const;
   void marks_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmCalendar>(const Arguments& args);
};

}

#endif

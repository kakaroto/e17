#ifndef C_ELM_CALENDAR_H
#define C_ELM_CALENDAR_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmCalendar : public CEvasObject {
   FACTORY(CElmCalendar)

protected:
   CPropHandler<CElmCalendar> prop_handler;
   /* the on_clicked function */
   Persistent<Value> on_changed_val;

public:
   CElmCalendar(CEvasObject *parent, Local<Object> obj);

   virtual ~CElmCalendar() { }

   Handle<Object> marks_set(Handle<Value> val);

   virtual void on_changed(void *);

   static void eo_on_changed(void *data, Evas_Object *, void *event_info);

   virtual void on_changed_set(Handle<Value> val);

   virtual Handle<Value> on_changed_get(void) const;

   virtual Handle<Value> weekday_names_get(void) const;

   virtual void weekday_names_set(Handle<Value> val);

   virtual Handle<Value> min_year_get(void) const;

   virtual void min_year_set(Handle<Value> val);

   virtual Handle<Value> max_year_get(void) const;

   virtual void max_year_set(Handle<Value> val);

   virtual Handle<Value> day_selection_enabled_get(void) const;

   virtual void day_selection_enabled_set(Handle<Value> val);

   virtual Handle<Value> selected_date_get(void) const;

   virtual void selected_date_set(Handle<Value> val);

   virtual Handle<Value> selected_month_get(void) const;

   virtual void selected_month_set(Handle<Value> val);

   virtual Handle<Value> selected_year_get(void) const;

   virtual void selected_year_set(Handle<Value> val);

   virtual Handle<Value> calendar_interval_get(void) const;

   virtual void calendar_interval_set(Handle<Value> val);
};

#endif

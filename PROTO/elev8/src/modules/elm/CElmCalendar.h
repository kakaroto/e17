#ifndef C_ELM_CALENDAR_H
#define C_ELM_CALENDAR_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmCalendar : public CEvasObject {
   FACTORY(CElmCalendar)

protected:
   CElmCalendar(CEvasObject *parent, Local<Object> obj);

   static void eo_didChange(void *data, Evas_Object *, void *event_info);

   CPropHandler<CElmCalendar> prop_handler;

   Persistent<Value> didChange;

public:
   virtual void didChange_set(Handle<Value> val);
   virtual Handle<Value> didChange_get(void) const;

   virtual Handle<Value> weekday_names_get(void) const;
   virtual void weekday_names_set(Handle<Value> val);

   virtual Handle<Value> min_year_get(void) const;
   virtual void min_year_set(Handle<Value> val);

   virtual Handle<Value> max_year_get(void) const;
   virtual void max_year_set(Handle<Value> val);

   virtual Handle<Value> day_selection_disabled_get(void) const;
   virtual void day_selection_disabled_set(Handle<Value> val);

   virtual Handle<Value> selected_day_get(void) const;
   virtual void selected_day_set(Handle<Value> val);

   virtual Handle<Value> selected_month_get(void) const;
   virtual void selected_month_set(Handle<Value> val);

   virtual Handle<Value> selected_year_get(void) const;
   virtual void selected_year_set(Handle<Value> val);

   virtual Handle<Value> calendar_interval_get(void) const;
   virtual void calendar_interval_set(Handle<Value> val);
};

#endif

#ifndef C_ELM_DAY_SELECTOR_H
#define C_ELM_DAY_SELECTOR_H

#include <v8.h>
#include "CEvasObject.h"

class CElmDaySelector : public CEvasObject {
    FACTORY(CElmDaySelector)

protected:
    CPropHandler<CElmDaySelector> prop_handler;

    /* the on_changed function */
    Persistent<Value> on_changed_val;

    /* the on_lang_changed function */
    Persistent<Value> on_lang_changed_val;

public:
    CElmDaySelector(CEvasObject *parent, Local<Object> obj);

    virtual ~CElmDaySelector() { }

    Handle<Value> day_selected_get(int day) const;

    void day_selected_set(int day, Handle<Value> val);

    Handle<Value> monday_get() const;

    void monday_set(Handle<Value> val);

    Handle<Value> tuesday_get() const;

    void tuesday_set(Handle<Value> val);

    Handle<Value> wednesday_get() const;

    void wednesday_set(Handle<Value> val);

    Handle<Value> thursday_get() const;

    void thursday_set(Handle<Value> val);

    Handle<Value> friday_get() const;

    void friday_set(Handle<Value> val);

    Handle<Value> saturday_get() const;

    void saturday_set(Handle<Value> val);

    Handle<Value> sunday_get() const;

    void sunday_set(Handle<Value> val);

    Handle<Value> week_start_get() const;

    void week_start_set(Handle<Value> val);

    Handle<Value> weekend_start_get() const;

    void weekend_start_set(Handle<Value> val);

    Handle<Value> weekend_length_get() const;

    void weekend_length_set(Handle<Value> val);

    void on_changed(void *);

    static void eo_on_changed(void *data, Evas_Object *, void *event_info);

    void on_changed_set(Handle<Value> val);

    Handle<Value> on_changed_get(void) const;

    void on_lang_changed(void *);

    static void eo_on_lang_changed(void *data, Evas_Object *, void *event_info);

    void on_lang_changed_set(Handle<Value> val);

    Handle<Value> on_lang_changed_get(void) const;

};

#endif

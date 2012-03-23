#ifndef C_ELM_DATE_TIME_H
#define C_ELM_DATE_TIME_H

#include <v8.h>
#include "CEvasObject.h"

class CElmDateTime : public CEvasObject {
    FACTORY(CElmDateTime)

protected:
    CPropHandler<CElmDateTime> prop_handler;

    /* the on_changed function */
    Persistent<Value> on_changed_val;

    /* the on_lang_changed function */
    Persistent<Value> on_lang_changed_val;

public:
    CElmDateTime(CEvasObject *parent, Local<Object> obj);

    virtual ~CElmDateTime() { }

    Local<Object> fill_tm(struct tm *) const;

    Handle<Value> day_selected_get(int day) const;

    void day_selected_set(int day, Handle<Value> val);

    Handle<Value> format_get() const;

    void format_set(Handle<Value> val);

    struct tm * get_tm(Handle<Value> val) const;

    Handle<Value> value_max_get() const;

    void value_max_set(Handle<Value> timevar);

    Handle<Value> value_min_get() const;

    void value_min_set(Handle<Value> timevar);

    Handle<Value> value_get() const;

    void value_set(Handle<Value> timevar);

    bool get_min_max_from_object(Handle<Value> val, int &min_out, int &max_out) const;

    Local<Object> get_field_limits(Elm_Datetime_Field_Type type) const;

    Handle<Value> field_limit_get() const;

    void field_limit_set(Handle<Value> obj);

    Handle<Value> field_visible_get() const;

    void field_visible_set(Handle<Value> obj);

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

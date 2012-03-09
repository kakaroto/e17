#ifndef C_ELM_COLOR_SELECTOR_H
#define C_ELM_COLOR_SELECTOR_H

#include <v8.h>
#include "CEvasObject.h"

class CElmColorSelector : public CEvasObject {
    FACTORY(CElmColorSelector)

protected:
    CPropHandler<CElmColorSelector> prop_handler;
    /* the on_clicked function */
    Persistent<Value> on_changed_val;

public:
    CElmColorSelector(CEvasObject *parent, Local<Object> obj);

    virtual ~CElmColorSelector() { }

    virtual Handle<Value> red_get() const;

    virtual void red_set(Handle<Value> val);

    virtual Handle<Value> green_get() const;

    virtual void green_set(Handle<Value> val);

    virtual Handle<Value> blue_get() const;

    virtual void blue_set(Handle<Value> val);

    virtual Handle<Value> alpha_get() const;

    virtual void alpha_set(Handle<Value> val);

    virtual void on_changed(void *);

    static void eo_on_changed(void *data, Evas_Object *, void *event_info);

    virtual void on_changed_set(Handle<Value> val);

    virtual Handle<Value> on_changed_get(void) const;
};
#endif

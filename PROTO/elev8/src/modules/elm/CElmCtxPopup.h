#ifndef C_ELM_CTXPOPUP_H
#define C_ELM_CTXPOPUP_H

#include <v8.h>
#include "CEvasObject.h"
#include <string>

class CElmCtxPopup : public CEvasObject {
    FACTORY(CElmCtxPopup)

protected:
    CPropHandler<CElmCtxPopup> prop_handler;

    /* the dismissed function */
    Persistent<Value> on_dismissed_val;
    Persistent<Value> parent;

   struct CtxPopupItemClass {
       std::string label;
       CEvasObject *icon;
       Persistent<Value> data;
       Persistent<Value> on_select;
   };

public:
    CElmCtxPopup(CEvasObject *parent, Local<Object> obj);

    virtual ~CElmCtxPopup() { };

    Handle<Value> hover_parent_get() const;

    void hover_parent_set(Handle<Value> val);

    Handle<Value> horizontal_get() const;

    void horizontal_set(Handle<Value> val);

    Handle<Value> direction_priority_get() const;

    void direction_priority_set(Handle<Value> val);

    Handle<Value> direction_get() const;

    void direction_set(Handle<Value> val);

    void on_dismissed(void *);

    static void eo_on_dismissed(void *data, Evas_Object *, void *event_info);

    void on_dismissed_set(Handle<Value> val);

    Handle<Value> on_dismissed_get(void) const;

    static Handle<Value> dismiss(const Arguments& args);

    static Handle<Value> clear(const Arguments& args);

    static Handle<Value> append(const Arguments& args);

    static void sel(void *data, Evas_Object *, void *);

};

#endif

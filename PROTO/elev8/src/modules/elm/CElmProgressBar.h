#ifndef C_ELM_PROGRESS_BAR_H
#define C_ELM_PROGRESS_BAR_H

#include <v8.h>
#include "CEvasObject.h"

class CElmProgressBar : public CEvasObject {
     FACTORY(CElmProgressBar)
   protected:
        CPropHandler<CElmProgressBar> prop_handler;
        Persistent<Value> the_icon;

        static Handle<Value> do_pulse(const Arguments& args);

   public:
        CElmProgressBar(CEvasObject *parent, Local<Object> obj);

        virtual ~CElmProgressBar();

        virtual void pulse(bool on);

        virtual Handle<Value> icon_get() const;

        virtual void icon_set(Handle<Value> value);

        virtual Handle<Value> inverted_get() const;

        virtual void inverted_set(Handle<Value> value);

        virtual Handle<Value> horizontal_get() const;

        virtual void horizontal_set(Handle<Value> value);

        virtual Handle<Value> units_get() const;

        virtual void units_set(Handle<Value> value);

        virtual Handle<Value> span_get() const;

        virtual void span_set(Handle<Value> value);

        virtual Handle<Value> pulser_get() const;

        virtual void pulser_set(Handle<Value> value);

        virtual Handle<Value> value_get() const;

        virtual void value_set(Handle<Value> value);
};

#endif

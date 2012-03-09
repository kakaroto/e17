#ifndef C_ELM_PANE_H
#define C_ELM_PANE_H

#include <v8.h>
#include "CEvasObject.h"

class CElmPane : public CEvasObject {
     FACTORY(CElmPane)
   protected:
        CPropHandler<CElmPane> prop_handler;

   public:
        CElmPane(CEvasObject *parent, Local<Object> obj);

        virtual ~CElmPane();

        virtual Handle<Value> horizontal_get() const;

        virtual void horizontal_set(Handle<Value> val);

        virtual void on_press_set(Handle<Value> val);

        virtual Handle<Value> on_press_get(void) const;
};

#endif


#ifndef C_ELM_ACTION_SLIDER_H
#define C_ELM_ACTION_SLIDER_H

#include <v8.h>

#include "elev8_elm.h"
#include "CEvasObject.h"

using namespace v8;

class CElmActionSlider : public CEvasObject {
   FACTORY(CElmActionSlider)

private:
   CPropHandler<CElmActionSlider> prop_handler;

public:
   CElmActionSlider(CEvasObject *parent, Local<Object> obj);

   /* there's 1 indicator label and 3 position labels */
   virtual void labels_set(Handle<Value> val);

   virtual Handle<Value> labels_get() const;

   bool position_from_string(Handle<Value> val, Elm_Actionslider_Pos &pos);

   virtual void slider_set(Handle<Value> val);

   virtual Handle<Value> slider_get() const;

   virtual void magnet_set(Handle<Value> val);

   virtual Handle<Value> magnet_get() const;
};

#endif

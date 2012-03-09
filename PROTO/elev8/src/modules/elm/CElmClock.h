#ifndef C_ELM_CLOCK_H
#define C_ELM_CLOCK_H

#include <v8.h>
#include "CEvasObject.h"

class CElmClock : public CEvasObject {
   FACTORY(CElmClock)
protected:
  CPropHandler<CElmClock> prop_handler;

public:
  CElmClock(CEvasObject *parent, Local<Object> obj);

  virtual ~CElmClock();

  virtual Handle<Value> show_am_pm_get() const;

  virtual void show_am_pm_set(Handle<Value> val);

  virtual Handle<Value> show_seconds_get() const;

  virtual void show_seconds_set(Handle<Value> val);

  virtual Handle<Value> hour_get() const;

  virtual Handle<Value> minute_get() const;

  virtual Handle<Value> second_get() const;

  virtual void hour_set(Handle<Value> val);

  virtual void minute_set(Handle<Value> val);

  virtual void second_set(Handle<Value> val);

  virtual Handle<Value> edit_get() const;

  virtual void edit_set(Handle<Value> val);

};

#endif


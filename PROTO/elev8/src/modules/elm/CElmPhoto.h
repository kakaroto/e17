#ifndef C_ELM_PHOTO_H
#define C_ELM_PHOTO_H

#include <v8.h>
#include "CEvasObject.h"

class CElmPhoto : public CEvasObject {
   FACTORY(CElmPhoto)
protected:
  CPropHandler<CElmPhoto> prop_handler;

public:
  CElmPhoto(CEvasObject *parent, Local<Object> obj);

  virtual Handle<Value> image_get() const;
  virtual void image_set(Handle<Value> val);
  virtual Handle<Value> size_get() const;
  virtual void size_set(Handle<Value> val);
  virtual Handle<Value> fill_get() const;
  virtual void fill_set(Handle<Value> val);
};

#endif // C_ELM_PHOTO_H

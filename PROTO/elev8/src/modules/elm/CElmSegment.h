#ifndef C_ELM_SEGMENT_H
#define C_ELM_SEGMENT_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmSegment : public CEvasObject {
   FACTORY(CElmSegment)

protected:
   CPropHandler<CElmSegment> prop_handler;

   CElmSegment(CEvasObject *parent, Local<Object> obj);
public:
   void items_set(Handle<Value> val);
   Handle<Value> items_get() const;
};

#endif

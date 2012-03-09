#ifndef C_ELM_SEGMENT_H
#define C_ELM_SEGMENT_H

#include <v8.h>
#include "CEvasObject.h"

using namespace v8;

class CElmSegment : public CEvasObject {
    FACTORY(CElmSegment)

protected:
    CPropHandler<CElmSegment> prop_handler;

public:
    CElmSegment(CEvasObject *parent, Local<Object> obj);

    virtual ~CElmSegment() { }

    Handle<Object> items_set(Handle<Value> val);
};

#endif

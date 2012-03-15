#ifndef C_ELM_BUBBLE_H
#define C_ELM_BUBBLE_H

#include <v8.h>
#include "CEvasObject.h"

class CElmBubble : public CEvasObject {
    FACTORY(CElmBubble)
protected:
    CPropHandler<CElmBubble> prop_handler;

public:
    CElmBubble(CEvasObject *parent, Local<Object> obj);

    virtual Handle<Value> text_part_get() const;
    virtual void text_part_set(Handle<Value> val);

    virtual Handle<Value> corner_get() const;
    virtual void corner_set(Handle<Value> val);
};

#endif

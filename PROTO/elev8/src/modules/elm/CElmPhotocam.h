#ifndef C_ELM_PHOTOCAM_H
#define C_ELM_PHOTOCAM_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmPhotocam : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmPhotocam(Local<Object> _jsObject, CElmObject *parent);

   static Handle<FunctionTemplate> GetTemplate();
public:
   static void Initialize(Handle<Object> target);

   void file_set(Handle<Value> val);
   Handle<Value> file_get(void) const;

   Handle<Value> zoom_get() const;
   void zoom_set(Handle<Value> value);

   Handle<Value> zoom_mode_get() const;
   void zoom_mode_set(Handle<Value> value);

   void paused_set(Handle<Value> val);
   Handle<Value> paused_get() const;

   void gesture_enabled_set(Handle<Value> val);
   Handle<Value> gesture_enabled_get() const;

   Handle<Value> image_size_get() const;

   Handle<Value> image_region_get() const;

   Handle<Value> image_region_show(const Arguments &args);

   Handle<Value> image_region_bring_in(const Arguments &args);

   friend Handle<Value> CElmObject::New<CElmPhotocam>(const Arguments &args);
};

}

#endif

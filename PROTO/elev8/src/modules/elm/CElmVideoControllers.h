#ifndef C_ELM_VIDEO_CONTROLLERS_H
#define C_ELM_VIDEO_CONTROLLERS_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

using namespace v8;

class CElmVideoControllers : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

   Persistent<Value> video;
protected:
   CElmVideoControllers(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

   static void OnStopClicked(void *, Evas_Object *, const char *, const char *);

   ~CElmVideoControllers();

public:
   static void Initialize(Handle<Object> target);

   void video_set(Handle<Value> val);
   Handle<Value> video_get(void) const;

   friend Handle<Value> CElmObject::New<CElmVideoControllers>(const Arguments& args);
};

}
#endif

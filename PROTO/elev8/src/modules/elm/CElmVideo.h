#ifndef C_ELM_VIDEO_H
#define C_ELM_VIDEO_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

using namespace v8;

class CElmVideo : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

   Persistent<Value> file;
protected:
   CElmVideo(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

   ~CElmVideo();

public:
   static void Initialize(Handle<Object> target);

   void file_set(Handle<Value> val);
   Handle<Value> file_get(void) const;

   Handle<Value> audio_level_get() const;
   void audio_level_set(Handle<Value> val);

   Handle<Value> audio_mute_get() const;
   void audio_mute_set(Handle<Value> val);

   Handle<Value> play_position_get() const;
   void play_position_set(Handle<Value> val);

   Handle<Value> remember_position_get() const;
   void remember_position_set(Handle<Value> val);

   Handle<Value> playing_get() const;
   Handle<Value> seekable_get() const;

   Handle<Value> title_get() const;

   Handle<Value> play(const Arguments&);
   Handle<Value> pause(const Arguments&);
   Handle<Value> stop(const Arguments&);

   friend Handle<Value> CElmObject::New<CElmVideo>(const Arguments& args);
};

}
#endif

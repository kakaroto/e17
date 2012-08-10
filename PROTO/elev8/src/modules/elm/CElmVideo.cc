#include "CElmVideo.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmVideo, file);
GENERATE_PROPERTY_CALLBACKS(CElmVideo, audio_level);
GENERATE_PROPERTY_CALLBACKS(CElmVideo, audio_mute);
GENERATE_PROPERTY_CALLBACKS(CElmVideo, play_position);
GENERATE_PROPERTY_CALLBACKS(CElmVideo, remember_position);
GENERATE_RO_PROPERTY_CALLBACKS(CElmVideo, playing);
GENERATE_RO_PROPERTY_CALLBACKS(CElmVideo, seekable);
GENERATE_RO_PROPERTY_CALLBACKS(CElmVideo, title);
GENERATE_METHOD_CALLBACKS(CElmVideo, play);
GENERATE_METHOD_CALLBACKS(CElmVideo, pause);
GENERATE_METHOD_CALLBACKS(CElmVideo, stop);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmVideo,
                       PROPERTY(file),
                       PROPERTY(audio_level),
                       PROPERTY(audio_mute),
                       PROPERTY(play_position),
                       PROPERTY(remember_position),
                       PROPERTY_RO(playing),
                       PROPERTY_RO(seekable),
                       PROPERTY_RO(title),
                       METHOD(play),
                       METHOD(pause),
                       METHOD(stop));

CElmVideo::CElmVideo(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_video_add(_parent->GetEvasObject()))
{
}

CElmVideo::~CElmVideo()
{
  file.Dispose();
}

void CElmVideo::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Video"),
               GetTemplate()->GetFunction());
}

void CElmVideo::file_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   String::Utf8Value str(val);
   if (!elm_video_file_set(eo, *str)) {
     ELM_WRN( "warning: can't set video file %s", *str);
     return;
   }

   file.Dispose();
   file = Persistent<Value>::New(val);
}

Handle<Value> CElmVideo::file_get(void) const
{
   return file;
}

Handle<Value> CElmVideo::audio_level_get() const
{
   return Number::New(elm_video_audio_level_get(eo));
}

void CElmVideo::audio_level_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_video_audio_level_set(eo, val->NumberValue());
}

Handle<Value> CElmVideo::audio_mute_get() const
{
   return Boolean::New(elm_video_audio_mute_get(eo));
}

void CElmVideo::audio_mute_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_video_audio_mute_set(eo, val->BooleanValue());
}

Handle<Value> CElmVideo::play_position_get() const
{
   return Number::New(elm_video_play_position_get(eo));
}

void CElmVideo::play_position_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_video_play_position_set(eo, val->NumberValue());
}

Handle<Value> CElmVideo::remember_position_get() const
{
   return Boolean::New(elm_video_remember_position_get(eo));
}

void CElmVideo::remember_position_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_video_remember_position_set(eo, val->BooleanValue());
}

Handle<Value> CElmVideo::playing_get() const
{
   return Boolean::New(elm_video_is_playing_get(eo));
}

Handle<Value> CElmVideo::seekable_get() const
{
   return Boolean::New(elm_video_is_seekable_get(eo));
}

Handle<Value> CElmVideo::title_get() const
{
   return String::New(elm_video_title_get(eo));
}

Handle<Value> CElmVideo::play(const Arguments&)
{
   elm_video_play(eo);
   return Undefined();
}

Handle<Value> CElmVideo::pause(const Arguments&)
{
   elm_video_pause(eo);
   return Undefined();
}

Handle<Value> CElmVideo::stop(const Arguments&)
{
   elm_video_stop(eo);
   return Undefined();
}

}

#include "CElmVideoControllers.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmVideoControllers, video);

GENERATE_TEMPLATE_FULL(CElmLayout, CElmVideoControllers,
                       PROPERTY(video));

void CElmVideoControllers::OnStopClicked(void *data, Evas_Object *, const char *,
                                    const char *)
{

   CElmVideoControllers *self = static_cast<CElmVideoControllers *>(data);
   Evas_Object *elm_video = GetEvasObjectFromJavascript(self->video);

   elm_video_play_position_set(elm_video, 0);
   elm_video_pause(elm_video);
}

CElmVideoControllers::CElmVideoControllers(Local<Object> _jsObject, CElmObject *_parent)
   : CElmLayout(_jsObject, elm_player_add(_parent->GetEvasObject()))
{
   elm_layout_signal_callback_add(eo, "elm,button,stop", "elm", OnStopClicked, this);
}

CElmVideoControllers::~CElmVideoControllers()
{
   video.Dispose();
}

void CElmVideoControllers::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("VideoControllers"),
               GetTemplate()->GetFunction());
}

void CElmVideoControllers::video_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   elm_object_content_set(eo, GetEvasObjectFromJavascript(val));

   video.Dispose();
   video = Persistent<Value>::New(val);
}

Handle<Value> CElmVideoControllers::video_get(void) const
{
   return video;
}

}

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "../include/emotionxx/VideoObject.h"

extern "C" {
#include <Emotion.h>
}

#include <iostream>
#include <assert.h>

using namespace std;

namespace Emotionxx {

VideoObject::VideoObject (Evasxx::Canvas &canvas, const std::string &moduleFilename) :
  AudioObject (canvas, moduleFilename)
{
}

VideoObject::VideoObject (Evasxx::Canvas &canvas, const std::string &filename, const std::string &moduleFilename) :
  AudioObject (canvas, filename, moduleFilename)
{
}

VideoObject::VideoObject (Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &moduleFilename) :
  AudioObject (canvas, pos, filename, moduleFilename)
{
}

VideoObject::VideoObject (Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &moduleFilename) :
  AudioObject (canvas,  rect, filename, moduleFilename)
{
}

VideoObject::~VideoObject ()
{
}

void VideoObject::setVideoMute (bool mute)
{
  emotion_object_video_mute_set (o, mute);
}

bool VideoObject::getVideoMute ()
{
  return emotion_object_video_mute_get (o);
}

int VideoObject::countVideoChannel ()
{
  return emotion_object_video_channel_count (o);
}

string VideoObject::getVideoChannelName (int channel)
{
  return emotion_object_video_channel_name_get (o, channel);
}

void VideoObject::setVideoChannel (int channel)
{
  emotion_object_video_channel_set (o, channel);
}

int VideoObject::getVideoChannel ()
{
  return emotion_object_video_channel_get (o);
}

void VideoObject::setSmoothScale (bool smooth)
{
  emotion_object_smooth_scale_set (o, smooth);
}

bool VideoObject::getSmoothScale ()
{
  return emotion_object_smooth_scale_get (o);
}

double VideoObject::getRatio ()
{
  return emotion_object_ratio_get (o);
}

Eflxx::Size VideoObject::getSize ()
{
  int iw = 0;
  int ih = 0;
  
  emotion_object_size_get (o, &iw, &ih);
  return Eflxx::Size (iw, ih);
}

int VideoObject::countChapter ()
{
  return emotion_object_chapter_count (o);
}

void VideoObject::setChapter (int chapter)
{
  emotion_object_chapter_set (o, chapter);
}

int VideoObject::getChapter ()
{
  return emotion_object_chapter_get (o);
}

string VideoObject::getChapterName (int chapter)
{
  return emotion_object_chapter_name_get (o, chapter);
}

void VideoObject::sendSimpleEvent (Emotion_Event ev)
{
  emotion_object_event_simple_send (o, ev);
}

} // end namespace Emotionxx

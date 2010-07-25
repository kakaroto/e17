#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "../include/emotionxx/AudioObject.h"

extern "C" {
#include <Emotion.h>
}

#include <iostream>
#include <assert.h>

using namespace std;

namespace Emotionxx {

AudioObject::AudioObject (Evasxx::Canvas &canvas, const std::string &moduleFilename) :
  Object (canvas, moduleFilename)
{
}

AudioObject::AudioObject (Evasxx::Canvas &canvas, const std::string &filename, const std::string &moduleFilename) :
  Object (canvas, filename, moduleFilename)
{
}

AudioObject::AudioObject (Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &moduleFilename) :
  Object (canvas, pos, filename, moduleFilename)
{
}

AudioObject::AudioObject (Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &moduleFilename) :
  Object (canvas,  rect, filename, moduleFilename)
{
}

AudioObject::~AudioObject ()
{
}

void AudioObject::setAudioVolume (double vol)
{
  emotion_object_audio_volume_set (o, vol);
}

double AudioObject::getAudioVolume ()
{
  return emotion_object_audio_volume_get (o);
}

void AudioObject::setAudioMute (bool mute)
{
  emotion_object_audio_mute_set (o, mute);
}

bool AudioObject::getAudioMute ()
{
  return emotion_object_audio_mute_get (o);
}

int AudioObject::countAudioChannel ()
{
  return emotion_object_audio_channel_count (o);
}

string AudioObject::getAudioChannelName (int channel)
{
  return emotion_object_audio_channel_name_get (o, channel);
}

void AudioObject::setAudioChannel (int channel)
{
  emotion_object_audio_channel_set (o, channel);
}

int AudioObject::getAudioChannel ()
{
  return emotion_object_audio_channel_get (o);
}

} // end namespace Emotionxx

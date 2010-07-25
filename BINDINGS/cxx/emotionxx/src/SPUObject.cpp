#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "../include/emotionxx/SPUObject.h"

extern "C" {
#include <Emotion.h>
}

#include <iostream>
#include <assert.h>

using namespace std;

namespace Emotionxx {

SPUObject::SPUObject (Evasxx::Canvas &canvas, const std::string &moduleFilename) :
  VideoObject (canvas, moduleFilename)
{
}

SPUObject::SPUObject (Evasxx::Canvas &canvas, const std::string &filename, const std::string &moduleFilename) :
  VideoObject (canvas, filename, moduleFilename)
{
}

SPUObject::SPUObject (Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &moduleFilename) :
  VideoObject (canvas, pos, filename, moduleFilename)
{
}

SPUObject::SPUObject (Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &moduleFilename) :
  VideoObject (canvas, rect, filename, moduleFilename)
{
}

SPUObject::~SPUObject ()
{
}

void SPUObject::setSPUMute (bool mute)
{
  emotion_object_spu_mute_set (o, mute);
}

bool SPUObject::getSPUMute ()
{
  return emotion_object_spu_mute_get (o);
}

int SPUObject::countSPUChannel ()
{
  return emotion_object_spu_channel_count (o);
}

std::string SPUObject::getSPUChannelName (int channel)
{
  return emotion_object_spu_channel_name_get (o, channel);
}

void SPUObject::setSPUChannel (int channel)
{
  emotion_object_spu_channel_set (o, channel);
}

int SPUObject::getSPUChannel ()
{
  return emotion_object_spu_channel_get (o);
}

int SPUObject::getSPUButtonCount ()
{
  return emotion_object_spu_button_count_get (o);
}

int SPUObject::getSPUButton ()
{
  return emotion_object_spu_button_get (o);
}


} // end namespace Emotionxx

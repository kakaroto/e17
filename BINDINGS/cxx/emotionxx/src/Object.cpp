#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "../include/emotionxx/Object.h"

extern "C" {
#include <Emotion.h>
}

#include <iostream>
#include <assert.h>

using namespace std;

namespace Emotionxx {

Object::Object( Evasxx::Canvas &canvas, const std::string &module_filename )
{
  o = emotion_object_add( canvas.obj() );
  init();
  engineInit(module_filename);
}

Object::Object( Evasxx::Canvas &canvas, const std::string &filename, const std::string &module_filename )
{
  o = emotion_object_add( canvas.obj() );
  init();
  engineInit(module_filename);
  setFile( filename );
}

Object::Object( Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &module_filename )
{
  o = emotion_object_add( canvas.obj() );
  init( );
  engineInit(module_filename);
  setFile( filename );
  move( pos );
}

Object::Object( Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &module_filename )
{
  o = emotion_object_add( canvas.obj() );

  init( );
  engineInit(module_filename);
  setFile( filename );
  setGeometry (rect);
}

Object::~Object()
{
}

bool Object::engineInit(const std::string &module_filename)
{
  return emotion_object_init(o, module_filename.c_str ());
}

void Object::setFile (const std::string &filename)
{
  emotion_object_file_set  (o, filename.c_str ());
}

std::string Object::getFile ()
{
  return emotion_object_file_get (o);
}

void Object::setPlay (bool play)
{
  emotion_object_play_set (o, play);
}

bool Object::getPlay ()
{
  return emotion_object_play_get (o);
}

void Object::setPosition (double sec)
{
  emotion_object_position_set (o, sec);
}

double Object::getPosition ()
{
  return emotion_object_position_get (o);
}

bool Object::getVideoHandled ()
{
  return emotion_object_video_handled_get (o);
}

bool Object::getAudioHandled ()
{
  return emotion_object_audio_handled_get (o);
}

bool Object::getSeekable ()
{
  return emotion_object_seekable_get (o);
}

double Object::getPlayLengh ()
{
  return emotion_object_play_length_get (o);
}

Eflxx::Size Object::getSize ()
{
  int iw = 0;
  int ih = 0;
  
  emotion_object_size_get (o, &iw, &ih);
  return Eflxx::Size (iw, ih);
}

void Object::setSmoothScale (bool smooth)
{
  emotion_object_smooth_scale_set (o, smooth);
}

bool Object::getSmoothScale ()
{
  return emotion_object_smooth_scale_get (o);
}

double Object::getRatio ()
{
  return emotion_object_ratio_get (o);
}

void Object::sendSimpleEvent (Emotion_Event ev)
{
  emotion_object_event_simple_send (o, ev);
}

void Object::setAudioVolume (double vol)
{
  emotion_object_audio_volume_set (o, vol);
}

double Object::getAudioVolume ()
{
  return emotion_object_audio_volume_get (o);
}

void Object::setAudioMute (bool mute)
{
  emotion_object_audio_mute_set (o, mute);
}

bool Object::getAudioMute ()
{
  return emotion_object_audio_mute_get (o);
}

int Object::countAudioChannel ()
{
  return emotion_object_audio_channel_count (o);
}

string Object::getChannelName (int channel)
{
  return emotion_object_audio_channel_name_get (o, channel);
}

void Object::setAudioChannel (int channel)
{
  emotion_object_audio_channel_set (o, channel);
}

int Object::getAudioChannel ()
{
  return emotion_object_audio_channel_get (o);
}

void Object::setVideoMute (bool mute)
{
  emotion_object_video_mute_set (o, mute);
}

bool Object::getVideoMute ()
{
  return emotion_object_video_mute_get (o);
}

int Object::countVideoChannel ()
{
  return emotion_object_video_channel_count (o);
}

string Object::getVideoChannelName (int channel)
{
  return emotion_object_video_channel_name_get (o, channel);
}

void Object::setVideoChannel (int channel)
{
  emotion_object_video_channel_set (o, channel);
}

int Object::getVideoChannel ()
{
  return emotion_object_video_channel_get (o);
}

void Object::setSPUMute (bool mute)
{
  emotion_object_spu_mute_set (o, mute);
}

bool Object::getSPUMute ()
{
  return emotion_object_spu_mute_get (o);
}

int Object::countSPUChannel ()
{
  return emotion_object_spu_channel_count (o);
}

std::string Object::getSPUChannelName (int channel)
{
  return emotion_object_spu_channel_name_get (o, channel);
}

void Object::setSPUChannel (int channel)
{
  emotion_object_spu_channel_set (o, channel);
}

int Object::getSPUChannel ()
{
  return emotion_object_spu_channel_get (o);
}

int Object::countChapter ()
{
  return emotion_object_chapter_count (o);
}

void Object::setChapter (int chapter)
{
  emotion_object_chapter_set (o, chapter);
}

int Object::getChapter ()
{
  return emotion_object_chapter_get (o);
}

string Object::getChapterName (int chapter)
{
  return emotion_object_chapter_name_get (o, chapter);
}

void Object::setPlaySpeed (double speed)
{
  emotion_object_play_speed_set (o, speed);
}

double Object::getPlaySpeed ()
{
  return emotion_object_play_speed_get (o);
}

void Object::eject ()
{
  emotion_object_eject (o);
}

std::string Object::getTitle ()
{
  return emotion_object_title_get (o);
}

string Object::getProgressInfo ()
{
  return emotion_object_progress_info_get (o);
}

double Object::getProgressStatus ()
{
  return emotion_object_progress_status_get (o);
}

string Object::getRefFile ()
{
  return emotion_object_ref_file_get (o);
}

int Object::getRefNum ()
{
  return emotion_object_ref_num_get (o);
}

int Object::getSPUButtonCount ()
{
  return emotion_object_spu_button_count_get (o);
}

int Object::getSPUButton ()
{
  return emotion_object_spu_button_get (o);
}

string Object::getMetaInfo (Emotion_Meta_Info meta)
{
  return emotion_object_meta_info_get (o, meta);
}

void Object::setVIS (Emotion_Vis visualization)
{
  emotion_object_vis_set (o, visualization);
}

Emotion_Vis Object::getVIS ()
{
  return emotion_object_vis_get (o);
}

bool Object::isVISSupported (Emotion_Vis visualization)
{
  return emotion_object_vis_supported (o, visualization);
}

} // end namespace Emotionxx

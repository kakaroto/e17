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

Object::Object( Evasxx::Canvas &canvas, const std::string &moduleFilename )
{
  o = emotion_object_add (canvas.obj ());
  init();
  engineInit (moduleFilename);
}

Object::Object (Evasxx::Canvas &canvas, const std::string &filename, const std::string &moduleFilename)
{
  o = emotion_object_add (canvas.obj ());
  init ();
  engineInit (moduleFilename);
  setFile (filename);
}

Object::Object (Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &moduleFilename)
{
  o = emotion_object_add( canvas.obj ());
  init ();
  engineInit (moduleFilename);
  setFile (filename);
  move (pos);
}

Object::Object (Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &moduleFilename )
{
  o = emotion_object_add (canvas.obj ());

  init ();
  engineInit (moduleFilename);
  setFile (filename);
  setGeometry (rect);
}

Object::~Object()
{
}

bool Object::engineInit (const std::string &moduleFilename)
{
  return emotion_object_init (o, moduleFilename.c_str ());
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

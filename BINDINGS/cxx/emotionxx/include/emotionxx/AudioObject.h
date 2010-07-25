#ifndef EMOTIONXX_AUDIO_OBJECT_H
#define EMOTIONXX_AUDIO_OBJECT_H

/* Project */
#include "Object.h"

/* EFL++ */
#include <eflxx/Common.h>
#include <evasxx/Canvas.h>
#include <evasxx/Object.h>

/* STD */
#include <iostream>
using namespace std;

namespace Emotionxx {

class AudioObject : public Emotionxx::Object
{
public:
  AudioObject (Evasxx::Canvas &canvas, const std::string &moduleFilename);
  AudioObject (Evasxx::Canvas &canvas, const std::string &filename, const std::string &moduleFilename);
  AudioObject (Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &moduleFilename);
  AudioObject (Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &moduleFilename);
  ~AudioObject ();

  void setAudioVolume (double vol);
  double getAudioVolume ();
  void setAudioMute (bool mute);
  bool getAudioMute ();
  int countAudioChannel ();
  string getAudioChannelName (int channel);
  void setAudioChannel (int channel);
  int getAudioChannel ();

};

} // end namespace Emotionxx

#endif // EMOTIONXX_AUDIO_OBJECT_H
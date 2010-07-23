#ifndef EMOTIONXX_OBJECT_H
#define EMOTIONXX_OBJECT_H

/* EFL */
#include <Emotion.h>

/* EFL++ */
#include <eflxx/Common.h>
#include <evasxx/Canvas.h>
#include <evasxx/Object.h>

/* STD */
#include <iostream>
using namespace std;

/**
 * C++ Wrapper for the Enlightenment Emotion Library (EMOTION)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace Emotionxx {

class Object : public Evasxx::Object
{
public:
  Object( Evasxx::Canvas &canvas, const std::string &module_filename );
  Object( Evasxx::Canvas &canvas, const std::string &filename, const std::string &module_filename );
  Object( Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &module_filename );
  Object( Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &module_filename );
  ~Object();

  /**
  * Initialize video engine to either use xine or gstreamer
  *
  * @param module_filename	name of viedo engine to be used
  */
  bool engineInit (const std::string &module_filename);

  void setFile (const std::string &filename);
  std::string getFile ();
  void setPlay (bool play);
  bool getPlay ();
  void setPosition (double sec);
  double getPosition ();
  bool getVideoHandled ();
  bool getAudioHandled ();
  bool getSeekable ();
  double getPlayLengh ();
  Eflxx::Size getSize ();
  void setSmoothScale (bool smooth);
  bool getSmoothScale ();
  double getRatio ();
  void sendSimpleEvent (Emotion_Event ev);
  void setAudioVolume (double vol);
  double getAudioVolume ();
  void setAudioMute (bool mute);
  bool getAudioMute ();
  int countAudioChannel ();
  string getChannelName (int channel);
  void setAudioChannel (int channel);
  int getAudioChannel ();
  void setVideoMute (bool mute);
  bool getVideoMute ();
  int countVideoChannel ();
  string getVideoChannelName (int channel);
  void setVideoChannel (int channel);
  int getVideoChannel ();
  void setSPUMute (bool mute);
  bool getSPUMute ();
  int countSPUChannel ();
  std::string getSPUChannelName (int channel);
  void setSPUChannel (int channel);
  int getSPUChannel ();
  int countChapter ();
  void setChapter (int chapter);
  int getChapter ();
  std::string getChapterName (int chapter);
  void setPlaySpeed (double speed);
  double getPlaySpeed ();
  void eject ();
  std::string getTitle ();
  string getProgressInfo ();
  double getProgressStatus ();
  string getRefFile ();
  int getRefNum ();
  int getSPUButtonCount ();
  int getSPUButton ();
  string getMetaInfo (Emotion_Meta_Info meta);

  void setVIS (Emotion_Vis visualization);
  Emotion_Vis getVIS ();
  bool isVISSupported (Emotion_Vis visualization);

/*





*/
};

} // end namespace Emotionxx

#endif // EMOTIONXX_OBJECT_H

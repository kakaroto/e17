#ifndef EMOTIONXX_VIDEO_OBJECT_H
#define EMOTIONXX_VIDEO_OBJECT_H

/* Project */
#include "AudioObject.h"

/* EFL++ */
#include <eflxx/Common.h>
#include <evasxx/Canvas.h>
#include <evasxx/Object.h>

/* STD */
#include <iostream>
using namespace std;

namespace Emotionxx {

class VideoObject : public Emotionxx::AudioObject
{
public:
  VideoObject (Evasxx::Canvas &canvas, const std::string &moduleFilename);
  VideoObject (Evasxx::Canvas &canvas, const std::string &filename, const std::string &moduleFilename);
  VideoObject (Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &moduleFilename);
  VideoObject (Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &moduleFilename);
  ~VideoObject ();
  
  void setVideoMute (bool mute);
  bool getVideoMute ();
  int countVideoChannel ();
  string getVideoChannelName (int channel);
  void setVideoChannel (int channel);
  int getVideoChannel ();

  /*!
   * Sets whether to use of high-quality image scaling algorithm
   * of the given video object.
   *
   * When enabled, a higher quality video scaling algorithm is used when
   * scaling videos to sizes other than the source video. This gives
   * better results but is more computationally expensive.
   *
   * @param smooth Whether to use smooth scale or not.
   */
  void setSmoothScale (bool smooth);

  /*!
   * Gets whether the high-quality image scaling algorithm
   * of the given video object is used.
   *
   * @return Whether the smooth scale is used or not.
   */
  bool getSmoothScale ();

  double getRatio ();

  Eflxx::Size getSize ();
  
  int countChapter ();
  void setChapter (int chapter);
  int getChapter ();
  std::string getChapterName (int chapter);

  /*!
   * Send a control event to the DVD.
   */
  void sendSimpleEvent (Emotion_Event ev);
};

} // end namespace Emotionxx

#endif // EMOTIONXX_VIDEO_OBJECT_H
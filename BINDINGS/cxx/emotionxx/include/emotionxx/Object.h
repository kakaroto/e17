#ifndef EMOTIONXX_OBJECT_H
#define EMOTIONXX_OBJECT_H

/* EFL */
#include <Emotion.h>

/* EFL++ */
#include <eflxx/Common.h>
#include <evasxx/Canvas.h>
#include <evasxx/Smart.h>

/* STD */
#include <iostream>
using namespace std;

/**
 * C++ Wrapper for the Enlightenment Emotion Library (EMOTION)
 *
 * @author Andreas Volz <andreas.volz@tux-style.com>
 */

namespace Emotionxx {

class Object : public Evasxx::Smart
{
public:
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

  void setPlaySpeed (double speed);
  double getPlaySpeed ();
  
  void eject ();
  
  std::string getTitle ();
  
  string getProgressInfo ();
  double getProgressStatus ();
  
  string getRefFile ();
  int getRefNum ();

  string getMetaInfo (Emotion_Meta_Info meta);

  void setVIS (Emotion_Vis visualization);
  Emotion_Vis getVIS ();
  bool isVISSupported (Emotion_Vis visualization);

protected:
  Object (Evasxx::Canvas &canvas, const std::string &moduleFilename);
  Object (Evasxx::Canvas &canvas, const std::string &filename, const std::string &moduleFilename);
  Object (Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &moduleFilename);
  Object (Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &moduleFilename);
  virtual ~Object ();

private:
  /**
   * Initialize video engine to either use xine or gstreamer
   *
   * @param moduleFilename name of viedo engine to be used
   */
  bool engineInit (const std::string &moduleFilename);
};

} // end namespace Emotionxx

#endif // EMOTIONXX_OBJECT_H

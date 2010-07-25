#ifndef EMOTIONXX_SPU_OBJECT_H
#define EMOTIONXX_SPU_OBJECT_H

/* Project */
#include "VideoObject.h"

/* EFL++ */
#include <eflxx/Common.h>
#include <evasxx/Canvas.h>
#include <evasxx/Object.h>

/* STD */
#include <iostream>
using namespace std;

namespace Emotionxx {

/*!
 * SPU == Subtitle Processing Unit
 */
class SPUObject : public Emotionxx::VideoObject
{
public:
  SPUObject (Evasxx::Canvas &canvas, const std::string &moduleFilename);
  SPUObject (Evasxx::Canvas &canvas, const std::string &filename, const std::string &moduleFilename);
  SPUObject (Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &moduleFilename);
  SPUObject (Evasxx::Canvas &canvas, const Eflxx::Rect &rect, const std::string &filename, const std::string &moduleFilename);
  ~SPUObject ();
  
  void setSPUMute (bool mute);
  bool getSPUMute ();
  int countSPUChannel ();
  std::string getSPUChannelName (int channel);
  void setSPUChannel (int channel);
  int getSPUChannel ();

  int getSPUButtonCount ();
  int getSPUButton ();
};

} // end namespace Emotionxx

#endif // EMOTIONXX_SPU_OBJECT_H
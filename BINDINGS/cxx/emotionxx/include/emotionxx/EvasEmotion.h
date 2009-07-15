#ifndef EVAS_EMOTION_H
#define EVAS_EMOTION_H


/* EFL++ */
#include <eflxx/eflpp_common.h>
#include <evasxx/EvasCanvas.h>
#include <evasxx/EvasObject.h>

/* STD */
#include <iostream>
using namespace std;

/**
 * C++ Wrapper for the Enlightenment Emotion Library (EMOTION)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

class EvasEmotion : public EvasObject
{
public:
  EvasEmotion( EvasCanvas &canvas, const std::string &module_filename );
  EvasEmotion( EvasCanvas &canvas, const std::string &filename, const std::string &module_filename );
  EvasEmotion( EvasCanvas &canvas, const Point &pos, const std::string &filename, const std::string &module_filename );
  EvasEmotion( EvasCanvas &canvas, const Rect &rect, const std::string &filename, const std::string &module_filename );
  ~EvasEmotion();

  /**
  * Initialize video engine to either use xine or gstreamer
  *
  * @param module_filename	name of viedo engine to be used
  */
  void engineInit(const std::string &module_filename);

  void setFile( const std::string &filename );
  void setPlay( bool b );
  void setSmoothScale( bool b );
};

}

#endif // EVAS_EMOTION_H

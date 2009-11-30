#ifndef EMOTIONXX_OBJECT_H
#define EMOTIONXX_OBJECT_H


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
  void engineInit(const std::string &module_filename);

  void setFile( const std::string &filename );
  void setPlay( bool b );
  void setSmoothScale( bool b );
};

} // end namespace Emotionxx

#endif // EMOTIONXX_OBJECT_H

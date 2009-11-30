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

void Object::engineInit(const std::string &module_filename)
{
  if (!emotion_object_init(o, module_filename.c_str ()))
    return;  // FIXME: why a return here?
}

void Object::setFile( const std::string &filename )
{
  emotion_object_file_set( o, filename.c_str () );
}

void Object::setPlay( bool b )
{
  emotion_object_play_set( o, b );
}

void Object::setSmoothScale( bool b )
{
  emotion_object_smooth_scale_set( o, b );
}

} // end namespace Emotionxx

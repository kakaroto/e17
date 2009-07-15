#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "../include/emotionxx/EvasEmotion.h"

extern "C" {
#include <Emotion.h>
}

#include <iostream>
#include <assert.h>

using namespace std;

namespace efl {

//===============================================================================================
// EvasEmotion
//===============================================================================================

EvasEmotion::EvasEmotion( EvasCanvas &canvas, const std::string &module_filename )
{
  o = emotion_object_add( canvas.obj() );
  init();
  engineInit(module_filename);
}

EvasEmotion::EvasEmotion( EvasCanvas &canvas, const std::string &filename, const std::string &module_filename )
{
  o = emotion_object_add( canvas.obj() );
  init();
  engineInit(module_filename);
  setFile( filename );
}

EvasEmotion::EvasEmotion( EvasCanvas &canvas, const Point &pos, const std::string &filename, const std::string &module_filename )
{
  o = emotion_object_add( canvas.obj() );
  init( );
  engineInit(module_filename);
  setFile( filename );
  move( pos );
}

EvasEmotion::EvasEmotion( EvasCanvas &canvas, const Rect &rect, const std::string &filename, const std::string &module_filename )
{
  o = emotion_object_add( canvas.obj() );

  init( );
  engineInit(module_filename);
  setFile( filename );
  setGeometry (rect);
}

void EvasEmotion::engineInit(const std::string &module_filename)
{
  if (!emotion_object_init(o, module_filename.c_str ()))
    return;  // FIXME: why a return here?
}

void EvasEmotion::setFile( const std::string &filename )
{
  emotion_object_file_set( o, filename.c_str () );
}

void EvasEmotion::setPlay( bool b )
{
  emotion_object_play_set( o, b );
}

void EvasEmotion::setSmoothScale( bool b )
{
  emotion_object_smooth_scale_set( o, b );
}

EvasEmotion::~EvasEmotion()
{
}

}

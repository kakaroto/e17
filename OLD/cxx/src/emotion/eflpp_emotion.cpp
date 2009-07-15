#include "eflpp_emotion.h"

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

EvasEmotion::EvasEmotion( const char * module_filename, EvasCanvas* canvas, const char* name )
{
    o = emotion_object_add( canvas->obj() );
    init( name ? name : "emotion" );
	engineInit(module_filename);
}

EvasEmotion::EvasEmotion( const char* filename, const char * module_filename, 
						 EvasCanvas* canvas, const char* name )
{
    o = emotion_object_add( canvas->obj() );
    init( name ? name : filename );
  	engineInit(module_filename);
    setFile( filename );
}

EvasEmotion::EvasEmotion( int x, int y, const char* filename, const char * module_filename, 
						 EvasCanvas* canvas, const char* name )
{
    o = emotion_object_add( canvas->obj() );
    init( name ? name : filename );
	engineInit(module_filename);
    setFile( filename );
    move( x, y );
}

EvasEmotion::EvasEmotion( int x, int y, int width, int height, const char* filename, 
						 const char * module_filename, EvasCanvas* canvas, const char* name )
{
	printf("EvasEmotion::EvasEmotion, begin \n");
   	o = emotion_object_add( canvas->obj() );
	
    init( name ? name : filename );
	engineInit(module_filename);
    setFile( filename );
    move( x, y );
    resize( width, height );
}

void EvasEmotion::engineInit (const char * module_filename)
{
	if (!emotion_object_init(o, module_filename))
     	return;
}

void EvasEmotion::setFile( const char* filename )
{
   	emotion_object_file_set( o, filename );
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

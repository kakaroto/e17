#ifndef EMOTION_BASE
#define EMOTION_BASE

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_evascanvas.h>
#include <eflpp_evasobject.h>

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
    EvasEmotion( const char * module_filename, EvasCanvas* canvas, const char* name = 0 );
    EvasEmotion( const char* filename, const char * module_filename, EvasCanvas* canvas, const char* name = 0 );
    EvasEmotion( int x, int y, const char* filename, const char * module_filename, EvasCanvas* canvas, const char* name = 0 );
    EvasEmotion( int x, int y, int width, int height, const char* filename, 
				const char * module_filename, EvasCanvas* canvas, const char* name = 0 );
    //EvasEmotion( Evas_Object* object, EvasCanvas* canvas, const char* name = 0 );
    ~EvasEmotion();
    
    /**
    * Initialize video engine to either use xine or gstreamer 
    *
    * @param module_filename	name of viedo engine to be used
    */
    void engineInit(const char * module_filename);
		
    void setFile( const char* filename );
    void setPlay( bool b );
    void setSmoothScale( bool b );
};

}

#endif

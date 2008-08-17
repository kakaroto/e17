#ifndef EVAS_EVENT_H
#define EVAS_EVENT_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <Evas.h>

/**
 * Events
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

template<typename T> class EvasObjectEvent
{
  public:
    EvasObjectEvent( T* ee ) : data( ee ) { };
    ~EvasObjectEvent() { };

    T* data;
};

typedef EvasObjectEvent<Evas_Event_Mouse_In> EvasMouseInEvent;
typedef EvasObjectEvent<Evas_Event_Mouse_Out> EvasMouseOutEvent;
typedef EvasObjectEvent<Evas_Event_Mouse_Up> EvasMouseUpEvent;
typedef EvasObjectEvent<Evas_Event_Mouse_Down> EvasMouseDownEvent;
typedef EvasObjectEvent<Evas_Event_Mouse_Move> EvasMouseMoveEvent;
typedef EvasObjectEvent<Evas_Event_Mouse_Wheel> EvasMouseWheelEvent;

typedef EvasObjectEvent<Evas_Event_Key_Up> EvasKeyUpEvent;
typedef EvasObjectEvent<Evas_Event_Key_Down> EvasKeyDownEvent;
}

#endif

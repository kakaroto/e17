#ifndef EVAS_EVENT_H
#define EVAS_EVENT_H

#include <Evas.h>

/**
 * Events
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace Evasxx {

template<typename T> class ObjectEvent
{
public:
  ObjectEvent( T* ee ) : data( ee ) { };
  ~ObjectEvent() { };

  T* data;
};

typedef ObjectEvent<Evas_Event_Mouse_In> EvasMouseInEvent;
typedef ObjectEvent<Evas_Event_Mouse_Out> EvasMouseOutEvent;
typedef ObjectEvent<Evas_Event_Mouse_Up> EvasMouseUpEvent;
typedef ObjectEvent<Evas_Event_Mouse_Down> EvasMouseDownEvent;
typedef ObjectEvent<Evas_Event_Mouse_Move> EvasMouseMoveEvent;
typedef ObjectEvent<Evas_Event_Mouse_Wheel> EvasMouseWheelEvent;
typedef ObjectEvent<Evas_Event_Key_Up> EvasKeyUpEvent;
typedef ObjectEvent<Evas_Event_Key_Down> EvasKeyDownEvent;
typedef ObjectEvent<Evas_Event_Hold> EvasEventHold;

} // end namespace Evasxx

#endif // EVAS_EVENT_H


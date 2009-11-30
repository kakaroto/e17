#ifndef EVASXX_EVENT_H
#define EVASXX_EVENT_H

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

typedef ObjectEvent<Evas_Event_Mouse_In> MouseInEvent;
typedef ObjectEvent<Evas_Event_Mouse_Out> MouseOutEvent;
typedef ObjectEvent<Evas_Event_Mouse_Up> MouseUpEvent;
typedef ObjectEvent<Evas_Event_Mouse_Down> MouseDownEvent;
typedef ObjectEvent<Evas_Event_Mouse_Move> MouseMoveEvent;
typedef ObjectEvent<Evas_Event_Mouse_Wheel> MouseWheelEvent;
typedef ObjectEvent<Evas_Event_Key_Up> KeyUpEvent;
typedef ObjectEvent<Evas_Event_Key_Down> KeyDownEvent;
typedef ObjectEvent<Evas_Event_Hold> EventHold;

} // end namespace Evasxx

#endif // EVASXX_EVENT_H


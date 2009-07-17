#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasObject.h"
#include "Evas.h"

#include <cassert>
#include <cmath>

using namespace std;

namespace efl {

EvasObject::EvasObject ()
{
  // no need to call init() here as it has to be done by the child class!
}

EvasObject::EvasObject (Evas_Object *eo)
{
  o = eo; // set member Evas_Edje*

  mFree = false; // don't manage the object and free it at delete time

  /* Set up magic object back link */
  evas_object_data_set( o, "obj_c++", this );

  /* Set up callbacks */
  registerCallbacks();
}

EvasObject *EvasObject::wrap( Evas_Object* o )
{
  return new EvasObject (o);
}

void EvasObject::init ()
{
  mFree = true; // manage the object and free it at delete time

  /* Set up magic object back link */
  evas_object_data_set( o, "obj_c++", this );

  /* Set up callbacks */
  registerCallbacks();
}

EvasObject::~EvasObject()
{
  Dout( dc::notice, *this << " EvasObject::~EvasObject" );

  /*if ( mFree )
  {
    evas_object_del( o );
  }*/
}

const std::string EvasObject::getName() const
{
  return evas_object_name_get( o );
}

void EvasObject::setName( const std::string& name )
{
  evas_object_name_set( o, name.c_str() );
}

const std::string EvasObject::getType () const
{
  return evas_object_type_get( o );
}

void EvasObject::move( const Point& point )
{
  Dout( dc::notice, *this << " move" << " x=" << x << " y=" << y );
  evas_object_move( o, point.x(), point.y() );
}

void EvasObject::resize( const Size& size, bool fill )
{
  Dout( dc::notice, *this << " resize" << " width=" << width << " height=" << height );
  evas_object_resize( o, size.width(), size.height() );
}

void EvasObject::rresize( double factor )
{
  Rect r = getGeometry();
  int width = static_cast <int> (round( r.width() * factor ));
  int height = static_cast <int> (round( r.height() * factor ));
  resize( width, height );
}

const Size EvasObject::getSize() const
{
  int x;
  int y;
  int width;
  int height;
  evas_object_geometry_get( o, &x, &y, &width, &height );
  return Size( x, y );
}

Rect EvasObject::getGeometry() const
{
  int x;
  int y;
  int width;
  int height;
  evas_object_geometry_get( o, &x, &y, &width, &height );
  return Rect( x, y, width, height );
}

void EvasObject::setGeometry( const Rect& r )
{
  move( r.pos () );
  resize( r.size (), true );
}

const Size EvasObject::getMinHintSize () const
{
  Evas_Coord w, h;
  evas_object_size_hint_min_get (o, &w, &h);
  return Size (w, h);
}

void EvasObject::setMinHintSize (const Size &size)
{
  evas_object_size_hint_min_set (o, size.width (), size.height ());
}

const Size EvasObject::getMaxHintSize () const
{
  Evas_Coord w, h;
  evas_object_size_hint_max_get (o, &w, &h);
  return Size (w, h);
}

void EvasObject::setMaxHintSize (const Size &size)
{
  evas_object_size_hint_max_set (o, size.width (), size.height ());
}
  
const Size EvasObject::getRequestHintSize () const
{
  Evas_Coord w, h;
  evas_object_size_hint_request_get (o, &w, &h);
  return Size (w, h);
}

void EvasObject::setRequestHintSize (const Size &size)
{
  evas_object_size_hint_request_set (o, size.width (), size.height ());
}
  
const Size EvasObject::getAspectHintSize (Evas_Aspect_Control &outAspect) const
{
  Evas_Coord w, h;
  evas_object_size_hint_aspect_get (o, &outAspect, &w, &h);
  return Size (w, h);
}

void EvasObject::setAspectHintSize (Evas_Aspect_Control aspect, const Size &size)
{
  evas_object_size_hint_aspect_set (o, aspect, size.width (), size.height ());
}
  
void EvasObject::getAlignHintSize (double &outX, double &outY) const
{
  evas_object_size_hint_align_get (o, &outX, &outY);
}

void EvasObject::setAlignHintSize (double x, double y)
{
  evas_object_size_hint_align_set (o, x, y);
}
  
void EvasObject::getWeightHintSize (double &outX, double &outY) const
{
  evas_object_size_hint_weight_get (o, &outX, &outY);
}

void EvasObject::setWeightHintSize (double x, double y)
{
  evas_object_size_hint_weight_set (o, x, y);
}
  
const Padding EvasObject::getPaddingHintSize () const
{
  Evas_Coord l, r, t, b;
  evas_object_size_hint_padding_get (o, &l, &r, &t, &b);
  return Padding (l, r, t, b);
}

void EvasObject::setPaddingHintSize (const Padding &border)
{
  evas_object_size_hint_padding_set (o, border.left (), border.right (), border.top (), border.bottom ());
}

EvasObject* EvasObject::getClip() const
{
  return objectLink( evas_object_clip_get( o ) );
}

void EvasObject::setClip( EvasObject* object )
{
  Dout( dc::notice, *this << " setClip" ); // FIXME: clipping with ? // TODO: is here a todo, if not remove comment!
  evas_object_clip_set( o, object->obj() );
}

Color EvasObject::getColor() const
{
  int r;
  int g;
  int b;
  int a;
  evas_object_color_get( o, &r, &g, &b, &a );
  return Color( r, g, b, a );
}

void EvasObject::setColor( const Color& c )
{
  Dout( dc::notice, *this << " color_set" << " r=" << r << " g=" << g << " b=" << b << " alpha=" << alpha );
  evas_object_color_set( o, c.red(), c.green(), c.blue(), c.alpha() );
}

int EvasObject::getColorInterpolation() const
{
  return evas_object_color_interpolation_get( o );
}

void EvasObject::setColorInterpolation( int colorspace )
{
  evas_object_color_interpolation_set( o, colorspace );
}

bool EvasObject::isAntiAliased() const
{
  return evas_object_anti_alias_get( o );
}

void EvasObject::setAntiAliased( bool antialias )
{
  evas_object_anti_alias_set( o, antialias );
}

void EvasObject::setLayer( int layer )
{
  Dout( dc::notice, *this << " layer_set" << " layer=" << layer );
  evas_object_layer_set( o, layer );
}

int EvasObject::getLayer() const
{
  return evas_object_layer_get( o );
}

void EvasObject::raise()
{
  Dout( dc::notice, *this << " raise" );
  evas_object_raise( o );
}

void EvasObject::lower()
{
  Dout( dc::notice, *this << " lower" );
  evas_object_lower( o );
}

void EvasObject::stackAbove( const EvasObject* obj )
{
  Dout( dc::notice, *this << " stackAbove" );
  evas_object_stack_above( o, obj->obj() );
}

void EvasObject::stackBelow( const EvasObject* obj )
{
  Dout( dc::notice, *this << " stackBelow" );
  evas_object_stack_below( o, obj->obj() );
}

void EvasObject::show()
{
  Dout( dc::notice, *this << " show" );
  evas_object_show( o );
}

void EvasObject::hide()
{
  Dout( dc::notice, *this << " hide" );
  evas_object_hide( o );
}

bool EvasObject::isVisible() const
{
  return evas_object_visible_get( o );
}

void EvasObject::setVisible( bool visible )
{
  Dout( dc::notice, *this << " setVisible(" << visible << ")" );
  if ( visible )
  {
    show();
  }
  else
  {
    hide();
  }
}

void EvasObject::setFocus( bool focus )
{
  Dout( dc::notice, *this << " setFocus:" << focus );
  evas_object_focus_set( o, focus );
}

bool EvasObject::hasFocus() const
{
  return evas_object_focus_get( o );
}

EvasObject *EvasObject::getParent (const EvasObject &obj)
{
  Evas_Object *eo = evas_object_smart_parent_get (o);
  
  return EvasObject::wrap (eo);
}

EvasSmart *EvasObject::getEvasSmart ()
{
  // FIXME
  //EAPI Evas_Smart       *evas_object_smart_smart_get       (const Evas_Object *obj);
#warning not implemented
}

EvasObject* EvasObject::objectLink( Evas_Object* evas_object )
{
  void *v = evas_object_data_get( evas_object, "obj_c++" );
  return static_cast<EvasObject*>( v );
}

const EvasObject* EvasObject::objectLink( const Evas_Object *evas_object )
{
  return static_cast<const EvasObject*>( evas_object_data_get( evas_object, "obj_c++" ) );
}

// PRIVATE
void EvasObject::registerCallbacks()
{
  // TODO: think about if it's good design to register all at start
  // does this cost performance?
  evas_object_event_callback_add( o, EVAS_CALLBACK_MOUSE_IN,     &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_MOUSE_IN ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_MOUSE_OUT,    &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_MOUSE_OUT ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_MOUSE_DOWN,   &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_MOUSE_DOWN ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_MOUSE_UP,     &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_MOUSE_UP ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_MOUSE_MOVE,   &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_MOUSE_MOVE ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_MOUSE_WHEEL,  &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_MOUSE_WHEEL ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_KEY_DOWN,     &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_KEY_DOWN ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_KEY_UP,       &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_KEY_UP ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_FREE,         &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_FREE ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_SHOW,         &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_SHOW ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_HIDE,         &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_HIDE ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_MOVE,         &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_MOVE ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_RESIZE,       &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_RESIZE ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_RESTACK,      &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_RESTACK ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_DEL,          &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_DEL ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_HOLD,         &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_HOLD ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_CHANGED_SIZE_HINTS,   &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_CHANGED_SIZE_HINTS ) );
  evas_object_event_callback_add( o, EVAS_CALLBACK_IMAGE_PRELOADED,      &dispatcher, reinterpret_cast<void*>( EVAS_CALLBACK_IMAGE_PRELOADED ) );
}

//PRIVATE
void EvasObject::dispatcher( void *data, Evas *evas, Evas_Object *evas_object, void *event_info )
{
  EvasObject* object = static_cast<EvasObject*>( evas_object_data_get( evas_object, "obj_c++" ) );
  long type = reinterpret_cast<long>(data);

  switch ( type )
  {
  case EVAS_CALLBACK_MOUSE_IN:
  {
    const EvasMouseInEvent e( static_cast<Evas_Event_Mouse_In*>( event_info ) );
    object->signalHandleMouseIn.emit( e );
    break;
  }
  case EVAS_CALLBACK_MOUSE_OUT:
  {
    const EvasMouseOutEvent e( static_cast<Evas_Event_Mouse_Out*>( event_info ) );
    object->signalHandleMouseOut.emit( e );
    break;
  }
  case EVAS_CALLBACK_MOUSE_DOWN:
  {
    const EvasMouseDownEvent e( static_cast<Evas_Event_Mouse_Down*>( event_info ) );
    object->signalHandleMouseDown.emit( e );
    break;
  }
  case EVAS_CALLBACK_MOUSE_UP:
  {
    const EvasMouseUpEvent e( static_cast<Evas_Event_Mouse_Up*>( event_info ) );
    object->signalHandleMouseUp.emit( e );
    break;
  }
  case EVAS_CALLBACK_MOUSE_MOVE:
  {
    const EvasMouseMoveEvent e( static_cast<Evas_Event_Mouse_Move*>( event_info ) );
    object->signalHandleMouseMove.emit( e );
    break;
  }
  case EVAS_CALLBACK_MOUSE_WHEEL:
  {
    const EvasMouseWheelEvent e( static_cast<Evas_Event_Mouse_Wheel*>( event_info ) );
    object->signalHandleMouseWheel.emit( e );
    break;
  }
  case EVAS_CALLBACK_KEY_DOWN:
  {
    const EvasKeyDownEvent e( static_cast<Evas_Event_Key_Down*>( event_info ) );
    object->signalHandleKeyDown.emit( e );
    break;
  }
  case EVAS_CALLBACK_KEY_UP:
  {
    const EvasKeyUpEvent e( static_cast<Evas_Event_Key_Up*>( event_info ) );
    object->signalHandleKeyUp.emit( e );
    break;
  }
  case EVAS_CALLBACK_FREE:
  {
    object->signalHandleFree.emit();
    break;
  }
  case EVAS_CALLBACK_SHOW:
  {
    object->signalHandleShow.emit();
    break;
  }
  case EVAS_CALLBACK_HIDE:
  {
    object->signalHandleHide.emit();
    break;
  }
  case EVAS_CALLBACK_MOVE:
  {
    object->signalHandleMove.emit();
    break;
  }
  case EVAS_CALLBACK_RESIZE:
  {
    object->signalHandleResize.emit();
    break;
  }
  case EVAS_CALLBACK_RESTACK:
  {
    object->signalHandleRestack.emit();
    break;
  }
  case EVAS_CALLBACK_DEL:
  {
    object->signalHandleDel.emit();
    break;
  }
  case EVAS_CALLBACK_HOLD:
  {
    object->signalHandleHold.emit();
    break;
  }
  case EVAS_CALLBACK_CHANGED_SIZE_HINTS:
  {
    object->signalHandleChangedSizeHints.emit();
    break;
  }
  case EVAS_CALLBACK_IMAGE_PRELOADED:
  {
    object->signalHandleImagePreload.emit();
    break;
  }
  default:
    assert( 0 );
  }
}

} // end namespace efl

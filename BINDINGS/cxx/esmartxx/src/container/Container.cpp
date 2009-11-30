#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Container.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace Esmartxx {

Container::Container( Evasxx::Canvas &canvas)
{
  o = esmart_container_new( canvas.obj() );
  init();
}

Container::Container( Evasxx::Canvas &canvas, Direction dir)
{
  o = esmart_container_new( canvas.obj() );
  init();

  setDirection( dir );
}

Container::Container( Evasxx::Canvas &canvas, const Eflxx::Point &pos, Direction dir)
{
  o = esmart_container_new( canvas.obj() );

  init();

  setDirection( dir );
  move( pos );
}

Container::Container( Evasxx::Canvas &canvas, const Eflxx::Rect &rect, Direction dir)
{
  o = esmart_container_new( canvas.obj() );
  init();

  setDirection( dir );
  setGeometry (rect);
}

Container::~Container()
{
  evas_object_del( o );
}

void Container::setDirection( Direction dir )
{
  esmart_container_direction_set( o, static_cast<Container_Direction>( dir ) );
}

Container::Direction Container::direction() const
{
  return static_cast<Container::Direction>( esmart_container_direction_get( o ) );
}

void Container::setPadding( double l, double r, double t, double b )
{
  esmart_container_padding_set( o, l, r, t, b );
}
/* Padding Container::padding() const;
{
    double l;
    double r;
    double t;
    double b;
    esmart_container_padding_get( o, &l, &r, &t, &b );
    return Padding( l, r, t, b );
}*/

void Container::setFillPolicy( FillPolicy fill )
{
  esmart_container_fill_policy_set( o, static_cast<Container_Fill_Policy>( fill ) );
}

Container::FillPolicy Container::getFillPolicy() const
{
  return static_cast<Container::FillPolicy>( esmart_container_fill_policy_get( o ) );
}

void Container::setAlignment( Alignment align )
{
  esmart_container_alignment_set( o, static_cast<Container_Alignment>( align ) );
}

Container::Alignment Container::getAlignment() const
{
  return static_cast<Container::Alignment>( esmart_container_alignment_get( o ) );
}

void Container::setSpacing( int spacing )
{
  esmart_container_spacing_set( o, spacing );
}

int Container::getSpacing() const
{
  return esmart_container_spacing_get( o );
}

void Container::setMovingButton( int move )
{
  esmart_container_move_button_set( o, move );
}

int Container::getMovingButton() const
{
  return esmart_container_move_button_get( o );
}

void Container::setLayoutPlugin (const std::string &name)
{
  esmart_container_layout_plugin_set (o, name.c_str());
}

void Container::append( Evasxx::Object* object, Evasxx::Object* after )
{
  if ( !after )
    esmart_container_element_append( o, object->obj() );
  else
    esmart_container_element_append_relative( o, object->obj(), after->obj() );
}

void Container::prepend( Evasxx::Object* object, Evasxx::Object* before )
{
  if ( !before )
    esmart_container_element_prepend( o, object->obj() );
  else
    esmart_container_element_prepend_relative( o, object->obj(), before->obj() );
}

void Container::remove( Evasxx::Object* object )
{
  esmart_container_element_remove( o, object->obj() );
}

void Container::destroy( Evasxx::Object* object )
{
  esmart_container_element_destroy( o, object->obj() );
}

void Container::clear()
{
  esmart_container_empty( o );
}

/*
void Container::sort( ... )
{
    esmart_container_sort( o, ... );

EvasList* children() const
{
...
}
*/

void Container::startScrolling( double velocity )
{
  esmart_container_scroll_start( o, velocity );
}

void Container::stopScrolling()
{
  esmart_container_scroll_stop( o );
}

void Container::scroll( int val )
{
  esmart_container_scroll( o, val );
}

void Container::setScrollOffset( int val )
{
  esmart_container_scroll_offset_set( o, val );
}

int Container::getScrollOffset() const
{
  return esmart_container_scroll_offset_get( o );
}

void Container::setScrollPercent( double percent )
{
  esmart_container_scroll_percent_set( o, percent );
}

double Container::getScrollPercent() const
{
  return esmart_container_scroll_percent_get( o );
}

void Container::scrollTo( Evasxx::Object* object )
{
  esmart_container_scroll_to( o, object->obj() );
}

} // end namespace Esmartxx

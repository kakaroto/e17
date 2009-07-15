#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "EvasSmartContainer.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace efl {

EvasSmartContainer::EvasSmartContainer( EvasCanvas &canvas)
{
  o = esmart_container_new( canvas.obj() );
  init();
}

EvasSmartContainer::EvasSmartContainer( EvasCanvas &canvas, Direction dir)
{
  o = esmart_container_new( canvas.obj() );
  init();

  setDirection( dir );
}

EvasSmartContainer::EvasSmartContainer( EvasCanvas &canvas, const Point &pos, Direction dir)
{
  o = esmart_container_new( canvas.obj() );

  init();

  setDirection( dir );
  move( pos );
}

EvasSmartContainer::EvasSmartContainer( EvasCanvas &canvas, const Rect &rect, Direction dir)
{
  o = esmart_container_new( canvas.obj() );
  init();

  setDirection( dir );
  setGeometry (rect);
}

EvasSmartContainer::~EvasSmartContainer()
{
  evas_object_del( o );
}

void EvasSmartContainer::setDirection( Direction dir )
{
  esmart_container_direction_set( o, static_cast<Container_Direction>( dir ) );
}

EvasSmartContainer::Direction EvasSmartContainer::direction() const
{
  return static_cast<EvasSmartContainer::Direction>( esmart_container_direction_get( o ) );
}

void EvasSmartContainer::setPadding( double l, double r, double t, double b )
{
  esmart_container_padding_set( o, l, r, t, b );
}
/* Padding EvasSmartContainer::padding() const;
{
    double l;
    double r;
    double t;
    double b;
    esmart_container_padding_get( o, &l, &r, &t, &b );
    return Padding( l, r, t, b );
}*/

void EvasSmartContainer::setFillPolicy( FillPolicy fill )
{
  esmart_container_fill_policy_set( o, static_cast<Container_Fill_Policy>( fill ) );
}

EvasSmartContainer::FillPolicy EvasSmartContainer::getFillPolicy() const
{
  return static_cast<EvasSmartContainer::FillPolicy>( esmart_container_fill_policy_get( o ) );
}

void EvasSmartContainer::setAlignment( Alignment align )
{
  esmart_container_alignment_set( o, static_cast<Container_Alignment>( align ) );
}

EvasSmartContainer::Alignment EvasSmartContainer::getAlignment() const
{
  return static_cast<EvasSmartContainer::Alignment>( esmart_container_alignment_get( o ) );
}

void EvasSmartContainer::setSpacing( int spacing )
{
  esmart_container_spacing_set( o, spacing );
}

int EvasSmartContainer::getSpacing() const
{
  return esmart_container_spacing_get( o );
}

void EvasSmartContainer::setMovingButton( int move )
{
  esmart_container_move_button_set( o, move );
}

int EvasSmartContainer::getMovingButton() const
{
  return esmart_container_move_button_get( o );
}

void EvasSmartContainer::setLayoutPlugin (const std::string &name)
{
  esmart_container_layout_plugin_set (o, name.c_str());
}

void EvasSmartContainer::append( EvasObject* object, EvasObject* after )
{
  if ( !after )
    esmart_container_element_append( o, object->obj() );
  else
    esmart_container_element_append_relative( o, object->obj(), after->obj() );
}

void EvasSmartContainer::prepend( EvasObject* object, EvasObject* before )
{
  if ( !before )
    esmart_container_element_prepend( o, object->obj() );
  else
    esmart_container_element_prepend_relative( o, object->obj(), before->obj() );
}

void EvasSmartContainer::remove( EvasObject* object )
{
  esmart_container_element_remove( o, object->obj() );
}

void EvasSmartContainer::destroy( EvasObject* object )
{
  esmart_container_element_destroy( o, object->obj() );
}

void EvasSmartContainer::clear()
{
  esmart_container_empty( o );
}

/*
void EvasSmartContainer::sort( ... )
{
    esmart_container_sort( o, ... );

EvasList* children() const
{
...
}
*/

void EvasSmartContainer::startScrolling( double velocity )
{
  esmart_container_scroll_start( o, velocity );
}

void EvasSmartContainer::stopScrolling()
{
  esmart_container_scroll_stop( o );
}

void EvasSmartContainer::scroll( int val )
{
  esmart_container_scroll( o, val );
}

void EvasSmartContainer::setScrollOffset( int val )
{
  esmart_container_scroll_offset_set( o, val );
}

int EvasSmartContainer::getScrollOffset() const
{
  return esmart_container_scroll_offset_get( o );
}

void EvasSmartContainer::setScrollPercent( double percent )
{
  esmart_container_scroll_percent_set( o, percent );
}

double EvasSmartContainer::getScrollPercent() const
{
  return esmart_container_scroll_percent_get( o );
}

void EvasSmartContainer::scrollTo( EvasObject* object )
{
  esmart_container_scroll_to( o, object->obj() );
}

} // end namespace efl

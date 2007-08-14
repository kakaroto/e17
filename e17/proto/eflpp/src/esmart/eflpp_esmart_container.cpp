#include "eflpp_esmart_container.h"

#include <iostream>
#include <assert.h>
using namespace std;

namespace efl {

//===============================================================================================
// EvasEsmartContainer
//===============================================================================================

EvasEsmartContainer::EvasEsmartContainer( EvasCanvas* canvas, const char* name )
    :EvasEsmart( canvas )
{
    o = esmart_container_new( canvas->obj() );
    init( name ? name : "esmart_container" );
}

EvasEsmartContainer::EvasEsmartContainer( Direction dir, EvasCanvas* canvas, const char* name )
    :EvasEsmart( canvas )
{
    o = esmart_container_new( canvas->obj() );
    init( name ? name : "esmart_container" );
  
    setDirection( dir );
}

EvasEsmartContainer::EvasEsmartContainer( int x, int y, Direction dir, EvasCanvas* canvas, const char* name )
    :EvasEsmart( canvas )
{
    o = esmart_container_new( canvas->obj() );
  
    init( name ? name : "esmart_container" );
  
    setDirection( dir );
    move( x, y );
}

EvasEsmartContainer::EvasEsmartContainer( int x, int y, int width, int height, Direction dir, EvasCanvas* canvas, const char* name )
    :EvasEsmart( canvas )
{
    o = esmart_container_new( canvas->obj() );
    init( name ? name : "esmart_container" );
  
    setDirection( dir );
    move( x, y );
    resize( width, height );
}

EvasEsmartContainer::~EvasEsmartContainer()
{
    evas_object_del( o );
}

void EvasEsmartContainer::setDirection( Direction dir )
{
  esmart_container_direction_set( o, static_cast<Container_Direction>( dir ) );
}

EvasEsmartContainer::Direction EvasEsmartContainer::direction() const
{
  return static_cast<EvasEsmartContainer::Direction>( esmart_container_direction_get( o ) );
}

void EvasEsmartContainer::setPadding( double l, double r, double t, double b )
{
  esmart_container_padding_set( o, l, r, t, b );
}
/* Padding EvasEsmartContainer::padding() const;
{
    double l;
    double r;
    double t;
    double b;
    esmart_container_padding_get( o, &l, &r, &t, &b );
    return Padding( l, r, t, b );
}*/

void EvasEsmartContainer::setFillPolicy( FillPolicy fill )
{
  esmart_container_fill_policy_set( o, static_cast<Container_Fill_Policy>( fill ) );
}

EvasEsmartContainer::FillPolicy EvasEsmartContainer::fillPolicy() const
{
  return static_cast<EvasEsmartContainer::FillPolicy>( esmart_container_fill_policy_get( o ) );
}

void EvasEsmartContainer::setAlignment( Alignment align )
{
  esmart_container_alignment_set( o, static_cast<Container_Alignment>( align ) );
}

EvasEsmartContainer::Alignment EvasEsmartContainer::alignment() const
{
  return static_cast<EvasEsmartContainer::Alignment>( esmart_container_alignment_get( o ) );
}

void EvasEsmartContainer::setSpacing( int spacing )
{
  esmart_container_spacing_set( o, spacing );
}

int EvasEsmartContainer::spacing() const
{
  return esmart_container_spacing_get( o );
}

void EvasEsmartContainer::setMovingButton( int move )
{
  esmart_container_move_button_set( o, move );
}

int EvasEsmartContainer::movingButton() const
{
  return esmart_container_move_button_get( o );
}

void EvasEsmartContainer::setLayoutPlugin( const char* name )
{
  esmart_container_layout_plugin_set( o, name );
}

void EvasEsmartContainer::append( EvasObject* object, EvasObject* after )
{
  if ( !after )
    esmart_container_element_append( o, object->obj() );
  else
    esmart_container_element_append_relative( o, object->obj(), after->obj() );
}

void EvasEsmartContainer::prepend( EvasObject* object, EvasObject* before )
{
  if ( !before )
    esmart_container_element_prepend( o, object->obj() );
  else
    esmart_container_element_prepend_relative( o, object->obj(), before->obj() );
}

void EvasEsmartContainer::remove( EvasObject* object )
{
  esmart_container_element_remove( o, object->obj() );
}

void EvasEsmartContainer::destroy( EvasObject* object )
{
  esmart_container_element_destroy( o, object->obj() );
}

void EvasEsmartContainer::clean()
{
  esmart_container_empty( o );
}

/*
void EvasEsmartContainer::sort( ... )
{
    esmart_container_sort( o, ... );
 
EvasList* children() const
{
...
}
*/

void EvasEsmartContainer::startScrolling( double velocity )
{
  esmart_container_scroll_start( o, velocity );
}

void EvasEsmartContainer::stopScrolling()
{
  esmart_container_scroll_stop( o );
}

void EvasEsmartContainer::scroll( int val )
{
  esmart_container_scroll( o, val );
}

void EvasEsmartContainer::setScrollOffset( int val )
{
  esmart_container_scroll_offset_set( o, val );
}

int EvasEsmartContainer::scrollOffset() const
{
  return esmart_container_scroll_offset_get( o );
}

void EvasEsmartContainer::setScrollPercent( double percent )
{
  esmart_container_scroll_percent_set( o, percent );
}

double EvasEsmartContainer::scrollPercent() const
{
  return esmart_container_scroll_percent_get( o );
}

void EvasEsmartContainer::scrollTo( EvasObject* object )
{
  esmart_container_scroll_to( o, object->obj() );
}

} // end namespace efl

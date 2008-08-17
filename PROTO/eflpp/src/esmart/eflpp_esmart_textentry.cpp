#include "eflpp_esmart_textentry.h"

#include <iostream>
#include <assert.h>
using namespace std;

namespace efl {

//===========================================================================//
// EvasEsmartTextEntry
//===========================================================================//

EvasEsmartTextEntry::EvasEsmartTextEntry( EvasCanvas* canvas, const char* name )
    :EvasEsmart( canvas )
{
    o = esmart_text_entry_new( canvas->obj() );
    init( name ? name : "esmart_textentry" );
}

EvasEsmartTextEntry::EvasEsmartTextEntry( int x, int y, EvasCanvas* canvas, const char* name )
    :EvasEsmart( canvas )
{
    o = esmart_text_entry_new( canvas->obj() );
    init( name ? name : "esmart_textentry" );

    move( x, y );
}

EvasEsmartTextEntry::EvasEsmartTextEntry( int x, int y, int width, int height, EvasCanvas* canvas, const char* name )
    :EvasEsmart( canvas )
{
    o = esmart_text_entry_new( canvas->obj() );
    init( name ? name : "esmart_textentry" );

    move( x, y );
    resize( width, height );
}

EvasEsmartTextEntry::~EvasEsmartTextEntry()
{
    evas_object_del( o );
}

void EvasEsmartTextEntry::setText (const char *str)
{
  esmart_text_entry_text_set (o, str);
}

void EvasEsmartTextEntry::isPassword (bool val)
{
  esmart_text_entry_is_password_set (o, val);
}

void EvasEsmartTextEntry::setMaxChars (int max)
{
  esmart_text_entry_max_chars_set (o, max);
}

void EvasEsmartTextEntry::setEdjePart (EvasEdje *edje, const char *part)
{
  esmart_text_entry_edje_part_set (o, edje->obj(), part);
}

Evas_Object *EvasEsmartTextEntry::getEdjeObject ()
{
  return esmart_text_entry_edje_object_get(o); 
}

const char *EvasEsmartTextEntry::getEdjePart ()
{
  return esmart_text_entry_edje_part_get(o);
}

const char *EvasEsmartTextEntry::getText ()
{
  
  return esmart_text_entry_text_get(o);
}

} // end namespace efl

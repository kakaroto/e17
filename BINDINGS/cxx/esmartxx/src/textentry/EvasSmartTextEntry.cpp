#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "EvasSmartTextEntry.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace efl {

//===========================================================================//
// EvasSmartTextEntry
//===========================================================================//

EvasSmartTextEntry::EvasSmartTextEntry( EvasCanvas &canvas )
{
  o = esmart_text_entry_new( canvas.obj() );
  init();
}

EvasSmartTextEntry::EvasSmartTextEntry( EvasCanvas &canvas, const Point &pos )
{
  o = esmart_text_entry_new( canvas.obj() );
  init();

  move( pos );
}

EvasSmartTextEntry::EvasSmartTextEntry( EvasCanvas &canvas, const Rect &rect )
{
  o = esmart_text_entry_new( canvas.obj() );
  init();

  setGeometry (rect);
}

EvasSmartTextEntry::~EvasSmartTextEntry()
{
  evas_object_del( o );
}

void EvasSmartTextEntry::setText (const std::string &str)
{
  esmart_text_entry_text_set (o, str.c_str ());
}

void EvasSmartTextEntry::isPassword (bool val)
{
  esmart_text_entry_is_password_set (o, val);
}

void EvasSmartTextEntry::setMaxChars (int max)
{
  esmart_text_entry_max_chars_set (o, max);
}

void EvasSmartTextEntry::setEdjePart (EdjeObject *edje, const char *part)
{
  esmart_text_entry_edje_part_set (o, edje->obj(), part);
}

Evas_Object *EvasSmartTextEntry::getEdjeObject ()
{
  return esmart_text_entry_edje_object_get(o);
}

const string EvasSmartTextEntry::getEdjePart ()
{
  return esmart_text_entry_edje_part_get(o);
}

const string EvasSmartTextEntry::getText ()
{

  return esmart_text_entry_text_get(o);
}

} // end namespace efl

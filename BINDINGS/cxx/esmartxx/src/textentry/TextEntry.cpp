#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TextEntry.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace Esmartxx {

//===========================================================================//
// TextEntry
//===========================================================================//

TextEntry::TextEntry( Evasxx::Canvas &canvas )
{
  o = esmart_text_entry_new( canvas.obj() );
  init();
}

TextEntry::TextEntry( Evasxx::Canvas &canvas, const Eflxx::Point &pos )
{
  o = esmart_text_entry_new( canvas.obj() );
  init();

  move( pos );
}

TextEntry::TextEntry( Evasxx::Canvas &canvas, const Eflxx::Rect &rect )
{
  o = esmart_text_entry_new( canvas.obj() );
  init();

  setGeometry (rect);
}

TextEntry::~TextEntry()
{
  evas_object_del( o );
}

void TextEntry::setText (const std::string &str)
{
  esmart_text_entry_text_set (o, str.c_str ());
}

void TextEntry::isPassword (bool val)
{
  esmart_text_entry_is_password_set (o, val);
}

void TextEntry::setMaxChars (int max)
{
  esmart_text_entry_max_chars_set (o, max);
}

void TextEntry::setEdjePart (Edjexx::Object *edje, const char *part)
{
  esmart_text_entry_edje_part_set (o, edje->obj(), part);
}

Evas_Object *TextEntry::getEdjeObject ()
{
  return esmart_text_entry_edje_object_get(o);
}

const string TextEntry::getEdjePart ()
{
  return esmart_text_entry_edje_part_get(o);
}

const string TextEntry::getText ()
{

  return esmart_text_entry_text_get(o);
}

} // end namespace Esmartxx

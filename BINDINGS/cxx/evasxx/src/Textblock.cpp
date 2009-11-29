#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Canvas.h"
#include "../include/evasxx/Textblock.h"
#include "../include/evasxx/TextblockStyle.h"

using namespace Eflxx;

namespace Evasxx {

Textblock::Textblock( Canvas& canvas )
{
  o = evas_object_textblock_add( canvas.obj() );
  init();
}

Textblock::Textblock(  Canvas &canvas, const Eflxx::Point &pos, const std::string &text )
{
  o = evas_object_textblock_add( canvas.obj() );
  init();

  move( pos );
  setText( text );
}

void Textblock::setStyle( const TextblockStyle &style )
{
  evas_object_textblock_style_set( o, style.o );
}

const TextblockStyle Textblock::getStyle () const
{
  const Evas_Textblock_Style *etbsc = evas_object_textblock_style_get (o);
  TextblockStyle etbs;
  etbs.o = const_cast <Evas_Textblock_Style*> (etbsc);
  etbs.mFree = false; // Evas manages the object and evasxx doesn't free it at delete time
  return etbs;
}

void Textblock::setText( const std::string &text )
{
  evas_object_textblock_text_markup_set( o, text.c_str () );
}

void Textblock::clear()
{
  evas_object_textblock_clear( o );
}

Textblock::~Textblock()
{
  evas_object_del( o );
}

} // end namespace efl

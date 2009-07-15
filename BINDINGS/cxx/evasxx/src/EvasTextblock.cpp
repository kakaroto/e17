#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasCanvas.h"
#include "../include/evasxx/EvasTextblock.h"
#include "../include/evasxx/EvasTextblockStyle.h"

namespace efl {

EvasTextblock::EvasTextblock( EvasCanvas& canvas )
{
  o = evas_object_textblock_add( canvas.obj() );
  init();
}

EvasTextblock::EvasTextblock(  EvasCanvas &canvas, const Point &pos, const std::string &text )
{
  o = evas_object_textblock_add( canvas.obj() );
  init();

  move( pos );
  setText( text );
}

void EvasTextblock::setStyle( const EvasTextblockStyle &style )
{
  evas_object_textblock_style_set( o, style.o );
}

const EvasTextblockStyle EvasTextblock::getStyle () const
{
  const Evas_Textblock_Style *etbsc = evas_object_textblock_style_get (o);
  EvasTextblockStyle etbs;
  etbs.o = const_cast <Evas_Textblock_Style*> (etbsc);
  etbs.mFree = false; // Evas manages the object and evasxx doesn't free it at delete time
  return etbs;
}

void EvasTextblock::setText( const std::string &text )
{
  evas_object_textblock_text_markup_set( o, text.c_str () );
}

void EvasTextblock::clear()
{
  evas_object_textblock_clear( o );
}

EvasTextblock::~EvasTextblock()
{
  evas_object_del( o );
}

} // end namespace efl

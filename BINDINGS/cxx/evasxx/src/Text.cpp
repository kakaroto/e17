#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Canvas.h"
#include "../include/evasxx/Text.h"
#include "../include/evasxx/Exceptions.h"

using namespace std;
using namespace Eflxx;

namespace Evasxx {

Text::Text( Canvas &canvas )
{
  o = evas_object_text_add( canvas.obj() );
  init();
}

Text::Text( Canvas &canvas, Text &ao )
{
  Dout( dc::notice, *this << " Text::Text copy constructor" );
  o = evas_object_text_add( canvas.obj() );
  init();

  setGeometry( ao.getGeometry() );
  setLayer( ao.getLayer() );
  setText( ao.getText() );
  setClip( ao.getClip() );
  setFont( ao.getFontName(), getFontSize () );
  setColor( ao.getColor() );
  setVisible( ao.isVisible() );
}

Text::Text( Canvas &canvas, const std::string &text )
{
  o = evas_object_text_add( canvas.obj() );
  init();

  setText( text );
}

Text::Text( Canvas &canvas, const std::string &font, Evas_Font_Size size, const std::string &text )
{
  o = evas_object_text_add( canvas.obj() );
  init();

  setFont( font, size );
  setText( text );
}

Text::Text( Canvas &canvas, const std::string &font, Evas_Font_Size size, const Point &pos, const std::string &text )
{
  o = evas_object_text_add( canvas.obj() );
  init();

  move( pos );
  setFont( font, size );
  setText( text );
}

Text::~Text()
{
  evas_object_del( o );
}

void Text::setFontSource( const std::string &source )
{
  evas_object_text_font_source_set( o, source.c_str () );
}

const string Text::getFontSource () const
{
   return evas_object_text_font_source_get (o);
}

void Text::setFont( const std::string &font, Evas_Font_Size size )
{
  evas_object_text_font_set( o, font.c_str (), size );
}

const std::string Text::getFontName() const
{
  const char* font;
  evas_object_text_font_get( o, &font, NULL );
  return font;
}

Evas_Font_Size Text::getFontSize () const
{
  Evas_Font_Size size;
  evas_object_text_font_get( o, NULL, &size );
  return size;
}

void Text::setText( const std::string &text )
{
  evas_object_text_text_set( o, text.c_str () );
}

const std::string Text::getText() const
{
  return evas_object_text_text_get( o );
}

int Text::getAscent() const
{
  return evas_object_text_ascent_get( o );
}

int Text::getDescent() const
{
  return evas_object_text_descent_get( o );
}

int Text::getMaxAscent() const
{
  return evas_object_text_max_ascent_get( o );
}

int Text::getMaxDescent() const
{
  return evas_object_text_max_descent_get( o );
}

int Text::getHorizontalAdvance() const
{
  return evas_object_text_horiz_advance_get( o );
}

int Text::getVerticalAdvance() const
{
  return evas_object_text_vert_advance_get( o );
}

int Text::getInset() const
{
  return evas_object_text_inset_get( o );
}

const Rect Text::getCharacterPosition(int pos) const
{
  Evas_Coord cx, cy, cw, ch;
  if (!evas_object_text_char_pos_get (o, pos, &cx, &cy, &cw, &ch))
  {
    throw FontCharacterPositionException ();
  }
  
  return Rect (cx, cy, cw, ch);
}

const Rect Text::getCharacterCoordinates(const Point &pos) const
{
  Evas_Coord cx, cy, cw, ch;
  // TODO: what to do with 'ret'?
  int ret = evas_object_text_char_coords_get (o, pos.x (), pos.y (), &cx, &cy, &cw, &ch);
  return Rect (cx, cy, cw, ch);
}

void Text::setStyle (Evas_Text_Style_Type type)
{
  evas_object_text_style_set (o, type);
}

Evas_Text_Style_Type Text::getStyle () const
{
  return evas_object_text_style_get (o);
}

void Text::setShadowColor (const Color &c)
{
  evas_object_text_shadow_color_set (o, c.red (), c.green (), c.blue (), c.alpha ());
}

const Color Text::getShadowColor () const
{
  int r, g, b, a;
  evas_object_text_shadow_color_get (o, &r, &g, &b, &a);
  return Color (r, b, g, a);
}

void Text::setGlowColor (const Color &c)
{
  evas_object_text_glow_color_set (o, c.red (), c.green (), c.blue (), c.alpha ());
}

const Color Text::getGlowColor () const
{
  int r, g, b, a;
  evas_object_text_glow_color_get (o, &r, &g, &b, &a);
  return Color (r, b, g, a);
}

void Text::setGlow2Color (const Color &c)
{
  evas_object_text_glow2_color_set (o, c.red (), c.green (), c.blue (), c.alpha ());
}

const Color Text::getGlow2Color () const
{
  int r, g, b, a;
  evas_object_text_glow2_color_get (o, &r, &g, &b, &a);
  return Color (r, b, g, a);
}

void Text::setOutlineColor (const Color &c)
{
  evas_object_text_outline_color_set (o, c.red (), c.green (), c.blue (), c.alpha ());
}

const Color Text::getOutlineColor () const
{
  int r, g, b, a;
  evas_object_text_outline_color_get (o, &r, &g, &b, &a);
  return Color (r, b, g, a);
}

void Text::getStylePad (int &outLeft, int &outRight, int &outTop, int &outBottom)
{
  evas_object_text_style_pad_get (o, &outLeft, &outRight, &outTop, &outBottom);
}

} // end namespace efl

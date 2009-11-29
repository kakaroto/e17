#ifndef EVASTEXT_H
#define EVASTEXT_H

/* STL */
#include <string>

/* EFL++ */
#include <eflxx/Common.h>
#include "Object.h"

/* EFL */
#include <Evas.h>

using std::string;

namespace Evasxx {

/* Forward declarations */
class Canvas;
  
/**
 * Wraps an Evas Text Object
 */
class Text : public Object
{
public:
  Text( Canvas &canvas );
  Text( Canvas &canvas, Text &ao );
  Text( Canvas &canvas, const std::string &text );
  Text( Canvas &canvas, const std::string &font, Evas_Font_Size size, const std::string &text );
  Text( Canvas &canvas, const std::string &font, Evas_Font_Size size, const Eflxx::Point &pos, const std::string &text );
  virtual ~Text();

  void setFontSource( const std::string &source );
  
  const string getFontSource () const;
  
  void setFont( const std::string &font, Evas_Font_Size size);
  const std::string getFontName() const;
  Evas_Font_Size getFontSize () const;

  const std::string getText() const;
  void setText( const std::string &text );

  int getAscent() const;
  int getDescent() const;
  int getMaxAscent() const;
  int getMaxDescent() const;
  int getHorizontalAdvance() const;
  int getVerticalAdvance() const;
  
  /*!
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  int getInset() const;

  /*!
   * This function is used to obtain the Rect of a the character located at @p pos.
   *
   * @param pos   The character position to request co-ordinates for.
   * @returns     A Rect for the requested character position.
   * @throws      FontCharacterPositionException
   */
  const Eflxx::Rect getCharacterPosition (int pos) const;
  
  /**
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  const Eflxx::Rect getCharacterCoordinates(const Eflxx::Point &pos) const;
  
  void setStyle (Evas_Text_Style_Type type);
  
  Evas_Text_Style_Type getStyle () const;
  
  void setShadowColor (const Eflxx::Color &c);
  
  const Eflxx::Color getShadowColor () const;
  
  void setGlowColor (const Eflxx::Color &c);
  
  const Eflxx::Color getGlowColor () const;
  
  void setGlow2Color (const Eflxx::Color &c);
  
  const Eflxx::Color getGlow2Color () const;
  
  void setOutlineColor (const Eflxx::Color &c);
  
  const Eflxx::Color getOutlineColor () const;
  
  /*!
   * Gets the text style pad.
   *
   * @param l The left pad.
   * @param r The right pad.
   * @param t The top pad.
   * @param b The bottom pad.
   *
   */
  void getStylePad (int &outLeft, int &outRight, int &outTop, int &outBottom);
  
};
 
} // end namespace Evasxx

#endif // EVAS_TEXT_H

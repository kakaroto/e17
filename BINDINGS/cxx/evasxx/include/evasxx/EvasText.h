#ifndef EVASTEXT_H
#define EVASTEXT_H

/* STL */
#include <string>

/* EFL++ */
#include <eflxx/eflpp_common.h>
#include "EvasObject.h"

/* EFL */
#include <Evas.h>

using std::string;

namespace efl {

/* Forward declarations */
class EvasCanvas;


  
/**
 * Wraps an Evas Text Object
 */
class EvasText : public EvasObject
{
public:
  EvasText( EvasCanvas &canvas );
  EvasText( EvasCanvas &canvas, EvasText &ao );
  EvasText( EvasCanvas &canvas, const std::string &text );
  EvasText( EvasCanvas &canvas, const std::string &font, Evas_Font_Size size, const std::string &text );
  EvasText( EvasCanvas &canvas, const std::string &font, Evas_Font_Size size, const Point &pos, const std::string &text );
  virtual ~EvasText();

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
  const Rect getCharacterPosition (int pos) const;
  
  /**
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  const Rect getCharacterCoordinates(const Point &pos) const;
  
  void setStyle (Evas_Text_Style_Type type);
  
  Evas_Text_Style_Type getStyle () const;
  
  void setShadowColor (const Color &c);
  
  const Color getShadowColor () const;
  
  void setGlowColor (const Color &c);
  
  const Color getGlowColor () const;
  
  void setGlow2Color (const Color &c);
  
  const Color getGlow2Color () const;
  
  void setOutlineColor (const Color &c);
  
  const Color getOutlineColor () const;
  
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
 
} // end namespace efl

#endif // EVASTEXT_H

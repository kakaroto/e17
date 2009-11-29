#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/TextblockStyle.h"

#include <assert.h>

namespace Evasxx {

TextblockStyle::TextblockStyle()
{
  o = evas_textblock_style_new();
  mFree = true; // manage the object and free it at delete time
}

TextblockStyle::TextblockStyle( const std::string &format )
{
  o = evas_textblock_style_new();
  evas_textblock_style_set( o, format.c_str () );
  mFree = true; // manage the object and free it at delete time
}

TextblockStyle::~TextblockStyle()
{
  // If Evas manages the C object than don't free it!
  if (mFree)
  {
	  evas_textblock_style_free( o );
  }
}

const std::string TextblockStyle::getFormat() const
{
  return evas_textblock_style_get( o );
}

void TextblockStyle::setFormat( const std::string &format )
{
  evas_textblock_style_set( o, format.c_str () );
}

} // end namespace Evasxx

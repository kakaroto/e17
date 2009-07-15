#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasTextblockStyle.h"

#include <assert.h>

namespace efl {

EvasTextblockStyle::EvasTextblockStyle()
{
  o = evas_textblock_style_new();
  mFree = true; // manage the object and free it at delete time
}

EvasTextblockStyle::EvasTextblockStyle( const std::string &format )
{
  o = evas_textblock_style_new();
  evas_textblock_style_set( o, format.c_str () );
  mFree = true; // manage the object and free it at delete time
}

EvasTextblockStyle::~EvasTextblockStyle()
{
  // If Evas manages the C object than don't free it!
  if (mFree)
  {
	  evas_textblock_style_free( o );
  }
}

const std::string EvasTextblockStyle::getFormat() const
{
  return evas_textblock_style_get( o );
}

void EvasTextblockStyle::setFormat( const std::string &format )
{
  evas_textblock_style_set( o, format.c_str () );
}

} // end namespace efl

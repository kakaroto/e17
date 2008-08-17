#include <eflpp_sys.h>
#include <eflpp_common.h>
#include <eflpp_ecore.h>

#include "eflpp_evastextblockstyle.h"

#include <assert.h>

namespace efl {

EvasTextblockStyle::EvasTextblockStyle()
{
    o = evas_textblock_style_new();
}

EvasTextblockStyle::EvasTextblockStyle( const char* format )
{
    o = evas_textblock_style_new();
    evas_textblock_style_set( o, format );
}

EvasTextblockStyle::~EvasTextblockStyle()
{
    evas_textblock_style_free( o );
}

const char* EvasTextblockStyle::format() const
{
    return evas_textblock_style_get( o );
}

void EvasTextblockStyle::setFormat( const char* format )
{
    assert( o );
    evas_textblock_style_set( o, format );

}

}

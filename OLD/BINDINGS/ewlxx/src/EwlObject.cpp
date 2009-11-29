#include "../include/ewlxx/EwlObject.h"
#include <eflxx/eflpp_common.h>

#include <iostream>

using namespace std;

namespace efl {

EwlObject::EwlObject( EwlObject* parent, const char* name )
{
  cout << "EwlObject::EwlObject - generating object '" << ( name ? name : "<unknown>" ) << "' (" << /*type <<*/ ")" << endl;
}

EwlObject::~EwlObject()
{
}

void EwlObject::resize( const Size& size )
{
  ewl_object_size_request( _o, size.width(), size.height() );
}

void EwlObject::setFillPolicy( FillPolicy p )
{
  ewl_object_fill_policy_set( EWL_OBJECT( _o ), p );
}

void EwlObject::setAlignment( Alignment a )
{
  ewl_object_alignment_set( EWL_OBJECT( _o ), a );
}

} // end namespace ef

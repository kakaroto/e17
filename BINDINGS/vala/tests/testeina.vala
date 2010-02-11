/**
 * Copyright (C) 2009 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */

using Eina;

//===========================================================================
public void test_eina_list()
{
    int[] data = { 6, 9, 42, 1, 7, 9, 81, 1664, 1337 };
    int[] res = { 81, 9, 9, 7, 1 };

    Eina.init();
    Eina.List<int> l = null;

    l.append( data[0] );
    assert ( l != null );
    assert ( l.count() == 1 );

    l.prepend( data[1] );
    assert ( l != null );
    assert ( l.count() == 2 );

    l.append( data[2] );
    assert ( l != null );
    assert ( l.count() == 3 );

    l.remove( data[0] );
    assert ( l != null );
    assert ( l.count() == 2 );

    l.remove( data[0] );
    assert ( l != null );
    assert ( l.count() == 2 );

    var l2 = l.data_find_list( data[2] );
    assert ( l2 != null );
/*
    debug( "l2 size = %u", l2.count() );
    assert ( l2.count() == 1 );
*/
    var item = l.data_find( data[2] );
    assert ( item == data[2] );

    l = null;

    /*
    Eina.shutdown();
    */
}


//===========================================================================
void main( string[] args )
{
    Test.init( ref args );

    Test.add_func( "/Eina/List", test_eina_list );

    Test.run();
}

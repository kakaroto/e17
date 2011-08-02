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

Elm.Win win;
Elm.Box box;
Elm.Button[] buttons;

public void add_test( T.Abstract t, int index )
{
    buttons[index] = new Elm.Button( win );
    buttons[index].text_set( t.name() );
    buttons[index].smart_callback_add( "clicked", t.run );
    buttons[index].show();
    buttons[index].size_hint_align_set( 0.5, 0.5 );
    buttons[index].size_hint_weight_set( 1.0, 1.0 );
    box.pack_end( buttons[index] );
}

public int main( string[] args )
{
    debug( "main()" );
    Elm.init( args );

    win = new Elm.Win( null, "myWindow", Elm.WinType.BASIC );
    win.title_set( "Elementary meets Vala" );
    win.autodel_set( true );
    win.resize( 320, 320 );
    win.smart_callback_add( "delete-request", Elm.exit );

/*
    var bg = new Bg( win );
    bg.size_hint_weight_set( 1.0, 1.0 );
    bg.show();
    win.resize_object_add( bg );
*/

    var layout = new Elm.Layout( win );
    layout.file_set( "/usr/local/share/elementary/objects/test.edj", "layout" );
    layout.size_hint_weight_set( 1.0, 1.0 );
    layout.show();
    win.resize_object_add( layout );

    buttons = new Elm.Button[10];
    box = new Elm.Box( win );
    box.size_hint_weight_set( 1.0, 1.0 );
    win.resize_object_add( box );

    var test1 = new T.Background();
    add_test( test1, 1 );
    var test2 = new T.Hoversel();
    add_test( test2, 2 );
    var test3 = new T.Genlist();
    add_test( test3, 3 );

    box.homogenous_set( true );

    box.show();
    win.show();
    Elm.run();
    Elm.shutdown();
    return 0;
}

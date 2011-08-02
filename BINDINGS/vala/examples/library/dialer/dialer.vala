/**
 * Copyright (C) 2009 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **/

//=======================================================================
class DemoApplication : EflVala.Application
//=======================================================================
{
    Elm.Win win;
    Elm.Layout layout;
    Elm.Box box;
    Elm.Button button;

    public DemoApplication( string[] args )
    {
        base( args );
        setup();
    }

    public void setup()
    {
        win = new Elm.Win();
        win.title_set( "Elementary meets Vala" );
        win.autodel_set( true );
        win.resize( 320, 320 );
        win.smart_callback_add( "delete-request", this.quit );
        win.show();

        layout = new Elm.Layout( win );
        layout.file_set( "/usr/local/share/elementary/objects/test.edj", "layout" );
        layout.size_hint_weight_set( 1.0, 1.0 );
        layout.show();
        win.resize_object_add( layout );

        box = new Elm.Box( win );
        box.show();
        //win.resize_object_add( box );

        button = new Elm.Button( win );
        button.smart_callback_add( "clicked", on_button_press );
        button.text_set( "This is a button" );
        button.show();
        box.pack_end( button );
    }

    public void on_button_press()
    {
        debug( "Send PING to backend!" );
    }
}

//=======================================================================
public static int main( string[] args )
//=======================================================================
{
    var app = new DemoApplication( args );
    var res = app.run();
    if ( res >= 0 )
        debug( "App exit OK" );
    else
        debug( "App exit with ERROR" );
    return res;
}


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

class Delegates : GLib.Object
{
    public static void onDeleteStatic( Evas.Object o, void* event_info )
    {
        debug( "static callback" );
    }

    public void onDeleteMember( Evas.Object o, void* event_info )
    {
        debug( "member callback" );
        Elm.exit();
    }

    public void onButtonClicked( Evas.Object b, void* event_info )
    {
        debug( "on button clicked" );
        b.hide();
    }
}

public void test_objects()
{
    Elm.init( new string[] { "elementary_test" } );
    var win = new Elm.Win( null, "window", Elm.WinType.BASIC );
    var bg = new Elm.Bg( win );
    Elm.shutdown();
}

public void test_mainloop()
{
    string[] args = { "yo", "kurt" };
    Test.message( "main()" );
    Elm.init( args );
    Elm.run();
    GLib.Timeout.add_seconds(1,()=>{Test.message("Timeout");Elm.exit(); return false;});
    Elm.shutdown();
}

//===========================================================================
void main (string[] args)
{
    Test.init(ref args);

    Test.add_func( "/Objects", test_objects );
    //FIXME: never returns from mainloop
    //Test.add_func( "/MainLoop/All", test_mainloop );
    Test.run ();
}

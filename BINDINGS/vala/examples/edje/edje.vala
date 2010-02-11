/**
 * (C) 2009 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **/
public static int main( string[] args)
{
    /* init */
    EcoreEvas.init();
    Edje.init();

    /* create a window */
    var ee = new EcoreEvas.Window( "software_x11", 0, 0, 320, 480, null );
    ee.title_set( "Edje Example Application" );
    ee.show();
    var evas = ee.evas_get();

    /* create an edje */
    var background = new Edje.Object( evas );
    background.file_set( "/tmp/angstrom-bootmanager.edj", "background" );
    background.resize( 320, 480 );
    background.layer_set( 0 );
    background.part_text_set( "version", "Hello World" );
    background.show();

    var buttons = new Edje.Object( evas );
    buttons.file_set( "/tmp/angstrom-bootmanager.edj", "buttons" );
    buttons.resize( 320, 480 );
    buttons.layer_set( 1 );
    buttons.show();

    buttons.signal_callback_add( "angstrom|system|suspend", "*", on_suspend );

    message( "-> mainloop" );
    Ecore.MainLoop.begin();
    message( "<- mainloop" );

    /* shutdown */

    /* FIXME: Something SIGSEGVs here...
    Edje.shutdown();
    EcoreEvas.shutdown();
    */
    return 0;
}

public static void on_suspend( Edje.Object obj, string emission, string source )
{
    Ecore.MainLoop.quit();
}

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

public bool onTimeout()
{
    message( "hello from glib mainloop!" );
    return true;
}

public static int main( string[] args)
{
    /* init */
    EcoreEvas.init();

    /* check available engines */

    Eina.List<string> engines = EcoreEvas.engines_get();
    for ( int i = 0; i < engines.count(); ++i )
    {
        message( "ecore.evas got engine '%s'", engines.nth( i ) );
    }

    /* create a window */
    var ee = new EcoreEvas.Window( "software_x11", 0, 0, 320, 480, null );
    ee.title_set( "Ecore Test Window" );
    ee.show();

    /* create a glib mainloop */
    GLib.MainLoop gmain = new GLib.MainLoop( null, false );

    /* create a glib timer */
    Timeout.add_seconds( 1, onTimeout );

    /* integrate glib mainloop into ecore mainloop */
    if ( Ecore.MainLoop.glib_integrate() )
    {
        message( "glib mainloop integration successfully completed" );
    }
    else
    {
        warning( "could not integrate glib mainloop. did you compile glib mainloop support into ecore?" );
    }

    /* run ecore main loop */
    Ecore.MainLoop.begin();

    /* shutdown */
    EcoreEvas.shutdown();
    return 0;
}


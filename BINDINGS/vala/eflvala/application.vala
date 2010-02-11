/**
 * Copyright (C) 2009-2010 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
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

static EflVala.Application theApp;

//=======================================================================
public abstract interface EflVala.IApplication : GLib.Object
//=======================================================================
{
    public abstract int run();
    public abstract void quit();
}

//=======================================================================
public class EflVala.Application : EflVala.IApplication, GLib.Object
//=======================================================================
{
    //
    // public API
    //
    public Application( string[] args )
    {
        debug( "Application()" );
        Elm.init( args );
        assert ( theApp == null ); // fail, if someone tries to create two apps per process
        theApp = this;
    }

    public int run()
    {
        GLib.MainLoop gmain = new GLib.MainLoop( null, false );
        if ( Ecore.MainLoop.glib_integrate() )
        {
            debug( "GLib mainloop integration successfully completed" );
        }
        else
        {
            critical( "Could not integrate glib mainloop. This library needs ecore compiled with glib mainloop support" );
        }
        Ecore.MainLoop.begin();
        return 0;
    }

    public void quit()
    {
        Ecore.MainLoop.quit();
    }

}

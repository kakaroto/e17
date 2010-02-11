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

public abstract class T.Abstract
{
    protected Elm.Win win;

    public void open()
    {
        debug( "open window" );
        win = new Elm.Win( null, "window", Elm.WinType.BASIC );
        win.smart_callback_add( "delete-request", close );
        win.autodel_set( true );
        win.resize( 320, 320 );
        win.show();
    }

    public abstract void run( Evas.Object obj, void* event_info );

    public void close()
    {
        debug( "close window" );
        win = null; // will call evas_object_del, hence close the window
    }

    public abstract string name();
}


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

public class T.Genlist : T.Abstract
{
    unowned Elm.Genlist? list;
    Elm.GenlistItemClass itc;

    public Genlist()
    {
        itc = new Elm.GenlistItemClass();
        itc.item_style = "default";
        itc.func.text_get = getText;
        itc.func.content_get = getContent;
        itc.func.state_get = getState;
        itc.func.del = delItem;
    }

    public override void run( Evas.Object obj, void* event_info )
    {
        open();
        list = Elm.Genlist.add( win );
        debug( "created genlist %p", list );
        for ( int i = 1; i <= 1000; ++i )
        {
            list.item_append( itc, (void*)i, null, Elm.GenlistItemType.NONE, onSelectedItem );
        }
        list.show();
        list.size_hint_weight_set( 1.0, 1.0 );
        win.resize_object_add( list );
    }

    public override string name()
    {
        return "Generic List Example";
    }

    public static string getText( void *data, Elm.Object obj, string part )
    {
        int number = (int)data;
        debug( "text_get: %p", obj );
        return "This is list item #%d".printf( number );
    }
    public static unowned Elm.Object? getContent( void *data, Elm.Object obj, string part )
    {
        return null;
        /* This leads to a SIGSEGV, something's still wrong wrt. those delegates */
        /*
        int number = (int)data;
        debug( "content_get for item %d", number );
        var icon = new Elm.Icon( list );
        icon.file_set( "/usr/share/icons/oxygen/128x128/apps/tux.png", null );
        return icon;
        */
    }
    public static bool getState( void *data, Elm.Object obj, string part )
    {
        int number = (int)data;
        debug( "state_get for item %d", number );
        return false;
    }
    public static void delItem( void *data, Elm.Object obj )
    {
        int number = (int)data;
        debug( "del for item %d", number );
    }
    public void onSelectedItem( Evas.Object obj, void* event_info)
    {
        debug( "item selected on list %p, item %p", obj, event_info );
    }
}

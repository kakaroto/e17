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

public class T.Hoversel : T.Abstract
{
    unowned Elm.Bg? bg;
    unowned Elm.Box? box;
    unowned Elm.Hoversel? hoversel;

    public override void run( Evas.Object obj, void* event_info )
    {
        open();

        bg = Elm.Bg.add( win );
        bg.file_set( "/usr/share/backgrounds/space-02.jpg" );
        bg.size_hint_weight_set( 1.0, 1.0 );
        bg.show();
        win.resize_object_add( bg );

        box = Elm.Box.add( win );
        win.resize_object_add( box );
        box.size_hint_weight_set( 1.0, 1.0 );
        box.show();

        hoversel = Elm.Hoversel.add( win );
        hoversel.hover_parent_set( win );
        hoversel.text_set( "Labels without any Item" );

        hoversel.item_add( "Label without Item 1", null, Elm.IconType.NONE, null );
        hoversel.item_add( "Label without Item 2", null, Elm.IconType.NONE, null );
        hoversel.item_add( "Label without Item 3", null, Elm.IconType.NONE, null );
        hoversel.item_add( "Label without Item 4", null, Elm.IconType.NONE, null );

        hoversel.size_hint_weight_set( 0.0, 0.0 );
        hoversel.size_hint_align_set( 0.5, 0.5 );
        box.pack_end( hoversel );
        hoversel.show();
		win.resize( 320, 320 );
    }

    public override string name()
    {
        return "Hover Selection Example";
    }

}

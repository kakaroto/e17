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

public class T.Background : T.Abstract
{
    Elm.Bg bg;

    public override void run( Evas.Object obj, void* event_info )
    {
        open();
        bg = new Elm.Bg( win );
        bg.file_set( "/usr/share/backgrounds/space-02.jpg" );
        bg.size_hint_weight_set( 1.0, 1.0 );
        bg.size_hint_min_set( 160, 160 );
        bg.size_hint_max_set( 640, 640 );
        bg.show();

        win.resize_object_add( bg );
    }

    public override string name()
    {
        return "Window with background";
    }
}

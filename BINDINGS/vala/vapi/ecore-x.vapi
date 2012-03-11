/*
 * Copyright (C) 2009-2010 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
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

[CCode (cprefix = "Ecore_X_", lower_case_cprefix = "ecore_x_", cheader_filename = "Ecore_X.h")]
namespace EcoreX
{
    public void init();
    public void shutdown();


    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Evas", cprefix = "ecore_evas_", free_function = "ecore_evas_free")]
    public class Window
    {
        [CCode (cname = "ecore_x_window_new")]
        public Window( Window? parent=null, int x, int y, int w, int h );

		//TODO: check all the mothods below in Ecore_X.h:
        public void        move( int x, int y );
        public void        managed_move( int x, int y );
        public void        resize( int w, int h );
        public void        move_resize( int x, int y, int w, int h );
        public void        geometry_get( out int x, out int y, out int w, out int h );
        public void        rotation_set( int rot );
        public void        rotation_with_resize_set( int rot );
        public int         rotation_get();
        public void        shaped_set( int shaped );
        public int         shaped_get();
        public void        alpha_set( int alpha );
        public int         alpha_get();
        public void        transparent_set( bool transparent );
        public int         transparent_get();
        public void        show();
        public void        hide();
        public int         visibility_get();
        public void        raise();
        public void        lower();
        public void        activate();
        public void        title_set( string t );
        public unowned string title_get();
        public void        name_class_set( string n, string c );
        public void        name_class_get( out string n, out string c );
        public void        size_min_set( int w, int h );
        public void        size_min_get( out int w, out int h );
        public void        size_max_set( int w, int h );
        public void        size_max_get( out int w, out int h );
        public void        size_base_set( int w, int h );
        public void        size_base_get( out int w, out int h );
        public void        size_step_set( int w, int h );
        public void        size_step_get( out int w, out int h );
        public void        cursor_set( string file, int layer, int hot_x, int hot_y );
        //public void        object_cursor_set( Evas.Object obj, int layer, int hot_x, int hot_y );
        //public void        cursor_get( out Evas.Object obj, out int layer, out int hot_x, out int hot_y );
        public void        layer_set( int layer );
        public int         layer_get();
        public void        focus_set( int on );
        public int         focus_get();
        public void        iconified_set( int on );
        public int         iconified_get();
        public void        borderless_set( int on );
        public int         borderless_get();
        public void        override_set( int on );
        public int         override_get();
        public void        maximized_set( int on );
        public int         maximized_get();
        public int         fullscreen_get();
        //public void        avoid_damage_set( AvoidDamageType avoid );
        //public AvoidDamageType avoid_damage_get();
        public void        withdrawn_set( int withdrawn );
        public int         withdrawn_get();
        public void        sticky_set( int sticky );
        public int         sticky_get();
        public void        ignore_events_set( int ignore );
        public int         ignore_events_get();
        public void        manual_render_set( int manual_render );
        public int         manual_render_get();
        public void        manual_render();
        public void        comp_sync_set( int do_sync );
        public int         comp_sync_get();
    }
}

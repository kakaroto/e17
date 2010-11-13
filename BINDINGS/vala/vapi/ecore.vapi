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
[CCode (cprefix = "Ecore_", lower_case_cprefix = "ecore_", cheader_filename = "Ecore.h")]
namespace Ecore
{
    public int init();
    public int shutdown();

    //=======================================================================
    [CCode (instance_pos = 0)]
    public delegate bool Callback();

    //=======================================================================
    [CCode (instance_pos = 0)]
    public delegate bool EventHandlerFunc( int type, void *event );

    //=======================================================================
    [CCode (instance_pos = 0)]
    public delegate bool FdHandlerFunc( FdHandler fdhandler );

    //=======================================================================
    [CCode (instance_pos = 0)]
    public delegate bool BufHandlerFunc( void* data );

    [CCode (cname="Ecore_Event_Signal_User")]
    public struct EventSignalUser
    {
        public int number;
    }

    [CCode (cname="Ecore_Event_Signal_Exit")]
    public struct EventSignalExit
    {
        public uint interrupt;
        public uint quit;
        public uint terminate;
    }

    [CCode (cname="Ecore_Event_Signal_Realtime")]
    public struct EventSignalRealtime
    {
        public int num;
    }

    [CCode (cprefix = "ECORE_EVENT_")]
    public enum EventType
    {
        NONE,
        SIGNAL_USER,
        SIGNAL_HUP,
        SIGNAL_EXIT,
        SIGNAL_POWER,
        SIGNAL_REALTIME,
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Event_Handler", free_function = "ecore_event_handler_del")]
    public class EventHandler
    {
        [CCode (cname = "ecore_event_handler_add")]
        public EventHandler( int type, EventHandlerFunc event_func );
    }

    //=======================================================================
    namespace MainLoop
    {
        void iterate();
        void begin();
        void quit();
        bool glib_integrate();
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Fd_Handler", free_function = "ecore_main_fd_handler_del")]
    public class FdHandler
    {
        [CCode (cname = "ecore_main_fd_handler_add")]
        public FdHandler( int fd, FdHandlerFlags flags, FdHandlerFunc fd_func, BufHandlerFunc? buf_func );
    }

    //=======================================================================
    [CCode (cprefix = "ECORE_FD_")]
    public enum FdHandlerFlags
    {
        READ,
        WRITE,
        ERROR,
    }

    //=======================================================================
    [CCode (cprefix = "ECORE_EXE_")]
    public enum ExeFlags
    {
        PIPE_READ,
        PIPE_WRITE,
        PIPE_ERROR,
        PIPE_READ_LINE_BUFFERED,
        PIPE_ERROR_LINE_BUFFERED,
        PIPE_AUTO,
        RESPAWN,
        USE_SH,
        NOT_LEADER,
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "ecore_idler_del")]
    public class Idler
    {
        [CCode (cname = "ecore_idler_add")]
        public Idler( Callback callback );
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Idle_Enterer", free_function = "ecore_idle_enterer_del")]
    public class IdleEnterer
    {
        [CCode (cname = "ecore_idle_enterer_add")]
        public IdleEnterer( Callback callback );
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Idle_Exiter", free_function = "ecore_idle_exiter_del")]
    public class IdleExiter
    {
        [CCode (cname = "ecore_idle_exiter_add")]
        public IdleExiter( Callback callback );
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Timer", free_function = "ecore_timer_del")]
    public class Timer
    {
        [CCode (cname = "ecore_timer_add")]
        public Timer( double in_, Callback callback );
        public double interval_get();
        public void interval_set(double in_);
        public void freeze();
        public void thaw();
        public void delay(double add);
        public double pending_get();
        public static double precision_get();
        public static void precision_set(double precision);
    }

    //=======================================================================
    [CCode (cprefix = "Ecore_X_", lower_case_cprefix = "ecore_x_", cheader_filename = "Ecore_X.h")]
    namespace X
    {
        [CCode (cname = "ecore_x_init")]
        public int _init( string name );
        public int init( string name = "" )
        {
            if ( name == "" )
            {
                string dname = GLib.Environment.get_variable( "DISPLAY" ) ?? ":0.0";
                return _init( dname );
            }
        }
        public int shutdown();

        [SimpleType]
        [CCode (cname = "Ecore_X_Screen_Size", cheader_filename = "Ecore_X.h")]
        public struct ScreenSize
        {
            public int width;
            public int height;
        }

        [Compact]
        [CCode (cname = "Ecore_X_Window", cprefix = "ecore_x_window_", free_function = "ecore_x_window_free")]
        public class Window
        {
            public static unowned Window root_first_get();
            [CCode (cname = "ecore_x_randr_current_screen_size_get")]
            public ScreenSize current_screen_size_get();
        }
    }
    //=======================================================================
    [CCode (cprefix = "Ecore_Fb_", lower_case_cprefix = "ecore_fb_", cheader_filename = "Ecore_Fb.h")]
    namespace Framebuffer
    {
        public int init( string name = "/dev/fb0" );
        public int shutdown();

        public void size_get( out int w, out int h );
        public void touch_screen_calibrate_set( int xscale, int xtrans, int yscale, int ytrans, int xyswap );
        public void touch_screen_calibrate_get( out int xscale, out int xtrans, out int yscale, out int ytrans, out int xyswap );
    }
}

//=======================================================================
[CCode (cprefix = "Ecore_Evas_", lower_case_cprefix = "ecore_evas_", cheader_filename = "Ecore_Evas.h")]
namespace EcoreEvas
{
    public void init();
    public void shutdown();
    public bool engine_type_supported_get( EngineType engine );
    public Eina.List<string> engines_get();
    public void engines_free( Eina.List<string> engines );

    //=======================================================================
    [CCode (cprefix = "ECORE_EVAS_ENGINE_")]
    public enum EngineType
    {
        SOFTWARE_BUFFER,
        SOFTWARE_X11,
        XRENDER_X11,
        OPENGL_X11,
        SOFTWARE_XCB,
        XRENDER_XCB,
        SOFTWARE_GDI,
        SOFTWARE_DDRAW,
        DIRECT3D,
        OPENGL_GLEW,
        QUARTZ,
        SOFTWARE_SDL,
        DIRECTFB,
        SOFTWARE_FB,
        SOFTWARE_16_X11,
        SOFTWARE_16_DDRAW,
        SOFTWARE_16_WINCE
    }

    //=======================================================================
    [CCode (cprefix = "ECORE_EVAS_AVOID_DAMAGE_")]
    public enum AvoidDamageType
    {
        NONE,
        EXPOSE,
        BUILT_IN
    }

    //=======================================================================
    [CCode (cprefix = "ECORE_EVAS_OBJECT_ASSOCIATE_")]
    public enum ObjectAssociateFlags
    {
        BASE,
        STACK,
        LAYER,
        DEL
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Evas", cprefix = "ecore_evas_", free_function = "ecore_evas_free")]
    public class Window
    {
        [CCode (cname = "ecore_evas_new")]
        public Window( string engine, int x, int y, int w, int h, string? extra_options );

        public string engine_name_get();
        // removed: ecore_evas_data_get, ecore_evas_data_set, ecore_evas_callback*
        [CCode (cname = "ecore_evas_get")]
        public Evas.Canvas evas_get();

        public void        move( int x, int y );
        public void        managed_move( int x, int y );
        public void        resize( int w, int h );
        public void        move_resize( int x, int y, int w, int h );
        public void        geometry_get( out int x, out int y, out int w, out int h );
        public void        rotation_set( int rot );
        public void        rotation_with_resize_set( int rot );
        public int         rotation_get();
        public void        shaped_set( int shaped);
        public int         shaped_get();
        public void        alpha_set( int alpha);
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
        public void        object_cursor_set( Evas.Object obj, int layer, int hot_x, int hot_y );
        public void        cursor_get( out Evas.Object obj, out int layer, out int hot_x, out int hot_y );
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
        public void        avoid_damage_set( AvoidDamageType avoid );
        public AvoidDamageType avoid_damage_get();
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

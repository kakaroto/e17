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

//TODO:
// * ecore_thread_*
// 
[CCode (cprefix = "Ecore_", lower_case_cprefix = "ecore_", cheader_filename = "Ecore.h")]
namespace Ecore
{
    public int init();
    public int shutdown();

    [CCode (lower_case_cprefix = "ecore_app_")]
    namespace Application
    {
        void args_get( out string[] args );
        void args_set( string[] args );
        void restart();
    }

    [CCode (cname = "ECORE_VERSION_MAJOR")]
    const uint major_version;
    [CCode (cname = "ECORE_VERSION_MINOR")]
    const uint minor_version;

    [SimpleType]
    class Version
    {
        int major;
        int minor;
        int micro;
        int revision;
    }
    Version version;

    //=======================================================================
    [CCode (instance_pos = 0, cname = "Ecore_Cb")]
    public delegate bool Callback();

    //=======================================================================
    [CCode (instance_pos = 0, cname = "Ecore_Filter_Cb")]
    public delegate bool FilterCallback();

    //=======================================================================
    [CCode (instance_pos = 0, cname = "Ecore_Select_Function")]
    public delegate bool SelectFunction();

    //=======================================================================
    [CCode (instance_pos = 0, cname = "Ecore_End_Cb")]
    public delegate bool EndCallback( void *func_data );

    //=======================================================================
    [CCode (instance_pos = 0, cname = "Ecore_Task_Cb")]
    public delegate bool TaskCallback();

    //=======================================================================
    [CCode (instance_pos = 0, cname = "Ecore_Event_Handler_Cb")]
    public delegate bool EventHandlerFunc( int type, void *event );

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
        SIGNAL_REALTIME;
        public static EventType new();
        static int current_type_get();
        static void* current_event_get();
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "ecore_animator_del")]
    public class Animator
    {
        [CCode (cname = "ecore_animator_add")]
        public Animator( TaskCallback func );
        public void freeze();
        public void thaw();
        public void frametime_set( double frametime );
        public double frametime_get();
    }
    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Event_Handler", free_function = "ecore_event_handler_del")]
    public class EventHandler
    {
        [CCode (cname = "ecore_event_handler_add")]
        public EventHandler( int type, EventHandlerFunc event_func );
        public void* data_get();
        public void data_set( void* data );
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "ecore_event_del")]
    public class Event
    {
        public Event( int type, void* ev, EndCallback func_free );
    }

    //=======================================================================
    [Compact]
    [CCode (cprefix = "Ecore_Exe_", free_function = "ecore_exe_free")]
    public class Exe
    {

        [CCode (instance_pos = 0, cname = "Ecore_Exe_Cb")]
        public delegate bool Callback( Exe exe );

        public static void run_priority_set( int priority );
        public static int run_priority_get();

        public Exe.run( string cmd, void* data );
        public Exe.pipe_run( string cmd, Flags flags, void* data );

        public void callback_pre_free_set( Callback func );
        public bool send( [CCode (type = "void*")] uint8[] data );
        public void close_stdin();
        public void auto_limits_set( int start_bytes, int end_bytes, int start_lines, int end_lines );
        public EventData event_data_get( Flags flags );
        public Posix.pid_t pid_get();
        public void tag_set( string tag );
        public string tag_get();
        public string cmd_get();
        public void* data_get();
        public Flags exe_flags_get();
        public void pause();
        public void continue();
        public void interrupt();
        public void quit();
        public void terminate();
        public void kill();
        public void signal( int num );
        public void hup();

        //=======================================================================
        [CCode (cprefix = "ECORE_EXE_")]
        public enum Flags
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

        [CCode (cname = "Ecore_Exe_Event_Add")]
        public struct EventAdd
        {
            Exe exe;
        }

        [CCode (cname = "Ecore_Exe_Event_Del")]
        public struct EventDel
        {
            Posix.pid_t pid;
            int exit_code;
            Exe exe;
            bool exited;
        }

        [CCode (cname = "Ecore_Exe_Event_Data_Line")]
        public struct EventDataLine
        {
            [CCode (array_length_cname = "size", array_length_type = "int")]
            string line;
        }

        [CCode (cname = "Ecore_Exe_Event_Data")]
        public struct EventData
        {
            Exe exe;
            [CCode (array_length_cname = "size", array_length_type = "int")]
            EventDataLine[] lines;
        }

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
        public FdHandler( int fd, Flags flags, Func fd_func, BufHandlerFunc? buf_func );
        public int fd_get();
        public bool active_get( Flags flags );
        public void active_set( Flags flags );

        //=======================================================================
        [CCode (cprefix = "ECORE_FD_")]
        public enum Flags
        {
            READ,
            WRITE,
            ERROR,
        }

        //=======================================================================
        [CCode (instance_pos = 0, cname = "Ecore_Fd_Cb")]
        public delegate bool Func( FdHandler fdhandler );

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
    [CCode (free_function = "ecore_job_del")]
    public class Job
    {
        [CCode (cname = "ecore_job_add")]
        public Job( Ecore.Callback cb );
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "ecore_pipe_del")]
    public class Pipe
    {
        [CCode (instance_pos = 0, cname = "Ecore_Pipe_Cb")]
        public delegate void Callback ( [CCode (type = "void*")] uint8[] buffer );

        [CCode (cname = "ecore_pipe_add")]
        public Pipe( Callback cb );
        public bool write( [CCode (type = "const void*")] uint8[] buffer );
        public void write_close();
        public void read_close();
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "ecore_poller_del")]
    public class Poller
    {
        public Poller( Type type, int interval, TaskCallback cb );
        public int interval_get();
        public void interval_set( int interval );

        public static double poll_interval_get( Type type );
        public static void poll_interval_set( Type type, double poll_time );

        public enum Type
        {
            CORE
        }
    }

    namespace Time
    {
        public double get();
        public double unix_get();
        public double loop_time_get();
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Ecore_Timer", free_function = "ecore_timer_del")]
    public class Timer
    {
        [CCode (cname = "ecore_timer_add")]
        public Timer( double in_, Callback callback );
        public double interval_get();
        public void interval_set( double in_ );
        public void freeze();
        public void thaw();
        public void delay( double add );
        public double pending_get();
        public static double precision_get();
        public static void precision_set( double precision );
    }

    //=======================================================================
    [CCode (cprefix = "Ecore_X_", lower_case_cprefix = "ecore_x_", cheader_filename = "Ecore_X.h")]
    namespace X
    {
        [CCode (cname = "ecore_x_init")]
        public int _init( string name );
        [CCode (cname = "_ecore_x_init")]
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

    //=======================================================================
    [CCode (cprefix = "Ecore_Con_", lower_case_cprefix = "ecore_con_", cheader_filename = "Ecore_Con.h")]
    namespace Connection
    {
        public int init();
        public int shutdown();

        //=======================================================================
        [Compact]
        [CCode (free_function = "ecore_con_client_del")]
        public class Client
        {
            public int send( [CCode (type = "void*")] uint8[] data );
            public Server server_get();
            public void data_set( void* data );
            public void* data_get();
            public string ip_get();
            public void flush();
            public double uptime_get();
            public double timeout_get();
            public void timeout_set( double timeout_get );
            public bool connected_get();

            [CCode (cname = "Ecore_Con_Event_Client_Add")]
            public struct EventAdd
            {
                Client client;
            }

            [CCode (cname = "Ecore_Con_Event_Client_Del")]
            public struct EventDelete
            {
                Client client;
            }

            [CCode (cname = "Ecore_Con_Event_Client_Data")]
            public struct EventData
            {
                Client client;
                [CCode (carray_length_cname = "size", type = "void*")]
                uint8[] data;
            }
            [CCode (cprefix = "ECORE_CON_EVENT_CLIENT_")]
            public enum Event
            {
                ADD,
                DEL,
                DATA
            }
        }

        //=======================================================================
        [Compact]
        [CCode (free_function = "ecore_con_server_del")]
        public class Server
        {
            public Server( Type type, string name, int port, void* data = null );
            [CCode (cname = "ecore_con_server_connect")]
            public Server.connect( Type type, string name, int port, void* data = null );
            public void* data_get();
            public void data_set( void* data );
            public bool connected_get();
            public Eina.List<Client> clients_get();
            public string name_get();
            public string port_get();
            public double uptime_get();
            public int send( [CCode (type = "void*")] uint8[] data );
            public void client_limit_set( int client_limit, char reject_excess_clients );
            public string ip_get();
            public void flush();
            public void timeout_set( double timeout );
            public double timeout_get();

            [CCode (cname = "Ecore_Con_Event_Server_Add")]
            public struct EventAdd
            {
                Server server;
            }

            [CCode (cname = "Ecore_Con_Event_Server_Del")]
            public struct EventDelete
            {
                Server server;
            }

            [CCode (cname = "Ecore_Con_Event_Server_Data")]
            public struct EventData
            {
                Server server;
                [CCode (carray_length_cname = "size", type = "void*")]
                uint8[] data;
            }
            [CCode (cprefix = "ECORE_CON_EVENT_SERVER_")]
            public enum Event
            {
                ADD,
                DEL,
                DATA
            }
        }

        //=======================================================================
        [Compact]
        [CCode (free_function = "ecore_con_url_free")]
        public class Url
        {
            static int init();
            static int shutdown();

            public Url( string url );
            public Url.custom( string url, string custom_request );

            public bool url_set( string url );

            public void data_set( void* data );
            public void* data_get( void* data );

            public void additional_header_add( string key, string value );
            public void additional_header_clear();

            public Eina.List<string> respones_headers_get();
            public void fd_set( int fd );
            public int received_bytes_get();

            public bool http_auth_set( string username, string password, bool safe );
            public bool send( [CCode (type = "void*")] uint8[] data, string content_type );
            public void time( Time time_condition, double timestamp );

            public bool ftp_upload( string filename, string user, string password, string upload_dir );
            public void ftp_use_epsv_set( bool use_epsv );
            public void verbose_set( bool verbose );


            [Compact]
            [CCode (cname = "Ecore_Con_Event_Server_Data")]
            public struct EventData
            {
                Url url_con;
                [CCode (carray_length_cname = "size", type = "void*")]
                uint8[] data;
            }
            [Compact]
            [CCode (cname = "Ecore_Con_Event_Url_Complete")]
            public struct EventComplete
            {
                Url url_con;
                int status;
            }
            [Compact]
            [CCode (cname = "Ecore_Con_Event_Url_Progress")]
            public struct EventProgress
            {
                Url url_con;
                [CCode (cname = "down.total")]
                double down_total;
                [CCode (cname = "up.total")]
                double up_total;
                [CCode (cname = "down.now")]
                double down_now;
                [CCode (cname = "up.now")]
                double up_now;
            }
            [CCode (cprefix = "CORE_CON_EVENT_URL_")]
            public enum Event
            {
                DATA,
                COMPLETE,
                PROGRESS
            }

            [CCode (cprefix = "ECORE_CON_URL_TIME_", cname = "Ecore_Con_Url_Time")]
            public enum Time
            {
                NONE,
                IFMODSINCE,
                IFUNMODSINCE
            }
        }

        //=======================================================================
        [CCode (cprefix = "ECORE_CON_")]
        public enum Type
        {
            LOCAL_USER,
            LOCAL_SYSTEM,
            LOCAL_ABSTRACT,
            REMOTE_TCP,
            REMOTE_MCAST,
            REMOTE_UDP,
            REMOTE_BROADCAST,
            REMOTE_NODELAY,
            USE_SSL2,
            USE_SSL3,
            USE_TLS,
            USE_MIXED,
            LOAD_CERT
        }

        //=======================================================================
        namespace Dns
        {
            [CCode (cname = "Ecore_Con_Dns_Cb")]
            public delegate void Callback( string canonname, string ip, Posix.SockAddr addr, int addrlen );
            [CCode (cname = "ecore_con_lookup")]
            public static bool lookup( string name, Dns.Callback cb );
        }

        //=======================================================================
        namespace Ssl
        {
            public bool available_get();

            [Compact]
            [CCode (cname = "Ecore_Con_Server")]
            public class Server: Ecore.Connection.Server
            {
                [CCode (cname = "_ecore_con_ssl_server_add")]
                public Server( Connection.Type type, string name, int port = 443, void * data = null )
                {
                    base( type | Connection.Type.USE_SSL2, name, port, data );
                }
                public bool cert_add( string cert );
                public bool privkey_add( string key_file );
                public bool crl_add( string crl_file );
                public bool cafile_add( string ca_file );
                public void verify();
            }
        }

        //=======================================================================
    }
}

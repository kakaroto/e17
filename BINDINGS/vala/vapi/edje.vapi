/**
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
[CCode (cprefix = "Edje_", lower_case_cprefix = "edje_", cheader_filename = "Edje.h")]
namespace Edje
{
    public int init();
    public int shutdown();

    public void frametime_set( double t );
    public double frametime_get();

    public void freeze();
    public void thaw();
    public void fontset_append_set( string fonts );
    public string fontset_append_get();
    public void scale_set( double scale );
    public double scale_get();

    public Eina.List<string> file_collection_list( string file );
    public void file_collection_list_free( Eina.List<string> list );
    public int file_group_exists( string file, string glob );
    public string file_data_get( string file, string key );
    public void file_cache_set( int count );
    public int file_cache_get();
    public void file_cache_flush();

    public void collection_cache_set( int count );
    public int collection_cache_get();
    public void collection_cache_flush();

    public void color_class_set( string color_class, int r, int g, int b, int a, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3 );
    public bool color_class_get( string color_class, out int r, out int g, out int b, out int a, out int r2, out int g2, out int b2, out int a2, out int r3, out int g3, out int b3, out int a3 );
    public void color_class_del( string color_class );
    public Eina.List<string> color_class_list();

    public void text_class_set( string text_class, string font, Evas.FontSize size );
    public void text_class_del( string text_class);
    public Eina.List<string> text_class_list();

    public void message_signal_process();

    [CCode (cprefix = "Edje_Message_", lower_case_cprefix = "edje_message_")]
    namespace Message
    {
        [CCode (cprefix = "EDJE_MESSAGE_")]
        public enum Type
        {
            NONE,
            SIGNAL, /* DONT USE THIS */
            STRING,
            INT,
            FLOAT,
            STRING_SET,
            INT_SET,
            FLOAT_SET,
            STRING_INT,
            STRING_FLOAT,
            STRING_INT_SET,
            STRING_FLOAT_SET
        }

        public struct AbstractMessage
        {
            [CCode (cname = "edje_message_send", instance_pos = -1)]
            public void send( Type type, int id );
        }

        [CCode (destroy_function = "")]
        public struct String : AbstractMessage
        {
            string str;
        }

        [CCode (destroy_function = "")]
        public struct Int : AbstractMessage
        {
            int val;
        }

        [CCode (destroy_function = "")]
        public struct Float : AbstractMessage
        {
            double val;
        }

        [CCode (destroy_function = "")]
        public struct String_Set : AbstractMessage
        {
            int count;
            char* str[1];
        }

        [CCode (destroy_function = "")]
        public struct Int_Set : AbstractMessage
        {
            int count;
            int val[1];
        }

        [CCode (destroy_function = "")]
        public struct Float_Set : AbstractMessage
        {
            int count;
            double val[1];
        }

        [CCode (destroy_function = "")]
        public struct String_Int : AbstractMessage
        {
            string str;
            int val;
        }

        [CCode (destroy_function = "")]
        public struct String_Float : AbstractMessage
        {
            string str;
            double val;
        }

        [CCode (destroy_function = "")]
        public struct String_Int_Set : AbstractMessage
        {
            string str;
            int count;
            int val[1];
        }

        [CCode (destroy_function = "")]
        public struct String_Float_Set : AbstractMessage
        {
            string str;
            int count;
            double val[1];
        }
    }

    //=======================================================================
    [CCode (cprefix = "EDJE_ASPECT_CONTROL_")]
    public enum AspectControl
    {
        NONE,
        NEITHER,
        HORIZONTAL,
        VERTICAL,
        BOTH
    }

    //=======================================================================
    [CCode (cprefix = "EDJE_OBJECT_TABLE_HOMOGENEOUS_")]
    public enum ObjectTableHomogenousMode
    {
        NONE,
        TABLE,
        ITEM
    }

    [CCode (cname = "Edje_Signal_Cb", instance_pos = 0)]
    public delegate void SignalCallback( Object obj, string emission, string source );

    [CCode (cname = "Edje_Text_Change_Cb", instance_pos = 0)]
    public delegate void TextChangedCallback( Object obj, string part );

    [CCode (cname = "Edje_Message_Handler_Cb", instance_pos = 0)]
    public delegate void MessageHandlerCallback( Object obj, Message.Type type, int id, void* msg );

    //=======================================================================
    [Compact]
    [CCode (cname = "Evas_Object", clower_case_prefix = "edje_object_", free_function = "evas_object_del")]
    public class Object : Evas.Object
    {
        [CCode (cname = "edje_object_add") ]
        public Object( Evas.Canvas parent );

        public void scale_set( double scale );
        public double scale_get();

        public string data_get( string key );

        public int file_set( string file, string group );
        public void file_get( out string file, out string group );
        public int load_error_get();
        public unowned string edje_load_error_str( int error );

        public void signal_callback_add( string emission, string source, SignalCallback callback );
        public void* signal_callback_del( string emission, string source, SignalCallback callback );
        public void signal_emit( string emission, string source );

        public void play_set( bool play);
        public bool play_get();
        public void animation_set( bool on );
        public bool animation_get();

        public int freeze();
        public int thaw();

        public void color_class_set( string color_class, int r, int g, int b, int a, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3 );
        public bool color_class_get( string color_class, out int r, out int g, out int b, out int a, out int r2, out int g2, out int b2, out int a2, out int r3, out int g3, out int b3, out int a3 );
        public void color_class_del( string color_class );

        public void text_class_set( string text_class, string font, Evas.FontSize size );

        public void size_min_get( out Evas.Coord minw, out Evas.Coord minh );
        public void size_max_get( out Evas.Coord maxw, out Evas.Coord maxh );
        public void calc_force();
        public void size_min_calc( out Evas.Coord minw, out Evas.Coord minh );
        public void size_min_restricted_calc( out Evas.Coord minw, out Evas.Coord minh, Evas.Coord restrictedw, Evas.Coord restrictedh );

        public bool part_exists( string part );
        public Evas.Object? part_object_get( string part );

        public void part_geometry_get( string part, out Evas.Coord x, out Evas.Coord y, out Evas.Coord w, out Evas.Coord h );

        public void text_change_cb_set( TextChangedCallback callback );
        public void part_text_set( string part, string text );
        public unowned string part_text_get( string part );
        public void part_text_unescaped_set( string part, string text_to_escape );
        public string part_text_unescaped_get( string part );

        public unowned string part_text_selection_get( string part );
        public void part_text_select_none( string part );
        public void part_text_select_all( string part );

        public void part_text_insert( string part, string text );
        public Eina.List<string> part_text_anchor_list_get( string part );
        public Eina.List<string> part_text_anchor_geometry_get( string part, string anchor );
        public void part_text_cursor_geometry_get( string part, out Evas.Coord x, out Evas.Coord y, out Evas.Coord w, out Evas.Coord h );
        public void part_text_select_allow_set( string part, bool allow );
        public void part_text_select_abort( string part );

        public void part_swallow( string part, Evas.Object obj_swallow );
        public void part_unswallow( Evas.Object obj_swallow );
        public Evas.Object part_swallow_get( string part);

        public unowned string part_state_get( string part, out double val_ret );
        public int part_drag_dir_get( string part );
        public void part_drag_value_set( string part, double dx, double dy );
        public void part_drag_value_get( string part, out double dx, out double dy );

        public void part_drag_size_set( string part, double dw, double dh );
        public void part_drag_size_get( string part, out double dw, out double dh );
        public void part_drag_step_set( string part, double dx, double dy );
        public void part_drag_step_get( string part, out double dx, out double dy );
        public void part_drag_page_set( string part, double dx, double dy );
        public void part_drag_page_get( string part, out double dx, out double dy );
        public void part_drag_step( string part, double dx, double dy );
        public void part_drag_page( string part, double dx, double dy );
        public bool part_box_append( string part, Evas.Object child );
        public bool part_box_prepend( string part, Evas.Object child );
        public bool part_box_insert_before( string part, Evas.Object child, Evas.Object reference );
        public bool part_box_insert_at( string part, Evas.Object child, uint pos );
        public Evas.Object part_box_remove( string part, Evas.Object child );
        public Evas.Object part_box_remove_at( string part, uint pos );
        public bool part_box_remove_all( string part, bool clear );
        public bool part_table_pack( string part, Evas.Object child_obj, ushort col, ushort row, ushort colspan, ushort rowspan );
        public bool part_table_unpack( string part, Evas.Object child_obj );
        public bool part_table_col_row_size_get( string part, out int cols, out int rows );
        public bool part_table_clear( string part, bool clear );

        public void message_send( Message.Type type, int id, Message.AbstractMessage msg );
        public void message_handler_set( MessageHandlerCallback callback );

        public void message_signal_process();
    }

}


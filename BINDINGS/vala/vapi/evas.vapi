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

// TODO:
// * evas cache
// * evas image preloading
// * event callbacks
// * async fd

[CCode (cprefix = "Evas_", lower_case_cprefix = "evas_", cheader_filename = "Evas.h")]
namespace Evas
{
    public void init();
    public void shutdown();

    [CCode (cname = "EVAS_VERSION_MAJOR")]
    const uint major_version;
    [CCode (cname = "EVAS_VERSION_MINOR")]
    const uint minor_version;

    [Compact]
    class Version
    {
        int major;
        int minor;
        int micro;
        int revision;
    }
    Version version;

    //=======================================================================
    namespace Render
    {
        public int method_lookup( string name );
        public Eina.List<string> method_list();
        public void method_list_free( Eina.List<string> methods );

        [CCode (cname = "Evas_Render_Op", cprefix = "EVAS_RENDER_")]
        public enum Operator
        {
            BLEND,
            BLEND_REL,
            COPY,
            COPY_REL,
            ADD,
            ADD_REL,
            SUB,
            SUB_REL,
            TINT,
            TINT_REL,
            MASK,
            MUL
        }
    }

    //=======================================================================
    namespace Output
    {
        public int output_method_get();
        public int output_method_set( int method );
    }

    //=======================================================================
    [CCode (cprefix = "Evas_Engine_")]
    namespace Engine
    {
        public class Info {}
        public Info info_get();
        public bool info_set( Info info );
    }

    //=======================================================================
    namespace Event
    {
        [CCode (cprefix = "EVAS_EVENT_FLAG_", cname = "Evas_Event_Flags")]
        public enum Flags
        {
            NONE,
            ON_HOLD,
            ON_SCROLL
        }

        [CCode (cname = "Evas_Event_Cb", instance_pos = 0)]
        public delegate void Callback( Evas.Canvas evas, void* event_info );
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_BIDI_DIRECTION_", cname = "Evas_BiDi_Direction")]
    public enum BiDiDirection
    {
        NATURAL,
        LTR,
        RTL
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_FONT_HINTING_", cname = "Evas_Font_Hinting_Flags")]
    public enum FontHintingFlags
    {
        NONE,       /**< No font hinting */
        AUTO,       /**< Automatic font hinting */
        BYTECODE    /**< Bytecode font hinting */
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_ASPECT_CONTROL_", cname = "Evas_Aspect_Control")]
    public enum AspectControl
    {
        NONE,
        NEITHER,
        HORIZONTAL,
        VERTICAL,
        BOTH
    }

    //======================================================================
    [CCode (cname = "Evas_Alloc_Error", cprefix = "EVAS_ALLOC_ERROR_")]
    public enum AllocError
    {
        NONE,
        FATAL,
        RECOVERED;
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_LOAD_ERROR_", cname = "int")]
    public enum LoadError
    {
        NONE,
        GENERIC,
        DOES_NOT_EXIST,
        PERMISSION_DENIED,
        RESOURCE_ALLOCATION_FAILED,
        CORRUPT_FILE,
        UNKNOWN_FORMAT;
        [CCode (cname = "evas_load_error_str")]
        unowned string to_string();
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_TEXTURE_", cname = "Evas_Fill_Spread")]
    public enum FillSpread
    {
        REFLECT,
        REPEAT,
        RESTRICT,
        RESTRICT_REFLECT,
        RESTRICT_REPEAT,
        PAD
    }

    //=======================================================================


    //=======================================================================
    [CCode (cprefix = "EVAS_TEXTURE_", cname = "int")]
    public enum TextureMode
    {
        REFLECT,
        REPEAT,
        RESTRICT,
        RESTRICT_REFLECT,
        RESTRICT_REPEAT,
        PAD
    }

    //=======================================================================
    namespace Hint
    {
        [CCode (cname = "EVAS_HINT_EXPAND")]
        public const double EXPAND;
        [CCode (cname = "EVAS_HINT_FILL")]
        public const double FILL;
    }

    //=======================================================================
    namespace Layer
    {
        [CCode (cname = "EVAS_LAYER_MIN")]
        public const int MIN;
        [CCode (cname = "EVAS_LAYER_MAX")]
        public const int MAX;
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_BORDER_FILL_", cname = "Evas_Border_Fill_Mode")]
    public enum BorderFillMode
    {
        NONE,
        DEFAULT,
        SOLID
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_CALLBACK_", cname = "Evas_Callback_Type")]
    public enum CallbackType
    {
        MOUSE_IN,
        MOUSE_OUT,
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_MOVE,
        MOUSE_WHEEL,
        MULTI_DOWN,
        MULTI_UP,
        MULTI_MOVE,
        FREE,
        KEY_DOWN,
        KEY_UP,
        FOCUS_IN,
        FOCUS_OUT,
        SHOW,
        HIDE,
        MOVE,
        RESIZE,
        RESTACK,
        DEL,
        HOLD,
        CHANGED_SIZE_HINTS,
        IMAGE_PRELOADED,
        CANVAS_FOCUS_IN,
        CANVAS_FOCUS_OUT,
        RENDER_FLUSH_PRE,
        REDER_FLUSH_POST,
        CANVAS_OBJECT_FOCUS_IN,
        CANVAS_OBJECT_FOCUS_OUT
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_BUTTON_", cname = "Evas_Button_Flags")]
    public enum ButtonFlags
    {
        NONE,
        DOUBLE_CLICK,
        TRIPLE_CLICK
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_COLORSPACE_")]
    public enum Colorspace
    {
        ARGB8888,
        /* these are not currently supported - but planned for the future */
        YCBCR422P601_PL,
        CBCR422P709_PL,
        RGB565_A5P,
        GRY8
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Evas", free_function = "evas_free", lower_case_cprefix = "evas_", cprefix = "Evas_")]
    public class Canvas
    {
        [CCode (cname = "evas_new" )]
        public Canvas();

        public void output_method_set( int render_method );
        public int output_method_get();

        public void output_size_set( int w, int h );
        public void output_size_get( out int w, out int h );
        public void output_viewport_set( Coord x, Coord y, Coord w, Coord h );
        public void output_viewport_get( out Coord x, out Coord y, out Coord w, out Coord h );

        public Coord coord_screen_x_to_world( int x );
        public Coord coord_screen_y_to_world( int y );
        public int coord_world_x_to_screen( Coord x );
        public int coord_world_y_to_screen( Coord y );

        public void pointer_output_xy_get( out int x, out int y );
        public void pointer_canvas_xy_get( out Coord x, out Coord y );
        public int pointer_button_down_mask_get();
        public bool pointer_inside_get();
        public void sync();

        public void data_attach_set( void *data );
        public void* data_attach_get();

        public void focus_in();
        public void focus_out();
        public bool focus_state_get();

        public void image_cache_flush();
        public void image_cache_reload();
        public void image_cache_set( int size );
        public int image_cache_get();

        public void nochange_push();
        public void nochange_pop();

        public void damage_rectangle_add( int x, int y, int w, int h );

        public void obscured_rectangle_add( int x, int y, int w, int h);
        public void obscured_clear();

        public void event_freeze();
        public void event_thaw();
        public int event_freeze_get();

        public void event_feed_mouse_down( int b, ButtonFlags flags, uint timestamp, void* data = null );
        public void event_feed_mouse_up( int b, ButtonFlags flags, uint timestamp, void* data = null );
        public void event_feed_mouse_move( int x, int y, uint timestamp, void* data = null );
        public void event_feed_mouse_in( uint timestamp, void* data = null );
        public void event_feed_mouse_out( uint timestamp, void* data = null );
        public void event_feed_mouse_cancel (uint timestamp, void* data = null );
        public void event_feed_mouse_wheel( int direction, int z, uint timestamp, void* data = null );

        public void event_feed_multi_down( int d, int x, int y, double rad, double radx, double rady, double pres, double ang, double fx, double fy, ButtonFlags flags, uint timestamp, void* data = null );
        public void event_feed_multi_up( int d, int x, int y, double rad, double radx, double rady, double pres, double ang, double fx, double fy, ButtonFlags flags, uint timestamp, void* data = null );
        public void event_feed_multi_move( int d, int x, int y, double rad, double radx, double rady, double pres, double ang, double fx, double fy, uint timestamp, void* data = null );

        public void event_feed_key_down( string keyname, string key, string @string, string compose, uint timestamp, void* data = null );
        public void event_feed_key_up( string keyname, string key, string @string, string compose, uint timestamp, void* data = null );
        public void event_feed_hold( int hold, uint timestamp, void* data = null );

        public void event_callback_add( CallbackType type, Event.Callback func );
        [CCode (cname = "evas_event_callback_del_full")]
        public void* event_callback_del( CallbackType type, Event.Callback func );

        public void post_event_callback_add( CallbackType type, Event.Callback func );
        [CCode (cname = "evas_post_event_callback_del_full")]
        public void post_event_callback_del( CallbackType type, Event.Callback func );

        public Eina.List<Eina.Rectangle> render_updates();
        public static void render_updates_free( Eina.List<Eina.Rectangle> updates );
        public void render();
        public void norender();
        public void render_idle_flush();
        public void render_dump();

        public void font_path_clear();
        public void font_path_append( string path );
        public void font_path_prepend( string path );
        public Eina.List<string> font_path_list();

        public void font_cache_flush();
        public void font_cache_set( int size );
        public int font_cache_get();

        public Eina.List<string> font_available_list();
        public void font_available_list_free( Eina.List<string> available );

        public void font_hinting_set( FontHintingFlags hinting );
        public FontHintingFlags font_hinting_get();
        public bool font_hinting_can_hint( FontHintingFlags hinting );

        public Object? bottom_get();
        public Object? top_get();
        public Object? focus_get();
        public Object? name_find( string name );
        public Object? top_at_pointer_get();
        public Object? top_at_xy_get( Coord x, Coord y, bool include_pass_events_objects = false, bool include_hidden_objects = false );
        public Object? top_in_rectangle_get( Coord x, Coord y, Coord w, Coord h, bool include_pass_events_objects = false, bool include_hidden_objects = false);
        [CCode (cname = "evas_objects_at_xy_get")]
        public Eina.List<Object> at_xy_get( Coord x, Coord y, bool include_pass_events_objects = false, bool include_hidden_objects = false );
        [CCode (cname = "evas_objects_in_rectangle_get")]
        public Eina.List<Object> in_rectangle_get( Coord x, Coord y, Coord w, Coord h, bool include_pass_events_objects = false, bool include_hidden_objects = false);
        /*namespace Intercept
        {
            [CCode (cname = "Evas_Object_Intercept_Show_Cb", has_target = false)]
            public void ShowCallback( void* data, Object obj );
            [CCode (cname = "Evas_Object_Intercept_Hide_Cb", has_target = false)]
            public void HideCallback( void* data, Object obj );
            [CCode (cname = "Evas_Object_Intercept_Move_Cb", has_target = false)]
            public void MoveCallback( void* data, Object obj, Coord x, Coord y );
            [CCode (cname = "Evas_Object_Intercept_Resize_Cb", has_target = false)]
            public void ResizeCallback( void* data, Object obj, Coord w, Coord h );
            [CCode (cname = "Evas_Object_Intercept_Stack_Above_Cb", has_target = false)]
            public void StackAboveCallback( void* data, Object obj, Object above );
            [CCode (cname = "Evas_Object_Intercept_Stack_Below_Cb", has_target = false)]
            public void StackBelowCallback( void* data, Object obj, Object above );
            [CCode (cname = "Evas_Object_Intercept_Layer_Set_Cb", has_target = false)]
            public void LayerSetCallback( void* data, Object obj, int l );
            [CCode (cname = "Evas_Object_Intercept_Raise_Cb", has_target = false)]
            public void RaiseCallback( void* data, Object obj );
            [CCode (cname = "Evas_Object_Intercept_Lower_Cb", has_target = false)]
            public void LowerCallback( void* data, Object obj );
            [CCode (cname = "Evas_Object_Intercept_Color_Set_Cb", has_target = false)]
            public void ColorSetCallback( void* data, Object obj );
            [CCode (cname = "Evas_Object_Intercept_Clip_Set_Cb", has_target = false)]
            public void ClibSetCallback( void* data, Object obj, Object clip );
            [CCode (cname = "Evas_Object_Intercept_Clip_Unset_Cb", has_target = false)]
            public void ClipUnsetCallback( void* data, Object obj );
        }*/
    }

    //=======================================================================
    [SimpleType]
    [BooleanType]
    public struct Bool {}

    //=======================================================================
    [SimpleType]
    [IntegerType (rank=6)]
    public struct Coord {}

    //=======================================================================
    [SimpleType]
    [IntegerType (rank=6)]
    public struct FontSize {}

    //=======================================================================
    [Compact]
    [CCode (cname = "Evas_Native_Surface")]
    public struct NativeSurface
    {
        [CCode (cname = "data.x11.visual")]
        void* x11_visual;
        [CCode (cname = "data.x11.pixmap")]
        ulong x11_pixmap;
    }

    //=======================================================================
    [CCode (cname = "Evas_Smart_Cb", instance_pos = 0)]
    public delegate void Callback( Evas.Object obj, void* event_info );

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Map", cprefix = "evas_map_", free_function = "evas_map_free")]
    public class Map
    {
        [CCode (cname = "evas_map_new")]
        public Map( int count );

        [CCode (cname = "evas_map_util_points_populate_from_object_full")]
        public void util_points_populate_from_object( Evas.Object obj, Coord z = 0 );
        public void util_points_populate_from_geometry( Coord x, Coord y, Coord w, Coord h, Coord z);
        public void util_points_color_set( int r, int g, int b, int a );
        public void util_rotate( double degrees, Evas.Coord cx, Evas.Coord cy );
        public void util_zoom( double zoomx, double zoomy, Evas.Coord cx, Evas.Coord cy );
        public void util_3d_rotate( double dx, double dy, double dz, Evas.Coord cx, Evas.Coord cy, Evas.Coord cz, Evas.Coord px, Evas.Coord py, Evas.Coord z0, Evas.Coord foc );
        public void util_3d_lighting( Coord lx, Coord ly, Coord lz, int lr, int lg, int lb, int ar, int ag, int ab );
        public void util_3d_perspective( Evas.Coord px, Evas.Coord py, Evas.Coord z0, Evas.Coord foc );

        public bool util_clockwise_get();

        public void smooth_set( bool enabled );
        public bool smooth_get();

        public void alpha_set( bool enabled );
        public bool alpha_get();

        [CCode (cname = "evas_map_dup")]
        public Map copy();

        public void point_coord_set( int idx, Evas.Coord x, Evas.Coord y, Evas.Coord z );
        public void point_coord_get( int idx, out Evas.Coord x, out Evas.Coord y, out Evas.Coord z );
        public void point_image_uv_set( int idx, double u, double v );
        public void point_image_uv_get( int idx, out double u, out double v );
        public void point_color_set( int idx, int r, int g, int b, int a );
        public void point_color_get( int idx, out int r, out int g, out int b, out int a );
    }

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", free_function="evas_object_del")]
    public abstract class Object
    {
        public void del();

        public void show();
        public void hide();
        public bool visible_get();

        public void scale_set( double scale );
        public double scale_get();

        public string type_get();

        public void layer_set( int layer );
        public int layer_get();

        public void render_op_set( Render.Operator op );
        public Render.Operator render_op_get();

        public void precise_is_inside_set( bool precise );
        public bool precise_is_inside_get();

        public void static_clip_set( bool is_static_clip );
        public bool static_clip_get();

        public void raise();
        public void lower();
        public void stack_above( Object above );
        public void stack_below( Object below );
        public Object above_get();
        public Object below_get();

        public void move( Coord x, Coord y );
        public void resize( Coord w, Coord h );
        public void geometry_get( out Coord x, out Coord y, out Coord w, out Coord h );

        public void size_hint_align_set( double x, double y );
        public void size_hint_min_set( Coord w, Coord h );
        public void size_hint_max_set( Coord w, Coord h );
        public void size_hint_padding_set( Coord l, Coord r, Coord t, Coord b );
        public void size_hint_weight_set( double x, double y );
        public void size_hint_aspect_set( AspectControl aspect, Coord w, Coord h);
        public void size_hint_request_set( Coord w, Coord h );

        public void size_hint_align_get( out double x, out double y );
        public void size_hint_min_get( out Coord w, out Coord h );
        public void size_hint_max_get( out Coord w, out Coord h );
        public void size_hint_padding_get( out Coord l, out Coord r, out Coord t, out Coord b );
        public void size_hint_weight_get( out double x, out double y );
        public void size_hint_aspect_get( out AspectControl aspect, out Coord w, out Coord h);
        public void size_hint_request_get( out Coord w, out Coord h );

        public void pointer_mode_set( PointerMode mode );
        public PointerMode pointer_mode_get();

        public void anti_alias_set( bool antialias );
        public bool anti_alias_get();

        public void color_set( int r, int g, int b, int a );
        public void color_get( out int r, out int g, out int b, out int a );
        public void color_interpolation_set( int color_space );
        public int color_interpolation_get();

        public void clip_set( Object clip );
        public Object clip_get();
        public void clip_unset( Object clip );
        public unowned Eina.List<Object> clipees_get();

        public void data_set( string key, void* data );
        public void* data_get( string key );
        public void* data_del( string key );

        public void map_enable_set( bool enabled );
        public bool map_enable_get();
        public void map_source_set( Evas.Object src );
        public Evas.Object map_source_get();
        public void map_set( Map map );
        public Map map_get();

        public void name_set( string name );
        public unowned string name_get();
        public Object? name_find( string name );

        public unowned Canvas evas_get();

        public void focus_set( bool focus );
        public bool focus_get();

        public void pass_events_set( bool pass );
        public bool pass_events_get();

        public void propagate_events_set( bool prop );
        public bool propagate_events_get();

        public void repeat_events_set( bool repeat );
        public bool repeat_events_get();

        public void smart_callback_add( string event, Callback func );
        public void event_callback_add( CallbackType type, Callback func );
        [CCode (cname = "evas_event_callback_del_full")]
        public void* event_callback_del( CallbackType type, Callback func );

        [CCode (cname = "Evas_Object_Pointer_Mode", cprefix = "EVAS_OBJECT_POINTER_MODE_")]
        public enum PointerMode
        {
            AUTOGRAB,
            NOGRAB
        }
    }

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Object", cprefix = "evas_object_rectangle_", free_function = "evas_object_del")]
    public class Rectangle : Object
    {
        [CCode (cname = "evas_object_rectangle_add")]
        public Rectangle( Canvas e );
    }

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Object", cprefix = "evas_object_line_", free_function = "evas_object_del")]
    public class Line : Object
    {
        [CCode (cname = "evas_object_line_add")]
        public Line( Canvas e );
        public void xy_set( Coord x1, Coord y1, Coord x2, Coord y2 );
        public void xy_get( out Coord x1, out Coord y1, out Coord x2, out Coord y2 );
    }

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Object", cprefix = "evas_object_gradient_", free_function = "evas_object_del")]
    public class Gradient : Object
    {
        [CCode (cname = "evas_object_gradient_add")]
        public Gradient( Canvas e );
    }

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Object", cprefix = "evas_object_polygon_", free_function = "evas_object_del")]
    public class Polygon : Object
    {
        [CCode (cname = "evas_object_polygon_add")]
        public Polygon( Canvas e );
        public void point_add( Coord x, Coord y );
        public void points_clear();
    }

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Object", cprefix = "evas_object_image_", free_function = "evas_object_del")]
    public class Image : Object
    {
        [CCode (cname = "evas_object_image_add")]
        public Image( Canvas e );
        [CCode (cname = "evas_object_image_filled_add")]
        public Image.filled( Canvas e );

        public void border_set( int l, int r, int t, int b );
        public void border_get( out int l, out int r, out int t, out int b );

        public void border_scale_set( double scale );
        public double border_scale_get();

        public void border_center_fill_set( BorderFillMode mode );
        public BorderFillMode border_center_fill_get();

        public void size_set( int w, int h );
        public void size_get( out int w, out int h);

        public int stride_get();

        public void filled_set( bool setting );
        public bool filled_get();

        public void fill_set( Coord x, Coord y, Coord w, Coord h );
        public void fill_get( out Coord x, out Coord y, out Coord w, out Coord h );

        public FillSpread fill_spread_get();
        public void fill_spread_set( FillSpread spread );

        public void file_set( string file, string key );
        public void file_get( out string file, out string key );


        [CCode (cname = "evas_object_image_data_set")]
        public void data_set( void* data );
        public void data_convert( Evas.Colorspace to_cspace );
        //public void*   data_get( in Evas_Object* obj, bool for_writing );

        public void data_copy_set( void* data );
        public void data_update_add( int x, int y, int w, int h );

        public void alpha_set( bool has_alpha );
        public bool alpha_get();

        public bool smooth_scale_get();
        public void smooth_scale_set( bool smooth_scale );

        public void preload( bool cancel );
        public void reload();

        public bool save( string file, string key, string flags );

        public bool pixels_import( ImportSource pixels );

        public void pixels_get_callback_set( PixelsGetCallback cb );

        public void pixels_dirty_set( bool dirty );
        public bool pixels_dirty_get();

        public void load_dpi_set( double dpi );
        public double load_dpi_get();

        public void load_size_set( int w, int h );
        public void load_size_get( out int w, out int h );

        public void load_scale_down_set( int scale_down );
        public int load_scale_down_get();

        public void load_region_set( int x, int y, int w, int h );
        public void load_region_get( out int x, out int y, out int w, out int h );

        public void colorspace_set( Colorspace cspace );
        public Colorspace colorspace_get();

        public NativeSurface native_surface_get();

        public void scale_hint_set( ScaleHint hint );
        public ScaleHint scale_hint_get();

        public void content_hint_set( ContentHint hint );
        public ContentHint content_hint_get();

        [CCode (cname = "Evas_Object_Image_Pixels_Get_Cb", instance_pos = 0)]
        public delegate void PixelsGetCallback(Object o);
        //=======================================================================
        [CCode (cprefix = "EVAS_IMAGE_SCALE_HINT_", cname = "Evas_Image_Scale_Hint")]
        public enum ScaleHint
        {
            NONE,
            DYNAMIC,
            STATIC
        }

        [CCode (cprefix = "EVAS_PIXEL_FORMAT_", cname = "Evas_Pixel_Import_Pixel_Format")]
        public enum PixelFormat
        {
            NONE,
            ARGB32,
            YUV420P_601
        }

        //=======================================================================
        [CCode (cname = "Evas_Image_Content_Hint", cprefix = "EVAS_IMAGE_CONTENT_HINT_")]
        public enum ContentHint
        {
            NONE,
            DYNAMIC,
            STATIC
        }

        //=====================================================================
        [Compact]
        [CCode (cname = "_Evas_Pixel_Import_Pixel_Format")]
        public struct ImportSource
        {
            PixelFormat format;
            int w;
            int h;
            void** rows;
        }
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Evas_Object")]
    public class Table : Object
    {
        [CCode (cname = "evas_object_table_add")]
        public Table( Evas.Canvas evas );
        [CCode (cname = "evas_object_table_add_to")]
        public Table.add_to( Evas.Object parent );

        public void homegenous_set( HomogenousMode mode );
        public HomogenousMode homogeneous_get();
        public void padding_set( Coord horizontal, Coord vertical );
        public void padding_get( out Coord horizontal, out Coord vertical );
        public void align_set( double horizontal, double vertical );
        public void align_get( out double horizontal, out double vertical );
        public bool pack( Evas.Object child, uint col, uint row, uint colspan, uint rowspan );
        public bool unpack( Evas.Object child );
        public void clear();
        public void col_row_size_get( out int cols, out int rows );
        public Eina.Iterator<Evas.Object> iterator_new();
        public Eina.Accessor<Evas.Object> accessor_new();
        public Eina.List<Evas.Object> children_get();
        public Evas.Object child_get( uint16 col, uint16 row );


        //=======================================================================
        [CCode (cprefix = "EVAS_OBJECT_TABLE_HOMOGENEOUS_", cname = "Evas_Object_Table_Homogeneous_Mode")]
        public enum HomogenousMode
        {
            NONE,
            TABLE,
            ITEM
        }
    }

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Object", cprefix = "evas_object_text_", free_function = "evas_object_del")]
    public class Text : Object
    {
        [CCode (cname = "evas_object_text_add")]
        public Text( Canvas e );

        public void text_set( string text );
        public unowned string text_get();

        public unowned string font_source_get();
        public void font_source_set( string font );

        public void font_get( out string font, out FontSize size );
        public void font_set( string font, FontSize size );

        public Coord ascent_get();
        public Coord max_ascent_get();
        public Coord descent_get();
        public Coord max_decent_get();

        public Coord horiz_advance_get();
        public Coord vert_advance_get();
        public Coord inset_get();

        public bool char_pos_get( int pos, out Coord cx, out Coord cy, out Coord cw, out Coord ch );
        public int char_coords_get( Coord x, Coord y, out Coord cx, out Coord cy, Coord cw, out Coord ch );
        public int last_update_to_pos( Coord x, Coord y );

        public Style style_get();
        public void style_set( Style style );

        public void shadow_color_set( int r, int g, int b, int a );
        public void shadow_color_get( out int r, out int g, out int b, out int a );

        public void glow_color_set( int r, int g, int b, int a );
        public void glow_color_get( out int r, out int g, out int b, out int a );

        public void glow2_color_set( int r, int g, int b, int a );
        public void glow2_color_get( out int r, out int g, out int b, out int a );

        public void outline_color_set( int r, int g, int b, int a );
        public void outline_color_get( out int r, out int g, out int b, out int a );

        public void style_pad_get( out int r, out int g, out int b, out int a );
        //=======================================================================
        [CCode (cprefix = "EVAS_TEXT_STYLE_", cname = "Evas_Text_Style_Type")]
        public enum Style
        {
            PLAIN,
            SHADOW,
            OUTLINE,
            SOFT_OUTLINE,
            GLOW,
            OUTLINE_SHADOW,
            FAR_SHADOW,
            OUTLINE_SOFT_SHADOW,
            SOFT_SHADOW,
            FAR_SOFT_SHADOW
        }
    }

    //=====================================================================
    [CCode (cname = "Evas_Object", cprefix = "Evas_Textblock_")]
    public class Textblock
    {
        [CCode (cname = "evas_object_textblock_add")]
        public Textblock( Evas.Canvas evas );

        public static string escape_string_get( string escape );
        public static string string_escape_get( string @string , out int len_ret );
        public static string escape_string_range_get( string escape_start, string escape_end );

        public void style_set( Style style );
        public unowned Style style_get();

        public void replace_char_set( string ch );
        public string replace_char_get();

        public void text_markup_set( string text );
        public unowned string markup_get();

        public unowned Cursor cursor_get();
        public unowned Cursor cursor_new();

        public unowned NodeFormat node_format_first_get();
        public unowned NodeFormat node_format_last_get();
        public void node_format_remove_pair( NodeFormat n );

        public bool line_number_geometry_get( int line, out Coord cx, out Coord cy, out Coord cw, out Coord ch );

        public void clear();

        public void size_formated_get( out Coord w, out Coord h );
        public void size_native_get( out Coord w, out Coord h );
        public void style_insets_get( out Coord l, out Coord r, out Coord t, out Coord b );

        //=====================================================================
        [CCode (free_function = "evas_textblock_style_free")]
        public class Style
        {
            public Style();
            public void set( string text );
            public string get();
        }

        //=====================================================================
        [CCode (free_function = "evas_textblock_cursor_free")]
        public class Cursor
        {
            public Cursor( Textblock obj );

            public void text_markup_prepend( string text );
            public void paragraph_first();
            public void paragraph_last();
            public void paragraph_char_first();
            public void paragraph_char_last();
            public bool paragraph_next();
            public bool paragraph_prev();
            public unowned string paragraph_text_get();
            public int paragraph_text_length_get();

            public void set_at_format( NodeFormat n );
            public unowned NodeFormat format_get();
            public void at_format_set( NodeFormat fmt );
            public bool format_is_visible_get();
            public bool format_next();
            public bool format_prev();
            public bool is_format();

            public bool char_next();
            public bool char_prev();
            public void line_char_first();
            public void line_char_last();

            public int pos_get();
            public void pos_set( int pos );

            public bool line_set( int line );

            public int compare( Cursor cur2 );

            public void copy( Cursor dst );

            public int text_append ( string text );
            public int text_prepend( string text );

            public bool format_append( string format );
            public bool format_prepend( string format );

            public void node_delete();
            public void char_delete();
            public void range_delete( Cursor cur2 );
            public string range_text_get( Cursor cur2, TextType format );
            public string content_get();

            public int geometry_get( out Coord cx, out Coord cy, out Coord cw, out Coord ch, out BiDiDirection direction, out Type ctype );
            public int char_getmetry_get( out Coord cx, out Coord cy, out Coord cw, out Coord ch );
            public int line_geometry_get( out Coord cx, out Coord cy, out Coord cw, out Coord ch );
            public Eina.List<Textblock.Rectangle> range_geometry_get( Cursor cur2 );
            public bool format_item_geometry_get( out Coord cx, out Coord cy, out Coord cw, out Coord ch );

            public bool eol_get();

            public bool char_coord_set( Coord cx, Coord cy );
            public int line_coord_set( Coord cy );

            //=====================================================================
            [CCode (cname = "Evas_Textblock_Cursor_Type", cprefix = "EVAS_TEXTBLOCK_CURSOR_")]
            public enum Type
            {
                UNDER,
                BEFORE
            }
        }

        //=====================================================================
        public class NodeFormat
        {
            public NodeFormat next_get();
            public NodeFormat prev_get();
        }

        //=====================================================================
        [CCode (cname = "Evas_Textblock_Text_Type", cprefix = "EVAS_TEXTBLOCK_TEXT_")]
        public enum TextType
        {
            RAW,
            PLAIN,
            MARKUP
        }

        public struct Rectangle
        {
            Coord x;
            Coord y;
            Coord w;
            Coord h;
        }
    }
}


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

    public int render_method_lookup( string name );

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
        UNKNOWN_FORMAT
    }

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
    [CCode (cprefix = "EVAS_BORDER_FILL_", cname = "Evas_Border_Fill_Mode")]
    public enum BorderFillMode
    {
        NONE,
        DEFAULT,
        SOLID
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_IMAGE_SCALE_HINT_", cname = "Evas_Image_Scale_Hint")]
    public enum ImageScaleHint
    {
        NONE,
        DYNAMIC,
        STATIC
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
        IMAGE_PRELOADED
    }

    //=======================================================================
    [Compact]
    [CCode (cname = "Evas", free_function = "evas_free")]
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

        public void data_attach_set( void *data );
        public void* data_attach_get();

        public void focus_in();
        public void focus_out();
        public bool focus_state_get();

        public void image_cache_flush();
        public void image_cache_reload();
        public void image_cache_set( int size );
        public int image_cache_get();

        public void font_path_clear();
        public void font_path_append( string path );
        public void font_path_prepend( string path );
        public Eina.List<string> font_path_list();
        public void font_hinting_set( FontHintingFlags hinting );
        public FontHintingFlags font_hinting_get();
        public bool font_hinting_can_hint( FontHintingFlags hinting );
        public void font_cache_flush();
        public void font_cache_set( int size );
        public int font_cache_get();
        public Eina.List<string> font_available_list();
        public void font_available_list_free( Eina.List<string> available );

        public Object? bottom_get();
        public Object? top_get();
        public Object? focus_get();
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
    [CCode (cname = "Evas_Smart_Cb", instance_pos = 0)]
    public delegate void Callback( Evas.Object obj, void* event_info );

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Map", cprefix = "evas_map_", free_function = "evas_map_free")]
    public class Map
    {
        [CCode (cname = "evas_map_new")]
        public Map( int count );

        public void util_rotate( double degrees, Evas.Coord cx, Evas.Coord cy );
        public void util_zoom( double zoomx, double zoomy, Evas.Coord cx, Evas.Coord cy );
        public void util_rotate_3d( double dx, double dy, double dz, Evas.Coord cx, Evas.Coord cy, Evas.Coord cz, Evas.Coord px, Evas.Coord py, Evas.Coord z0, Evas.Coord foc );
        public void util_perspective( Evas.Coord px, Evas.Coord py, Evas.Coord z0, Evas.Coord foc );

        public bool util_clockwise_get();

        public void smooth_set( bool enabled );
        public bool smooth_get();

        public void alpha_set( bool enabled );
        public bool alpha_get();

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
        public void map_set( Map map );
        public Map map_get();

        public void name_set( string name );
        public unowned string name_get();
        public Object? name_find( string name );

        public unowned Canvas evas_get();

        public void focus_set( bool focus );
        public bool focus_get();

        public void repeat_events_set( bool repeat );
        public bool repeat_events_get();

        public void smart_callback_add( string event, Callback func );
        public void event_callback_add( CallbackType type, Callback func );

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
    }

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Object", cprefix = "evas_object_image_", free_function = "evas_object_del")]
    public class Image : Object
    {
        [CCode (cname = "evas_object_image_add")]
        public Image( Canvas e );

        public void size_set( int w, int h );
        public void size_get( out int w, out int h);

        public void filled_set(bool setting );
        public void file_set( string file, string key );

        [CCode (cname = "evas_object_image_data_set")]
        public void image_data_set( void* data );

        //public void   data_convert( Evas_Colorspace to_cspace );
        //public void*   data_get( in Evas_Object* obj, bool for_writing );

        public void data_copy_set( void* data );
        public void data_update_add( int x, int y, int w, int h );

        public void alpha_set( bool has_alpha );
        public bool alpha_get();
    }

    //=======================================================================
    [CCode (cprefix = "EVAS_TEXT_STYLE_", cname = "Evas_Text_Style_Type")]
    public enum TextStyle
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

    //=======================================================================
    [Compact]
    [CCode (cheader_filename = "Evas.h", cname = "Evas_Object", cprefix = "evas_object_text_", free_function = "evas_object_del")]
    public class Text : Object
    {
        [CCode (cname = "evas_object_text_add")]
        public Text( Canvas e );

        public void text_set( string text );
        public unowned string text_get();
    }

}


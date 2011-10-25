/*
 * Copyright (C) 2009-2011 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or ( at your option ) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */
[CCode (cprefix = "Elm_", lower_case_cprefix = "elm_", cheader_filename = "Elementary.h")]
namespace Elm
{
public void init( [CCode(array_length_pos = 0.9)] string[] args );
public void shutdown();
public void run();
public void exit();
public void need_efreet(); // ???
public void need_e_dbus(); // ???

//=======================================================================
namespace Scale
{
    public double get();
    public void set( double scale );
    public void all_set( double scale );
}

//=======================================================================
namespace Finger
{
    public Evas.Coord size_get();
    public void size_set( Evas.Coord size );
    public void size_all_set( Evas.Coord size );
}

//=======================================================================
namespace Policy
{
    [CCode (cname = "ELM_POLICY_QUIT")]
    public const uint QUIT;

    [CCode (cprefix = "ELM_POLICY_QUIT_")]
    public enum Quit
    {
        NONE,
        LAST_WINDOW_CLOSED,
    }

    public bool set( uint policy, int value );
    public int get( uint policy );
}


//=======================================================================
namespace Coords
{
    public void finger_size_adjust( int times_w, out Evas.Coord w, int times_h, out Evas.Coord h );
}


//=======================================================================
namespace Quicklaunch
{
    [CCode (has_target = false)]
    public delegate void Postfork_Func ( void* data );
    public void init( [CCode(array_length_pos = 0.9)] string[] args );
    public void sub_init( [CCode (array_length_pos = 0.9)] string[] args );
    public void sub_shutdown();
    public void shutdown();
    public void seed();
    public bool prepare( [CCode (array_length_pos = 0.9)] string[] args );
    public bool fork( [CCode (array_length_pos = 0.9)] string[] args, string cwd, Postfork_Func postfork_func, void *postfork_data );
    public void cleanup();
    public int fallback( [CCode (array_length_pos = 0.9)] string[] args );
    public string exe_path_get( string exe );

}


//=======================================================================
[CCode (cname = "Elm_Theme", free_function = "elm_theme_free")]
public class Theme
{
    [CCode (cname = "elm_theme_new")]
    public Theme();

    public void overlay_add( string item );
    public void overlay_del( string item );
    public void extension_add( string item );
    public void extension_del( string item );

    public void @set( string theme );
    public string get();
    public void flush();
    public void full_flush();
    public void all_set( string theme );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public abstract class Object : Evas.Object
{
    public void scale_set( double scale );
    public double scale_get();
    public void style_set( string style );
    public string style_get();
    public void disabled_set( bool disabled );
    public bool disabled_get();

    public bool focus_get();
    public void focus();
    public void unfocus();
    public void focus_allow_set( bool enable );
    public bool focus_allow_get();

    public void scroll_hold_push();
    public void scroll_hold_pop();
    public void scroll_freeze_push();
    public void scroll_freeze_pop();
    
    public unowned string text_get();
    public void text_set( string label );

    public bool widget_check();
    public Elm.Object parent_widget_get();
    public string widget_type_get();

    public void theme_set( Theme th );
    public Theme theme_get();
}


//=======================================================================
[CCode (cprefix = "ELM_WIN_")]
public enum WinType
{
    BASIC,
    DIALOG_BASIC,
    DESKTOP,
    DOCK,
    TOOLBAR,
    MENU,
    UTILITY,
    SPLASH
}


//=======================================================================
[CCode (cprefix = "ELM_WIN_KEYBOARD_")]
public enum WinKeyboardMode
{
    UNKNOWN,
    OFF,
    ON,
    ALPHA,
    NUMERIC,
    PIN,
    PHONE_NUMBER,
    HEX,
    TERMINAL,
    PASSWORD,
    IP,
    HOST,
    FILE,
    URL,
    KEYPAD,
    J2ME
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Win : Elm.Object
{
    [CCode (cname = "elm_win_add")]
    public Win( Elm.Object? parent = null, string name = "Untitled", WinType t = WinType.BASIC );

    public void resize_object_add( Evas.Object subobj );
    public void resize_object_del( Evas.Object subobj );
    public void title_set( string title );
    public void autodel_set( bool autodel );
    public void activate();
    public void lower();
    public void raise();
    public void borderless_set( bool borderless );
    public bool borderless_get();
    public void shaped_set( bool shaped );
    public bool shaped_get();
    public void alpha_set( bool alpha );
    public bool alpha_get();
    public bool transparent_get();
    public void transparent_set( bool transparent );
    public void override_set( bool override_ );
    public bool override_get();
    public void fullscreen_set( bool fullscreen );
    public bool fullscreen_get();
    public void maximized_set( bool maximized );
    public bool maximized_get();
    public void iconified_set( bool iconified );
    public bool iconified_get();
    public void layer_set( int layer );
    public int layer_get();
    public void rotation_set( int rotation );
    public int rotation_get();
    public void sticky_set( bool sticky );
    public bool sticky_get();
    public void conformant_set( bool conformant );
    public bool conformant_get();

    public void keyboard_mode_set( WinKeyboardMode mode );
    public void keyboard_win_set( bool is_keyboard );

    public void screen_position_get( out int x, out int y );

    public Elm.Win inwin_add();
    public void inwin_activate();
    public void inwin_content_set( Elm.Object content );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Bg : Elm.Object
{
    [CCode (cname = "elm_bg_add")]
    public Bg( Elm.Object? parent );

    public void file_set( string file, string? group=null );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Icon : Elm.Object
{
    [CCode (cname = "elm_icon_add")]
    public Icon( Elm.Object? parent );

    public void file_set( string file, string? group=null );
    public void standard_set( string name );
    public void smooth_set( bool smooth );
    public void no_scale_set( bool no_scale );
    public void scale_set( bool scale_up, bool scale_down );
    public void fill_outside_set( bool fill_outside );
    public void prescale_set( int size );
}


//=======================================================================
[CCode (cprefix = "ELM_IMAGE_")]
public enum ImageOrient
{
    ORIENT_NONE,
    ROTATE_90_CW,
    ROTATE_180_CW,
    ROTATE_90_CCW,
    FLIP_HORIZONTAL,
    FLIP_VERTICAL,
    FLIP_TRANSPOSE,
    FLIP_TRANSVERSE
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Image : Elm.Object
{
    [CCode (cname = "elm_image_add")]
    public Image( Elm.Object? parent );

    public void file_set( string file, string? group=null );
    public void smooth_set( bool smooth );
    public void object_size_get( out int w, out int h );
    public void no_scale_set( bool no_scale );
    public void scale_set( bool scale_up, bool scale_down );
    public void fill_outside_set( bool fill_outside );
    public void prescale_set( int size );
    public void orient_set( ImageOrient orient );
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Box : Elm.Object
{
    [CCode (cname = "elm_box_add")]
    public Box( Elm.Object? parent );

    public void horizontal_set( bool horizontal );
    public void homogenous_set( bool homogenous );
    public void pack_start( Elm.Object subobj );
    public void pack_end( Elm.Object subobj );
    public void pack_before( Elm.Object subobj, Elm.Object before );
    public void pack_after( Elm.Object subobj, Elm.Object after );
    public void clear();
    public void unpack( Elm.Object subobj );
    public void unpack_all();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Button : Elm.Object
{
    [CCode (cname = "elm_button_add")]
    public Button( Elm.Object? parent );

    public void icon_set( Elm.Object icon );
    public Elm.Object icon_get();
    public void autorepeat_set( bool autorepeat );
    public void autorepeat_initial_timeout_set( double t );
    public void autorepeat_gap_timeout_set( double t );

}


//=======================================================================
[CCode (cprefix = "ELM_SCROLLER_POLICY_")]
public enum ScrollerPolicy
{
    AUTO,
    ON,
    OFF
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Scroller : Elm.Object
{
    [CCode (cname = "elm_scroller_add")]
    public Scroller( Elm.Object? parent );

    public void content_set( Elm.Object child );
    public void content_min_limit( bool w, bool h );
    public void region_show( Evas.Coord x, Evas.Coord y, Evas.Coord w, Evas.Coord h );
    public void policy_set(ScrollerPolicy h_policy, ScrollerPolicy v_policy);
    public void region_get( out Evas.Coord x, out Evas.Coord y, out Evas.Coord w, out Evas.Coord h );
    public void child_size_get( out Evas.Coord w, out Evas.Coord h );
    public void bounce_set( bool h_bounce, bool v_bounce );
    public void page_relative_set( double h_pagerel, double v_pagerel );
    public void page_size_set( Evas.Coord h_pagesize, Evas.Coord v_pagesize );
    public void region_bring_in( Evas.Coord x, Evas.Coord y, Evas.Coord w, Evas.Coord h );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Label : Elm.Object
{
    [CCode (cname = "elm_label_add")]
    public Label( Elm.Object? parent );

    public void line_wrap_set( bool wrap );
    public bool line_wrap_get();
    public void wrap_width_set( Evas.Coord w );
    public Evas.Coord wrap_width_get();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Toggle : Elm.Object
{
    [CCode (cname = "elm_toggle_add")]
    public Toggle( Elm.Object? parent );

    public void icon_set( Elm.Object icon );
    public Elm.Object icon_get();
    public void states_labels_set( string onlabel, string offlabel );
    public void state_set( bool state );
    public bool state_get();
    public void state_pointer_set( bool* statep );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Frame : Elm.Object
{
    [CCode (cname = "elm_frame_add")]
    public Frame( Elm.Object? parent );

    public void content_set( Elm.Object content );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Table : Elm.Object
{
    [CCode (cname = "elm_table_add")]
    public Table( Elm.Object? parent );

    public void homogenous_set( bool homogenous );
    public void pack( Elm.Object subobj, int x, int y, int w, int h );
    public void padding_set( Evas.Coord horizontal, Evas.Coord vertical );
    public void unpack( Elm.Object subobj );
    public void clear();
}


//=======================================================================
[CCode (cprefix = "ELM_CLOCK__")]
public enum ClockDigedit
{
    NONE,
    HOUR_DECIMAL,
    HOUR_UNIT,
    MIN_DECIMAL,
    MIN_UNIT,
    SEC_DECIMAL,
    SEC_UNIT,
    ALL
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Clock : Elm.Object
{
    [CCode (cname = "elm_clock_add")]
    public Clock( Elm.Object? parent );

    public void time_set( int hrs, int min, int sec );
    public void time_get( out int hrs, out int min, out int sec );
    public void edit_set( bool edit );
    public bool edit_get();
    public void digit_edit_set( ClockDigedit digedit );
    public ClockDigedit digit_edit_get();
    public void show_am_pm_set( bool am_pm );
    public void show_seconds_set( bool seconds );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Layout : Elm.Object
{
    [CCode (cname = "elm_layout_add")]
    public Layout( Elm.Object? parent );

    public void file_set( string file, string group );
    public void theme_set( string clas, string group, string style );
    public void content_set( string swallow, Elm.Object content );
    public unowned Edje.Object edje_get();
    public void sizing_eval();
}


//=======================================================================
[CCode (cprefix = "ELM_NOTIFY_ORIENT_")]
public enum NotifyOrient
{
    TOP,
    CENTER,
    BOTTOM,
    LEFT,
    RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Notify : Elm.Object
{
    [CCode (cname = "elm_notify_add")]
    public Notify( Elm.Object? parent );

    public void content_set( Evas.Object content );
    public void parent_set( Evas.Object parent );
    public void orient_set( NotifyOrient orient );
    public void timeout_set( int timeout );
    public void timer_init();
    public void repeat_events_set( bool repeat );
}

//=======================================================================
[CCode (cprefix = "ELM_HOVER_AXIS_")]
public enum HoverAxis
{
    NONE,
    HORIZONTAL,
    VERTICAL,
    BOTH,
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Hover : Elm.Object
{
    [CCode (cname = "elm_hover_add")]
    public Hover( Elm.Object? parent );

    public void target_set( Elm.Object target );
    public void parent_set( Elm.Object parent );
    public void content_set( string swallow, Elm.Object content );
    public string best_content_location_get( HoverAxis pref_axis );
}


//=======================================================================
[CCode (cname = "Elm_Entry_Anchor_Info")]
public struct EntryAnchorInfo
{
    string name;
    int button;
    Evas.Coord x;
    Evas.Coord y;
    Evas.Coord w;
    Evas.Coord h;
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Entry : Elm.Object
{
    [CCode (cname = "elm_entry_add")]
    public Entry( Elm.Object? parent );

    public void single_line_set( bool single_line );
    public void password_set( bool password );
    public void entry_set( string entry );
    public unowned string entry_get();
    public string selection_get();
    public void entry_insert( string entry );
    public void line_wrap_set( bool wrap );
    public void editable_set( bool editable );
    public void select_none();
    public void select_all();

    public bool cursor_next();
    public bool cursor_prev();
    public bool cursor_up();
    public bool cursor_down();
    public void cursor_begin_set();
    public void cursor_end_set();
    public void cursor_line_begin_set();
    public void cursor_line_end_set();
    public void cursor_selection_begin();
    public void cursor_selection_end();
    public bool cursor_is_format_get();
    public bool cursor_is_visible_format_get();
    public string cursor_content_get();
    public void selection_cut();
    public void selection_copy();
    public void selection_paste();
    public void context_menu_clear();
    //public void context_menu_item_add( string label, string icon_file, IconType icon_type, EventCallback callback );
    public void context_menu_disabled_set( bool disabled );
    public bool context_menu_disabled_get();
    public static string markup_to_utf8( string s );
    public static string utf8_to_markup( string s );
}


//=======================================================================
[CCode (cprefix = "ELM_TEXT_FORMAT_")]
public enum TextFormat
{
    PLAIN_UTF8,
    MARKUP_UTF8,
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Notepad : Elm.Object
{
    [CCode (cname = "elm_notepad_add")]
    public Notepad( Elm.Object? parent );

    public void file_set( string file, TextFormat format );
    public void bounce_set( bool h_bounce, bool v_bounce );
}


//=======================================================================
[CCode (cname = "Elm_Entry_Anchorview_Info")]
public struct EntryAnchorviewInfo
{
    string name;
    int button;
    Elm.Object hover;
    /*
    struct
    {
     Evas_Coord x, y, w, h;
    } anchor, hover_parent;
    */
    bool hover_left;
    bool hover_right;
    bool hover_top;
    bool hover_bottom;
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Anchorview : Elm.Object
{
    [CCode (cname = "elm_anchorview_add")]
    public Anchorview( Elm.Object? parent );

    public void text_set( string text );
    public void hover_parent_set( Elm.Object parent );
    public void hover_style_set( string style );
    public void hover_end();
    public void bounce_set( bool h_bounce, bool v_bounce );
}


//=======================================================================
[CCode (cname = "Elm_Entry_Anchorblock_Info")]
public struct EntryAnchorblockInfo
{
    string name;
    int button;
    Elm.Object hover;
    /*
    struct
    {
     Evas_Coord x, y, w, h;
    } anchor, hover_parent;
    */
    bool hover_left;
    bool hover_right;
    bool hover_top;
    bool hover_bottom;
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Anchorblock : Elm.Object
{
    [CCode (cname = "elm_anchorblock_add")]
    public Anchorblock( Elm.Object? parent );

    public void hover_parent_set( Elm.Object parent );
    public void hover_style_set( string style );
    public void hover_end();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Bubble : Elm.Object
{
    [CCode (cname = "elm_bubble_add")]
    public Bubble( Elm.Object? parent );

    public void label_set( string label );
    public void info_set( string info );
    public void content_set( Elm.Object content );
    public void icon_set( Elm.Object icon );
    public Elm.Object icon_get();
    public void corner_set( string corner );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Photo : Elm.Object
{
    [CCode (cname = "elm_photo_add")]
    public Photo( Elm.Object? parent );

    public void file_set( string file );
    public void size_set( int size );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Hoversel : Elm.Object
{
    [CCode (cname = "elm_hoversel_add")]
    public Hoversel( Elm.Object? parent );

    public void horizontal_set( bool horizontal );
    public void hover_parent_set( Elm.Object parent );
    public void icon_set( Elm.Object icon );
    public Elm.Object icon_get();
    public void hover_begin();
    public void hover_end();
    public void clear();
    public Eina.List items_get();
    public unowned HoverselItem item_add( string label, string? icon_file, IconType icon_type, Evas.Callback? func = null );
    public void item_del( HoverselItem item );
    /*
    EAPI void         elm_hoversel_item_del_cb_set(Elm_Hoversel_Item *it, void (*func)(void *data, Evas_Object *obj, void *event_info));
    EAPI void        *elm_hoversel_item_data_get(Elm_Hoversel_Item *it);
    EAPI const char  *elm_hoversel_item_label_get(Elm_Hoversel_Item *it);
    EAPI void         elm_hoversel_item_icon_set(Elm_Hoversel_Item *it, const char *icon_file, const char *icon_group, Elm_Icon_Type icon_type);
    EAPI void         elm_hoversel_item_icon_get(Elm_Hoversel_Item *it, const char **icon_file, const char **icon_group, Elm_Icon_Type *icon_type);
    */
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Toolbar : Elm.Object
{
    [CCode (cname = "elm_toolbar_add")]
    public Toolbar( Elm.Object? parent );

    public void icon_size_set( int icon_size );
    public int icon_size_get();
    [CCode (cname = "elm_toolbar_item_append")]
    public ToolbarItem append( string icon, string label, Evas.Callback func );
    [CCode (cname = "elm_toolbar_item_prepend")]
    public ToolbarItem prepend( string icon, string label, Evas.Callback func );
    public ToolbarItem item_insert_before( ToolbarItem before, string icon, string label, Evas.Callback cb);
    public ToolbarItem item_insert_after( ToolbarItem after, string icon, string label, Evas.Callback cb);
    public ToolbarItem first_item_get();
    public ToolbarItem last_item_get();
    // Note: elm_boolbar_item_ functions living in ToolbarItem scope
    public void scrollable_set( bool scrollable );
    public void homogenous_set( bool homogenous );
    public void menu_parent_set( Elm.Object parent );
    [CCode (cname = "elm_toolbar_item_unselect_all")]
    public void unselect_all();
    public void align_set( bool align );
    [CCode (cname = "elm_toolbar_item_find_by_label")]
    public ToolbarItem find_by_label(string label);
    public ToolbarItem selected_item_get();
    public ShrinkMode mode_shrink_get();
    public void mode_shrink_set( ShrinkMode mode );
    public bool menu_get();
    public void menu_set( bool menu );

    [CCode (cname = "Elm_Toolbar_Shrink_Mode", cprefix = "ELM_TOOLBAR_SHRINK_")]
    public enum ShrinkMode
    {
        NONE,
        HIDE,
        SCROLL,
        MENU
    }
}


//=======================================================================
[Compact]
[CCode (cname = "Elm_Toolbar_Item", free_function = "elm_toolbar_item_del")]
public class ToolbarItem
{
    public Elm.Object icon_get();
    public void icon_set( string icon );
    public unowned string label_get();
    public void label_set( string label );
    public ToolbarItem next_get();
    public ToolbarItem prev_get();
    //public void del_cb_set( ... );
    public bool disabled_get();
    public void disabled_set( bool disabled );
    public bool separator_get();
    public void separator_set( bool separator );
    public void menu_set( bool menu );
    public Elm.Object menu_get();
    public Elm.Object toolbar_get();
    public void priority_set( int priority );
    public int priority_get();
    public string icon_name_get();
    public void* data_get();
    public void data_set( void* data );
    public bool selected_get();
    public void selected_set( bool selected );
    public bool homogenous_get();
    public void homogenous_set( bool selected );
    public Elm.Object end_get();
    public void end_set( Elm.Object obj );
    public State state_add( string icon, string lable, Evas.Callback cb);
    public bool state_set( State state );
    public void state_unset();
    public State state_get();
    public State state_next();
    public State state_prev();

    public void tooltip_text_set( string text );
    public void tooltip_unset();
    public void tooltip_style_set( string style );
    public unowned string tooltip_style_get();

    public void cursor_set( string text );
    public void cursor_unset();
    public void cursor_style_set( string style );
    public unowned string cursor_style_get();
    public bool cursor_engine_only_get();
    public void cursor_engine_only_set( bool only );

    [CCode (cname = "Elm_Toolbar_Item_State", free_function = "g_free")]
    public class State {}
}


//=======================================================================
[Compact]
[CCode (cname = "Elm_Menu_Item", free_function = "elm_menu_item_del")]
public class MenuItem
{
    [CCode (cname = "elm_menu_object_get")]
    public Elm.Object object_get();
    public void label_set( string label );
    //public void del_cb_set( ... );
    public unowned string label_get();
    public void icon_set( Elm.Object icon );
    public void disabled_set( bool disabled );
    public void* item_data_get();
    public void data_set( void* data );
    public Eina.List item_subitems_get();
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Menu : Elm.Object
{
    [CCode (cname = "elm_menu_add")]
    public Menu( Elm.Object? parent );

    public void parent_set( Elm.Object parent );
    public void move( Evas.Coord x, Evas.Coord y );
    public MenuItem item_add( Elm.Object icon, string label, Evas.Callback? func = null );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class List : Elm.Object
{
    [CCode (cname = "elm_list_add")]
    public List( Elm.Object? parent );

    [CCode (cname = "elm_list_item_append")]
    public ListItem append( string label, Elm.Object? icon, Elm.Object? end, Evas.Callback? func);
    [CCode (cname = "elm_list_item_prepend")]
    public ListItem prepend( string label, Elm.Object? icon, Elm.Object? end, Evas.Callback? func );
    public ListItem insert_before( ListItem before, string label, Elm.Object? icon, Elm.Object? end, Evas.Callback? func );
    public ListItem insert_after( ListItem after, string label, Elm.Object? icon, Elm.Object? end, Evas.Callback? func );
    public void go();
    public void multi_select_set( bool multi );
    public bool multi_select_get();
    public void horizontal_mode_set( ListMode mode );
    public ListMode horizontal_mode_get();
    public void always_select_mode_set( bool always_select );
    public unowned Eina.List<ListItem> items_get();
    public unowned ListItem selected_item_get();
    public unowned Eina.List<ListItem> selected_items_get();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Carousel : Elm.Object
{
    [CCode (cname = "elm_carousel_add")]
    public Carousel( Elm.Object? parent );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Slider : Elm.Object
{
    [CCode (has_target = false)]
    public delegate string IndicatorFormatFunc( double val );

    [CCode (cname = "elm_slider_add")]
    public Slider( Elm.Object? parent );

    public void label_set( string label );
    public unowned string label_get();
    public void icon_set( Elm.Object icon );
    public Elm.Object icon_get();
    public void span_size_set( Evas.Coord size );
    public void unit_format_set( string format );
    public void indicator_format_set( string indicator );
    public void indicator_format_function_set( IndicatorFormatFunc func );
    public void horizontal_set( bool horizontal );
    public void min_max_set( double min, double max );
    public void value_set( double val );
    public double value_get();
    public void inverted_set( bool inverted );
}


//=======================================================================
[CCode (cprefix = "ELM_GENLIST_ITEM_")]
public enum GenlistItemFlags
{
    NONE,
    SUBITEMS,
}

[CCode (cname = "Elm_Gen_Item_Label_Get_Cb", has_target = false)]
public delegate string GenlistItemLabelGetFunc( Elm.Object obj, string part );
[CCode (cname = "Elm_Gen_Item_Content_Get_Cb", has_target = false)]
public delegate Elm.Object? GenlistItemContentGetFunc( Elm.Object obj, string part );
[CCode (cname = "Elm_Gen_Item_State_Get_Cb", has_target = false)]
public delegate bool GenlistItemStateGetFunc( Elm.Object obj, string part );
[CCode (cname = "Elm_Gen_Item_Del_Cb", has_target = false)]
public delegate void GenlistItemDelFunc( Elm.Object obj );

//=======================================================================
[CCode (cname = "Elm_Gen_Item_Class_Func", destroy_function = "")]
public struct GenlistItemClassFunc
{
    public GenlistItemLabelGetFunc label_get;
    public GenlistItemContentGetFunc content_get;
    public GenlistItemStateGetFunc state_get;
    public GenlistItemDelFunc del;
}

//=======================================================================
[CCode (cname = "Elm_Genlist_Item_Class", destroy_function = "")]
public struct GenlistItemClass
{
    public string item_style;
    public GenlistItemClassFunc func;
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Genlist : Elm.Object
{
    [CCode (cname = "elm_genlist_add")]
    public Genlist( Elm.Object? parent );
    public unowned GenlistItem item_append( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItemFlags flags, Evas.Callback callback );
    public unowned GenlistItem item_prepend( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItemFlags flags, Evas.Callback callback );
    public unowned GenlistItem item_insert_before( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItem before, GenlistItemFlags flags, Evas.Callback callback );
    public unowned GenlistItem item_insert_after( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItem after, GenlistItemFlags flags, Evas.Callback callback );

    public void clear();
    public void multi_select_set( bool multi );
    public void horizontal_mode_set( ListMode mode );
    public void always_select_mode_set( bool always_select );
    public void no_select_mode_set( bool no_select );

    public unowned GenlistItem at_xy_item_get( Evas.Coord x, Evas.Coord y, out int posret );
    public unowned GenlistItem selected_item_get();
    public Eina.List<unowned GenlistItem> selected_items_get();
    public unowned GenlistItem first_item_get();
    public unowned GenlistItem last_item_get();
}


//=======================================================================
[Compact]
[CCode (cname = "Elm_Genlist_Item", free_function = "") /* Caution! Genlist items are owned by the list. */ ]
public class GenlistItem
{
   public GenlistItem next_get();
   public GenlistItem prev_get();
   public Genlist genlist_get();
   public void subitems_clear();

   public void selected_set( bool selected );
   public bool selected_get();
   public void expanded_set( bool expanded );
   public bool expanded_get();
   public void disabled_set( bool disabled );
   public bool disabled_get();
   public void display_only_set( bool display_only );
   public bool display_only_get();

   public void show();
   public void bring_in();
   public void top_show();
   public void top_bring_in();
   public void middle_show();
   public void middle_bring_in();
   public void del();

   // FIXME: Caution: Do we really have access to the data field or is it used by Vala?
   public void* data_get();
   public void data_set( void* data );
   public void update();

   public Elm.Genlist object_get();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Check : Elm.Object
{
    [CCode (cname = "elm_check_add")]
    public Check( Elm.Object? parent );

    public void label_set( string label );
    public unowned string label_get();
    public void icon_set( Elm.Object icon );
    public Elm.Object icon_get();
    public void state_set( bool state );
    public bool state_get();
    public void state_pointer_set( bool* statep );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Radio : Elm.Object
{
    [CCode (cname = "elm_radio_add")]
    public Radio( Elm.Object? parent );

    public void label_set( string label );
    public unowned string label_get();
    public void icon_set( Elm.Object icon );
    public Elm.Object icon_get();
    public void group_add( Elm.Object group );
    public void state_value_set( int value );
    public void value_set( int value );
    public int value_get();
    public void value_pointer_set( out int valuep );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Pager : Elm.Object
{
    [CCode (cname = "elm_pager_add")]
    public Pager( Elm.Object? parent );

    public void content_push( Elm.Object content );
    public void content_pop();
    public void content_promote( Elm.Object content );
    public Elm.Object content_bottom_get();
    public Elm.Object content_top_get();

    public void style_set( string style );
}


//=======================================================================
[Compact]
[CCode (cname = "Elm_Slideshow_Item", free_function = "elm_slideshow_item_del")]
public class SlideshowItem
{
    public Slideshow object_get();
}

public delegate Evas.Object? SlideshowItemGetFunc( Elm.Object obj );
public delegate void SlideshowItemDelFunc( Elm.Object obj );


//=======================================================================
[CCode (cname = "Elm_Slideshow_Item_Class_Func", copy_function = "", destroy_function = "")]
public struct SlideshowItemClassFunc
{
    [CCode (delegate_target = false)]
    public SlideshowItemGetFunc get;
    [CCode (delegate_target = false)]
    public SlideshowItemDelFunc del;
}


//=======================================================================
[CCode (cname = "Elm_Slideshow_Item_Class", destroy_function = "")]
public struct SlideshowItemClass
{
    public SlideshowItemClassFunc func;
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Slideshow : Elm.Object
{
    [CCode (cname = "elm_slideshow_add")]
    public Slideshow( Elm.Object? parent );

    public SlideshowItem item_add( SlideshowItemClass itc, void* data );
    public static void show( SlideshowItem item );
    public void next();
    public void previous();
    public Eina.List<string> transitions_get();
    public void transition_set( string transition );
    public void timeout_set( int timeout );
    public int timeout_get();
    public void loop_set( int loop );
    public void clear();
    public Eina.List<SlideshowItem> items_get();
    public SlideshowItem item_current_get();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Fileselector : Elm.Object
{
    [CCode (cname = "elm_fileselector_add")]
    public Fileselector( Elm.Object? parent );

    public void is_save_set( bool is_save );
    public bool is_save_get();
    public void folder_only_set( bool only );
    public bool folder_only_get();
    public void buttons_ok_cancel_set( bool buttons );
    public bool buttons_ok_cancel_get();
    public void expandable_set( bool expand );
    public void path_set( string path );
    public string path_get();
    public unowned string selected_get();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class ProgressBar : Elm.Object
{
    [CCode (cname = "elm_progressbar_add")]
    public ProgressBar( Elm.Object? parent );

    public void pulse_set( bool pulse );
    public bool pulse_get();
    public void value_set( double val );
    public double value_get();
    public void label_set( string label );
    public void icon_set( Elm.Object icon );
    public void span_size_set( Evas.Coord size );
    public void unit_format_set( string format );
    public void horizontal_set( bool horizontal );
    public void inverted_set( bool inverted );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Separator : Elm.Object
{
    [CCode (cname = "elm_separator_add")]
    public Separator( Elm.Object? parent );
    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Spinner : Elm.Object
{
    [CCode (cname = "elm_spinner_add")]
    public Spinner( Elm.Object? parent );
    public void label_format_set( string format );
    public unowned string label_format_get();
    public void min_max_set( double min, double max );
    public void step_set( double step );
    public void value_set( double val );
    public double value_get();
    public void wrap_set( bool wrap );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Index : Elm.Object
{
    [CCode (cname = "elm_index_add")]
    public Index( Elm.Object? parent );
    public void active_set( bool active );
    public void item_level_set( int level );
    public int item_level_get();
    public void* item_selected_get( int level );
    public void item_append( string letter, void* item );
    public void item_prepend( string letter, void* item );
    public void item_append_relative( string letter, void *item, void *relative );
    public void item_prepend_relative( string letter,void *item, void *relative );
    public void item_del( void *item );
    public void item_clear();
    public void item_go( int level );
}


//=======================================================================
[CCode (cprefix = "ELM_SCROLLER_AXIS_")]
public enum ScrollerAxis
{
    HORIZONTAL,
    VERTICAL,
}

//=======================================================================
[CCode (cprefix = "ELM_ICON_")]
public enum IconType
{
    NONE,
    FILE,
    STANDARD,
}


//=======================================================================
[CCode (cprefix = "ELM_LIST_")]
public enum ListMode
{
    COMPRESS,
    SCROLL,
    LIMIT,
}


//=======================================================================
[Compact]
[CCode (cname = "Elm_Hoversel_Item", free_function = "")]
public class HoverselItem
{
}


//=======================================================================
[Compact]
[CCode (cname = "Elm_List_Item", free_function = "elm_list_item_del")]
public class ListItem
{
    public void selected_set( bool selected );
    public void show();
    public void* data_get();
    public unowned string label_get();
    public void label_set( string label );
    public Elm.Object icon_get();
    public void icon_set( Elm.Object icon );
    public Elm.Object end_get();
    public void end_set( Elm.Object end );
}


//=======================================================================
[CCode (cprefix = "ELM_PHOTOCAM_ZOOM_MODE_")]
public enum PhotocamZoomMode
{
    MANUAL,
    AUTO_FIT,
    AUTO_FILL,
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Photocam : Elm.Object
{
    [CCode (cname = "elm_photocam_add")]
    public Photocam( Elm.Object? parent );

    public int file_set( string file );
    public string file_get();
    public void zoom_set( double zoom );
    public double zoom_get();
    public void zoom_mode_set( PhotocamZoomMode mode );
    public PhotocamZoomMode zoom_mode_get();
    public void image_size_get( out int w, out int h );
    public void region_get( out int x, out int y, out int w, out int h );
    public void image_region_show( int x, int y, int w, int h );
    public void image_region_bring_in( int x, int y, int w, int h );
    public void paused_set( bool paused );
    public bool paused_get();
}


//=======================================================================
[CCode (cprefix = "ELM_MAP_ZOOM_MODE_")]
public enum MapZoomMode
{
    MANUAL,
    AUTO_FIT,
    AUTO_FILL,
    LAST
}


//=======================================================================
[CCode (cprefix = "ELM_MAP_SOURCE_MODE_")]
public enum MapSource
{
    MAPNIK,
    OSMARENDER,
    CYCLEMAP,
    MAPLINT,
    CUSTOM_1,
    CUSTOM_2,
    CUSTOM_3,
    CUSTOM_4,
    CUSTOM_5,
    CUSTOM_6,
    CUSTOM_7,
    LAST
}

//=======================================================================
[Compact]
[CCode (cname = "Elm_Map_Marker_Class", free_function = "")]
public class MapMarkerClass
{
    [CCode (cname = "elm_map_marker_class_new")]
    public MapMarkerClass( Elm.Map? map );

    public void style_set( string style );
    //public void icon_cb_set( ElmMapMarkerIconGetFunc icon_get );
    //public void get_cb_set( ElmMapMarkerGetFunc get );
    //public void del_cb_set( ElmMapMarkerDelFunc del );
}


//=======================================================================
[Compact]
[CCode (cname = "Elm_Map_Group_Class", free_function = "")]
public class MapGroupClass
{
    [CCode (cname = "elm_map_group_class_new")]
    public MapGroupClass( Elm.Map? map );

   public void style_set( string style );
   //public void icon_cb_set( ElmMapGroupIconGetFunc icon_get );
   //public void data_set( void *data );
   public void zoom_displayed_set( int zoom );
   public void zoom_grouped_set( int zoom );
   [CCode (instance_pos = 0.1)]
   public void hide_set( Elm.Map obj, bool hide );
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Map : Elm.Object
{
    [CCode (cname = "elm_map_add")]
    public Map( Elm.Object? parent );

    public void zoom_set( int zoom );
    public double zoom_get();
    public void zoom_mode_set( MapZoomMode mode );
    public MapZoomMode zoom_mode_get();

    public void geo_region_get( out double lon, out double lat );
    public void geo_region_bring_in( double lon, double lat );
    public void geo_region_show( double lon, double lat );

    public void paused_set( bool paused );
    public bool paused_get();
    public void paused_markers_set( bool paused );
    public bool paused_markers_get();

    [CCode (cname = "elm_map_utils_convert_coord_into_geo")]
    public static void convert_coord_into_geo( int x, int y, int size, out double lon, out double lat );
    [CCode (cname = "elm_map_utils_convert_geo_into_coord")]
    public static void convert_geo_into_coord( double lon, double lat, int size, out int x, out int y );

    public void max_marker_per_group_set( int max );
    public static void markers_list_show( Eina.List<MapMarker> list );
    public void bubbles_close();

    public void source_set( MapSource source );
    public MapSource source_get();

    //public static void source_custom_api_set( MapSource source, const char *label, int zoom_min, int zoom_max, ElmMapSourceURLFunc url_cb );
    public static int source_zoom_min_get( MapSource source );
    public static int source_zoom_max_get( MapSource source );
    public static string source_name_get( MapSource source );
}

//=======================================================================
[Compact]
[CCode (cname = "Elm_Map_Marker", free_function = "elm_map_marker_remove")]
public class MapMarker : Elm.Object
{
    [CCode (cname = "elm_map_marker_add")]
    public MapMarker( Elm.Object parent, double lon, double lat, MapMarkerClass klass, MapGroupClass group, void* data );

    public void         bring_in();
    public void         show();
    public Elm.Map      object_get();
    public void         update();
}

//=======================================================================
[CCode (cprefix = "ELM_PANEL_ORIENT_")]
public enum PanelOrient
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Panel : Elm.Object
{
    [CCode (cname = "elm_panel_add")]
    public Panel( Elm.Object? parent );

    public void orient_set( PanelOrient orient );
    public void content_set( Elm.Object content );
}


//=======================================================================
[CCode (cprefix = "ELM_FLIP_MODE_")]
public enum FlipMode
{
    ROTATE_Y_CENTER_AXIS,
    ROTATE_X_CENTER_AXIS,
    ROTATE_XZ_CENTER_AXIS,
    ROTATE_YZ_CENTER_AXIS
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Flip : Elm.Object
{
    [CCode (cname = "elm_flip_add")]
    public Flip( Elm.Object? parent );

    public void content_front_set( Elm.Object front );
    public void content_back_set( Elm.Object back );
    public bool front_get();
    public void perspective_set( Evas.Coord foc, Evas.Coord x, Evas.Coord y );
    public void go( FlipMode mode );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class ScrolledEntry : Elm.Object
{
    [CCode (cname = "elm_scrolled_entry_add")]
    public ScrolledEntry( Elm.Object? parent );

    public void single_line_set( bool single_line );
    public void password_set( bool password );
    public void entry_set(  string entry );
    public unowned string entry_get();
    public unowned string selection_get();
    public void entry_insert( string entry );
    public void line_wrap_set( bool wrap );
    public void line_char_wrap_set(  bool wrap );
    public void editable_set( bool editable);
    public void select_none();
    public void select_all();
    public bool cursor_next();
    public bool cursor_prev();
    public bool cursor_up();
    public bool cursor_down();
    public void cursor_begin_set();
    public void cursor_end_set();
    public void cursor_line_begin_set();
    public void cursor_line_end_set();
    public void cursor_selection_begin();
    public void cursor_selection_end();
    public bool cursor_is_format_get();
    public bool cursor_is_visible_format_get();
    public unowned string cursor_content_get();
    public void selection_cut();
    public void selection_copy();
    public void selection_paste();
    public void context_menu_clear();
    public void context_menu_item_add( string label, string icon_file, IconType icon_type, Evas.Callback callback );
}

} /* namespace Elm */

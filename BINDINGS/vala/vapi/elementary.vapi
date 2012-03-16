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
public int init( [CCode(array_length_pos = 0.9)] string[] args );
public int shutdown();
public void run();
public void exit();
public bool need_efreet();
public bool need_e_dbus();
public bool need_ethumb();
public bool need_web();
public int policy_get( uint policy );
public bool policy_set( uint policy, int value );
public void language_set( string lang );

/////////////////////////////////////////////////////////////////////////
// BASE OBJECT CLASSES:
/////////////////////////////////////////////////////////////////////////

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public abstract class Object : Evas.Object
{ //TODO: add stuff from elm_cursor.h

    [CCode (cname = "Elm_Object_Select_Mode", cprefix = "ELM_OBJECT_SELECT_MODE_")]
    public enum SelectMode
    {
        DEFAULT,
        ALWAYS,
        NONE,
        ONLY,
        MAX
    }

    public void             access_info_set( string txt );

    public unowned Elm.Object? part_content_get( string? part );
    public void             part_content_set( string? part, Elm.Object content );
    public void             part_content_unset( string? part, Elm.Object content );
    public unowned Elm.Object? content_get();
    public void             content_set( Elm.Object content );
    public void             content_unset( Elm.Object content );

    public void             domain_translatable_text_part_set( string? part, string? domain, string text );
    public void             domain_translatable_text_set( string? domain, string text );
    public unowned string?  translatable_text_part_get( string? part );
    public unowned string?  translatable_text_get();
    public void             translatable_text_set( string text );

    public unowned string?  part_text_get( string? part );
    public void             part_text_set( string? part, string label );
    public unowned string?  text_get();
    public void             text_set( string label );

    public bool             disabled_get();
    public void             disabled_set( bool disabled );

    public bool             mirrored_get();
    public void             mirrored_set( bool mirrored );
    public bool             mirrored_automatic_get();
    public void             mirrored_automatic_set( bool mirrored_automatic );

    public double           scale_get();
    public void             scale_set( double scale );

    public unowned string   style_get();
    public bool             style_set( string? style );

    public bool             widget_check();
    public Elm.Object?      parent_widget_get();
    public Elm.Object?      top_widget_get();
    public unowned string   widget_type_get();

    public Elm.Object?      name_find( string name, int recurse );

    public bool             cnp_selection_clear( SelType selection );

    public void             cursor_set( string text );
    public void             cursor_unset();
    public void             cursor_style_set( string style );
    public unowned string   cursor_style_get();

    public bool             focus_get();
    public void             focus_set();
    public void             focus_allow_set( bool enable );
    public bool             focus_allow_get();
    public unowned Eina.List focus_custom_chain_get();
    public void             focus_custom_chain_set( Eina.List objs );
    public void             focus_custom_chain_unset();
    public void             focus_custom_chain_append( Elm.Object child, Elm.Object? relative_child );
    public void             focus_custom_chain_prepend( Elm.Object child, Elm.Object? relative_child );
    public void             focus_next( Elm.FocusDirection dir );
    public bool             tree_focus_allow_get();
    public void             tree_focus_allow_set( bool focusable );


    public void             scroll_hold_push();
    public void             scroll_hold_pop();
    public void             scroll_freeze_push();
    public void             scroll_freeze_pop();
    public bool             scroll_lock_x_get();
    public void             scroll_lock_x_set( bool lock );
    public bool             scroll_lock_y_get();
    public void             scroll_lock_y_set( bool lock );

    public Theme            theme_get();
    public void             theme_set( Theme th );

    public void             tooltip_show();
    public void             tooltip_hide();
    public void             tooltip_text_set( string text );
    public void             tooltip_domain_translatable_text_set( string? domain, string text );
    public void             tooltip_translatable_text_set( string text );
    public void             tooltip_content_cb_set( Elm.TooltipContentCallback func, void* data, Evas.Callback del_cb );
    public void             tooltip_unset();
    public unowned string   tooltip_style_get();
    public void             tooltip_style_set( string style );
    public bool             tooltip_window_mode_get();
    public bool             tooltip_window_mode_set( bool disable );

    public void             signal_emit( string? emission, string? source );

    public void             signal_callback_add( string? emission, string? source, Edje.SignalCallback func );
    public void             signal_callback_del( string? emission, string? source, Edje.SignalCallback func );

    public void             event_callback_add( Elm.EventCallback func );
    public void             event_callback_del( Elm.EventCallback func );

    public void             tree_dump();
    public void             tree_dot_dump( string file );

}


//=======================================================================
[Compact]
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public abstract class ObjectItem
{
    [CCode (cname = "Elm_Object_Item_Smart_Cb", instance_pos = 1.5)]
    public delegate void SmartCallback( void* data, void* event_info );

    public void                 access_info_set( string txt );

    public unowned Elm.Object?  widget_get();

    public unowned Elm.Object?  part_content_get( string? part );
    public void                 part_content_set( string? part, Elm.Object content );
    public void                 part_content_unset( string? part, Elm.Object content );
    public unowned Elm.Object?  content_get();
    public void                 content_set( Elm.Object content );
    public void                 content_unset( Elm.Object content );

    public unowned string?      part_text_get( string? part );
    public void                 part_text_set( string? part, string label );
    public unowned string?      text_get();
    public void                 text_set( string label );

    public void*                data_get();
    public void                 data_set( void* data );

    public bool                 disabled_get();
    public void                 disabled_set( bool disabled );

    public void                 tooltip_text_set( string text );
    public void                 tooltip_content_cb_set( Elm.TooltipContentCallback func, void* data, Evas.Callback del_cb );
    public void                 tooltip_unset();
    public unowned string       tooltip_style_get();
    public void                 tooltip_style_set( string style );
    public bool                 tooltip_window_mode_get();
    public bool                 tooltip_window_mode_set( bool disable );

    public unowned string?      cursor_get();
    public void                 cursor_set( string cursor );
    public void                 cursor_unset();
    public unowned string?      cursor_style_get();
    public void                 cursor_style_set( string style );
    public bool                 cursor_engine_only_get();
    public void                 cursor_engine_only_set( bool engine_only );

    //public void                 smart_callback_add( string event, Elm.ObjectItem.SmartCallback func, void* data );
    //public void                 smart_callback_del( string event, Elm.ObjectItem.SmartCallback func );
    public void                 del_cb_set( Evas.Callback func );

    public void                 del();

    public void                 signal_emit( string? emission, string? source );

}

/////////////////////////////////////////////////////////////////////////
// elc_ctxpopup.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Ctxpopup_Direction", cprefix = "ELM_CTXPOPUP_DIRECTION_")]
public enum CtxpopupDirection
{
    DOWN,
    RIGHT,
    LEFT,
    UP,
    UNKNOWN
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Ctxpopup : Elm.Object
{

    public static unowned Ctxpopup? add( Elm.Object parent );

    public void hover_parent_set( Elm.Object parent );
    public unowned Elm.Object? hover_parent_get();
    public void clear();
    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
    public void item_append( string label, Elm.Icon icon, Evas.Callback func, void* data );
    public void direction_priority_set( CtxpopupDirection first, CtxpopupDirection second, CtxpopupDirection third, CtxpopupDirection fourth );
    public void direction_priority_get( out CtxpopupDirection first, out CtxpopupDirection second, out CtxpopupDirection third, out CtxpopupDirection fourth );
    public CtxpopupDirection direction_get();
    public void dismiss();
}



/////////////////////////////////////////////////////////////////////////
// elc_fileselector_button.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class FileselectorButton : Elm.Object
{
    public static unowned FileselectorButton? add( Elm.Object parent );

    public void window_title_set( string title );
    public unowned string window_title_get();
    public void window_size_set( Evas.Coord width, Evas.Coord height );
    public void window_size_get( out Evas.Coord width, out Evas.Coord height );
    public void path_set( string path );
    public unowned string path_get();
    public void expandable_set( bool expandable );
    public bool expandable_get();
    public void folder_only_set( bool val );
    public bool folder_only_get();
    public void is_save_set( bool val );
    public bool is_save_get();
    public void inwin_mode_set( bool val );
    public bool inwin_mode_get();
}

/////////////////////////////////////////////////////////////////////////
// elc_fileselector_entry.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class FileselectorEntry : Elm.Object
{
    public static unowned FileselectorEntry? add( Elm.Object parent );

    public void window_title_set( string title );
    public unowned string window_title_get();
    public void window_size_set( Evas.Coord width, Evas.Coord height );
    public void window_size_get( out Evas.Coord width, out Evas.Coord height );
    public void path_set( string path );
    public unowned string path_get();
    public void expandable_set( bool expandable );
    public bool expandable_get();
    public void folder_only_set( bool val );
    public bool folder_only_get();
    public void is_save_set( bool val );
    public bool is_save_get();
    public void inwin_mode_set( bool val );
    public bool inwin_mode_get();
    public void selected_set( bool val );
    public bool selected_mode_get();
}

/////////////////////////////////////////////////////////////////////////
// elc_fileselector.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Fileselector_Mode", cprefix = "ELM_FILESELECTOR_")]
public enum FileselectorMode
{
    LIST,
    GRID,
    LAST
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Fileselector : Elm.Object
{
    public static unowned Fileselector? add( Elm.Object parent );

    public void is_save_set( bool is_save );
    public bool is_save_get();
    public void folder_only_set( bool only );
    public bool folder_only_get();
    public void buttons_ok_cancel_set( bool buttons );
    public bool buttons_ok_cancel_get();
    public void expandable_set( bool expand );
    public bool expandable_get();
    public void path_set( string path );
    public unowned string path_get();
    public bool selected_set( string path );
    public unowned string selected_get();
    public void mode_set( FileselectorMode mode );
    public FileselectorMode mode_get();
}

/////////////////////////////////////////////////////////////////////////
// elc_hoversel.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Hoversel_Item", free_function = "elm_object_item_del")]
public class HoverselItem : Elm.ObjectItem
{
    public void icon_set( string icon_file, string icon_group, IconType icon_type );
    public void icon_get( out unowned string icon_file, out unowned string icon_group, out IconType icon_type );
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Hoversel : Elm.Object
{
    public static unowned Hoversel? add( Elm.Object parent );

    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
    public void hover_parent_set( Elm.Object parent );
    public Elm.Object hover_parent_get();
    public void hover_begin();
    public void hover_end();
    public bool expanded_get();
    public void clear();
    public Eina.List<unowned Elm.ObjectItem> items_get();
    public unowned HoverselItem item_add( string label, string? icon_file, IconType icon_type, Evas.Callback? func = null );
    //TODO: is data param used by vala or can be used by the user?
}


/////////////////////////////////////////////////////////////////////////
// elc_multibuttonentry.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class MultibuttonentryItem : Elm.ObjectItem
{
    public void selected_set( bool selected );
    public bool selected_get();
    public unowned MultibuttonentryItem? prev_get();
    public unowned MultibuttonentryItem? next_get();
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Multibuttonentry : Elm.Object
{

    [CCode (has_target = false)]
    public delegate void MultibuttonentryItemFilterCb(Elm.Multibuttonentry obj,  string item_label, void* item_data, void* data );

    public static unowned Multibuttonentry? add( Elm.Object parent );

    public Elm.Entry entry_get();
    public bool expanded_get();
    public void expanded_set( bool expanded );
    public unowned MultibuttonentryItem? item_prepend( string label, Evas.Callback? func = null, void* data = null );
    public unowned MultibuttonentryItem? item_append( string label, Evas.Callback? func = null, void* data = null );
    public unowned MultibuttonentryItem? item_insert_before( Multibuttonentry before, Evas.Callback? func = null, void* data = null );
    public unowned MultibuttonentryItem? item_insert_after( Multibuttonentry after, Evas.Callback? func = null, void* data = null );
    public unowned Eina.List<unowned MultibuttonentryItem>? items_get();
    public unowned MultibuttonentryItem? first_item_get();
    public unowned MultibuttonentryItem? last_item_get();
    public unowned MultibuttonentryItem? selected_item_get();
    public void clear();
    public void item_filter_append( MultibuttonentryItemFilterCb func, void* data );
    public void item_filter_prepend( MultibuttonentryItemFilterCb func, void* data );
    public void item_filter_remove( MultibuttonentryItemFilterCb func, void* data );
}

/////////////////////////////////////////////////////////////////////////
// elc_naviframe.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class NaviframeItem : Elm.ObjectItem
{
    public void pop_to();
    public void promote();
    public void style_set( string? item_style = null );
    public unowned string style_get();
    public void title_visible_set( bool visible );
    public bool title_visible_get();
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Naviframe : Elm.Object
{
    public static unowned Naviframe? add( Elm.Object parent );

    public unowned Elm.NaviframeItem? item_push( string title_label, Elm.Object?  prev_btn, Elm.Object? next_btn, Elm.Object content, string? item_style );
    public unowned Elm.Object? item_pop();
    public void content_preserve_on_pop_set( bool preserve );
    public bool content_preserve_on_pop_get();
    public unowned Elm.NaviframeItem? top_item_get();
    public unowned Elm.NaviframeItem? bottom_item_get();
    public void prev_btn_auto_pushed_set( bool auto_pushed );
    public bool prev_btn_auto_pushed_get();
    public Eina.List<unowned Elm.NaviframeItem>? items_get();
    public void event_enabled_set( bool enabled );
    public bool event_enabled_get();
    public unowned Elm.NaviframeItem? item_simple_push( Elm.Object content );
    public void item_simple_promote( Elm.Object content );
    public unowned Elm.NaviframeItem? item_insert_before( NaviframeItem before, string title_label, Elm.Object?  prev_btn, Elm.Object? next_btn, Elm.Object content, string? item_style );
    public unowned Elm.NaviframeItem? item_insert_after( NaviframeItem after, string title_label, Elm.Object?  prev_btn, Elm.Object? next_btn, Elm.Object content, string? item_style );

}

/////////////////////////////////////////////////////////////////////////
// elm_actionslider.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Actionslider_Pos", cprefix = "ELM_ACTIONSLIDER_")]
public enum ActionsliderPos
{
    NONE,
    LEFT,
    CENTER,
    RIGHT,
    ALL
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Actionslider : Elm.Object
{
    public static unowned Actionslider? add( Elm.Object parent );

    public unowned string selected_label_get();
    public void indicator_pos_set( ActionsliderPos pos );
    public ActionsliderPos indicator_pos_get();
    public void magnet_pos_set( ActionsliderPos pos );
    public ActionsliderPos magnet_pos_get();
    public void enabled_pos_set( ActionsliderPos pos );
    public ActionsliderPos enabled_pos_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_app.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
namespace App
{
    public void info_set( void* mainfunc, string dom, string checkfile );
    public void compile_bin_dir_set( string dir );
    public void compile_data_dir_set( string dir );
    public void compile_lib_dir_set( string dir );
    public void compile_locale_set( string dir );
    public void prefix_dir_set( string dir );
    public unowned string bin_dir_get();
    public unowned string data_dir_get();
    public unowned string lib_dir_get();
    public unowned string prefix_dir_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_bg.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Bg_Option", cprefix = "ELM_BG_OPTION_")]
public enum BgOption
{
    CENTER,
    SCALE,
    STRETCH,
    TILE,
    LAST
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Bg : Elm.Object
{
    public static unowned Bg? add( Elm.Object parent );

    public void file_get( out unowned string file, out unowned string group );
    public bool file_set( string file, string? group=null );

    public BgOption option_get();
    public void option_set( BgOption option );

    public void color_get( out int r, out int g, out int b );
    public void color_set( int r, int g, int b );

    public void load_size_set( Evas.Coord w, Evas.Coord h );
}

/////////////////////////////////////////////////////////////////////////
// elm_box.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//TODO: move this callbacks to Ecore_Cb
public delegate void BoxFreeData( void* data );

public delegate void TransitionEndCb( void* data );

[Compact]
[CCode (cname = "Elm_Box_Transition", free_function = "elm_box_transition_free")]
public class BoxTransition {
    public BoxTransition( double duration, 
                            Evas.ObjectBoxLayout start_layout, void* start_layout_data, Elm.BoxFreeData start_layout_free_data, 
                            Evas.ObjectBoxLayout end_layout, void* end_layout_data, Elm.BoxFreeData end_layout_free_data, 
                            Elm.TransitionEndCb func, void* transition_end_data );
}

[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Box : Elm.Object
{
    public static unowned Box? add( Elm.Object parent );

    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
    public void homogeneous_set( bool homogeneous );
    public bool homogeneous_get();
    public void pack_start( Elm.Object subobj );
    public void pack_end( Elm.Object subobj );
    public void pack_before( Elm.Object subobj, Elm.Object before );
    public void pack_after( Elm.Object subobj, Elm.Object after );
    public void clear();
    public void unpack( Elm.Object subobj );
    public void unpack_all();
    Eina.List<unowned Elm.Object> children_get();
    public void padding_set( Evas.Coord horizontal, Evas.Coord vertical );
    public void padding_get( out Evas.Coord horizontal, out Evas.Coord vertical );
    public void align_set( double horizontal, double vertical );
    public void align_get( out double horizontal, out double vertical );
    public void recalculate();
    public void layout_set( Evas.ObjectBoxLayout cb, void* data, Elm.BoxFreeData free_func );
    public void layout_transition( Evas.ObjectBoxData priv, void* data );
}


/////////////////////////////////////////////////////////////////////////
// elm_bubble.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Bubble_Pos", cprefix = "ELM_BUBBLE_POS_")]
public enum BubblePos
{
    INVALID,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Bubble : Elm.Object
{
    public static unowned Bubble? add( Elm.Object parent );

    public void pos_set( BubblePos corner );
    public BubblePos pos_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_button.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Button : Elm.Object
{
    public static unowned Button? add( Elm.Object parent );

    public void autorepeat_set( bool autorepeat );
    public bool autorepeat_get();
    public void autorepeat_initial_timeout_set( double t );
    public bool autorepeat_initial_timeout_get();
    public void autorepeat_gap_timeout_set( double t );
    public bool autorepeat_gap_timeout_get();

}

/////////////////////////////////////////////////////////////////////////
// elm_cache.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
namespace Cache
{
    public void all_flush();
}


/////////////////////////////////////////////////////////////////////////
// elm_calendar.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Calendar_Mark_Repeat_Type", cprefix = "ELM_CALENDAR_")]
public enum CalendarMarkRepeatType
{
    UNIQUE,
    DAILY,
    WEEKLY,
    MONTHLY,
    ANUALLY
}

//=======================================================================
[CCode (cname = "Elm_Calendar_Mark", free_function = "elm_calendar_mark_del")]
public struct CalendarMark {
    public void del();
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Calendar : Elm.Object
{
    public delegate string FormatCb( GLib.Time stime );

    public static unowned Calendar? add( Elm.Object parent );

    public unowned string[] weekdays_names_get();
    public void weekdays_names_set( string[] weekdays );
    public void min_max_year_set( int min, int max );
    public void min_max_year_get( out int min, out int max );
    public void day_selection_disabled_set( bool disabled );
    public bool day_selection_disabled_get();
    public void selected_time_set( GLib.Time selected_time );
    public bool selected_time_get( out GLib.Time selected_time );
    public void format_function_set( FormatCb format_function );
    public unowned CalendarMark mark_add( string mark_type, GLib.Time mark_time, CalendarMarkRepeatType repeat );
    public void marks_clear();
    public Eina.List<unowned CalendarMark>? marks_get();
    public void marks_draw();
    public void interval_set( double interval );
    public double interval_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_check.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Check : Elm.Object
{
    public static unowned Check? add( Elm.Object parent );

    public void state_set( bool state );
    public bool state_get();
    public void state_pointer_set( bool* statep );
}


/////////////////////////////////////////////////////////////////////////
// elm_clock.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Clock_Digedit", cprefix = "ELM_CLOCK_EDIT_")]
public enum ClockEditMode
{
    DEFAULT,
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
    public static unowned Clock? add( Elm.Object parent );

    public void time_set( int hrs, int min, int sec );
    public void time_get( out int hrs, out int min, out int sec );
    public void edit_set( bool edit );
    public bool edit_get();
    public void edit_mode_set( ClockEditMode digedit );
    public ClockEditMode edit_mode_get();
    public void show_am_pm_set( bool am_pm );
    public bool show_am_pm_get();
    public void show_seconds_set( bool seconds );
    public bool show_seconds_get();
    public void first_interval_set( double interval );
    public double first_interval_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_cnp.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Sel_Type", cprefix = "ELM_SEL_TYPE_")]
public enum SelType
{
    PRIMARY,
    SECONDARY,
    XDND,
    CLIPBOARD
}

//=======================================================================
[CCode (cname = "Elm_Sel_Format", cprefix = "ELM_SEL_FORMAT_")]
public enum SelFormat
{
    TARGETS,
    NONE,
    TEXT,
    MARKUP,
    IMAGE,
    VCARD,
    HTML,
    MAX
}

//=======================================================================
[CCode (cname = "Elm_Selection_Data")]
public struct SelectionData {
    Evas.Coord x;
    Evas.Coord y;
    void *data;
    size_t len;
}

//=======================================================================
namespace Cnp
{
    public delegate bool DropCallback(void *data, Elm.Object o, SelectionData ev );

    public bool selection_set( SelType selection, Elm.Object widget, SelFormat format, void* buf, size_t buflen );
    public bool selection_get( Elm.Object obj, SelType selection, SelFormat format, Elm.Object widget, DropCallback datacb, void* udata );
}


/////////////////////////////////////////////////////////////////////////
// elm_colorselector.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Color_RGBA")]
public struct ColorRGBA {
    uint r;
    uint g;
    uint b;
    uint a;
}

[CCode (cname = "Elm_Custom_Palette")]
public struct CustomPalette {
    unowned string palette_name;
    Eina.List<ColorRGBA>? color_list;
}

//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class ColorselectorPaletteItem : Elm.ObjectItem
{

    public void color_set( int r, int g, int b, int a );
    public void color_get( out int r, out int g, out int b, out int a );
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Colorselector : Elm.Object
{
    [CCode (cname = "Elm_Colorselector_Mode", cprefix = "ELM_COLORSELECTOR_")]
    public enum Mode
    {
        PALETTE,
        COMPONENTS,
        BOTH
    }

    public static unowned Colorselector? add( Elm.Object parent );

    public void color_set( int r, int g, int b, int a );
    public void color_get( out int r, out int g, out int b, out int a );
    public void mode_set( Mode mode );
    public Mode mode_get();
    public unowned ColorselectorPaletteItem? palette_color_add( int r, int g, int b, int a );
    public void clear();
    public void palette_name_set( string palette_name );
    public unowned string palette_name_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_config.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
namespace Config
{
    public void save();
    public void reload();
    public void all_flush();
    public double tooltip_delay_get();
    public void tooltip_delay_set( double delay );
    public unowned string engine_get();
    public void engine_set( string engine );
    public unowned string preferred_engine_get();
    public void preferred_engine_set( string engine );
    public bool mirrored_get();
    public void mirrored_set( bool mirrored );

    namespace Cache
    {
        public void all_flush();

        public bool flush_enabled_get();
        public void flush_enabled_set( bool enabled );

        public int flush_interval_get();
        public void flush_interval_set( int size );
    }

    namespace CacheFont
    {
        public void cache_size_set( int size );
        public int cache_size_get();
    }

    namespace CacheImage
    {
        public void cache_size_set( int size );
        public int cache_size_get();
    }

    namespace CacheEdjeFile
    {
        public void cache_size_set( int size );
        public int cache_size_get();
    }

    namespace CacheEdjeCollection
    {
        public void cache_size_set( int size );
        public int cache_size_get();
    }

    namespace Cursor
    {
        public void engine_only_set( bool engine_only );
        public bool engine_only_get();
    }

    namespace Finger
    {
        public Evas.Coord size_get();
        public void size_set( Evas.Coord size );
    }

    namespace Focus
    {

        public bool highlight_animate_get();
        public void highlight_animate_set( bool animate );

        public bool highlight_enabled_get();
        public void highlight_enabled_set( bool enable );

    }

    namespace Longpress
    {
        public double timeout_get();
        public void timeout_set( double longpress_timeout );
    }

    namespace Password
    {
        public bool show_last_get();
        public void show_last_set( bool password_show_last );
        public double show_last_timeout_get();
        public void show_last_timeout_set( double password_show_last_timeout );
    }

    namespace Profile
    {
        public unowned string current_get();
        public unowned string dir_get( string profile, bool is_user );
        public void dir_free( string p_dir );
        public unowned Eina.List<string> list_get();
        public void list_free( Eina.List<string> l );
        public void set( string profile );
    }

    namespace Scale
    {
        public double get();
        public void set( double scale );
    }

    namespace Scroll
    {
        public bool bounce_enabled_get();
        public void bounce_enabled_set( bool enabled );

        public double bounce_friction_get();
        public void bounce_friction_set( double friction );

        public double page_friction_get();
        public void page_friction_set( double friction );

        public double bring_in_scroll_friction_get();
        public void bring_in_scroll_friction_set( double friction );

        public double zoom_friction_get();
        public void zoom_friction_set( double friction );

        public bool thumbscroll_enabled_get();
        public void thumbscroll_enabled_set( bool enabled );

        public uint thumbscroll_threshold_get();
        public void thumbscroll_threshold_set( uint threshold );

        public double thumbscroll_momentum_threshold_get();
        public void thumbscroll_momentum_threshold_set( double threshold );

        public double thumbscroll_friction_get();
        public void thumbscroll_friction_set( double friction );

        public double thumbscroll_border_friction_get();
        public void thumbscroll_border_friction_set( double friction );

        public double thumbscroll_sensitivity_friction_get();
        public void thumbscroll_sensitivity_friction_set( double friction );
    }

    namespace Text
    {
        [CCode (cname = "Elm_Text_Class")]
        public struct Class
        {
            unowned string name;
            unowned string desc;
        }

        public unowned Eina.List<Elm.Config.Text.Class> classes_list_get(); //TODO: Class items should be unowned?
        public void classes_list_free( Eina.List<Elm.Config.Text.Class> list );
    }

    namespace Font
    {

        [CCode (cname = "Elm_Font_Overlay")]
        public struct Overlay
        {
            unowned string text_class;
            unowned string font;
            Evas.FontSize size;
        }

        [CCode (cname = "Elm_Font_Properties", free_func="elm_font_properties_free")]
        public struct Properties
        {
            unowned string name;
            Eina.List<string> styles;
        }

        public unowned Eina.List<Elm.Config.Font.Overlay> overlay_list_get(); //TODO: Class items should be unowned?
        public void overlay_set( string text_class, string font, Evas.FontSize size );
        public void overlay_unset( string text_class );
        public void overlay_apply();
    }
}

/////////////////////////////////////////////////////////////////////////
// elm_conformant.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Conformant : Elm.Object
{
    public static unowned Conformant? add( Elm.Object parent );

    public Evas.Object content_area_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_diskselector.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class DiskselectorItem : ObjectItem
{
    public unowned GenlistItem? next_get();
    public unowned GenlistItem? prev_get();

    public void selected_set( bool selected );
    public bool selected_get();
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Diskselector : Elm.Object
{
    public static unowned Diskselector? add( Elm.Object parent );

    public bool round_enabled_get();
    public void round_enabled_set( bool round );
    public int side_text_max_length_get();
    public void side_text_max_length_set( int len );
    public int display_item_num_get();
    public void display_item_num_set( int num );
    public void bounce_get( out bool h_bounce, out bool v_bounce );
    public void bounce_set( bool h_bounce, bool v_bounce );
    public void scroller_policy_get( out ScrollerPolicy policy_h, out ScrollerPolicy policy_v );
    public void scroller_policy_set( ScrollerPolicy policy_h, ScrollerPolicy policy_v );
    public void clear();
    public unowned Eina.List<unowned Elm.ObjectItem> items_get();
    public unowned Elm.ObjectItem? item_append( string label, Elm.Icon icon, Evas.Callback func );
    public unowned Elm.ObjectItem? selected_item_get();
    public unowned Elm.ObjectItem? first_item_get();
    public unowned Elm.ObjectItem? last_item_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_engine.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
namespace Engine
{
    public unowned string get();
    public void set( string engine );
}

//=======================================================================
namespace PreferredEngine
{
    public unowned string get();
    public void set( string engine );
}

/////////////////////////////////////////////////////////////////////////
// elm_entry.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Cnp_Mode", cprefix = "ELM_CNP_MODE_")]
public enum CnpMode
{
    MARKUP,
    NO_IMAGE,
    PLAINTEXT
}

//=======================================================================
[CCode (cname = "Elm_Wrap_Type", cprefix = "ELM_WRAP_")]
public enum WrapType
{
    NONE,
    CHAR,
    WORD,
    MIXED,
    LAST
}

//=======================================================================
[CCode (cname = "Elm_Text_Format", cprefix = "ELM_TEXT_FORMAT_")]
public enum TextFormat
{
    PLAIN_UTF8,
    MARKUP_UTF8
}

//=======================================================================
[CCode (cname = "Elm_Input_Panel_Layout", cprefix = "ELM_INPUT_PANEL_LAYOUT_")]
public enum InputPanelLayout
{
    NORMAL,
    NUMBER,
    EMAIL,
    URL,
    PHONENUMBER,
    IP,
    MONTH,
    NUMBERONLY,
    INVALID,
    HEX,
    TERMINAL,
    PASSWORD
}

//=======================================================================
[CCode (cname = "Elm_Input_Panel_Lang", cprefix = "ELM_INPUT_PANEL_LANG_")]
public enum InputPanelLang
{
    AUTOMATIC,
    ALPHABET
}

//=======================================================================
[CCode (cname = "Elm_Autocapital_Type", cprefix = "ELM_AUTOCAPITAL_TYPE_")]
public enum AutocapitalType
{
    NONE,
    WORD,
    SENTENCE,
    ALLCHARACTER
}

//=======================================================================
[CCode (cname = "Elm_Input_Panel_Return_Key_Type", cprefix = "ELM_INPUT_PANEL_RETURN_KEY_TYPE_")]
public enum InputPanelReturnKeyType
{
    DEFAULT,
    DONE,
    GO,
    JOIN,
    LOGIN,
    NEXT,
    SEARCH,
    SEND
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
//anonymous struct in C API. I think anonymous structs are not allowed in vala.
public struct HoverParent
{
    Evas.Coord x;
    Evas.Coord y;
    Evas.Coord w;
    Evas.Coord h;
}

[CCode (cname = "Elm_Entry_Anchor_Hover_Info")]
public struct EntryAnchorHoverInfo
{
    EntryAnchorInfo anchor_info;
    unowned Elm.Hover? hover;
    HoverParent hover_parent;
    bool hover_left;
    bool hover_right;
    bool hover_top;
    bool hover_bottom;
}

//=======================================================================
public struct EntryFilterLimitSize
{
    int max_char_count;
    int max_byte_count;
}

//=======================================================================
public struct EntryFilterAcceptSet {
    unowned string accepted;
    unowned string rejected;
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Entry : Elm.Object
{
    [CCode (has_target = false)]
    public delegate void EntryFilterCallback( void *data, Elm.Entry entry, string[] text );
    [CCode (has_target = false)]
    public delegate Elm.Object? ItemProviderCallback( void* data, Elm.Entry entry, string item );

    [CCode (cname = "Elm_Entry_Item_Provider_Cb", instance_pos = 1.5)]
    public delegate unowned Elm.Object? ItemProviderCb( void* data, string item );

    public static unowned Entry? add( Elm.Object parent );

    public void single_line_set( bool single_line );
    public bool single_line_get();
    public void password_set( bool password );
    public bool password_get();
    public void entry_set( string entry );
    public unowned string? entry_get();
    public void entry_append( string entry );
    public bool is_empty();
    public unowned string? selection_get();
    public unowned Elm.Object textblock_get();
    public void calc_force();
    public void entry_insert( string entry );
    public void line_wrap_set( WrapType wrap );
    public WrapType line_wrap_get();
    public void editable_set( bool editable );
    public bool editable_get();
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
    public bool cursor_geometry_get( out Evas.Coord x, out Evas.Coord y, out Evas.Coord w, out Evas.Coord h );
    public void cursor_pos_set( int pos );
    public int cursor_pos_get();
    public void selection_cut();
    public void selection_copy();
    public void selection_paste();
    public void context_menu_clear();
    public void context_menu_item_add( string label, string icon_file, IconType icon_type, EventCallback callback );
    public void context_menu_disabled_set( bool disabled );
    public bool context_menu_disabled_get();
    public void item_provider_append( ItemProviderCb func );
    public void item_provider_prepend( ItemProviderCb func );
    public void item_provider_remove( ItemProviderCb func );
    public void markup_filter_append( ItemProviderCb func );
    public void markup_filter_prepend( ItemProviderCb func );
    public void markup_filter_remove( ItemProviderCb  func );

    public bool file_set( string file, TextFormat format );
    public void file_get( out unowned string file, out TextFormat format );
    public void file_save();
    public void autosave_set( bool autosave );
    public bool autosave_get();
    public void scrollable_set( bool scrollable );
    public bool scrollable_get();

    public bool icon_visible_set( bool setting );
    public void end_visisble_set( bool setting );
    public void scrollbar_policy_set( ScrollerPolicy h, ScrollerPolicy v );
    public void bounce_set( bool v_bounce );
    public bool bounce_get();
    public void input_panel_layout_set( InputPanelLayout layout );
    public InputPanelLayout input_panel_layout_get();
    public void autocapital_type_set( AutocapitalType autocapital );
    public AutocapitalType autocapital_type_get();
    public void input_panel_enabled_set( bool enabled );
    public bool input_panel_enabled_get();
    public void input_panel_show();
    public void input_panel_hide();
    public void input_panel_language_set( InputPanelLang lang );
    public InputPanelLang input_panel_language_get();
    public void input_panel_imdata_set( void* data, int len );
    public void input_panel_imdata_get( void* data, out int len );
    public void input_panel_return_key_type_set( InputPanelReturnKeyType return_key_type );
    public InputPanelReturnKeyType input_panel_return_key_type_get();
    public void input_panel_return_key_disabled_set( bool disabled);
    public bool input_panel_return_key_disabled_get();
    public void input_panel_return_key_autoenabled_set( bool enabled );
    public void imf_context_reset();
    public void prediction_allow_set( bool prediction );
    public bool prediction_allow_get();

    [CCode (instance_pos = 1.5)] //entry is 2nd param in C API here:
    public void filter_limit_size( void* data, string[] text );
    [CCode (instance_pos = 1.5)] //entry is 2nd param in C API here:
    public void filter_accept_set( void* data, string[] text );

    public static string markup_to_utf8( string s );
    public static string utf8_to_markup( string s );
    //TODO: Fix  imf_context_get() return type
    public void imf_context_get();

    public void cnp_mode_set( CnpMode cnp_mode );
    public CnpMode cnp_mode_get();
    public void anchor_hover_parent_set( Elm.Object parent );
    public unowned Elm.Object? anchor_hover_parent_get();
    public void anchor_hover_style_set( string style );
    public unowned string anchor_hover_style_get();
    public void anchor_hover_end();
}

/////////////////////////////////////////////////////////////////////////
// elm_factory.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Factory : Elm.Object
{
    public static unowned Factory? add( Elm.Object parent );

    public bool maxmin_mode_get();
    public void maxmin_mode_set( bool enabled );
    public void maxmin_reset_set();
}


/////////////////////////////////////////////////////////////////////////
// elm_finger.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
namespace Coords
{
    public void finger_size_adjust( int times_w, out Evas.Coord w, int times_h, out Evas.Coord h );
}


/////////////////////////////////////////////////////////////////////////
// elm_flip.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cprefix = "ELM_FLIP_")]
public enum FlipMode
{
    ROTATE_Y_CENTER_AXIS,
    ROTATE_X_CENTER_AXIS,
    ROTATE_XZ_CENTER_AXIS,
    ROTATE_YZ_CENTER_AXIS,
    CUBE_LEFT,
    CUBE_RIGHT,
    CUBE_UP,
    CUBE_DOWN,
    PAGE_LEFT,
    PAGE_RIGHT,
    PAGE_UP,
    PAGE_DOWN
}

//=======================================================================
[CCode (cprefix = "ELM_FLIP_INTERACTION_")]
public enum FlipInteraction
{
    NONE,
    ROTATE,
    CUBE,
    PAGE
}

//=======================================================================
[CCode (cprefix = "ELM_FLIP_DIRECTION_")]
public enum FlipDirection
{
    UP,
    DOWN,
    LEFT,
    RIGHT
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Flip : Elm.Object
{
    public static unowned Flip? add( Elm.Object parent );

    public bool front_visible_get();
    public void perspective_set( Evas.Coord foc, Evas.Coord x, Evas.Coord y );
    public void go( FlipMode mode );

    public FlipInteraction interaction_get();
    public void interaction_set( FlipInteraction mode );
    public bool interaction_direction_enabled_get( FlipDirection dir );
    public void interaction_direction_set( FlipDirection dir, bool enabled );
    public double interaction_direction_hitsize_get( FlipDirection dir );
    public void interaction_direction_hitsize_set( FlipDirection dir, double hitsize );
}


/////////////////////////////////////////////////////////////////////////
// elm_flipselector.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class FlipselectorItem : ObjectItem
{
    public unowned GenlistItem? prev_get();
    public unowned GenlistItem? next_get();

    public void selected_set( bool selected );
    public bool selected_get();
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Flipselector : Elm.Object
{
    public static unowned Flipselector? add( Elm.Object parent );

    public void flip_next();
    public void flip_prev();
    public unowned FlipselectorItem? item_prepend( string label, Evas.Callback func );
    public unowned Eina.List<unowned FlipselectorItem>? items_get();
    public unowned FlipselectorItem? first_item_get();
    public unowned FlipselectorItem? last_item_get();
    public unowned FlipselectorItem? selected_item_get();
    public void first_interval_set( double interval );
    public double first_interval_get();
}

/////////////////////////////////////////////////////////////////////////
// elm_font.h / elm_fonts.h
/////////////////////////////////////////////////////////////////////////
//TODO: check/upgrade these files
//=======================================================================
namespace Font
{

    [CCode (cname = "Elm_Font_Properties", free_func="elm_font_properties_free")]
    public struct Properties
    {
        unowned string name;
        Eina.List<string> styles;
    }

    public Elm.Font.Properties properties_get( string font );

    public unowned string fontconfig_name_get( string name, string? style );
    public unowned string fontconfig_name_free( string name );

    public unowned Eina.Hash<string, Elm.Font.Properties> available_hash_add( Eina.List<string> list );
    public void available_hash_del( Eina.Hash<string, Elm.Font.Properties> hash );
}


/////////////////////////////////////////////////////////////////////////
// elm_frame.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Frame : Elm.Object
{
    public static unowned Frame? add( Elm.Object parent );

    public bool autocollapse_get();
    public void autocollapse_set( bool autocollapse );
    public bool collapse_get();
    public void collapse_set( bool collapse );
    public void collapse_go( bool collapse );
}

/////////////////////////////////////////////////////////////////////////
// elm_general.h
/////////////////////////////////////////////////////////////////////////


[CCode (cname = "Elm_Event_Cb", instance_pos = 0)]
public delegate bool EventCallback( Elm.Object obj, Elm.Object src, Evas.CallbackType type, void* event_info );

//=======================================================================
[CCode (cname = "Elm_Object_Layer", cprefix = "ELM_OBJECT_LAYER_")]
public enum ObjectLayer
{
    BACKGROUND,
    DEFAULT,
    FOCUS,
    TOOLTIP,
    CURSOR,
    LAST
}

//=======================================================================
[CCode (cname = "Elm_Focus_Direction", cprefix = "ELM_FOCUS_")]
public enum FocusDirection
{
    PREVIOUS,
    NEXT
}

//=======================================================================
[CCode (cprefix = "ELM_")]
public const int ECORE_EVENT_ETHUMB_CONNECT;

[CCode (cprefix = "ELM_")]
public const int EVENT_CONFIG_ALL_CHANGED;

[CCode (cprefix = "ELM_")]
public const int EVENT_POLICY_CHANGED;

[CCode (cname = "Elm_Event_Policy_Changed")]
public struct EventPolicyChanged
{
    uint policy;
    int new_value;
    int old_value;
}

//=======================================================================
namespace Policy
{
    [CCode (cname = "ELM_POLICY_QUIT")]
    public const uint QUIT;

    [CCode (cname = "ELM_POLICY_LAST")]
    public const uint LAST;

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
namespace Quicklaunch
{
    [CCode (has_target = false)]
    public delegate void Postfork_Func ( void* data );

    public void mode_set( bool ql_on );
    public bool mode_get();
    public int init( [CCode(array_length_pos = 0.9)] string[] args );
    public int sub_init( [CCode (array_length_pos = 0.9)] string[] args );
    public int sub_shutdown();
    public int shutdown();
    public void seed();
    public bool prepare( [CCode (array_length_pos = 0.9)] string[] args );
    public bool fork( [CCode (array_length_pos = 0.9)] string[] args, string cwd, Postfork_Func postfork_func, void *postfork_data );
    public void cleanup();
    public int fallback( [CCode (array_length_pos = 0.9)] string[] args );
    public string exe_path_get( string exe );

}


/////////////////////////////////////////////////////////////////////////
// elm_gengrid.h
/////////////////////////////////////////////////////////////////////////
//TODO

/////////////////////////////////////////////////////////////////////////
// elm_genlist.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Genlist_Item_Type", cprefix = "ELM_GENLIST_ITEM_")]
public enum GenlistItemType
{
    NONE,
    TREE,
    GROUP,
    MAX
}

[CCode (cname = "Elm_Genlist_Item_Field_Flags", cprefix = "ELM_GENLIST_ITEM_FIELD_")]
public enum GenlistItemFieldType
{
    ALL,
    TEXT,
    CONTENT,
    STATE
}

//=======================================================================
[CCode (cname = "Elm_Genlist_Item_Scrollto_Type", cprefix = "ELM_GENLIST_ITEM_SCROLLTO_")]
public enum GenlistItemScrolltoType
{
    NONE,
    IN,
    TOP,
    MIDDLE
}

[CCode (cname = "Elm_Genlist_Item_Text_Get_Cb", has_target = false)]
public delegate string GenlistItemTextGetFunc( void *data, Elm.Object obj, string part );
[CCode (cname = "Elm_Genlist_Item_Content_Get_Cb", has_target = false)]
public delegate unowned Elm.Object? GenlistItemContentGetFunc( void *data, Elm.Object obj, string part );
[CCode (cname = "Elm_Genlist_Item_State_Get_Cb", has_target = false)]
public delegate bool GenlistItemStateGetFunc( void *data, Elm.Object obj, string part );
[CCode (cname = "Elm_Genlist_Item_Del_Cb", has_target = false)]
public delegate void GenlistItemDelFunc( void *data, Elm.Object obj );

//=======================================================================
[CCode (cname = "Elm_Gen_Item_Class_Func", destroy_function = "")]
public struct GenlistItemClassFunc
{
    public GenlistItemTextGetFunc text_get;
    public GenlistItemContentGetFunc content_get;
    public GenlistItemStateGetFunc state_get;
    public GenlistItemDelFunc del;
}

//=======================================================================
[Compact]
[CCode (cname = "Elm_Genlist_Item_Class", free_function = "elm_genlist_item_class_free")]
public class GenlistItemClass
{

    public string item_style;
    public GenlistItemClassFunc func;

    [CCode (cname = "elm_genlist_item_class_new")]
    public GenlistItemClass();

    public void free();
    public void ref();
    public void unref();
}

//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class GenlistItem : ObjectItem
{
    public unowned GenlistItem? next_get();
    public unowned GenlistItem? prev_get();
    public unowned GenlistItem? parent_get();
    public void subitems_clear();

    public void selected_set( bool selected );
    public bool selected_get();
    public void expanded_set( bool expanded );
    public bool expanded_get();
    public void disabled_set( bool disabled );
    public bool disabled_get();

    public void show( GenlistItemScrolltoType type = GenlistItemScrolltoType.NONE );
    public void bring_in( GenlistItemScrolltoType type = GenlistItemScrolltoType.NONE );
    //public void top_show();
    //public void top_bring_in();
    //public void middle_show();
    //public void middle_bring_in();

    public void all_contents_unset(out Eina.List l);

    public void update();
    public void promote();
    public void demote();
    public void fields_update( string parts, GenlistItemFieldType itf );
    public void item_class_update( ref GenlistItemClass itc );
    public unowned GenlistItemClass item_class_get();

    public void tooltip_text_set( string text );
    public void tooltip_content_cb_set( TooltipItemContentCallback func, void* data, Evas.Callback del_cb );
    public void tooltip_unset();
    public void tooltip_style_set( string style );
    public unowned string tooltip_style_get();
    public bool tooltip_window_mode_set( bool disable );
    public bool tooltip_window_mode_get();

    public void cursor_set( string cursor );
    public unowned string cursor_get();
    public void cursor_unset();
    public void cursor_style_set( string style );
    public unowned string cursor_style_get();
    public void cursor_engine_only_set( bool engine_only );
    public bool cursor_engine_only_get();

    public int index_get();
    public void decorate_mode_set( string mode_type, bool mode_set );
    public unowned string decorate_mode_get();
    public GenlistItemType type_get();
    public void flip_set( bool flip );
    public bool flip_get();
    public void select_mode_set( Elm.Object.SelectMode mode );
    public Elm.Object.SelectMode select_mode_get();
    
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Genlist : Elm.Object
{
    public static unowned Genlist? add( Elm.Object parent );

    public void clear();
    public void multi_select_set( bool multi );
    public bool multi_select_get();
    public void mode_set( ListMode mode );
    public ListMode mode_get();
    public void bounce_set(bool h_bounce, bool v_bounce );
    public void bounce_get(out bool h_bounce, out bool v_bounce );
    public void homogeneous_set( bool homogeneous );
    public bool homogeneous_get();
    public void block_count_set( int n );
    public int block_count_get();
    public void longpress_timeout_set( double timeout );
    public double longpress_timeout_get();
    public void scroller_policy_set( Elm.ScrollerPolicy policy_h, Elm.ScrollerPolicy policy_v );
    public void scroller_policy_get( out Elm.ScrollerPolicy policy_h, out Elm.ScrollerPolicy policy_v );

    public unowned GenlistItem? item_append( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItemType type, Evas.Callback? callback = null );
    public unowned GenlistItem? item_prepend( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItemType type, Evas.Callback? callback = null );
    public unowned GenlistItem? item_insert_before( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItem before, GenlistItemType type, Evas.Callback? callback = null );
    public unowned GenlistItem? item_insert_after( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItem after, GenlistItemType type, Evas.Callback? callback = null );
    public unowned GenlistItem? item_sorted_insert( GenlistItemClass itc, void *data, GenlistItem? parent, GenlistItemType type, Eina.CompareCb comp, Evas.Callback? callback = null );
    public static void item_del( GenlistItem item );

    public unowned GenlistItem? selected_item_get();
    public unowned Eina.List<unowned GenlistItem>? selected_items_get();
    public Eina.List<unowned GenlistItem>? realized_items_get();
    public unowned GenlistItem at_xy_item_get( Evas.Coord x, Evas.Coord y, out int posret );
    public unowned GenlistItem first_item_get();
    public unowned GenlistItem last_item_get();

    public void realized_items_update();
    public unowned string mode_type_get();
    public unowned GenlistItem? decorated_item_get();
    public void reorder_mode_set( bool reorder_mode );
    public bool reorder_mode_get();
    public uint items_count();
    public void decorate_mode_set( bool edit_mode );
    public bool decorate_mode_get();
    public void tree_effect_enabled_set( bool enabled );
    public bool tree_effect_enabled_get();
    public void select_mode_set( Elm.Object.SelectMode mode );
    public Elm.Object.SelectMode select_mode_get();
    public void highlight_mode_set( bool highlight );
    public bool highlight_mode_get();
}

/////////////////////////////////////////////////////////////////////////
// elm_gesture_layer.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname="Elm_Gesture_Type", cprefix = "ELM_GESTURE_")]
public enum GestureType
{
    FIRST,
    N_TAPS,
    N_LONG_TAPS,
    N_DOUBLE_TAPS,
    N_TRIPLE_TAPS,
    MOMENTUM,
    N_LINES,
    N_FLICKS,
    ZOOM,
    ROTATE,
    LAST
}

//=======================================================================
[CCode (cname="Elm_Gesture_State", cprefix = "ELM_GESTURE_STATE_")]
public enum GestureState
{
    UNDEFINED,
    START,
    MOVE,
    END,
    ABORT
}

//=======================================================================
[CCode (cname = "Elm_Gesture_Taps_Info")]
public struct GestureTapsInfo
{
    Evas.Coord x;
    Evas.Coord y;
    uint n;
    uint timestamp;
}

//=======================================================================
[CCode (cname = "Elm_Gesture_Momentum_Info")]
public struct GestureMomentumInfo
{
    Evas.Coord x1;
    Evas.Coord y1;
    Evas.Coord x2;
    Evas.Coord y2;
    uint tx;
    uint ty;
    Evas.Coord mx;
    Evas.Coord my;
    uint n;
}

//=======================================================================
[CCode (cname = "Elm_Gesture_Line_Info")]
public struct GestureLineInfo
{
    GestureMomentumInfo momentum;
    double angle;
}

//=======================================================================
[CCode (cname = "Elm_Gesture_Zoom_Info")]
public struct GestureZoomInfo
{
    Evas.Coord x;
    Evas.Coord y;
    double zoom;
    double momentum;
}

//=======================================================================
[CCode (cname = "Elm_Gesture_Rotate_Info")]
public struct GestureRotateInfo
{
    Evas.Coord x;
    Evas.Coord y;
    Evas.Coord radius;
    double base_angle;
    double angle;
    double momentum;
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class GestureLayer : Elm.Object
{

    [CCode (cname="Elm_Gesture_Event_Cb", has_target = false)]
    public delegate Evas.Event.Flags GestureEventCallback( void *data, void *event_info );

    public static unowned GestureLayer? add( Elm.Object parent );
    public void cb_set( GestureType idx, GestureState cb_type, GestureEventCallback cb, void *data );
    public bool hold_events_get();
    public void hold_events_set( bool hold_events );
    public double zoom_step_get();
    public void zoom_step_set( double step );
    public double rotate_step_get();
    public void rotate_step_set( double step );
    public bool attach( Evas.Object t );
}

/////////////////////////////////////////////////////////////////////////
// elm_glview.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_GLView_Mode", cprefix = "ELM_GLVIEW_")]
public enum GLViewMode
{
    NONE,
    ALPHA,
    DEPTH,
    STENCIL,
    DIRECT
}

//=======================================================================
[CCode (cname = "Elm_GLView_Render_Policy", cprefix = "ELM_GLVIEW_RENDER_POLICY_")]
public enum GLViewRenderPolicy
{
    ON_DEMAND,
    ALWAYS
}

//=======================================================================
[CCode (cname = "Elm_GLView_Resize_Policy", cprefix = "ELM_GLVIEW_RESIZE_POLICY_")]
public enum GLViewResizePolicy
{
    RECREATE,
    SCALE
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Glview : Elm.Object
{

    [CCode (cname = "Elm_GLView_Func_Cb", instance_pos = 0)]
    public delegate void GLViewFuncCb( Evas.Object obj );

    public static unowned Glview? add( Elm.Object parent );

    public void size_set( Evas.Coord w, Evas.Coord h );
    public void size_get( out Evas.Coord w, out Evas.Coord h );
    //public EvasGL.API api_get (); //TODO: vapi for Evas_GL.h
    public void mode_set( GLViewMode mode );
    public bool resize_policy_set( GLViewResizePolicy policy );
    public bool render_policy_set( GLViewRenderPolicy policy );
    public void init_func_set( Elm.Glview.GLViewFuncCb func );
    public void del_func_set( Elm.Glview.GLViewFuncCb func );
    public void resize_func_set( Elm.Glview.GLViewFuncCb func );
    public void render_func_set( Elm.Glview.GLViewFuncCb func );
    public void changed_set();
}


/////////////////////////////////////////////////////////////////////////
// elm_grid.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Grid: Elm.Object
{
    public static unowned Grid? add( Elm.Object parent );

    public void size_set( Evas.Coord w, Evas.Coord h );
    public void size_get( out Evas.Coord w, out Evas.Coord h );
    public void pack( Elm.Object subobj, Evas.Coord x, Evas.Coord y, Evas.Coord w, Evas.Coord h );
    public void unpack( Elm.Object subobj );
    public void clear( bool clear );
    public static void pack_set( Elm.Object subobj, Evas.Coord x, Evas.Coord y, Evas.Coord w, Evas.Coord h );
    public static void pack_get( Elm.Object subobj, out Evas.Coord x, out Evas.Coord y, out Evas.Coord w, out Evas.Coord h );
    public Eina.List<unowned Elm.Object> children_get();
}

/////////////////////////////////////////////////////////////////////////
// elm_hover.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cprefix = "ELM_HOVER_AXIS_")]
public enum HoverAxis
{
    NONE,
    HORIZONTAL,
    VERTICAL,
    BOTH
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Hover : Elm.Object
{
    public static unowned Hover? add( Elm.Object parent );

    public void target_set( Elm.Object target );
    public Elm.Object target_get();
    public void parent_set( Elm.Object parent );
    public Elm.Object parent_get();
    public unowned string best_content_location_get( HoverAxis pref_axis );
    public void dismiss();
}



/////////////////////////////////////////////////////////////////////////
// elm_icon.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname="Elm_Icon_Type", cprefix = "ELM_ICON_")]
public enum IconType
{
    NONE,
    FILE,
    STANDARD,
}

//=======================================================================
[CCode (cname="Elm_Icon_Lookup_Order", cprefix = "ELM_ICON_LOOKUP_")]
public enum IconLookupOrder
{
    FDO_THEME,
    THEME_FDO,
    FDO,
    THEME
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Icon : Elm.Object
{
    public static unowned Icon? add( Elm.Object parent );

    public void file_set( string file, string? group=null );
    public void file_get( out unowned string file, out unowned string group );
    public void memfile_set( void* img, size_t size, string format, string key );
    public void thumb_set( string file, string group );
    public bool standard_set( string name );
    public unowned string standard_get();
    public void smooth_set( bool smooth );
    public bool smooth_get();
    public void no_scale_set( bool no_scale );
    public bool no_scale_get();
    public void resizable_set( bool size_up, bool size_down );
    public void resizable_get( out bool size_up, out bool size_down );
    public void size_get( out int w, out int h );
    public void fill_outside_set( bool fill_outside );
    public bool fill_outside_get();
    public void prescale_set( int size );
    public int prescale_get();
    public Elm.Image object_get();
    public void order_lookup_set( IconLookupOrder order );
    public IconLookupOrder order_lookup_get();
    public void preload_disabled_set( bool disabled );
    public bool animated_available_get();
    public void animated_set( bool animated );
    public bool animated_get();
    public void animated_play_set( bool play );
    public bool animated_play_get();
    public void aspect_fixed_set( bool fixed );
    public bool aspect_fixed_get();
}

/////////////////////////////////////////////////////////////////////////
// elm_image.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cprefix = "ELM_IMAGE_")]
public enum ImageOrient
{
    ORIENT_NONE,
    ORIENT_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
    FLIP_HORIZONTAL,
    FLIP_VERTICAL,
    FLIP_TRANSPOSE,
    FLIP_TRANSVERSE
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Image : Elm.Object
{
    public static unowned Image? add( Elm.Object parent );

    public void file_set( string file, string? group=null );
    public void file_get( out unowned string file, out unowned string group );
    public void smooth_set( bool smooth );
    public bool smooth_get();
    public void object_size_get( out int w, out int h );
    public void no_scale_set( bool no_scale );
    public bool no_scale_get();
    public void resizable_set( bool size_up, bool size_down );
    public void resizable_get( out bool size_up, out bool size_down );
    public void size_get( out int w, out int h );
    public void fill_outside_set( bool fill_outside );
    public bool fill_outside_get();
    public void prescale_set( int size );
    public int prescale_get();
    public void orient_set( ImageOrient orient );
    public ImageOrient orient_get();
    public void editable_set( bool editable );
    public bool editable_get();
    public unowned Evas.Object object_get();
    public void aspect_fixed_set( bool retained );
    public bool aspect_fixed_get();
    public void preload_disabled_set( bool disabled );
}

/////////////////////////////////////////////////////////////////////////
// elm_index.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class IndexItem : ObjectItem
{
    public unowned string item_letter_get();
    public void selected_set( bool selected );
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Index : Elm.Object
{
    public static unowned Index? add( Elm.Object parent ); 

    public void autohide_disabled_set( bool disabled );
    public bool autohide_disabled_get();
    public void item_level_set( int level );
    public int item_level_get();
    public unowned IndexItem? selected_item_get( int level );
    public void item_append( string letter, Evas.Callback? func, void* data );
    public void item_prepend( string letter, Evas.Callback? func, void* data );
    public unowned IndexItem? item_insert_before(IndexItem  before, string letter, Evas.Callback? func, void* data );
    public unowned IndexItem? item_insert_after(IndexItem  after, string letter, Evas.Callback? func, void* data );
    public void item_sorted_insert( string letter, Evas.Callback? func, void* data, Eina.CompareCb cmp_func, Eina.CompareCb cmp_data_func );
    public unowned IndexItem? item_find( void *data );
    public void item_clear();
    public void level_go( int level );
    public void indicator_disabled_set( bool disabled );
    public bool indicator_disabled_get();
    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_label.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Label : Elm.Object
{
    public static unowned Label? add( Elm.Object parent );

    public void line_wrap_set( bool wrap );
    public WrapType line_wrap_get();
    public void wrap_width_set( Evas.Coord w );
    public Evas.Coord wrap_width_get();
    public void ellipsis_set( bool ellipsis );
    public bool ellipsis_get();
    public void slide_set( bool slide );
    public bool slide_get();
    public void slide_duration_set( double duration );
    public double slide_duration_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_layout.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Layout : Elm.Object
{
    public static unowned Layout? add( Elm.Object parent );

    public void file_set( string file, string group );
    public bool theme_set( string clas, string group, string style );
    public bool box_append( string part, Elm.Object child );
    public bool box_prepend( string part, Elm.Object child );
    public bool box_insert_before( string part, Elm.Object child, Elm.Object reference );
    public bool box_insert_at( string part, Elm.Object child, uint pos );
    public unowned Elm.Object? box_remove( string part, Elm.Object child );
    public bool box_remove_all( string part, bool clear );
    public bool table_pack( string part, Elm.Object child_obj, ushort col, ushort row, ushort colspan, ushort rowspan );
    public void table_unpack( string part, Elm.Object child_obj );
    public bool table_clear( string part, bool clear );
    public unowned Edje.Object edje_get();
    public unowned string data_get( string key );
    public void sizing_eval();
    public bool part_cursor_set( string part_name, string cursor );
    //TODO: rest of cursor methods. Not sure if they are going to be removed soon.
    public void icon_set( Elm.Icon obj );
    public unowned Elm.Icon? icon_get();
    public void end_set( Elm.Object obj );
    public unowned Elm.Object? end_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_list.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname="Elm_List_Mode", cprefix = "ELM_LIST_")]
public enum ListMode
{
    COMPRESS,
    SCROLL,
    LIMIT,
    EXPAND,
    LAST
}


//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class ListItem : ObjectItem
{
    public void selected_set( bool selected );
    public bool selected_get();
    public void separator_set( bool setting );
    public bool separator_get();
    public void show();
    public void bring_in();
    public unowned Evas.Object object_get();
    public unowned ListItem? prev();
    public unowned ListItem? next();
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class List : Elm.Object
{
    public static unowned List? add( Elm.Object parent );


    public unowned ListItem? item_append( string label, Elm.Object? icon, Elm.Object? end, Evas.Callback? func);
    public unowned ListItem? item_prepend( string label, Elm.Object? icon, Elm.Object? end, Evas.Callback? func );
    public unowned ListItem? item_insert_before( ListItem before, string label, Elm.Object? icon, Elm.Object? end, Evas.Callback? func );
    public unowned ListItem? item_insert_after( ListItem after, string label, Elm.Object? icon, Elm.Object? end, Evas.Callback? func );
    public unowned ListItem? item_sorted_insert( string label, Elm.Object? icon, Elm.Object? end, Evas.Callback func, Eina.CompareCb comp ); //FIXME: not sure this line is correct

    public void go();
    public void multi_select_set( bool multi );
    public bool multi_select_get();
    public void mode_set( ListMode mode );
    public ListMode mode_get();
    public void horizontal_set( bool multi );
    public bool horizontal_get();
    public void bounce_set( bool h_bounce, bool v_bounce );
    public void bounce_get( out bool h_bounce, out bool v_bounce );
    public void scroller_policy_set( ScrollerPolicy policy_h, ScrollerPolicy policy_v );
    public void scroller_policy_get( out ScrollerPolicy policy_h, out ScrollerPolicy policy_v );
    public void clear();
    public unowned Eina.List<unowned ListItem> items_get();
    public unowned ListItem? selected_item_get();
    public unowned Eina.List<unowned ListItem>? selected_items_get();
    public void select_mode_set( Elm.Object.SelectMode mode );
    public Elm.Object.SelectMode select_mode_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_mapbuf.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Mapbuf : Elm.Object
{
    public static unowned Mapbuf? add( Elm.Object parent );

    public void enabled_set( string widget, bool enabled );
    public bool enabled_get();
    public void smooth_set( bool smooth );
    public bool smooth_get();
    public void alpha_set( bool alpha );
    public bool alpha_get();
}

/////////////////////////////////////////////////////////////////////////
// elm_map.h
/////////////////////////////////////////////////////////////////////////
//TODO: Upgrade this file.
//TODO: Finish adding necessary delegates for the commented method and add EMap bindings
/*
//=======================================================================
[Compact]
[CCode (cname = "Elm_Map_Overlay", free_function = "elm_map_overlay_del")]
public class MapOverlay
{
    public void del();
    public MapOverlayType type_get();
    public void data_set( void* data );
    public void* data_get();
    public void hide_set( bool hide );
    public bool hide_get();
    public void displayed_zoom_min_set( int zoom );
    public int displayed_zoom_min_get();
    public void paused_set( bool paused );
    public bool paused_get();
    public void contet_set( Elm.Object obj );
    public unowned Elm.Object? content_get();
    public void icon_set( Elm.Icon icon );
    public unowned Elm.Icon? icon_get();
    public void geo_set( double lon, double lat );
    public void geo_get( out double lon, out double lat );
    public void show();
    public void class_append( MapOverlay overlay );
    public void class_remove( MapOverlay overlay );
    public void class_zoom_max_set( int zoom );
    public int class_zoom_max_get();
    public void bubble_follow( MapOverlay parent );
    public void bubble_content_append( Elm.Object content );
    public void bubble_content_clear();
    //TODO: write delegate
    //public void get_cb_set( MapOverlayGetCb get_cb, void* data );
}
//=======================================================================
[Compact]
[CCode (cname = "Elm_Map_Route")]
public class MapRoute
{
    public void remove();
    public void color_set( int r, int g, int b, int a );
    public void color_get( out int r, out int g, out int b, out int a );
    public double distance_get();
    public unowned string node_get();
    public unowned string waypoint_get();
}

//=======================================================================
[Compact]
[CCode (cname = "Elm_Map_Name")]
public class MapName
{
    public unowned string address_get();
    public void region_get( out double lon, out double lat );
    public void remove();
}

//=======================================================================
//[CCode (cname = "Elm_Map_Track")]
//public struct Track { }

//=======================================================================
[CCode (cname = "Elm_Map_Zoom_Mode", cprefix = "ELM_MAP_ZOOM_MODE_")]
public enum MapZoomMode
{
    MANUAL,
    AUTO_FIT,
    AUTO_FILL,
    LAST
}


//=======================================================================
[CCode (cname = "Elm_Map_SOURCE_TYPE", cprefix = "ELM_MAP_SOURCE_TYPE_")]
public enum MapSourceType
{
    TILE,
    ROUTE,
    NAME,
    LAST
}

//=======================================================================
[CCode (cname = "Elm_Map_Route_Type", cprefix = "ELM_MAP_ROUTE_TYPE_")]
public enum MapRouteType
{
    MOTOCAR,
    BICYCLE,
    FOOT,
    LAST
}

//=======================================================================
[CCode (cname = "Elm_Map_Route_Method", cprefix = "ELM_MAP_ROUTE_METHOD_")]
public enum MapRouteMethod
{
    FASTEST,
    SHORTEST,
    LAST
}

//=======================================================================
[CCode (cname = "Elm_Map_Name_Method", cprefix = "ELM_MAP_NAME_METHOD_")]
public enum MapNameMethod
{
    SEARCH,
    REVERSE,
    LAST
}

//=======================================================================
[CCode (cname = "Elm_Map_Overlay_Type", cprefix = "ELM_MAP_OVERLAY_TYPE_")]
public enum MapOverlayType
{
    NONE,
    DEFAULT,
    CLASS,
    BUBBLE,
    ROUTE
}

//=======================================================================
[Compact]
[CCode (cname = "Elm_Map_Group_Class")]
public class MapGroupClass
{
    [CCode (cname = "elm_map_group_class_new", free_function = "")]
    public MapGroupClass( Elm.Map? map );

   public void style_set( string style );
   //public void icon_cb_set( ElmMapGroupIconGetFunc icon_get );
   public void data_set( void *data );
   public void zoom_displayed_set( int zoom );
   public void zoom_grouped_set( int zoom );
   [CCode (instance_pos = 0.1)]
   public void hide_set( Elm.Map obj, bool hide );
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Map : Elm.Object
{

    [CCode (cname = "Elm_Map_Overlay_Get_Cb", instance_pos = 1.5)]
    public delegate Elm.Object OverlayGetCb( void* data, MapOverlay overlay );

    [CCode (cname = "Elm_Map_Name_Cb", instance_pos = 1.5)]
    public delegate Elm.Object NameGetCb( void* data, MapName name );
    
    [CCode (cname = "Elm_Map_Route_Cb", instance_pos = 1.5)]
    public delegate Elm.Object RouteGetCb( void* data, MapRoute name );

    public static unowned Map? add( Elm.Object parent );

    public void zoom_set( int zoom );
    public double zoom_get();
    public void zoom_mode_set( MapZoomMode mode );
    public MapZoomMode zoom_mode_get();
    public void zoom_min_set( int zoom );
    public int zoom_min_get();
    public void zoom_max_set( int zoom );
    public int zoom_max_get();

    public void region_get( out double lon, out double lat );
    public void region_bring_in( double lon, double lat );
    public void region_show( double lon, double lat );

    public void paused_set( bool paused );
    public bool paused_get();
    public void paused_markers_set( bool paused );
    public bool paused_markers_get();

    public void canvas_to_geo_convert(Evas.Coord x, Evas.Coord y, out double lon, out double lat );
    public MapMarker marker_add( double lon, double lat, ref MapMarkerClass clas, ref MapGroupClass clas_group, void* data );

    public void max_marker_per_group_set( int max );
    public static void markers_list_show( Eina.List<MapMarker> list );
    public void bubbles_close();
    
    public unowned string[] source_names_get();
    public void source_name_set( string source_name );
    public unowned string source_name_get();
    public void route_source_set( MapRouteSource source );
    public MapRouteSource route_source_get();

    public void source_zoom_min_set( int zoom );
    public int source_zoom_min_get();
    public void source_zoom_max_set( int zoom );
    public static int source_zoom_max_get();
    public void user_agent_set( string user_agent );
    
    public unowned MapRoute? route_add( MapRouteType type, MapRouteMethod method, double flon, double flat, double tlon, double tlat );
    public void rotate_set( double degree, Evas.Coord cx, Evas.Coord xy );
    public void rotate_get( out double degree, out Evas.Coord cx, out Evas.Coord cy );
    
    public void wheel_disabled_set( bool disabled );
    public bool wheel_disabled_get();
    public unowned MapOverlay? overlay_add( double lon, double lat );
    public static void overlays_show( Eina.List<unowned MapOverlay> overlays );
    public MapOverlay? overlay_class_add();
    public MapOverlay? overlay_bubble_add();

    //TODO: add emap bindings
    //public void track_add( EMapRoute emap );
    //public void track_remove( EMapRoute emap );
}
*/

/////////////////////////////////////////////////////////////////////////
// elm_menu.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Menu_Item", free_function = "elm_object_item_del")]
public class MenuItem : ObjectItem
{
    public unowned Evas.Object? object_get();
    public void icon_name_set( string icon );
    public unowned string icon_name_get();
    public void selected_set( bool selected );
    public bool selected_get();
    public bool is_separator();
    public unowned Eina.List<unowned MenuItem>? subitems_get();
    public uint index_get();
    public unowned MenuItem? next_get();
    public unowned MenuItem? prev_get();
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Menu : Elm.Object
{
    public static unowned Menu? add( Elm.Object parent );

    public void parent_set( Elm.Object parent );
    public unowned Elm.Object parent_get();
    public void move( Evas.Coord x, Evas.Coord y );
    public void close();
    public unowned Eina.List<unowned MenuItem>? items_get();
    public unowned MenuItem item_add( MenuItem? parent, string icon, string label, Evas.Callback? func = null );
    public unowned MenuItem item_add_object( MenuItem? parent, Elm.Object subobj, string icon, string label, Evas.Callback? func = null );
    public unowned MenuItem? separator_add(MenuItem parent);
    public unowned MenuItem? selected_item_get();
    public unowned MenuItem? last_item_get();
    public unowned MenuItem? first_item_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_notify.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Notify_Orient", cprefix = "ELM_NOTIFY_ORIENT_")]
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
    BOTTOM_RIGHT,
    LAST
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Notify : Elm.Object
{
    public static unowned Notify? add( Elm.Object parent );

    public void parent_set( Evas.Object parent );
    public unowned Evas.Object parent_get();
    public void orient_set( NotifyOrient orient );
    public NotifyOrient orient_get();
    public void timeout_set( double timeout );
    public double timeout_get();
    public void allow_events_set( bool allow );
    public bool alloq_events_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_panel.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Panel_Orient", cprefix = "ELM_PANEL_ORIENT_")]
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
    public static unowned Panel? add( Elm.Object parent );

    public void orient_set( PanelOrient orient );
    public PanelOrient orient_get();
    public void hidden_set( bool hidden );
    public bool hidden_get();
    public void toggle();
}


/////////////////////////////////////////////////////////////////////////
// elm_panes.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Panes : Elm.Object
{
    public static unowned Panes? add( Elm.Object parent );

    public void fixed_set( bool fixed );
    public bool fixed_get();
    public double content_left_size_get();
    public void content_left_size_set( double size );
    public double content_right_size_get();
    public void content_right_size_set( double size );
    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_password.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
namespace Password
{
    public bool show_last_get();
    public void show_last_set( bool password_show_last );
    public double show_last_timeout_get();
    public void show_last_timeout_set( double password_show_last_timeout );
}


/////////////////////////////////////////////////////////////////////////
// elm_photocam.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Photocam_Zoom_Mode", cprefix = "ELM_PHOTOCAM_ZOOM_MODE_")]
public enum PhotocamZoomMode
{
    MANUAL,
    AUTO_FIT,
    AUTO_FILL,
    AUTO_FIT_IN,
    LAST
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Photocam : Elm.Object
{
    public static unowned Photocam? add( Elm.Object parent );

    //TODO: this returns Evas_Load_Error
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
    public unowned Evas.Object? internal_image_get();
    public void bounce_set( bool h_bounce, bool v_bounce );
    public void bounce_get( out bool h_bounce, out bool v_bounce );
    public void gesture_enabled_set( bool gesture );
    public bool gesture_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_photo.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Photo : Elm.Object
{
    public static unowned Photo? add( Elm.Object parent );

    public bool file_set( string file );
    public void thumb_set( string file, string group );
    public void size_set( int size );
    public void fill_inside_set( bool fill );
    public void editable_set( bool editable );
    public void aspect_fixed_set( bool fixed );
    public bool aspect_fixed_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_plug.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Plug : Elm.Object
{
    public static unowned Plug? add( Elm.Object parent );

    public bool connect( string svcname, int svcnum, bool svcsys );
    public unowned Elm.Image? image_object_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_progressbar.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Progressbar : Elm.Object
{
    public static unowned Progressbar? add( Elm.Object parent );

    public void pulse_set( bool pulse );
    public bool pulse_get();
    public void pulse( bool  state );
    public void value_set( double val );
    public double value_get();
    public void span_size_set( Evas.Coord size );
    public Evas.Coord span_size_get();
    public void unit_format_set( string format );
    public unowned string unit_format_get();
    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
    public void inverted_set( bool inverted );
    public bool inverted_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_radio.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Radio : Elm.Object
{
    public static unowned Radio? add( Elm.Object parent );

    public void group_add( Elm.Object group );
    public void state_value_set( int value );
    public int state_value_get();
    public void value_set( int value );
    public int value_get();
    public void value_pointer_set( out int valuep );
    public unowned Elm.Object? selected_object_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_route.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Route : Elm.Object
{
    public static unowned Route? add( Elm.Object parent );

    public void longitude_min_max_get( out double min, out double max );
    public void latitude_min_max_get( out double min, out double max );
    //TODO: add bindings for EMap
    //public void emap_set( EMap_Route emap );
}

/////////////////////////////////////////////////////////////////////////
// elm_scroller.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Scroller_Policy",  cprefix = "ELM_SCROLLER_POLICY_")]
public enum ScrollerPolicy
{
    AUTO,
    ON,
    OFF,
    LAST
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Scroller : Elm.Object
{
    public static unowned Scroller? add( Elm.Object parent );

    public void custom_widget_base_theme_set( string widget, string base );
    public void content_min_limit( bool w, bool h );
    public void region_show( Evas.Coord x, Evas.Coord y, Evas.Coord w, Evas.Coord h );
    public void policy_set( ScrollerPolicy h_policy, ScrollerPolicy v_policy );
    public void policy_get( out ScrollerPolicy h_policy, out ScrollerPolicy v_policy );
    public void region_get( out Evas.Coord x, out Evas.Coord y, out Evas.Coord w, out Evas.Coord h );
    public void child_size_get( out Evas.Coord w, out Evas.Coord h );
    public void bounce_set( bool h_bounce, bool v_bounce );
    public void bounce_get( out bool h_bounce, out bool v_bounce );
    public void page_relative_set( double h_pagerel, double v_pagerel );
    public void page_size_set( Evas.Coord h_pagesize, Evas.Coord v_pagesize );
    public void current_page_get( out int h_pagenumber, out int v_pagenumber );
    public void last_page_get( out int h_pagenumber, out int v_pagenumber );
    public void page_show( int h_pagenumber, int v_pagenumber );
    public void page_bring_in( int h_pagenumber, int v_pagenumber );
    public void region_bring_in( Evas.Coord x, Evas.Coord y, Evas.Coord w, Evas.Coord h );
    public void propagate_events_set( bool propagation );
    public bool propagate_events_get();
    public void gravity_set( double x, double y );
    public void gravity_get( out double x, out double y );
}


/////////////////////////////////////////////////////////////////////////
// elm_segment_control.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class SegmentControlItem : ObjectItem
{
    public int index_get();
    public unowned Elm.SegmentControl object_get();
    public void selected_set( bool selected );
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class SegmentControl : Elm.Object
{
    public static unowned SegmentControl? add( Elm.Object parent );

    public unowned SegmentControlItem? item_insert_at( Elm.Icon icon, string label, int index );
    public void item_del_at( int index );
    public int item_count_get();
    public unowned SegmentControlItem? item_get( int index );
    public unowned string? item_label_get( int index );
    public unowned Elm.Icon? item_icon_get( int index );
    public unowned SegmentControlItem? item_selected_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_separator.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Separator : Elm.Object
{
    public static unowned Separator? add( Elm.Object parent );

    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
}

/////////////////////////////////////////////////////////////////////////
// elm_slider.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Slider : Elm.Object
{
    [CCode (has_target = false)]
    public delegate unowned string IndicatorFormatFunc( double val );

    [CCode (has_target = false)]
    public delegate void IndicatorFormatFreeFunc( string str );

    public static unowned Slider? add( Elm.Object parent );

    public void span_size_set( Evas.Coord size );
    public Evas.Coord span_size_get();
    public void unit_format_set( string format );
    public unowned string unit_format_get();
    public void indicator_format_set( string indicator );
    public unowned string indicator_format_function_get();
    public void indicator_format_function_set( IndicatorFormatFunc func, IndicatorFormatFreeFunc? free_func = null );
    public void units_format_function_set( IndicatorFormatFunc func, IndicatorFormatFreeFunc? free_func = null );
    public void horizontal_set( bool horizontal );
    public bool horizontal_get();
    public void min_max_set( double min, double max );
    public void min_max_get( out double min, out double max );
    public void value_set( double val );
    public double value_get();
    public void inverted_set( bool inverted );
    public bool inverted_get();
    public void indicator_show_set( bool show );
    public bool indicator_show_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_slideshow.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "elm_object_item_del")]
public class SlideshowItem : Elm.ObjectItem
{
    public unowned Evas.Object? object_get();
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
[Compact]
[CCode (cname = "Elm_Slideshow_Item_Class", destroy_function = "")]
public struct SlideshowItemClass
{
    public SlideshowItemClassFunc func;
    public void show();
    public unowned Slideshow object_get();
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Slideshow : Elm.Object
{
    public static unowned Slideshow? add( Elm.Object parent );

    public unowned SlideshowItem item_add( ref SlideshowItemClass itc, void* data );
    public unowned SlideshowItem item_sorted_insert( ref SlideshowItemClass itc, void* data, Eina.CompareCb func );
    //TODO: this api should be renamed in C API:
    public static void show( SlideshowItem item );
    public void next();
    public void previous();
    public unowned Eina.List<unowned string> transitions_get();
    public void transition_set( string transition );
    public unowned string transition_get();
    public void timeout_set( double timeout );
    public double timeout_get();
    public void loop_set( bool loop );
    public bool loop_get();
    public void clear();
    public unowned Eina.List<unowned SlideshowItem>? items_get();
    public unowned SlideshowItem? item_current_get();
    public unowned SlideshowItem? item_nth_get( uint nth );
    public void layout_set( string layout );
    public unowned string layout_get();
    public unowned Eina.List<unowned string> layouts_get();
    public void cache_before_set( int count );
    public int cache_before_get();
    public void cache_after_set( int count );
    public int cache_after_get();
    public uint count_get();
}


/////////////////////////////////////////////////////////////////////////
// elm_spinner.h
/////////////////////////////////////////////////////////////////////////
//TODO: check/upgrade this class
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Spinner : Elm.Object
{
    public static unowned Spinner? add( Elm.Object parent ); 

    public void label_format_set( string format );
    public unowned string label_format_get();
    public void min_max_set( double min, double max );
    public void step_set( double step );
    public void value_set( double val );
    public double value_get();
    public void wrap_set( bool wrap );
}


/////////////////////////////////////////////////////////////////////////
// elm_theme.h
/////////////////////////////////////////////////////////////////////////
//TODO: check/upgrade this class
//=======================================================================
[CCode (cname = "Elm_Theme", free_function = "elm_theme_free")]
public class Theme
{
    [CCode (cname = "elm_theme_new")]
    public Theme();

    public void copy( Theme thdst );

    public Theme? ref_get();
    public void ref_set( Theme thref );

    public unowned string? data_get( string key );

    public Theme default_get();

    public void overlay_add( string item );
    public void overlay_del( string item );
    public unowned Eina.List overlay_list_get();
    public void extension_add( string item );
    public void extension_del( string item );
    public unowned Eina.List extension_list_get();

    public void @set( string theme );
    public unowned string get();

    public unowned Eina.List<unowned string> list_get();
    public static string list_item_path_get( string f, bool in_search_path );

    public void flush();
    public void full_flush();

    public unowned Eina.List<unowned Theme> name_available_list_new();
    public void name_available_list_free( Eina.List<unowned Theme> list );
}

/////////////////////////////////////////////////////////////////////////
// elm_table.h
/////////////////////////////////////////////////////////////////////////
//TODO: check/upgrade content from this file
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Table : Elm.Object
{
    public static unowned Table? add( Elm.Object parent ); 
    
    public void homogeneous_set( bool homogeneous );
    public void pack( Elm.Object subobj, int x, int y, int w, int h );
    public void padding_set( Evas.Coord horizontal, Evas.Coord vertical );
    public void unpack( Elm.Object subobj );
    public void clear();
}


/////////////////////////////////////////////////////////////////////////
// elm_toolbar.h
/////////////////////////////////////////////////////////////////////////
//TODO: check/upgrade content from this file
//=======================================================================
[CCode (cname = "Elm_Object_Item", free_function = "object_item_del")]
public class ToolbarItem: ObjectItem
{
    public Elm.Object icon_get();
    public void icon_set( string icon );
    public unowned ToolbarItem? next_get();
    public unowned ToolbarItem? prev_get();
    //public void del_cb_set( ... );
    public bool separator_get();
    public void separator_set( bool separator );
    public void menu_set( bool menu );
    public Elm.Object menu_get();
    public void priority_set( int priority );
    public int priority_get();
    public bool selected_get();
    public void selected_set( bool selected );
    public bool homogeneous_get();
    public void homogeneous_set( bool homogeneous );
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
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Toolbar : Elm.Object
{
    public static unowned Toolbar? add( Elm.Object parent ); 

    public void icon_size_set( int icon_size );
    public int icon_size_get();
    [CCode (cname = "elm_toolbar_item_append")]
    public unowned ToolbarItem? append( string icon, string label, Evas.Callback func );
    [CCode (cname = "elm_toolbar_item_prepend")]
    public unowned ToolbarItem? prepend( string icon, string label, Evas.Callback func );
    public unowned ToolbarItem? item_insert_before( ToolbarItem before, string icon, string label, Evas.Callback cb);
    public unowned ToolbarItem? item_insert_after( ToolbarItem after, string icon, string label, Evas.Callback cb);
    public unowned ToolbarItem? first_item_get();
    public unowned ToolbarItem? last_item_get();
    // Note: elm_boolbar_item_ functions living in ToolbarItem scope
    public void scrollable_set( bool scrollable );
    public void homogeneous_set( bool homogeneous );
    public void menu_parent_set( Elm.Object parent );
    [CCode (cname = "elm_toolbar_item_unselect_all")]
    public void unselect_all();
    public void align_set( double align );
    [CCode (cname = "elm_toolbar_item_find_by_label")]
    public unowned ToolbarItem? find_by_label(string label);
    public unowned ToolbarItem? selected_item_get();
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
        MENU,
        EXPAND,
        LAST
    }
}


/////////////////////////////////////////////////////////////////////////
// elm_tooltip.h
/////////////////////////////////////////////////////////////////////////
//TODO: check/update content from this file
[CCode (cname = "Elm_Tooltip_Content_Cb", instance_pos = 0)]
public delegate Elm.Object TooltipContentCallback( Elm.Object obj, Elm.Object tooltip );

[CCode (cname = "Elm_Tooltip_Item_Content_Cb", instance_pos = 0)]
public delegate Elm.Object TooltipItemContentCallback( Elm.Object obj, Elm.Object tooltip, void* item );
//FIXME: use generics here for "item"? ^



/////////////////////////////////////////////////////////////////////////
// elm_win.h
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cprefix = "ELM_ILLUME_COMMAND_")]
public enum IllumeCommand
{
    FOCUS_BACK,
    FOCUS_FORWARD,
    FOCUS_HOME,
    CLOSE
}

//=======================================================================
[CCode (cname ="Elm_Win_Keyboard_Mode", cprefix = "ELM_WIN_KEYBOARD_")]
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
[CCode (cname = "Elm_Win_Type", cprefix = "ELM_WIN_")]
public enum WinType
{
    BASIC,
    DIALOG_BASIC,
    DESKTOP,
    DOCK,
    TOOLBAR,
    MENU,
    UTILITY,
    SPLASH,
    DROPDOWN_MENU,
    POPUP_MENU,
    TOOLTIP,
    NOTIFICATION,
    COMBO,
    DND,
    INLINED_IMAGE,
    SOCKET_IMAGE
}

//=======================================================================
[CCode (cname = "Elm_Win_Indicator_Opacity_Mode",  cprefix = "ELM_WIN_INDICATOR_")]
public enum WinIndicatorOpacityMode
{
    OPACITY_UNKNOWN,
    OPAQUE,
    TRANSLUCENT,
    TRANSPARENT
}

//=======================================================================
[CCode (cname = "Elm_Win_Indicator_Mode",  cprefix = "ELM_WIN_INDICATOR_")]
public enum WinIndicatorMode
{
    UNKNOWN,
    HIDE,
    SHOW
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Win : Elm.Object
{
    [CCode (cname = "elm_win_add")]
    public Win( Elm.Object? parent = null, string name = "Untitled", WinType t = WinType.BASIC );

    public static Win? util_standard_add( string name, string title );

    public void resize_object_add( Evas.Object subobj );
    public void resize_object_del( Evas.Object subobj );
    public void title_set( string title );
    public unowned string title_get();
    public void icon_name_set( string icon_name );
    public unowned string icon_name_get();
    public void role_set( string role );
    public unowned string role_get();
    public void icon_object_set( Elm.Icon icon );
    public unowned Elm.Icon icon_object_get();
    public void autodel_set( bool autodel );
    public bool autodel_get();
    public void activate();
    public void lower();
    public void raise();
    public void center( bool h, bool v );
    public void borderless_set( bool borderless );
    public bool borderless_get();
    public void shaped_set( bool shaped );
    public bool shaped_get();
    public void alpha_set( bool alpha );
    public bool alpha_get();
    public void override_set( bool override_ );
    public bool override_get();
    public void fullscreen_set( bool fullscreen );
    public bool fullscreen_get();
    public void maximized_set( bool maximized );
    public bool maximized_get();
    public void iconified_set( bool iconified );
    public bool iconified_get();
    public void withdraw_set( bool withdraw );
    public bool withdraw_get();
    public void urgent_set( bool urgent );
    public bool urgent_get();
    public void demand_attention_set( bool demand_attention );
    public bool demand_attention_get();
    public void modal_set( bool modal );
    public bool modal_get();
    public void aspect_set( double aspect );
    public double aspect_get();
    public void layer_set( int layer );
    public int layer_get();
    public void rotation_set( int rotation );
    public void rotation_with_resize_set( int rotation );
    public int rotation_get();
    public void sticky_set( bool sticky );
    public bool sticky_get();
    public void conformant_set( bool conformant );
    public bool conformant_get();

    public void quickpanel_set( bool quickpanel );
    public bool quickpanel_get();
    public void quickpanel_priority_major_set( int priority );
    public int quickpanel_priority_major_get();
    public void quickpanel_priority_minor_set( int priority );
    public int quickpanel_priority_minor_get();
    public void quickpanel_zone_set( int zone );
    public int quickpanel_zone_get();

    public void prop_focus_skip_set( bool skip );
    public bool focus_get();
    public void focus_highlight_style_set( string style );
    public unowned string? focus_highlight_style_get();

    public void illume_command_send( Elm.IllumeCommand command, void* params );

    public unowned Evas.Object? inlined_image_object_get();

    public void screen_constrain_set( bool constrain );
    public bool screen_constrain_get();
    public void screen_size_get( out int x, out int y, out int w, out int h );

    public void keyboard_mode_set( WinKeyboardMode mode );
    public WinKeyboardMode keyboard_mode_get();
    public void keyboard_win_set( bool is_keyboard );
    public bool keyboard_win_get();

    public void indicator_mode_set( WinIndicatorMode mode );
    public WinIndicatorMode indicator_mode_get();
    public void indicator_opacity_set( WinIndicatorOpacityMode mode );
    public WinIndicatorOpacityMode indicator_opacity_get();  

    public void screen_position_get( out int x, out int y );

    public bool socket_listen( string svcname, int svcnum, bool svcsys );

    public unowned Elm.Win? inwin_add();
    public void inwin_activate();
    public void inwin_content_set( Elm.Object content );
    public unowned Elm.Object inwin_content_get();
    public Elm.Object inwin_content_unset();

    public EcoreX.Window xwindow_get();
}



/////////////////////////////////////////////////////////////////////////
// STUFF under this line is DEPRECATED (can be probably found in elm_deprecated.h)
/////////////////////////////////////////////////////////////////////////
//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Pager : Elm.Object
{
    public static unowned Pager? add( Elm.Object parent ); 

    public void content_push( Elm.Object? content );
    public void content_pop();
    public void content_promote( Elm.Object content );
    public unowned Elm.Object? content_bottom_get();
    public unowned Elm.Object? content_top_get();

    public void style_set( string style );
}

// elc_anchorblock.h
//=======================================================================

public struct EntryAnchorblockInfoGeometry
{
    Evas.Coord x;
    Evas.Coord y;
    Evas.Coord w;
    Evas.Coord h;
}

[CCode (cname = "Elm_Entry_Anchorblock_Info")]
public struct EntryAnchorblockInfo
{
    string name;
    int button;
    unowned Elm.Object hover;
    EntryAnchorblockInfoGeometry anchor;
    EntryAnchorblockInfoGeometry hover_parent;
    bool hover_left;
    bool hover_right;
    bool hover_top;
    bool hover_bottom;
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Anchorblock : Elm.Object
{
    [CCode (has_target = false)]
    public delegate Elm.Object AnchorblockCallback(void* data, Elm.Anchorblock anchorblock, string item );

    public static unowned Anchorblock? add( Elm.Object parent );

    public void hover_parent_set( Elm.Object parent );
    public unowned Elm.Object? hover_parent_get();
    public void hover_style_set( string style );
    public unowned string hover_style_get();
    public void hover_end();
    public void item_provider_append( AnchorblockCallback func, void* data );
    public void item_provider_prepend( AnchorblockCallback func, void* data );
    public void item_provider_remove( AnchorblockCallback func, void* data );
}


// elc_anchorview.h
//=======================================================================
public struct EntryAnchorviewInfoGeometry
{
    Evas.Coord x;
    Evas.Coord y;
    Evas.Coord w;
    Evas.Coord h;
}

[CCode (cname = "Elm_Entry_Anchorview_Info")]
public struct EntryAnchorviewInfo
{
    string name;
    int button;
    unowned Elm.Object hover;
    EntryAnchorviewInfoGeometry anchor;
    EntryAnchorviewInfoGeometry hover_parent;
    bool hover_left;
    bool hover_right;
    bool hover_top;
    bool hover_bottom;
}


//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Anchorview : Elm.Object
{
    [CCode (has_target = false)]
    public delegate Elm.Object AnchorviewCallback(void* data, Elm.Anchorblock anchorblock, string item );

    public static unowned Anchorview? add( Elm.Object parent );

    public void hover_parent_set( Elm.Object parent );
    public unowned Elm.Object? hover_parent_get();
    public void hover_style_set( string style );
    public unowned string hover_style_get();
    public void hover_end();
    public void bounce_set( bool h_bounce, bool v_bounce );
    public void bounce_get( out bool h_bounce, out bool v_bounce );
    public void item_provider_append( AnchorviewCallback func, void* data );
    public void item_provider_prepend( AnchorviewCallback func, void* data );
    public void item_provider_remove( AnchorviewCallback func, void* data );
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Notepad : Elm.Object
{
    public static unowned Notepad? add( Elm.Object parent ); 
 
    public void file_set( string file, TextFormat format );
    public void bounce_set( bool h_bounce, bool v_bounce );
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class Carousel : Elm.Object
{
    public static unowned Carousel? add( Elm.Object parent ); 

}

//=======================================================================
[CCode (cprefix = "ELM_SCROLLER_AXIS_")]
public enum ScrollerAxis
{
    HORIZONTAL,
    VERTICAL
}

//=======================================================================
[CCode (cname = "Evas_Object", free_function = "evas_object_del")]
public class ScrolledEntry : Elm.Object
{
    public static unowned ScrolledEntry? add( Elm.Object parent ); 

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

/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_types.h */
#ifndef _ETK_TYPES_H_
#define _ETK_TYPES_H_

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Types Etk_Types
 * @{
 */

#define ETK_FALSE 0
#define ETK_TRUE 1
typedef unsigned char Etk_Bool;

typedef struct Etk_Notification_Callback Etk_Notification_Callback;
typedef struct Etk_Object Etk_Object;
typedef struct Etk_Property Etk_Property;
typedef struct Etk_Property_Value Etk_Property_Value;
typedef struct Etk_Signal Etk_Signal;
typedef struct Etk_Signal_Callback Etk_Signal_Callback;
typedef struct Etk_String Etk_String;
typedef struct Etk_Type Etk_Type;

typedef void (*Etk_Accumulator)(void *return_value, const void *value_to_accum, void *data);
typedef void (*Etk_Constructor)(Etk_Object *object);
typedef void (*Etk_Destructor)(Etk_Object *object);
typedef Etk_Bool (*Etk_Callback)();
typedef Etk_Bool (*Etk_Callback_Swapped)(void *data);
typedef void (*Etk_Marshaller)(Etk_Callback callback, Etk_Object *object, void *data, void *return_value, va_list arguments);

typedef struct Etk_Alignment Etk_Alignment;
typedef struct Etk_Bin Etk_Bin;
typedef struct Etk_Box Etk_Box;
typedef struct Etk_HBox Etk_HBox;
typedef struct Etk_VBox Etk_VBox;
typedef struct Etk_Box_Cell Etk_Box_Cell;
typedef struct Etk_Button Etk_Button;
typedef struct Etk_Cache Etk_Cache;
typedef struct Etk_Canvas Etk_Canvas;
typedef struct Etk_Check_Button Etk_Check_Button;
typedef struct Etk_Colorpicker Etk_Colorpicker;
typedef struct Etk_Combobox Etk_Combobox;
typedef struct Etk_Combobox_Column Etk_Combobox_Column;
typedef struct Etk_Combobox_Item Etk_Combobox_Item;
typedef struct Etk_Combobox_Entry Etk_Combobox_Entry;
typedef struct Etk_Combobox_Entry_Column Etk_Combobox_Entry_Column;
typedef struct Etk_Combobox_Entry_Item Etk_Combobox_Entry_Item;
typedef struct Etk_Container Etk_Container;
typedef struct Etk_Dialog Etk_Dialog;
typedef struct Etk_Drag Etk_Drag;
typedef struct Etk_Embed Etk_Embed;
typedef struct Etk_Engine Etk_Engine;
typedef struct Etk_Entry Etk_Entry;
typedef struct Etk_Evas_Object Etk_Evas_Object;
typedef union  Etk_Event_Global Etk_Event_Global;
typedef struct Etk_Event_Key_Down Etk_Event_Key_Down;
typedef struct Etk_Event_Key_Up Etk_Event_Key_Up;
typedef struct Etk_Event_Mouse_In Etk_Event_Mouse_In;
typedef struct Etk_Event_Mouse_Out Etk_Event_Mouse_Out;
typedef struct Etk_Event_Mouse_Move Etk_Event_Mouse_Move;
typedef struct Etk_Event_Mouse_Down Etk_Event_Mouse_Down;
typedef struct Etk_Event_Mouse_Up Etk_Event_Mouse_Up;
typedef struct Etk_Event_Mouse_Wheel Etk_Event_Mouse_Wheel;
typedef struct Etk_Event_Selection_Request Etk_Event_Selection_Request;
typedef struct Etk_Filechooser_Widget Etk_Filechooser_Widget;
typedef struct Etk_Fixed Etk_Fixed;
typedef struct Etk_Frame Etk_Frame;
typedef struct Etk_Iconbox Etk_Iconbox;
typedef struct Etk_Iconbox_Icon Etk_Iconbox_Icon;
typedef struct Etk_Iconbox_Model Etk_Iconbox_Model;
typedef struct Etk_Image Etk_Image;
typedef struct Etk_Label Etk_Label;
typedef struct Etk_Mdi_Area Etk_Mdi_Area;
typedef struct Etk_Mdi_Window Etk_Mdi_Window;
typedef struct Etk_Menu Etk_Menu;
typedef struct Etk_Menu_Bar Etk_Menu_Bar;
typedef struct Etk_Menu_Item Etk_Menu_Item;
typedef struct Etk_Menu_Item_Check Etk_Menu_Item_Check;
typedef struct Etk_Menu_Item_Image Etk_Menu_Item_Image;
typedef struct Etk_Menu_Item_Radio Etk_Menu_Item_Radio;
typedef struct Etk_Menu_Item_Separator Etk_Menu_Item_Separator;
typedef struct Etk_Menu_Shell Etk_Menu_Shell;
typedef struct Etk_Message_Dialog Etk_Message_Dialog;
typedef struct Etk_Notebook Etk_Notebook;
typedef struct Etk_Paned Etk_Paned;
typedef struct Etk_HPaned Etk_HPaned;
typedef struct Etk_VPaned Etk_VPaned;
typedef struct Etk_Popup_Window Etk_Popup_Window;
typedef struct Etk_Progress_Bar Etk_Progress_Bar;
typedef struct Etk_Radio_Button Etk_Radio_Button;
typedef struct Etk_Range Etk_Range;
typedef struct Etk_Scrollbar Etk_Scrollbar;
typedef struct Etk_HScrollbar Etk_HScrollbar;
typedef struct Etk_VScrollbar Etk_VScrollbar;
typedef struct Etk_Scrolled_View Etk_Scrolled_View;
typedef struct Etk_Selection_Data Etk_Selection_Data;
typedef struct Etk_Selection_Data_Files Etk_Selection_Data_Files;
typedef struct Etk_Selection_Data_Targets Etk_Selection_Data_Targets;
typedef struct Etk_Selection_Data_Text Etk_Selection_Data_Text;
typedef struct Etk_Selection_Event Etk_Selection_Event;
typedef struct Etk_Separator Etk_Separator;
typedef struct Etk_HSeparator Etk_HSeparator;
typedef struct Etk_VSeparator Etk_VSeparator;
typedef struct Etk_Shadow Etk_Shadow;
typedef struct Etk_Slider Etk_Slider;
typedef struct Etk_HSlider Etk_HSlider;
typedef struct Etk_VSlider Etk_VSlider;
typedef struct Etk_Spinner Etk_Spinner;
typedef struct Etk_Statusbar Etk_Statusbar;
typedef struct Etk_Table Etk_Table;
typedef struct Etk_Text_View Etk_Text_View;
typedef struct Etk_Text_View2 Etk_Text_View2;
typedef struct Etk_Textblock Etk_Textblock;
typedef struct Etk_Textblock_Format Etk_Textblock_Format;
typedef struct Etk_Textblock_Iter Etk_Textblock_Iter;
typedef struct Etk_Textblock_Node Etk_Textblock_Node;
typedef struct Etk_Textblock2 Etk_Textblock2;
typedef struct Etk_Textblock2_Format Etk_Textblock2_Format;
typedef struct Etk_Textblock2_Iter Etk_Textblock2_Iter;
typedef struct Etk_Textblock2_Line Etk_Textblock2_Line;
typedef struct Etk_Textblock2_Node Etk_Textblock2_Node;
typedef struct Etk_Textblock2_Paragraph Etk_Textblock2_Paragraph;
typedef struct Etk_Toggle_Button Etk_Toggle_Button;
typedef struct Etk_Toplevel Etk_Toplevel;
typedef struct Etk_Toolbar Etk_Toolbar;
typedef struct Etk_Tool_Button Etk_Tool_Button;
typedef struct Etk_Tool_Toggle_Button Etk_Tool_Toggle_Button;
typedef struct Etk_Tree Etk_Tree;
typedef struct Etk_Tree_Col Etk_Tree_Col;
typedef struct Etk_Tree_Model Etk_Tree_Model;
typedef struct Etk_Tree_Row Etk_Tree_Row;
typedef struct Etk_Viewport Etk_Viewport;
typedef struct Etk_Widget Etk_Widget;
typedef struct Etk_Window Etk_Window;

/**
 * @struct Etk_Position
 * @brief The structure of a position
 */
typedef struct Etk_Position
{
   int x;       /**< The x component */
   int y;       /**< The y component */
} Etk_Position;

/**
 * @struct Etk_Size
 * @brief The structure of a size
 */
typedef struct Etk_Size
{
   int w;       /**< The width */
   int h;       /**< The height */
} Etk_Size;

/**
 * @struct Etk_Geometry
 * @brief The structure of a geometry
 */
typedef struct Etk_Geometry
{
   int x;       /**< The x component */
   int y;       /**< The y component */
   int w;       /**< The width */
   int h;       /**< The height */
} Etk_Geometry;

/**
 * @struct Etk_Color
 * @brief A RGBA color
 */
typedef struct Etk_Color
{
   int r;       /**< The red component of the color */
   int g;       /**< The green component of the color */
   int b;       /**< The blue component of the color */
   int a;       /**< The alpha component of the color */
} Etk_Color;

/** @} */

#ifdef __cplusplus
}
#endif

#endif

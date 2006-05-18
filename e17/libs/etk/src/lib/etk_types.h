/** @file etk_types.h */
#ifndef _ETK_TYPES_H_
#define _ETK_TYPES_H_

#include <stdarg.h>

/**
 * @defgroup Etk_Types Etk_Types
 * @{
 */

#define ETK_FALSE 0
#define ETK_TRUE 1

typedef unsigned char Etk_Bool;
typedef struct _Etk_Type Etk_Type;
typedef struct _Etk_Object Etk_Object;
typedef struct _Etk_Notification_Callback Etk_Notification_Callback;
typedef struct _Etk_Signal Etk_Signal;
typedef struct _Etk_Signal_Callback Etk_Signal_Callback;
typedef struct _Etk_Property Etk_Property;
typedef struct _Etk_Property_Value Etk_Property_Value;
typedef struct _Etk_String Etk_String;

typedef void (*Etk_Constructor)(Etk_Object *object);
typedef void (*Etk_Destructor)(Etk_Object *object);
typedef void (*Etk_Signal_Callback_Function)(void);
typedef void (*Etk_Signal_Swapped_Callback_Function)(void *data);
typedef void (*Etk_Marshaller)(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
typedef void (*Etk_Accumulator)(void *return_value, const void *value_to_accum, void *data); 
typedef void (*Etk_Notification_Callback_Function)(Etk_Object *object, const char *property_name, void *data);

typedef struct _Etk_Widget Etk_Widget;
typedef struct _Etk_Event_Mouse_In_Out Etk_Event_Mouse_In_Out;
typedef struct _Etk_Event_Mouse_Up_Down Etk_Event_Mouse_Up_Down;
typedef struct _Etk_Event_Mouse_Move Etk_Event_Mouse_Move;
typedef struct _Etk_Event_Mouse_Wheel Etk_Event_Mouse_Wheel;
typedef struct _Etk_Event_Key_Up_Down Etk_Event_Key_Up_Down;
typedef struct _Etk_Size Etk_Size;
typedef struct _Etk_Geometry Etk_Geometry;
typedef struct _Etk_Toplevel_Widget Etk_Toplevel_Widget;
typedef struct _Etk_Window Etk_Window;
typedef struct Etk_Dialog Etk_Dialog;
typedef struct Etk_Container Etk_Container;
typedef struct Etk_Bin Etk_Bin;
typedef struct Etk_Box Etk_Box;
typedef struct Etk_HBox Etk_HBox;
typedef struct Etk_VBox Etk_VBox;
typedef struct _Etk_Table Etk_Table;
typedef struct Etk_Alignment Etk_Alignment;
typedef struct Etk_Frame Etk_Frame;
typedef struct _Etk_Paned Etk_Paned;
typedef struct _Etk_HPaned Etk_HPaned;
typedef struct _Etk_VPaned Etk_VPaned;
typedef struct _Etk_Scrolled_View Etk_Scrolled_View;
typedef struct _Etk_Viewport Etk_Viewport;
typedef struct Etk_Image Etk_Image;
typedef struct Etk_Label Etk_Label;
typedef struct Etk_Button Etk_Button;
typedef struct _Etk_Toggle_Button Etk_Toggle_Button;
typedef struct Etk_Check_Button Etk_Check_Button;
typedef struct _Etk_Radio_Button Etk_Radio_Button;
typedef struct _Etk_Textblock Etk_Textblock;
typedef struct _Etk_Textblock_Iter Etk_Textblock_Iter;
typedef struct _Etk_Entry Etk_Entry;
typedef struct _Etk_Text_View Etk_Text_View;
typedef struct Etk_Canvas Etk_Canvas;
typedef struct Etk_Colorpicker Etk_Colorpicker;
typedef struct _Etk_Tree Etk_Tree;
typedef struct _Etk_Tree_Col Etk_Tree_Col;
typedef struct _Etk_Tree_Row Etk_Tree_Row;
typedef struct _Etk_Tree_Model Etk_Tree_Model;
typedef struct _Etk_Range Etk_Range;
typedef struct _Etk_Slider Etk_Slider;
typedef struct _Etk_HSlider Etk_HSlider;
typedef struct _Etk_VSlider Etk_VSlider;
typedef struct _Etk_Scrollbar Etk_Scrollbar;
typedef struct _Etk_HScrollbar Etk_HScrollbar;
typedef struct _Etk_VScrollbar Etk_VScrollbar;
typedef struct _Etk_HSeparator Etk_HSeparator;
typedef struct _Etk_VSeparator Etk_VSeparator;
typedef struct _Etk_Popup_Window Etk_Popup_Window;
typedef struct Etk_Menu_Shell Etk_Menu_Shell;
typedef struct Etk_Menu Etk_Menu;
typedef struct Etk_Menu_Bar Etk_Menu_Bar;
typedef struct _Etk_Menu_Item Etk_Menu_Item;
typedef struct _Etk_Menu_Item_Separator Etk_Menu_Item_Separator;
typedef struct _Etk_Menu_Item_Image Etk_Menu_Item_Image;
typedef struct _Etk_Menu_Item_Check Etk_Menu_Item_Check;
typedef struct _Etk_Menu_Item_Radio Etk_Menu_Item_Radio;
typedef struct Etk_Combobox Etk_Combobox;
typedef struct Etk_Combobox_Column Etk_Combobox_Column;
typedef struct Etk_Combobox_Item Etk_Combobox_Item;
typedef struct _Etk_Statusbar Etk_Statusbar;
typedef struct _Etk_Filechooser_Widget Etk_Filechooser_Widget;
typedef struct _Etk_Notebook Etk_Notebook;
typedef struct _Etk_Progress_Bar Etk_Progress_Bar;
typedef struct _Etk_Spin_Button Etk_Spin_Button;
typedef struct _Etk_Event_Selection_Request Etk_Event_Selection_Request;
typedef struct _Etk_Selection_Data Etk_Selection_Data;
typedef struct _Etk_Selection_Data_Files Etk_Selection_Data_Files;
typedef struct _Etk_Selection_Data_Text Etk_Selection_Data_Text;
typedef struct _Etk_Selection_Data_Targets Etk_Selection_Data_Targets;
typedef struct _Etk_Drag Etk_Drag;
typedef struct _Etk_Argument Etk_Argument;
typedef struct _Etk_Message_Dialog Etk_Message_Dialog;
typedef struct Etk_Iconbox Etk_Iconbox;
typedef struct Etk_Iconbox_Icon Etk_Iconbox_Icon;
typedef struct Etk_Iconbox_Model Etk_Iconbox_Model;

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

#endif

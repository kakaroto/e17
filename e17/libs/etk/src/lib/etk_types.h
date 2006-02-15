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
typedef enum   _Etk_Property_Type Etk_Property_Type;
typedef enum   _Etk_Property_Flags Etk_Property_Flags;
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
typedef enum   _Etk_Stock_Id Etk_Stock_Id;
typedef enum   _Etk_Stock_Size Etk_Stock_Size;
typedef enum   _Etk_Pointer_Type Etk_Pointer_Type;
typedef struct _Etk_Toplevel_Widget Etk_Toplevel_Widget;
typedef struct _Etk_Window Etk_Window;
typedef struct _Etk_Dialog Etk_Dialog;
typedef enum   _Etk_Dialog_Response_ID Etk_Dialog_Response_ID;
typedef struct _Etk_Container Etk_Container;
typedef struct _Etk_Bin Etk_Bin;
typedef struct _Etk_Box Etk_Box;
typedef struct _Etk_Box_Child_Properties Etk_Box_Child_Properties;
typedef struct _Etk_HBox Etk_HBox;
typedef struct _Etk_VBox Etk_VBox;
typedef struct _Etk_Table Etk_Table;
typedef struct _Etk_Alignment Etk_Alignment;
typedef struct _Etk_Frame Etk_Frame;
typedef struct _Etk_Paned Etk_Paned;
typedef struct _Etk_HPaned Etk_HPaned;
typedef struct _Etk_VPaned Etk_VPaned;
typedef enum   _Etk_Scrolled_View_Policy Etk_Scrolled_View_Policy;
typedef struct _Etk_Scrolled_View Etk_Scrolled_View;
typedef struct _Etk_Viewport Etk_Viewport;
typedef struct _Etk_Image Etk_Image;
typedef struct _Etk_Label Etk_Label;
typedef struct _Etk_Button Etk_Button;
typedef struct _Etk_Toggle_Button Etk_Toggle_Button;
typedef struct _Etk_Check_Button Etk_Check_Button;
typedef struct _Etk_Radio_Button Etk_Radio_Button;
typedef struct _Etk_Text_Buffer Etk_Text_Buffer;
typedef struct _Etk_Text_Iter Etk_Text_Iter;
typedef struct _Etk_Entry Etk_Entry;
typedef struct _Etk_Text_View Etk_Text_View;
typedef struct _Etk_Canvas Etk_Canvas;
typedef struct _Etk_Colorpicker Etk_Colorpicker;
typedef struct _Etk_Colorpicker_Vertical Etk_Colorpicker_Vertical;
typedef struct _Etk_Colorpicker_Square Etk_Colorpicker_Square;
typedef enum   _Etk_Color_Mode Etk_Color_Mode;
typedef struct _Etk_Tree Etk_Tree;
typedef enum   _Etk_Tree_Mode Etk_Tree_Mode;
typedef struct _Etk_Tree_Col Etk_Tree_Col;
typedef struct _Etk_Tree_Row Etk_Tree_Row;
typedef struct _Etk_Tree_Model Etk_Tree_Model;
typedef enum   _Etk_Tree_Model_Image_Type Etk_Tree_Model_Image_Type;
typedef struct _Etk_Range Etk_Range;
typedef struct _Etk_Slider Etk_Slider;
typedef struct _Etk_HSlider Etk_HSlider;
typedef struct _Etk_VSlider Etk_VSlider;
typedef struct _Etk_Scrollbar Etk_Scrollbar;
typedef struct _Etk_HScrollbar Etk_HScrollbar;
typedef struct _Etk_VScrollbar Etk_VScrollbar;
typedef struct _Etk_HSeparator Etk_HSeparator;
typedef struct _Etk_VSeparator Etk_VSeparator;
typedef struct _Etk_Menu_Shell Etk_Menu_Shell;
typedef struct _Etk_Menu Etk_Menu;
typedef struct _Etk_Menu_Bar Etk_Menu_Bar;
typedef struct _Etk_Menu_Item Etk_Menu_Item;
typedef struct _Etk_Menu_Separator Etk_Menu_Separator;
typedef struct _Etk_Menu_Check_Item Etk_Menu_Check_Item;
typedef struct _Etk_Menu_Radio_Item Etk_Menu_Radio_Item;
typedef struct _Etk_Statusbar Etk_Statusbar;
typedef struct _Etk_Filechooser_Widget Etk_Filechooser_Widget;
typedef struct _Etk_Notebook Etk_Notebook;
typedef struct _Etk_Progress_Bar Etk_Progress_Bar;
typedef enum   _Etk_Progress_Bar_Orientation Etk_Progress_Bar_Orientation;
typedef struct _Etk_Spin_Button Etk_Spin_Button;
typedef struct _Etk_Event_Selection_Request Etk_Event_Selection_Request;
typedef struct _Etk_Selection_Data Etk_Selection_Data;
typedef struct _Etk_Selection_Data_Files Etk_Selection_Data_Files;
typedef struct _Etk_Selection_Data_Text Etk_Selection_Data_Text;
typedef struct _Etk_Selection_Data_Targets Etk_Selection_Data_Targets;
typedef enum   _Etk_Selection_Content Etk_Selection_Content;
typedef struct _Etk_Drag Etk_Drag;
/**
 * @enum Etk_Fill_Policy_Flags
 * @brief Describes how a child should fill the space allocated by its parent container
 */ 
typedef enum _Etk_Fill_Policy_Flags
{
   ETK_FILL_POLICY_NONE = 1 << 0,
   ETK_FILL_POLICY_HFILL = 1 << 1,
   ETK_FILL_POLICY_VFILL = 1 << 2,
   ETK_FILL_POLICY_HEXPAND = 1 << 3,
   ETK_FILL_POLICY_VEXPAND = 1 << 4
} Etk_Fill_Policy_Flags;

/**
 * @struct Etk_Color
 * @brief A color
 */
typedef struct _Etk_Color
{
   int r, g, b, a;
} Etk_Color;

#endif

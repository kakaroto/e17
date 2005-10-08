/** @file etk_types.h */
#ifndef _ETK_TYPES_H_
#define _ETK_TYPES_H_

#include <stdarg.h>
#include <Evas.h>

/**
 * @defgroup Etk_Types Etk_Types
 * @{
 */

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/* Etk Object: */
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
typedef void (*Etk_Copy_Constructor)(Etk_Object *dst, const Etk_Object *src);
typedef void (*Etk_Signal_Callback_Function)(void);
typedef void (*Etk_Signal_Swapped_Callback_Function)(void *data);
typedef void (*Etk_Marshaller)(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments);
typedef void (*Etk_Accumulator)(void *return_value, const void *value_to_accum, void *data); 
typedef void (*Etk_Notification_Callback_Function)(Etk_Object *object, const char *property_name, void *data);

/* Widgets: */
typedef struct _Etk_Widget Etk_Widget;
typedef struct _Etk_Event_Mouse_In_Out Etk_Event_Mouse_In_Out;
typedef struct _Etk_Event_Mouse_Up_Down Etk_Event_Mouse_Up_Down;
typedef struct _Etk_Event_Mouse_Move Etk_Event_Mouse_Move;
typedef struct _Etk_Event_Mouse_Wheel Etk_Event_Mouse_Wheel;
typedef Evas_Event_Key_Down Etk_Event_Key_Up_Down;
typedef struct _Etk_Size Etk_Size;
typedef struct _Etk_Geometry Etk_Geometry;
typedef struct _Etk_Toplevel_Widget Etk_Toplevel_Widget;
typedef struct _Etk_Container Etk_Container;
typedef struct _Etk_Bin Etk_Bin;
typedef struct _Etk_Box Etk_Box;
typedef struct _Etk_Box_Child_Properties Etk_Box_Child_Properties;
typedef struct _Etk_HBox Etk_HBox;
typedef struct _Etk_VBox Etk_VBox;
typedef struct _Etk_Table Etk_Table;
typedef struct _Etk_Table_Col_Row Etk_Table_Col_Row;
typedef struct _Etk_Table_Child_Properties Etk_Table_Child_Properties;
typedef struct _Etk_Alignment Etk_Alignment;
typedef struct _Etk_Window Etk_Window;
typedef struct _Etk_Image Etk_Image;
typedef struct _Etk_Label Etk_Label;
typedef struct _Etk_Button Etk_Button;
typedef struct _Etk_Toggle_Button Etk_Toggle_Button;
typedef struct _Etk_Check_Button Etk_Check_Button;
typedef struct _Etk_Radio_Button Etk_Radio_Button;
typedef struct _Etk_Entry Etk_Entry;
typedef struct _Etk_Adjustment Etk_Adjustment;
typedef struct _Etk_Range Etk_Range;
typedef struct _Etk_Canvas Etk_Canvas;
typedef struct _Etk_Colorpicker Etk_Colorpicker;
typedef struct _Etk_Colorpicker_Vertical Etk_Colorpicker_Vertical;
typedef struct _Etk_Colorpicker_Square Etk_Colorpicker_Square;
typedef struct _Etk_Tree Etk_Tree;
typedef struct _Etk_Tree_Col Etk_Tree_Col;
typedef struct _Etk_Tree_Row Etk_Tree_Row;
typedef union _Etk_Tree_Cell Etk_Tree_Cell;
typedef struct _Etk_Tree_Node Etk_Tree_Node;

/* Enums: */

/**
 * @enum Etk_Property_Type
 * @brief The type of a property
 */
typedef enum _Etk_Property_Type
{
   /* Used when the property is not initialized to a value */
   ETK_PROPERTY_NONE,
   ETK_PROPERTY_INT,
   ETK_PROPERTY_BOOL,
   ETK_PROPERTY_CHAR,
   ETK_PROPERTY_FLOAT,
   ETK_PROPERTY_DOUBLE,
   ETK_PROPERTY_SHORT,
   ETK_PROPERTY_LONG,
   ETK_PROPERTY_POINTER,
   ETK_PROPERTY_STRING
} Etk_Property_Type;

/** 
 * @enum Etk_Property_Flags
 * @brief  Describes if the property is readable, writable or both
 */
typedef enum _Etk_Property_Flags
{
   ETK_PROPERTY_READABLE = 1 << 0,
   ETK_PROPERTY_WRITABLE = 1 << 1,
   ETK_PROPERTY_READABLE_WRITABLE = ETK_PROPERTY_READABLE | ETK_PROPERTY_WRITABLE
} Etk_Property_Flags;

/**
 * @enum Etk_Child_Fill_Policy_Flag
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

/**
 * @enum Etk_Color_Mode
 * @brief The color modes used by the colorpicker
 */
typedef enum _Etk_Color_Mode
{
   ETK_COLOR_MODE_H, 
   ETK_COLOR_MODE_S,
   ETK_COLOR_MODE_V,
   ETK_COLOR_MODE_R,
   ETK_COLOR_MODE_G,
   ETK_COLOR_MODE_B
} Etk_Color_Mode;

/**
 * @enum Etk_Tree_Col_Type
 * @brief The type of the objects of a column of a tree
 */
typedef enum _Etk_Tree_Col_Type
{
   ETK_TREE_COL_TEXT,
   ETK_TREE_COL_IMAGE,
   ETK_TREE_COL_ICON_TEXT,
   ETK_TREE_COL_INT,
   ETK_TREE_COL_DOUBLE
} Etk_Tree_Col_Type;

/**
 * @enum Etk_Tree_Mode
 * @brief The mode of the tree: List (rows can not have children) or tree (rows can have children)
 */
typedef enum _Etk_Tree_Mode
{
   ETK_TREE_MODE_LIST,
   ETK_TREE_MODE_TREE
} Etk_Tree_Mode;

#endif

#ifndef EDJE_TYPES_H
#define EDJE_TYPES_H

#include <Eina.h>

typedef enum Edje_Param_Type
{
   EDJE_PARAM_TYPE_INT,
   EDJE_PARAM_TYPE_DOUBLE,
   EDJE_PARAM_TYPE_STRING,
   EDJE_PARAM_TYPE_BOOL,
   EDJE_PARAM_TYPE_CHOICE
} Edje_Param_Type;

typedef enum Edje_Compression_Type
{
   EDJE_COMPRESSION_TYPE_RAW,
   EDJE_COMPRESSION_TYPE_COMP,
   EDJE_COMPRESSION_TYPE_LOSSY,
   EDJE_COMPRESSION_TYPE_USER
} Edje_Compression_Type;

typedef enum Edje_Box_Layout
{
   EDJE_BOX_LAYOUT_HORIZONTAL,
   EDJE_BOX_LAYOUT_VERTICAL,
   EDJE_BOX_LAYOUT_HORIZONTAL_HOMOGENEOUS,
   EDJE_BOX_LAYOUT_VERTICAL_HOMOGENEOUS,
   EDJE_BOX_LAYOUT_HORIZONTAL_MAX,
   EDJE_BOX_LAYOUT_VERTICAL_MAX,
   EDJE_BOX_LAYOUT_HORIZONTAL_FLOW,
   EDJE_BOX_LAYOUT_VERTICAL_FLOW,
   EDJE_BOX_LAYOUT_STACK,
   EDJE_BOX_LAYOUT_CUSTOM
} Edje_Box_Layout;

typedef enum Edje_Program_Action
{
   EDJE_PROGRAM_ACTION_UNKNOWN,
   EDJE_PROGRAM_ACTION_STATE_SET,
   EDJE_PROGRAM_ACTION_ACTION_STOP,
   EDJE_PROGRAM_ACTION_SIGNAL_EMIT,
   EDJE_PROGRAM_ACTION_DRAG_VAL_SET,
   EDJE_PROGRAM_ACTION_DRAG_VAL_STEP,
   EDJE_PROGRAM_ACTION_DRAG_VAL_PAGE,
   EDJE_PROGRAM_ACTION_FOCUS_SET,
   EDJE_PROGRAM_ACTION_FOCUS_OBJECT,
   EDJE_PROGRAM_ACTION_PARAM_COPY,
   EDJE_PROGRAM_ACTION_PARAM_SET
} Edje_Program_Action;

typedef enum Edje_Program_Transition
{
   EDJE_PROGRAM_TRANSITION_UNKNOWN,
   EDJE_PROGRAM_TRANSITION_LINEAR,
   EDJE_PROGRAM_TRANSITION_SINUSOIDAL,
   EDJE_PROGRAM_TRANSITION_ACCELERATE,
   EDJE_PROGRAM_TRANSITION_DECELERATE
} Edje_Program_Transition;

typedef enum Edje_Image_Middle
{
   EDJE_IMAGE_MIDDLE_NONE,
   EDJE_IMAGE_MIDDLE_DEFAULT,
   EDJE_IMAGE_MIDDLE_SOLID
} Edje_Image_Middle;

typedef enum Edje_Image_Scale_Hint
{
   EDJE_IMAGE_SCALE_HINT_NONE,
   EDJE_IMAGE_SCALE_HINT_DYNAMIC,
   EDJE_IMAGE_SCALE_HINT_STATIC
} Edje_Image_Scale_Hint;

typedef enum Edje_Part_Type
{
   EDJE_PART_TYPE_UNKNOWN,
   EDJE_PART_TYPE_RECT,
   EDJE_PART_TYPE_TEXT,
   EDJE_PART_TYPE_IMAGE,
   EDJE_PART_TYPE_SWALLOW,
   EDJE_PART_TYPE_TEXTBLOCK,
   EDJE_PART_TYPE_GROUP,
   EDJE_PART_TYPE_BOX,
   EDJE_PART_TYPE_TABLE,
   EDJE_PART_TYPE_EXTERNAL,
   EDJE_PART_TYPE_PROXY
} Edje_Part_Type;

typedef enum Edje_Part_Ignore_Flag
{
   EDJE_IGNORE_FLAG_NONE,
   EDJE_IGNORE_FLAG_ON_HOLD
} Edje_Part_Ignore_Flag;

typedef enum Edje_Part_Pointer_Mode
{
   EDJE_POINTER_MODE_AUTOGRAB,
   EDJE_POINTER_MODE_NOGRAB
} Edje_Part_Pointer_Mode;

typedef enum Edje_Part_Effect
{
   EDJE_PART_EFFECT_NONE,
   EDJE_PART_EFFECT_PLAIN,
   EDJE_PART_EFFECT_OUTLINE,
   EDJE_PART_EFFECT_SOFT_OUTLINE,
   EDJE_PART_EFFECT_SHADOW,
   EDJE_PART_EFFECT_SOFT_SHADOW,
   EDJE_PART_EFFECT_OUTLINE_SHADOW,
   EDJE_PART_EFFECT_OUTLINE_SOFT_SHADOW,
   EDJE_PART_EFFECT_FAR_SHADOW,
   EDJE_PART_EFFECT_FAR_SOFT_SHADOW,
   EDJE_PART_EFFECT_GLOW
} Edje_Part_Effect;

typedef enum Edje_Part_Entry_Mode
{
   EDJE_PART_ENTRY_MODE_NONE,
   EDJE_PART_ENTRY_MODE_PLAIN,
   EDJE_PART_ENTRY_MODE_EDITABLE,
   EDJE_PART_ENTRY_MODE_PASSWORD
} Edje_Part_Entry_Mode;

typedef enum Edje_Part_Select_Mode
{
   EDJE_PART_SELECT_MODE_DEFAULT,
   EDJE_PART_SELECT_MODE_EXPLICIT_DEFAULT
} Edje_Part_Select_Mode;

typedef enum Edje_Part_Description_Aspect_Pref
{
   EDJE_PART_DESCRIPTION_ASPECT_PREF_NONE,
   EDJE_PART_DESCRIPTION_ASPECT_PREF_HORIZONTAL,
   EDJE_PART_DESCRIPTION_ASPECT_PREF_VERTICAL,
   EDJE_PART_DESCRIPTION_ASPECT_PREF_BOTH
} Edje_Part_Description_Aspect_Pref;

typedef struct Edje_Style
{
   EINA_INLIST;
   const char *doc;
   const char *name;
   const char *base;
   Eina_Hash  *tags;
} Edje_Style;

typedef struct Edje_Color_Class
{
   EINA_INLIST;
   const char *doc;
   const char *name;
   int         color[4];
   int         color2[4];
   int         color3[4];
} Edje_Color_Class;

typedef struct Edje_Data
{
   EINA_INLIST;
   const char *doc;
   Eina_Hash  *items;
   Eina_Hash  *files;
} Edje_Data;

typedef struct Edje_External
{
   EINA_INLIST;
   const char *doc;
   const char *external;
} Edje_External;

typedef struct Edje_Set_Image
{
   EINA_INLIST;
   const char           *doc;
   const char           *filename;
   Edje_Compression_Type comp;
   int                   comp_amount;
   int                   size[4];
} Edje_Set_Image;

typedef struct Edje_Set
{
   EINA_INLIST;
   const char     *doc;
   const char     *name;
   Edje_Set_Image *images;
} Edje_Set;

typedef struct Edje_Images
{
   EINA_INLIST;
   const char     *doc;
   Edje_Set_Image *images;
   Edje_Set       *sets;
} Edje_Images;

typedef struct Edje_Image
{
   EINA_INLIST;
   const char           *doc;
   const char           *normal;
   Eina_List            *tween; /* stringshared tween filenames */
   int                   border[4];
   Edje_Set_Image       *images;
   Edje_Set             *sets;
   Edje_Images          *imageses;
   Edje_Image_Middle     middle;
   Eina_Bool             border_scale : 1;
   Edje_Image_Scale_Hint scale_hint;
} Edje_Image;

typedef struct Edje_Font
{
   EINA_INLIST;
   const char *doc;
   const char *name;
   const char *alias;
} Edje_Font;

typedef struct Edje_Fonts
{
   EINA_INLIST;
   const char *doc;
   Edje_Font  *fonts;
} Edje_Fonts;

typedef struct Edje_Program
{
   EINA_INLIST;
   const char *doc;
   const char *name;
   const char *signal;
   const char *source;
   const char *script;
   Eina_List  *targets; /* stringshared */
   Eina_List  *after; /* stringshared */
   struct
   {
      const char *name;
      const char *description;
   } api;
   struct
   {
      const char *part;
      const char *state;
   } filter;
   struct
   {
      double from;
      double range;
   } in;
   struct
   {
      Edje_Program_Action type;
      struct
      {
         Eina_List *strings; /* stringshared in order */
         double     numbers[2];
      } params;
      struct
      {
         Edje_Program_Transition type;
         double                  length;
      } transition;
   } action;
} Edje_Program;

typedef struct Edje_Programs
{
   EINA_INLIST;
   const char     *doc;

   Edje_Set_Image *images;
   Edje_Images    *imageses;
   Edje_Set       *sets;
   Edje_Font      *fonts;
   Edje_Fonts     *fontses;
   Edje_Program   *programs;
} Edje_Programs;

typedef struct Edje_Part_Description_Relative
{
   double      relative[2];
   int         offset[2];
   const char *to;
   const char *to_x;
   const char *to_y;
} Edje_Part_Description_Relative;

typedef struct Edje_Param
{
   Edje_Param_Type type;
   union
   {
      int         i;
      double      d;
      const char *s;
      Eina_Bool   b;
   } data;
} Edje_Param;

typedef struct Edje_Part_Description
{
   EINA_INLIST;
   const char *doc;
   const char *script;
   struct
   {
      const char *name;
      double       index;
   } inherit;
   struct
   {
      const char *name;
      double       index;
   } state;
   union
   {
      struct
      {
         const char *text;
         const char *text_class;
         const char *font;
         const char *style;
         const char *repch;
         const char *source;
         const char *text_source;
         double       elipsis;
         double       align[2];
         int         size;
         Eina_Bool   fit[2];
         Eina_Bool   min[2];
         Eina_Bool   max[2];
      } text;
      struct
      {
         const char *perspective;
         const char *light;
         Eina_Bool   on : 1;
         Eina_Bool   smooth : 1;
         Eina_Bool   alpha : 1;
         Eina_Bool   backface_cull : 1;
         Eina_Bool   perspective_on : 1;
         struct
         {
            const char *center;
            double       x;
            double       y;
            double       z;
         } rotation;
      } map;
      struct
      {
         Edje_Box_Layout primary;
         Edje_Box_Layout fallback;
         double align[2];
         int padding[2];
         Eina_Bool min[2];
      } box;
   } type;
   struct
   {
      int zplane;
      int focal;
   } perspective;
   struct
   {
      Eina_Bool smooth : 1;
      struct
      {
         double relative[2];
         int    offset[2];
      } origin;
      struct
      {
         double relative[2];
         int    offset[2];
      } size;
   } fill;
   double                             align[2];
   double                            aspect[2];
   int                               min[2];
   int                               max[2];
   int                               step[2];
   Eina_Bool                         fixed[2];
   Eina_Bool                         visible : 1;
   Edje_Part_Description_Aspect_Pref aspect_preference;
   Edje_Part_Description_Relative    rel[2];
   const char                       *color_class;
   int                               color[4];
   int                               color2[4];
   int                               color3[4];
   Eina_Hash                        *params; /* Edje_Param */
   Edje_Fonts                       *fontses;
   Edje_Font                        *fonts;
   Edje_Image                       *image;
   Edje_Images                      *imageses;
   Edje_Color_Class                 *color_classes;
   Edje_Program                     *programs;
   Edje_Programs                    *programses;
   Edje_Style                       *styles;
} Edje_Part_Description;

typedef struct Edje_Part
{
   EINA_INLIST;
   const char            *doc;
   const char            *script;
   const char            *name;
   const char            *clip_to;
   const char            *source;
   const char            *source2;
   const char            *source3;
   const char            *source4;
   const char            *source5;
   const char            *source6;
   Edje_Part_Ignore_Flag  ignore_flags;
   Edje_Part_Type         type;
   Edje_Part_Effect       effect;
   Edje_Part_Pointer_Mode pointer_mode;
   Edje_Part_Entry_Mode   entry_mode;
   Edje_Part_Select_Mode  select_mode;
   Eina_Bool              mouse_events : 1;
   Eina_Bool              repeat_events : 1;
   Eina_Bool              scale : 1;
   Eina_Bool              precise_is_inside : 1;
   Eina_Bool              use_alternate_font_metrics : 1;
   Eina_Bool              multiline : 1;
   struct
   {
      const char *confine;
      const char *events;
      int         x[3];
      int         y[3];
   } dragable;
   Edje_Part_Description *descriptions;
   Edje_Set_Image        *set_images;
   Edje_Color_Class      *color_classes;
   Edje_Set              *sets;
   Edje_Set_Image        *images;
   Edje_Images           *imageses;
   Edje_Fonts            *fontses;
   Edje_Font             *fonts;
   Edje_Style            *styles;
   Edje_Program          *programs;
   Edje_Programs         *programses;
} Edje_Part;

typedef struct Edje_Parts
{
   const char       *doc;
   const char       *script;
   Eina_Hash        *aliases;
   Edje_Color_Class *color_classes;
   Edje_Set         *sets;
   Edje_Set_Image   *images;
   Edje_Font        *fonts;
   Edje_Fonts       *fontses;
   Edje_Images      *imageses;
   Edje_Part        *parts;
   Edje_Program     *programs;
   Edje_Programs    *programses;
   Edje_Style       *styles;
} Edje_Parts;

typedef struct Edje_Group
{
   EINA_INLIST;
   const char       *doc;
   const char       *name;
   const char       *alias;
   const char       *script;
   int               min[2];
   int               max[2];
   Eina_Bool         script_only : 1;
   Edje_Color_Class *color_classes;
   Edje_Data        *data;
   Edje_External    *externals;
   Edje_Font        *fonts;
   Edje_Fonts       *fontses;
   Edje_Images      *imageses;
   Edje_Parts       *parts;
   Edje_Program     *programs;
   Edje_Programs    *programses;
   Edje_Set         *sets;
   Edje_Set_Image   *images;
   Edje_Style       *styles;
} Edje_Group;

typedef struct Edje_Collection
{
   EINA_INLIST;
   const char       *doc;
   Edje_Color_Class *color_classes;
   Edje_External    *externals;
   Edje_Font        *fonts;
   Edje_Fonts       *fontses;
   Edje_Group       *groups;
   Edje_Images      *imageses;
   Edje_Set         *sets;
   Edje_Set_Image   *images;
   Edje_Style       *styles;
} Edje_Collection;

typedef struct Edje
{
   Edje_Collection  *collections;
   Edje_Color_Class *color_classes;
   Edje_Data        *data;
   Edje_External    *externals;
   Edje_Fonts       *fontses;
   Edje_Images      *imageses;
   Edje_Set         *sets;
   Edje_Style       *styles;
} Edje;
#endif

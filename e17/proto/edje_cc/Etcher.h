#ifndef ETCHER_H
#define ETCHER_H

#include <Evas.h>

typedef enum _Etcher_Image_Type Etcher_Image_Type;
typedef enum _Etcher_Part_Type Etcher_Part_Type;
typedef enum _Etcher_Text_Effect Etcher_Text_Effect;
typedef enum _Etcher_Action Etcher_Action;
typedef enum _Etcher_Transition Etcher_Transition;
typedef enum _Etcher_Aspect_Preference Etcher_Aspect_Preference;

typedef struct _Etcher_File Etcher_File;
typedef struct _Etcher_Data Etcher_Data;
typedef struct _Etcher_Image Etcher_Image;
typedef struct _Etcher_Font Etcher_Font;
typedef struct _Etcher_Group Etcher_Group;
typedef struct _Etcher_Part Etcher_Part;
typedef struct _Etcher_Program Etcher_Program;
typedef struct _Etcher_Part_State Etcher_Part_State;

typedef enum _Etcher_Parse_Sectoin Etcher_Parse_Section;


enum _Etcher_Image_Type
{
  ETCHER_IMAGE_TYPE_RAW,
  ETCHER_IMAGE_TYPE_COMP,
  ETCHER_IMAGE_TYPE_LOSSY,
  ETCHER_IMAGE_TYPE_EXTERNAL,
  ETCHER_IMAGE_TYPE_NUM

};

enum _Etcher_Part_Type
{
  ETCHER_PART_TYPE_IMAGE,
  ETCHER_PART_TYPE_TEXT,
  ETCHER_PART_TYPE_RECT,
  ETCHER_PART_TYPE_SWALLOW,
  ETCHER_PART_TYPE_NUM
};

enum _Etcher_Text_Effect
{
  ETCHER_TEXT_EFFECT_NONE,
  ETCHER_TEXT_EFFECT_PLAIN,
  ETCHER_TEXT_EFFECT_OUTLINE,
  ETCHER_TEXT_EFFECT_SOFT_OUTLINE,
  ETCHER_TEXT_EFFECT_SHADOW,
  ETCHER_TEXT_EFFECT_OUTLINE_SHADOW,
  ETCHER_TEXT_EFFECT_SOFT_SHADOW,
  ETCHER_TEXT_EFFECT_OUTLINE_SOFT_SHADOW,
  ETCHER_TEXT_EFFECT_NUM
};

enum _Etcher_Action
{
  ETCHER_ACTION_STATE_SET,
  ETCHER_ACTION_STOP,
  ETCHER_ACTION_SIGNAL_EMIT,
  ETCHER_ACTION_DRAG_VAL_SET,
  ETCHER_ACTION_DRAG_VAL_STEP,
  ETCHER_ACTION_DRAG_VAL_PAGE,
  ETCHER_ACTION_SCRIPT,
  ETCHER_ACTION_NUM
};

enum _Etcher_Transition
{
  ETCHER_TRANSITION_LINEAR,
  ETCHER_TRANSITION_SINUSOIDAL,
  ETCHER_TRANSITION_ACCELERATE,
  ETCHER_TRANSITION_DECELERATE,
  ETCHER_TRANSITION_NUM
};

enum _Etcher_Aspect_Preference
{
  ETCHER_ASPECT_PREFERENCE_NONE,
  ETCHER_ASPECT_PREFERENCE_VERTICAL,
  ETCHER_ASPECT_PREFERENCE_HORIZONTAL,
  ETCHER_ASPECT_PREFERENCE_BOTH,
  ETCHER_ASPECT_PREFERENCE_NUM
};

struct _Etcher_File
{
  Evas_List *images;
  Evas_List *fonts;
  Evas_List *data;
  Evas_List *groups;  
};

struct _Etcher_Data
{
  char *key;
  char *value;
  int int_value;
};

struct _Etcher_Image
{
  char *name; /* basename */
  char *path; /* dir path */
  Etcher_Image_Type type;
  double value;
};

struct _Etcher_Font
{
  char *name; /* alias */
  char *file; /* basename */
  char *path; /* dir path */
};

struct _Etcher_Group
{
  char *name;
  struct
  {
    int w, h;
  } min, max;

  Evas_List *parts;
  Evas_List *programs;
  Evas_List *data;

  char *script;
};

struct _Etcher_Part
{
  char *name;
  Etcher_Part_Type type;
  Etcher_Text_Effect effect;
  int mouse_events;
  int repeat_events;
  char *clip_to;

  struct
  {
    signed char x, y; /* can drag in x/y, and which dir to count in */
    struct
    {
      int x, y;
    } step, count; 
    char *confine;

  } dragable;

  Evas_List *states;
};

struct _Etcher_Program 
{
  char *name;
  char *signal;
  char *source;
  Evas_List *targets;
  Evas_List *afters;

  struct {
    double from, range;
  } in;
  
  Etcher_Action action;
  char *state, *state2;
  double value, value2;

  Etcher_Transition transition;
  double duration;
 
  char *script;
};

struct _Etcher_Part_State
{
  char *name;
  double value;

  unsigned char visible;

  struct
  {
    double x, y;
  } align, step;

  struct
  {
    double w, h;
  } min, max;

  struct
  {
    double w, h;
    Etcher_Aspect_Preference prefer;
  } aspect;

  struct
  {
    struct
    {
      double x, y;
    } relative;
    
    struct
    {
      int x, y;
    } offset;
    
    char *to_x;
    char *to_y;
  } rel1, rel2;

  struct
  {
    Etcher_Image *normal;
    Evas_List *tween;
  } image;

  struct
  {
    int l, r, t, b;
  } border;

  char *color_class;

  struct
  {
    int r, g, b, a;
  } color, color2, color3;

  struct
  {
    int           smooth; 

    struct
    {
      double x, y;
    } pos_rel, rel;
    struct
    {
      int x, y;
    } pos_abs, abs;
  } fill;

  struct
  {
    char          *text; 
    char          *text_class; 
    char          *font; 

    int            size; 

    struct {
      int x, y;
    } fit, min;

    struct {
      double      x, y; 
    } align;
  } text;


};

enum Etcher_Parse_Section
{
BASE,
COLLECTIONS,
DATA,
DESCRIPTION,
DRAGABLE,
FILL,
FONT,
FONTS,
GROUP,
GROUPS,
IMAGES,
PART,
PARTS,
PROGRAM,
PROGRAMS,
REL1,
REL2,
SCRIPT,
TEXT
};


#endif

#ifndef ETCHER_H
#define ETCHER_H

#include <Evas.h>

typedef enum _Etcher_Image_Type Etcher_Image_Type;
typedef enum _Etcher_Part_Type Etcher_Part_Type;
typedef enum _Etcher_Text_Effect Etcher_Text_Effect;
typedef enum _Etcher_Action Etcher_Action;
typedef enum _Etcher_Transition Etcher_Transition;

typedef struct _Etcher_File Etcher_File;
typedef struct _Etcher_Data Etcher_Data;
typedef struct _Etcher_Image Etcher_Image;
typedef struct _Etcher_Font Etcher_Font;
typedef struct _Etcher_Group Etcher_Group;
typedef struct _Etcher_Part Etcher_Part;
typedef struct _Etcher_Part_State Etcher_Part_State;



enum _Etcher_Image_Type
{
  ETCHER_IMAGE_TYPE_RAW,
  ETCHER_IMAGE_TYPE_COMP,
  ETCHER_IMAGE_TYPE_LOSSY,
  ETCHER_IMAGE_TYPE_EXTERNAL

};

enum _Etcher_Part_Type
{
  ETCHER_PART_TYPE_IMAGE,
  ETCHER_PART_TYPE_TEXT,
  ETCHER_PART_TYPE_RECT,
  ETCHER_PART_TYPE_SWALLOW
};

enum _Etcher_Text_Effect
{
  ETCHER_TEXT_EFFECT_NONE,
  ETCHER_TEXT_EFFECT_PLAIN,
  ETCHER_TEXT_EFFECT_OUTLINE,
  ETCHER_TEXT_EFFECT_SHADOW,
  ETCHER_TEXT_EFFECT_OUTLINE_SHADOW,
  ETCHER_TEXT_EFFECT_SOFT_SHADOW,
  ETCHER_TEXT_EFFECT_OUTLINE_SOFT_SHADOW
};

enum _Etcher_Action
{
  ETCHER_ACTION_STATE_SET,
  ETCHER_ACTION_STOP,
  ETCHER_ACTION_SIGNAL_EMIT,
  ETCHER_ACTION_DRAG_VAL_SET,
  ETCHER_ACTION_DRAG_VAL_STEP,
  ETCHER_ACTION_DRAG_VAL_PAGE,
  ETCHER_ACTION_SCRIPT
};

enum _Etcher_Transition
{
  ETCHER_TRANSITION_LINEAR,
  ETCHER_TRANSITION_SINUSOIDAL,
  ETCHER_TRANSITION_ACCELERATE,
  ETCHER_TRANSITION_DECELERATE
};

struct _Etcher_File
{
  Evas_List *images;
  Evas_List *fonts;
  
};

struct _Etcher_Data
{
  char *key;
  char *value;
};

struct _Etcher_Image
{
  char *image;
  char *path;
  Etcher_Image_Type type;
  double value;
};

struct _Etcher_Font
{
  char *name;
  char *font;
  char *path;
};

struct _Etcher_Group
{
  char *name;
  int min, max;

  Evas_List *parts;
  Evas_List *programs;
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

  int in1, in2;
  
  Etcher_Action action;
  Etcher_Transition transition;
  
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
    unsigned char prefer; /* NEITHER = 0, VERTICAL = 1, HORIZONTAL = 2 */
  } aspect;

  struct
  {
    struct
    {
      double x, y;
    } relative, offset;
    
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
  };

  struct
  {
    unsigned char r, g, b, a;
  } color, color2, color3;

  struct
  {
    char           smooth; 

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
      unsigned char x, y;
    } fit, min;

    struct {
      double      x, y; 
    } align;
  } text;


};

#endif

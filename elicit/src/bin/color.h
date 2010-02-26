#ifndef ELICIT_COLOR_H
#define ELICIT_COLOR_H

#define COLOR_HEX_HASH 1
#define COLOR_HEX_ALPHA 2
#define COLOR_HEX_CAPS 4

#include <Eina.h>

typedef struct Color Color;
typedef enum Color_Mode Color_Mode;
typedef enum Color_Type Color_Type;
typedef enum Color_Hex_Option Color_Hex_Option;

enum Color_Mode
{
  COLOR_MODE_RGBA,
  COLOR_MODE_HSVA
};

enum Color_Type
{
  COLOR_TYPE_RED,
  COLOR_TYPE_GREEN,
  COLOR_TYPE_BLUE,
  COLOR_TYPE_HUE,
  COLOR_TYPE_SATURATION,
  COLOR_TYPE_VALUE,
  COLOR_TYPE_ALPHA,
  COLOR_TYPE_HEX,
  COLOR_TYPE_INVALID
};

struct Color
{
  const char *name;
  Color_Mode mode;
  int r, g, b;
  float h, s, v;
  int a;

  int refcount;

  Eina_List *changed_callbacks;
};

typedef void (*Color_Callback_Func) (Color *color, void *data);

struct Color_Callback
{
  Color_Callback_Func func;
  void *data;
};

typedef struct Color_Callback Color_Callback;

Color *color_new();
void color_ref(Color *c);
void color_unref(Color *c);
void color_free(Color *color);

Color *color_clone(Color *color);
void color_copy(Color *from, Color *to);

void color_name_set(Color *color, const char *name);
const char *color_name_get(Color *color);

Color_Mode color_mode_get(Color *color);
void color_rgba_set(Color *color, int r, int g, int b, int a);
void color_hsva_set(Color *color, float h, float s, float v, int a);
void color_argb_int_set(Color *color, int argb);

void color_rgba_get(Color *color, int *r, int *g, int *b, int *a);
void color_hsva_get(Color *color, float *h, float *s, float *v, int *a);
int  color_argb_int_get(Color *color);

void color_hex_set(Color *color, const char *hex);
const char *color_hex_get(Color *color, int options);

void color_changed(Color *color);
void color_callback_changed_add(Color *color, Color_Callback_Func func, void *data);
void color_callback_changed_del(Color *color, Color_Callback_Func func);



#endif

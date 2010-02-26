#include <stdlib.h>
#include <stdio.h>
#include <Eina.h>
#include <Evas.h>
#include "color.h"

/**
 * Create a new color
 */
Color *
color_new()
{
  Color *color;

  color = calloc(1, sizeof(Color));
  if (!color) return NULL;

  color->r = color->g = color->b = color->a = 255;
  color->h = 0; color->s = 1; color->v = 1;
  color->refcount = 1;
  return color;
}

void
color_ref(Color *color)
{
  color->refcount++;
}

void
color_unref(Color *color)
{
  color->refcount--;
  if (color->refcount == 0)
    color_free(color);
}

/**
 * Free a color
 * @param color - the color to free
 */
void 
color_free(Color *color)
{
  Color_Callback *cb;

  if (!color) return;

  if (color->name) eina_stringshare_del(color->name);
  EINA_LIST_FREE(color->changed_callbacks, cb)
    free(cb);

  free(color);
}

/**
 * Clone a color (make a deep copy of it)
 * @param color - the color to clone
 * @return - the clone
 */
Color *
color_clone(Color *color)
{
  Color *clone;
  clone = color_new();
  color_copy(color, clone);
  return clone;
}

/**
 * Copy the color values from one color struct to another
 * @param from
 * @param to
 */
void color_copy(Color *from, Color *to)
{
  if (from->name)
    to->name = eina_stringshare_add(from->name);

  to->mode = from->mode;
  to->r = from->r;
  to->g = from->g;
  to->b = from->b;
  to->h = from->h;
  to->s = from->s;
  to->v = from->v;
  to->a = from->a;

  color_changed(to);
}

/**
 * Set a color's name
 * @param color
 * @param name
 */
void 
color_name_set(Color *color, const char *name)
{
  if (color->name) eina_stringshare_del(color->name);
  if (name)
    color->name = eina_stringshare_add(name);
  else
    color->name = NULL;
}

/**
 * Get a color's name 
 * @param color
 * @return the name or "" if none is set
 */
const char *
color_name_get(Color *color)
{
  return color->name ? color->name : "";
}

/**
 * Get a color's mode
 * @param color
 * @return the mode (color space) the color was specified in
 */
Color_Mode
color_mode_get(Color *color)
{
  return color->mode;
}

/**
 * Set a color from RGBA values
 * @param color
 * @param r - red (0-255)
 * @param g - green (0-255)
 * @param b - blue (0-255)
 * @param a - alpha (0-255)
 *
 * If -1 is passed for r, g, b, or a, then that value will not be changed.
 */
void
color_rgba_set(Color *color, int r, int g, int b, int a)
{
  color->mode = COLOR_MODE_RGBA;

  if (r >= 0 && r <= 255) color->r = r;
  if (g >= 0 && g <= 255) color->g = g;
  if (b >= 0 && b <= 255) color->b = b;
  if (a >= 0 && a <= 255) color->a = a;
  evas_color_rgb_to_hsv(color->r, color->g, color->b, &(color->h), &(color->s), &(color->v));
  color_changed(color);
}

/**
 * Set a color from HSVA values
 * @param color
 * @param h - the hue (0-360)
 * @param s - the saturation (0.0 - 1.0)
 * @param v - the value (0.0 - 1.0)
 * @param a - alpha (0-255)
 *
 * If -1 is passed for h, s, v, or a, then that value will not be changed.
 */
void
color_hsva_set(Color *color, float h, float s, float v, int a)
{
  color->mode = COLOR_MODE_HSVA;
  if (h >= 0 && h <= 360) color->h = h;
  if (s >= 0 && s <= 1) color->s = s;
  if (v >= 0 && v <= 1) color->v = v;
  if (a >= 0 && a <= 255) color->a = a;
  evas_color_hsv_to_rgb(color->h, color->s, color->v, &(color->r), &(color->g), &(color->b));
  color_changed(color);
}

/**
 * Set a color from ARGB values packed into an integer
 * @param color
 * @param argb
 */
void
color_argb_int_set(Color *color, int argb)
{
  color_rgba_set(
    color,
    argb >> 16 & 0xff,
    argb >> 8  & 0xff,
    argb       & 0xff,
    argb >> 24 & 0xff
  );
}

/**
 * Get the RGBA values of a color
 * @param color
 * @param r - an int pointer to return the red value in 
 * @param g - an int pointer to return the green value in 
 * @param b - an int pointer to return the blue value in 
 * @param a - an int pointer to return the alpha value in 
 *
 * Any return pointers which are NULL will be skipped.
 */
void
color_rgba_get(Color *color, int *r, int *g, int *b, int *a)
{
  if (r) *r = color->r;
  if (g) *g = color->g;
  if (b) *b = color->b;
  if (a) *a = color->a;
}

/**
 * Get the HSVA values of a color
 * @param color
 * @param h - a float pointer to return the hue value in
 * @param s - a float pointer to return the saturation value in
 * @param v - a float pointer to return the valuee value in 
 * @param a - an int pointer to return the alpha value in
 *
 * Any return pointers which are NULL will be skipped.
 */
void
color_hsva_get(Color *color, float *h, float *s, float *v, int *a)
{
  if (h) *h = color->h;
  if (s) *s = color->s;
  if (v) *v = color->v;
  if (a) *a = color->a;
}

/**
 * Get the ARGB values of a color as a packed integer
 * @param color
 * @return - rgba packed in to integer  
 */
int
color_argb_int_get(Color *color)
{
  return color->a << 24 |
         color->r << 16 |
         color->g << 8 |
         color->b;
}

/**
 * Set a color from a hex string
 * @param color
 * @param hex
 *
 * The hex string consists of an optional hash mark ('#') followed by 2 digits each of r, g, b and optionally, a.
 * The following are all valid for 'opaque white' (r = g = b = a = 255):
 *   ffffff 
 *   #ffffff
 *   ffffffff
 *   #ffffffff
 *
 * (if the alpha value is left off, 255 is implied)
 */
void
color_hex_set(Color *color, const char *hex)
{
  int r, g, b;
  if (hex[0] == '#') hex++;
  if (sscanf(hex, "%2x%2x%2x", &r, &g, &b)) {
    color_rgba_set(color, r, g, b, color->a);
  }
}

/**
 * Get the RGB(A) value of a color as a hex string with an optional 
 * leading '#'
 * @param color
 * @param options - bitmap of the following options
 *        COLOR_HEX_HASH - include hash 
 *        COLOR_HEX_ALPHA - include alph
 */
const char *
color_hex_get(Color *color, int options)
{
  static char buf[10];
  char *formats[8] = {
    "%02x%02x%02x",
    "#%02x%02x%02x",
    "%02x%02x%02x%02x",
    "#%02x%02x%02x%02x",
    "%02X%02X%02X",
    "#%02X%02X%02X",
    "%02X%02X%02X%02X",
    "#%02X%02X%02X%02X"
  };

  sprintf(buf, formats[options & 7], color->r, color->g, color->b, color->a);
  return buf;
}

void
color_changed(Color *color)
{
  Eina_List *l;
  Color_Callback *cb;

  EINA_LIST_FOREACH(color->changed_callbacks, l, cb)
  {
    cb->func(color, cb->data);
  }
}

void
color_callback_changed_add(Color *color, Color_Callback_Func func, void *data)
{
  Color_Callback *cb;

  cb = calloc(1, sizeof(Color_Callback));

  cb->func = func;
  cb->data = data;

  color->changed_callbacks = eina_list_prepend(color->changed_callbacks, cb);
}
void
color_callback_changed_del(Color *color, Color_Callback_Func func)
{
  Eina_List *l, *nl;
  Color_Callback *cb;

  EINA_LIST_FOREACH_SAFE(color->changed_callbacks, l, nl, cb)
  {
    if (cb->func == func)
    {
      color->changed_callbacks = eina_list_remove_list(color->changed_callbacks, l);
      break;
    }
  }
}

int
color_rgba_compare(Color *c1, Color *c2)
{
  if (c1->r < c2->r) return -1;
  if (c1->r > c2->r) return 1;
  if (c1->g < c2->g) return -1;
  if (c1->g > c2->g) return 1;
  if (c1->b < c2->b) return -1;
  if (c1->b > c2->b) return 1;
  if (c1->a < c2->a) return -1;
  if (c1->a > c2->a) return 1;
  return 0;
}

#ifndef BITS_H
#define BITS_H 1
#include <Evas.h>

#define EDITOR 1

typedef struct _Ebits_Object * Ebits_Object;

Ebits_Object ebits_load(char *file);
void ebits_free(Ebits_Object o);
void ebits_add_to_evas(Ebits_Object o, Evas e);
void ebits_show(Ebits_Object o);
void ebits_hide(Ebits_Object o);
void ebits_set_layer(Ebits_Object o, int l);
void ebits_raise(Ebits_Object o);
void ebits_lower(Ebits_Object o);
void ebits_move(Ebits_Object o, double x, double y);
void ebits_resize(Ebits_Object o, double w, double h);
void ebits_get_padding(Ebits_Object o, int *l, int *r, int *t, int *b);
void ebits_get_insets(Ebits_Object o, int *l, int *r, int *t, int *b);
void ebits_get_min_size(Ebits_Object o, int *w, int *h);
void ebits_get_max_size(Ebits_Object o, int *w, int *h);
void ebits_get_size_step(Ebits_Object o, int *x, int *y);
void ebits_get_bit_geometry(Ebits_Object o, char *c, double *x, double *y, double *w, double *h);

/* private stuff */

typedef struct _Ebits_Object_Description * Ebits_Object_Description;
typedef struct _Ebits_Object_State Ebits_Object_State;
typedef struct _Ebits_Object_Bit_Description * Ebits_Object_Bit_Description;
typedef struct _Ebits_Object_Bit_State * Ebits_Object_Bit_State;

Ebits_Object ebits_new(void);
#ifdef EDITOR
void ebits_save(Ebits_Object o, char *file);
Ebits_Object_Bit_State ebits_new_bit(Ebits_Object o, char *file);
Ebits_Object_Description ebits_new_description(void);
void ebits_del_bit(Ebits_Object o, Ebits_Object_Bit_State state);
void ebits_set_state(Ebits_Object o, int st);
void _ebits_evaluate(Ebits_Object_Bit_State state);
Ebits_Object_Bit_State ebits_get_bit_name(Ebits_Object o, char *name);
#endif

struct _Ebits_Object_Description
{
   char *file;
   int   references;
   struct {
      int w, h;
   } min, max;
   struct {
      int x, y;
   } step;
   struct {
      int l, r, t, b;
   } padding, inset;
   Evas_List bits;
};

struct _Ebits_Object_Bit_Description
{
   char *name;
   char *class;
   char *color_class;
   struct {
      char *image;
   } normal, hilited, clicked, disabled, selected;
   struct {
      int l, r, t, b;
   } border;
   struct {
      int w, h;
   } tile;
   struct {
      char *name;
      int x, y;
      double rx, ry;
      int ax, ay;
   } rel1, rel2;
   struct {
      double w, h;
   } align;
   struct {
      int x, y;
   } aspect, step;
   struct {
      int w, h;
   } min, max;
   Evas_List sync;   
};

struct _Ebits_Object_State
{
   double x, y, w, h;
   int    layer;
   int    visible;
   Evas   evas;
};

struct _Ebits_Object_Bit_State
{
   Ebits_Object                 o;
   Evas_Object                  object;
   Ebits_Object_Bit_Description description;
   int                          recalc;
   int                          calculated;
   int                          calc_pending;
   int                          x, y, w, h;
   int                          mouse_in;
   int                          state;
   int                          syncing;
#ifdef EDITOR   
   struct {
      int                          saved;
      Imlib_Image                  image;
   } normal, hilited, clicked, disabled, selected;
#endif   
};

struct _Ebits_Object
{
   Ebits_Object_Description description;
   Ebits_Object_State state;
   Evas_List bits;
};
#endif

#ifndef BITS_H
#define BITS_H 1
#include <Evas.h>

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
void ebits_save(Ebits_Object o, char *file);

struct _Ebits_Object_Description
{
   char *file;
   int   references;
   struct {
      int w, h;
   } min, max;
   struct {
      int l, r, t, b;
   } padding, inset;
   Evas_List bits;
};

struct _Ebits_Object_Bit_Description
{
   char *name;
   char *class;
   struct {
      char *image;
   } normal, hilited, clicked, disabled;
   struct {
      int l, r, t, b;
   } border;
   struct {
      char *name;
      int x, y;
      double rx, ry;
      int ax, ay;
   } rel1, rel2;
   struct {
      double w, h;
   } aspect, align;
   struct {
      int x, y;
   } step;
   Evas_List *sync;   
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
   Evas_Object                  object;
   Ebits_Object_Bit_Description description;
   int                          recalc;
   int                          calculated;
   double                       x, y, w, h;
};

struct _Ebits_Object
{
   Ebits_Object_Description description;
   Ebits_Object_State state;
   Evas_List bits;
};
#endif

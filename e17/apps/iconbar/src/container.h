#ifndef CONTAINER_H
#define CONTAINER_H

#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

/*****
 Todo:

   o add a "mover" object that tracks where moving element will go

****/
typedef struct _Container Container;
typedef struct _Container_Element Container_Element;
typedef enum _Container_Alignment Container_Alignment;
typedef enum _Container_Fill_Policy Container_Fill_Policy;

enum _Container_Alignment
{
  CONTAINER_ALIGN_CENTER,
  CONTAINER_ALIGN_LEFT,
  CONTAINER_ALIGN_RIGHT,
  CONTAINER_ALIGN_BOTTOM = CONTAINER_ALIGN_LEFT,
  CONTAINER_ALIGN_TOP = CONTAINER_ALIGN_RIGHT
};

enum _Container_Fill_Policy
{
  CONTAINER_FILL_POLICY_NONE = 0,
  CONTAINER_FILL_POLICY_KEEP_ASPECT = 0x01,
  CONTAINER_FILL_POLICY_FILL_X = 0x02,
  CONTAINER_FILL_POLICY_FILL_Y = 0x04,
  CONTAINER_FILL_POLICY_FILL = 0x08 
};

struct _Container
{
  Evas *evas;
  Evas_Object *obj;     /* the evas smart object */
  Evas_Object *clipper; /* element clip */
  Evas_Object *grabber; /* event grabber (for the container as a whole) */

  Evas_List *elements;  /* things contained */

  struct
  {
    double l, r, t, b;
  } padding;

  int spacing;          /* space between elements */

  int direction;        /* 0 = horizontal or 1 = vertical */
//  int fit_x, fit_y;     /* shrink elements if area is smaller than them */
//  int fill_x, fill_y;   /* make elements fill the container */
  Container_Alignment align;  /* CONTAINER_ALIGN_LEFT, _CENTER, or _RIGHT */
  Container_Fill_Policy fill;

  int move_button;      /* which button to move elements with? (0 for none) */

  int scroll;
  Ecore_Timer *scroll_timer;

  void (*cb_order_change) (void *data);
  void *data_order_change;
};

struct _Container_Element
{
  Container *container;
  Evas_Object *obj;
  Evas_Object *grabber;

  double orig_w, orig_h;

  struct
  {
    double x, y;
  } down, delta, current;

  int mouse_down;
  int dragging;
};

Evas_Object *e_container_new(Evas *evas);


void e_container_direction_set(Evas_Object *container, int direction);
int  e_container_direction_get(Evas_Object *container);

void e_container_padding_set(Evas_Object *container, double l, double r,
                             double t, double b);
void e_container_padding_get(Evas_Object *container, double *l, double *r,
                             double *t, double *b);

void e_container_fill_policy_set(Evas_Object *container,
                                 Container_Fill_Policy fill);
Container_Fill_Policy  e_container_fill_policy_get(Evas_Object *container);

void e_container_alignment_set(Evas_Object *container,
                               Container_Alignment align);
Container_Alignment e_container_alignment_get(Evas_Object *container);

void e_container_scroll_set(Evas_Object *container, int scroll);
int  e_container_scroll_get(Evas_Object *container);

void    e_container_spacing_set(Evas_Object *container,
                                int spacing);
int  e_container_spacing_get(Evas_Object *container);

/* element adding/removing */
void e_container_element_append(Evas_Object *container, Evas_Object *element);
void e_container_element_prepend(Evas_Object *container, Evas_Object *element);
void e_container_element_append_relative(Evas_Object *container,
                                         Evas_Object *element,
                                         Evas_Object *relative);
void e_container_element_prepend_relative(Evas_Object *container,
                                          Evas_Object *element,
                                          Evas_Object *relative);
void e_container_element_remove(Evas_Object *container, Evas_Object *element);

Evas_List *e_container_elements_get(Evas_Object *container);

/* callbacks */
void e_container_callback_order_change_set(Evas_Object *obj, 
                                           void (*func)(void *data),
                                           void *data);

#endif

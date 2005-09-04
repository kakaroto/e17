#ifndef ESMART_CONTAINER_H
#define ESMART_CONTAINER_H

#include <Evas.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

/*****
 Todo:

   o add a "mover" object that tracks where moving element will go

****/
#ifdef __cplusplus
extern "C" {
#endif

enum _Container_Direction
{
  CONTAINER_DIRECTION_HORIZONTAL,  
  CONTAINER_DIRECTION_VERTICAL
};
typedef enum _Container_Direction Container_Direction;

enum _Container_Alignment
{
  CONTAINER_ALIGN_CENTER = 0,
  CONTAINER_ALIGN_LEFT = 0x01,
  CONTAINER_ALIGN_RIGHT = 0x02,
  CONTAINER_ALIGN_BOTTOM = 0x04,
  CONTAINER_ALIGN_TOP = 0x08
};
typedef enum _Container_Alignment Container_Alignment;

enum _Container_Fill_Policy
{
  CONTAINER_FILL_POLICY_NONE = 0,
  CONTAINER_FILL_POLICY_KEEP_ASPECT = 0x01,
  CONTAINER_FILL_POLICY_FILL_X = 0x02,
  CONTAINER_FILL_POLICY_FILL_Y = 0x04,
  CONTAINER_FILL_POLICY_FILL = 0x08, 
  CONTAINER_FILL_POLICY_HOMOGENOUS = 0x10
};
typedef enum _Container_Fill_Policy Container_Fill_Policy;

Evas_Object *esmart_container_new(Evas *evas);

void esmart_container_direction_set(Evas_Object *container, Container_Direction direction);
Container_Direction esmart_container_direction_get(Evas_Object *container);


void esmart_container_padding_set(Evas_Object *container, double l, double r,
                             double t, double b);
void esmart_container_padding_get(Evas_Object *container, double *l, double *r,
                             double *t, double *b);


void esmart_container_fill_policy_set(Evas_Object *container,
                                 Container_Fill_Policy fill);
Container_Fill_Policy  esmart_container_fill_policy_get(Evas_Object *container);


void esmart_container_alignment_set(Evas_Object *container,
                               Container_Alignment align);
Container_Alignment esmart_container_alignment_get(Evas_Object *container);


void esmart_container_spacing_set(Evas_Object *container,
                                int spacing);
int  esmart_container_spacing_get(Evas_Object *container);


void esmart_container_move_button_set(Evas_Object *container, int move_button);
int  esmart_container_move_button_get(Evas_Object *container);


/* element adding/removing */
void esmart_container_element_append(Evas_Object *container, Evas_Object *element);
void esmart_container_element_prepend(Evas_Object *container, Evas_Object *element);
void esmart_container_element_append_relative(Evas_Object *container,
                                         Evas_Object *element,
                                         Evas_Object *relative);
void esmart_container_element_prepend_relative(Evas_Object *container,
                                          Evas_Object *element,
                                          Evas_Object *relative);
void esmart_container_element_remove(Evas_Object *container, Evas_Object *element);
void esmart_container_element_destroy(Evas_Object *container, Evas_Object *element);
void esmart_container_empty (Evas_Object *container);
void esmart_container_sort(Evas_Object *container, int (*func)(Evas_Object *, Evas_Object *));

Evas_List *esmart_container_elements_get(Evas_Object *container);

/* scrolling */
void esmart_container_scroll_start(Evas_Object *container, double velocity);
void esmart_container_scroll_stop(Evas_Object *container);
void esmart_container_scroll(Evas_Object *container, int val);

void   esmart_container_scroll_offset_set(Evas_Object *container, int val);
int    esmart_container_scroll_offset_get(Evas_Object *container);
void   esmart_container_scroll_percent_set(Evas_Object *container, double percent);
double esmart_container_scroll_percent_get(Evas_Object *container);
void   esmart_container_scroll_to(Evas_Object *container, Evas_Object *element);

/* callbacks */
void esmart_container_callback_order_change_set(Evas_Object *obj, 
                                           void (*func)(void *data),
                                           void *data);

double esmart_container_elements_length_get(Evas_Object *container);
double esmart_container_elements_orig_length_get(Evas_Object *container);

int esmart_container_layout_plugin_set(Evas_Object *container, const char *name);


#ifdef __cplusplus
}
#endif

#endif

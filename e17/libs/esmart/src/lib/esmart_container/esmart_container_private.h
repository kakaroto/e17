#ifndef CONTAINER_PRIVATE_H
#define CONTAINER_PRIVATE_H

typedef struct _Container Container;
typedef struct _Container_Element Container_Element;
typedef struct _Scroll_Data Scroll_Data;
typedef struct _Container_Layout_Plugin Container_Layout_Plugin;
   
struct _Container
{
  Evas *evas;
  Evas_Object *obj;     /* the evas smart object */
  Evas_Object *clipper; /* element clip */
  Evas_Object *grabber; /* event grabber (for the container as a whole) */

  Container_Layout_Plugin *plugin;

  Evas_List *elements;  /* things contained */

  struct
  {
    double l, r, t, b;
  } padding;

  double x, y, w, h;    /* geometry */
  
  int clipper_orig_alpha;		/* original alpha value of clipper */

  int spacing;          /* space between elements */

  Container_Direction direction; /* CONTAINER_DIRECTION_HORIZONTAL, _VERTICAL */
  Container_Alignment align;  /* CONTAINER_ALIGN_LEFT, _CENTER, or _RIGHT */
  Container_Fill_Policy fill;

  int move_button;      /* which button to move elements with? (0 for none) */

  struct
  {
    int offset;
    Ecore_Timer *timer;
    double start_time;
    double velocity;
  } scroll;
  
  double length;

  void (*cb_order_change) (void *data);
  void *data_order_change;

  unsigned char changed : 1;
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


struct _Container_Layout_Plugin{
  void *handle;

  void (*shutdown)(void);
  
  void (*layout)(Container *cont);
  
  void (*scroll_start)(Container *cont, double velocity);
  void (*scroll_stop)(Container *cont);
  void (*scroll_to)(Container *cont, Container_Element *el);

  void (*post_init)(Container *cont);
  void (*changed)(Container *cont);
};

Container *_container_fetch(Evas_Object *obj);
Container_Element *_container_element_new(Container *cont, Evas_Object *obj);
void _container_elements_fix(Container *cont);
void _container_elements_changed(Container *cont);
void _container_element_del(Container_Element *el);
double _container_elements_orig_length_get(Container *cont);
void _cb_container(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_move(void *data, Evas *e, Evas_Object *obj, void *event_info);

void _container_scale_scroll(Container *cont, double old_length);
int is_dir(const char *dir);
Evas_List *_dir_get_files(const char *directory);

void _container_layout_plugin_free(Container_Layout_Plugin *p);
Container_Layout_Plugin *_container_layout_plugin_new(const char *name);

#endif

#ifndef FLOW_CHART_H
#define FLOW_CHART_H

#include <Ecore.h>
#include <Evas.h>
#include <Edje.h>

typedef struct _Chart_Container Chart_Container;
typedef struct _Flow_Chart Flow_Chart;

#include "e_mod_main.h"

struct _Chart_Container {
   Evas               *evas;
   Evas_Object        *clip;

   int                 x, y, w, h;

   Evas_List          *charts;
};

struct _Flow_Chart {
   Evas               *evas;
   Evas_Object        *clip;
   Evas_List          *old_values;

   int                 layer;

   Evas_Object        *edje;
   const char         *edje_text_part;

   int                 r, g, b, a;
   int                 (*get_value_func) (void);
   int                 current_value;
   void                (*chart_callback) (Flow_Chart * chart, void *data);
   void               *callback_data;

   int                 x, y, w, h;
   double              start_rel, end_rel;
   double              update_rate;

   Ecore_Timer        *timer;

   unsigned char       is_runable:1;
   unsigned char       is_running:1;
   unsigned char       alignment_bottom:1;
};

Flow_Chart         *flow_chart_new(void);
void                flow_chart_del(Flow_Chart * chart);

void                flow_chart_color_set(Flow_Chart * chart, int r, int g,
                                         int b, int a);
void                flow_chart_layer_set(Flow_Chart * chart, int layer);
void                flow_chart_get_value_function_set(Flow_Chart * chart,
                                                      int (*func) (void));
void                flow_chart_callback_set(Flow_Chart * chart,
                                            void (*cb) (Flow_Chart * chart,
                                                        void *data),
                                            void *data);
void                flow_chart_alignment_set(Flow_Chart * chart, int alignment);
void                flow_chart_range_set(Flow_Chart * chart, double start_rel,
                                         double end_rel);
void                flow_chart_update_rate_set(Flow_Chart * chart, double rate);

void                flow_chart_run(Flow_Chart * chart);
void                flow_chart_stop(Flow_Chart * chart);

//void flow_chart_set_usage_text_to_edje_part(Flow_Chart *chart, Evas_Object *edje, const char *text_part);

Chart_Container    *chart_container_new(Evas * evas, int x, int y, int w,
                                        int h);
void                chart_container_del(Chart_Container * con);
void                chart_container_move(Chart_Container * con, int x, int y);
void                chart_container_resize(Chart_Container * chart, int w,
                                           int h);
void                chart_container_chart_add(Chart_Container * con,
                                              Flow_Chart * chart);
void                chart_container_chart_remove(Chart_Container * con,
                                                 Flow_Chart * chart);
void                chart_container_stop_charts(Chart_Container * chart_con);
void                chart_container_run_charts(Chart_Container * chart_con);

#endif

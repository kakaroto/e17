#include "flow_chart.h"
#include <stdlib.h>

static int
flow_chart_timer(void *data)
{
   Flow_Chart *chart = (Flow_Chart *)data;

   Evas_Object *o;
   Evas_Object *last = NULL;
   Evas_List *l;
   int i, j = 0;
   int value = 0, text_value = 0;
   double factor = 0.0;

   double start_rel = chart->start_rel;
   double end_rel = chart->end_rel;

   int x = chart->x;
   int y = chart->y;
   int w = chart->w;
   int h = chart->h;

   int dist_right = (int)((double)w * start_rel);
   int dist_left = (int)((double)w * (1.0 - end_rel));

   x = x - dist_right;

   chart->current_value = text_value = value = chart->get_value_func();

   /* a value less than zero is an error handle it */
   if (value < 0)
     {
        if (chart->edje && chart->edje_text_part)
           edje_object_part_text_set(chart->edje, chart->edje_text_part,
                                     "error");
        chart->is_runable = 0;
        flow_chart_stop(chart);
     }

   factor = ((double)h) / ((double)100);

   value = (int)(((double)value) * factor);

   o = evas_object_line_add(chart->evas);
   evas_object_clip_set(o, chart->clip);
   evas_object_layer_set(o, chart->layer);

   if (value == 0)
      evas_object_hide(o);
   else
     {
        if (chart->alignment_bottom == 1)
           evas_object_line_xy_set(o, x + w, y + h, x + w, y + h - value);
        else
           evas_object_line_xy_set(o, x + w, y, x + w, y + value);
     }

   evas_object_color_set(o, chart->r, chart->g, chart->b, chart->a);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);

   chart->old_values = evas_list_prepend(chart->old_values, o);
   l = chart->old_values;

   for (i = x + w; l && j - 2 < w - dist_right - dist_left; l = l->next, j++)
     {
        Evas_Coord y;
        Evas_Object *lo;

        lo = (Evas_Object *)evas_list_data(l);

        evas_object_geometry_get(lo, NULL, &y, NULL, NULL);

        evas_object_move(lo, i--, y);

        last = lo;
     }

   if (j - 2 >= w - dist_right - dist_left)
     {
        chart->old_values = evas_list_remove(chart->old_values, last);
        evas_object_del(last);
     }

   if (chart->chart_callback)
     {
        chart->chart_callback(chart, chart->callback_data);
     }

   return 1;
}

Flow_Chart *
flow_chart_new(void)
{
   Flow_Chart *chart = NULL;

   chart = (Flow_Chart *)calloc(1, sizeof(Flow_Chart));
   if (!chart)
      return NULL;

   chart->is_runable = 0;
   chart->is_running = 0;
   chart->alignment_bottom = 1;

   chart->get_value_func = NULL;
   chart->current_value = 0;
   chart->chart_callback = NULL;
   chart->callback_data = NULL;

   chart->evas = NULL;
   chart->old_values = NULL;

   chart->start_rel = 0.0, chart->end_rel = 1.0;
   chart->update_rate = 1.0;

   chart->layer = 0;
   chart->r = 0;
   chart->g = 0;
   chart->b = 0;
   chart->a = 255;

   chart->edje = NULL;
   chart->edje_text_part = NULL;

   return chart;
}

void
flow_chart_color_set(Flow_Chart *chart, int r, int g, int b, int a)
{
   unsigned char was_running = 0;

   if (chart->is_running)
     {
        flow_chart_stop(chart);
        was_running = 1;
     }

   chart->r = r;
   chart->g = g;
   chart->b = b;
   chart->a = a;

   if (was_running)
      flow_chart_run(chart);
}

void
flow_chart_layer_set(Flow_Chart *chart, int layer)
{
   chart->layer = layer;
}

void
flow_chart_alignment_set(Flow_Chart *chart, int align)
{
   chart->alignment_bottom = align;
}

void
flow_chart_callback_set(Flow_Chart *chart,
                        void (*cb) (Flow_Chart *chart, void *data), void *data)
{
   chart->chart_callback = cb;
   chart->callback_data = data;
}

void
flow_chart_get_value_function_set(Flow_Chart *chart, int (*func) (void))
{
   unsigned char was_running = 0;
   int test = -1;

   if (chart->is_running)
     {
        flow_chart_stop(chart);
        was_running = 1;
     }

   if ((test = func()) != -1)
     {
        chart->get_value_func = func;
        if (chart->evas)
           chart->is_runable = 1;
     }
   else
     {
        chart->get_value_func = NULL;
        chart->is_runable = 0;
        return;
     }

   if (was_running)
      flow_chart_run(chart);
}

void
flow_chart_update_rate_set(Flow_Chart *chart, double rate)
{
   unsigned char was_running = 0;

   if (chart->is_running)
     {
        flow_chart_stop(chart);
        was_running = 1;
     }

   if (rate < 0.0 || rate > 1000.0)
      chart->update_rate = 1.0;
   else
      chart->update_rate = rate;

   if (was_running)
      flow_chart_run(chart);
}

void
flow_chart_set_usage_text_to_edje_part(Flow_Chart *chart, Evas_Object *edje,
                                       const char *text_part)
{
   chart->edje = edje;
   chart->edje_text_part = text_part;
}

void
flow_chart_run(Flow_Chart *chart)
{
   if (!chart->is_runable || chart->is_running)
      return;

   chart->timer = ecore_timer_add(chart->update_rate, flow_chart_timer, chart);
   chart->is_running = 1;

   evas_object_show(chart->clip);
}

void
flow_chart_stop(Flow_Chart *chart)
{
   Evas_List *l;

   if (!chart->is_running)
      return;

   //evas_object_hide(chart->clip);

   ecore_timer_del(chart->timer);

   for (l = chart->old_values; l; l = l->next)
     {
        Evas_Object *o;

        o = evas_list_data(l);

        evas_object_del(o);
     }

   evas_list_free(chart->old_values);

   chart->old_values = NULL;
   chart->is_running = 0;
}

void
flow_chart_del(Flow_Chart *chart)
{
   if (chart->is_running)
      flow_chart_stop(chart);

   free(chart);
}

/* Chart Container */

Chart_Container *
chart_container_new(Evas *evas, int x, int y, int w, int h)
{
   Chart_Container *chart_con = NULL;
   Evas_Object *o = NULL;

   /* allocat memory return NULL on false */
   chart_con = (Chart_Container *)calloc(1, sizeof(Chart_Container));
   if (!chart_con)
      return NULL;

   /* set pointers and geometry */
   chart_con->evas = evas;
   chart_con->charts = NULL;

   chart_con->x = x;
   chart_con->y = y;
   chart_con->w = w;
   chart_con->h = h;

   /* create container clip */
   o = evas_object_rectangle_add(evas);
   chart_con->clip = o;
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_move(o, x, y);
   evas_object_resize(o, w, h);
   evas_object_pass_events_set(o, 1);

   //evas_object_show(o);

   return chart_con;
}

void
chart_container_del(Chart_Container *chart_con)
{
   Evas_List *l = NULL;

   if (!chart_con)
      return;

   l = chart_con->charts;

   if (l)
      for (; l; l = l->next)
         flow_chart_del((Flow_Chart *)evas_list_data(l));

   evas_list_free(chart_con->charts);

   evas_object_del(chart_con->clip);

   free(chart_con);
}

void
chart_container_chart_remove(Chart_Container *chart_con, Flow_Chart *chart)
{
   if (!chart_con || !chart)
      return;

   if (!chart_con->charts)
      return;
   else
     {
        chart_con->charts = evas_list_remove(chart_con->charts, chart);
        flow_chart_del(chart);
        if (evas_list_count(chart_con->charts) == 0)
          {
             evas_list_free(chart_con->charts);
             chart_con->charts = NULL;
          }
     }
}

void
chart_container_move(Chart_Container *chart_con, int x, int y)
{
   if (!chart_con)
      return;

   chart_con->x = x;
   chart_con->y = y;

   evas_object_move(chart_con->clip, x, y);

   {
      Evas_List *l = chart_con->charts;

      if (!l)
         return;

      for (; l; l = l->next)
        {
           Flow_Chart *chart = (Flow_Chart *)evas_list_data(l);

           flow_chart_stop(chart);
           chart->x = x;
           chart->y = y;
           flow_chart_run(chart);
        }
   }

}

void
chart_container_resize(Chart_Container *chart_con, int w, int h)
{
   if (!chart_con)
      return;

   chart_con->w = w;
   chart_con->h = h;

   evas_object_resize(chart_con->clip, w, h);

   {
      Evas_List *l = chart_con->charts;

      if (!l)
         return;

      for (; l; l = l->next)
        {
           Flow_Chart *chart = (Flow_Chart *)evas_list_data(l);

           flow_chart_stop(chart);
           chart->w = w;
           chart->h = h;
           flow_chart_run(chart);
        }
   }

}

void
chart_container_chart_add(Chart_Container *chart_con, Flow_Chart *chart)
{
   chart_con->charts = evas_list_append(chart_con->charts, chart);

   chart->evas = chart_con->evas;
   chart->clip = chart_con->clip;

   chart->x = chart_con->x;
   chart->y = chart_con->y;
   chart->w = chart_con->w;
   chart->h = chart_con->h;

   if (chart->get_value_func)
      chart->is_runable = 1;
}

void
chart_container_stop_charts(Chart_Container *chart_con)
{
   Evas_List *l = chart_con->charts;

   if (l)
      for (; l; l = l->next)
         flow_chart_stop((Flow_Chart *)evas_list_data(l));
}

void
chart_container_run_charts(Chart_Container *chart_con)
{
   Evas_List *l = chart_con->charts;

   if (l)
      for (; l; l = l->next)
         flow_chart_run((Flow_Chart *)evas_list_data(l));
}

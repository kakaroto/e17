#include "defines.h"

#include "document.h"
#include "toolbar.h"
#include "plugin.h"
#include "animations.h"
#include "view.h"
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Evas.h>
#include <Esmart/Esmart_Container.h>
#include <Edje.h>
#include <Efreet.h>
#include <stdlib.h>
#include <stdio.h>

#define API_VERSION_REQ 1

typedef struct _Document_Plugin_Data
{
   Ecore_Hash *files;
}
Document_Plugin_Data;

typedef struct _Document_Page_Resize_Cb_Data
{
   Evas_Object *page;
   Evas_Object *border;
   int top_margin;
   int bottom_margin;
   int left_margin;
   int right_margin;
   Ecore_Timer *render_timer;
}
Document_Page_Resize_Cb_Data;

void
identify(char **name, char **version, char **email)
{
#   if defined PDF
   // TODO: document.c: i18n
   *name = "PDF viewer using epdf";
   *version = "0.1";
   *email = "sevcsik@gmail.com";

#   elif defined DVI

   *name = "DVI viewer using edvi";
   *version = "0.1";
   *email = "sevcsik@gmail.com";

#   elif defined PS
   *name = "PostScript viewer using eps";
   *version = "0.1";
   *email = "sevcsik@gmail.com";

#   endif
}

char
init(void **plugin_data)
{
   if (PLUGIN_API_VERSION != API_VERSION_REQ)
      return PLUGIN_INIT_API_MISMATCH;

   *plugin_data = malloc(sizeof (Document_Plugin_Data));
   ((Document_Plugin_Data *) * plugin_data)->files = ecore_hash_new(NULL, NULL);
   if (!((Document_Plugin_Data *) * plugin_data)->files)
      return PLUGIN_INIT_FAIL;

#   if defined DVI

   edvi_init(300, "cx", 4, 1.0, 1.0, 255, 255, 255, 255, 0, 0, 0);
#   endif

   efreet_init();
   // No need to init epdf
   return PLUGIN_INIT_SUCCESS;
}

void
shutdown(void **plugin_data, Evas *evas)
{
   evas_object_del(evas_object_name_find(evas, "page"));
   evas_object_del(evas_object_name_find(evas, "border"));
   evas_object_del(evas_object_name_find(evas, "tmp_page"));
   evas_object_del(evas_object_name_find(evas, "tmp_border"));

#   if defined DVI
   edvi_shutdown();
#   endif
}

char
open_file(void **_plugin_data, char *filename, Evas_Object *main_window,
          Evas *evas)
{
   Document_Plugin_Data *plugin_data = *_plugin_data;
   Document_File_Data *file_data = NULL;
   Evas_Object *page;
   Evas_Object *border;
   char *themefile;
   int ew, eh, nw, nh;
   double scale;
   int top_margin, bottom_margin, left_margin, right_margin;
   Document_Page_Resize_Cb_Data *resize_cb_data;
   Ecore_List *resize_callbacks;
   Controls_Resize_Cbdata *cbdata;
   char *tmp;

   page = doc_add(evas);
   if (!doc_init(page))
      return 0;

#   if defined DVI

   printf("Trying to open %s with dvi plugin... ", filename);
#   elif defined PDF

   printf("Trying to open %s with pdf plugin... ", filename);
#   endif

#   ifndef PS // FIXME: EPS workaround, need to be removed!
   if (!doc_file_set(page, filename)) // Check that file open was successful
   {
      printf("failed.\n");
      evas_object_del(page);
      return 0; // If not, jump to the next plugin
   }
#   else   
   return 0; // TEMP CODE!!! (disables eps plugin)
#   endif

   printf("ok\n");

   evas_object_name_set(page, "page");

   // Set up border
   edje_object_file_get(main_window, (const char **) & themefile, NULL);
   border = edje_object_add(evas);
   edje_object_file_set(border, themefile, "eyesight/border_opaque");
   edje_object_part_swallow(border, "eyesight/border_opaque/content", page);
   evas_object_name_set(border, "border");

   // Set up callbacks for dragging

   evas_object_event_callback_add(border, EVAS_CALLBACK_MOUSE_MOVE, page_mmove_cb,
                                  NULL);

   // Resize
   evas_object_geometry_get(main_window, NULL, NULL, &ew, &eh);

   doc_size_get(page, &nw, &nh);

   // Scale to fit window
   // TODO: pdf/pdf.c: Add more scaling options
   top_margin = atoi(tmp = edje_file_data_get(themefile, "top_margin"));
   free(tmp);
   bottom_margin = atoi(tmp = edje_file_data_get(themefile, "bottom_margin"));
   free(tmp);
   left_margin = atoi(tmp = edje_file_data_get(themefile, "left_margin"));
   free(tmp);
   right_margin = atoi(tmp = edje_file_data_get(themefile, "right_margin"));
   free(tmp);

   // Add callback to resize callback list
   // TODO: pdf.c: Free resize_cb_data somewhere
   resize_cb_data = malloc(sizeof (Document_Page_Resize_Cb_Data));
   resize_cb_data->page = page;
   resize_cb_data->border = border;
   resize_cb_data->top_margin = top_margin;
   resize_cb_data->bottom_margin = bottom_margin;
   resize_cb_data->left_margin = left_margin;
   resize_cb_data->right_margin = right_margin;
   resize_cb_data->render_timer = NULL;

   cbdata = malloc(sizeof (Controls_Resize_Cbdata));
   cbdata->data = resize_cb_data;
   cbdata->func = page_resize_cb;

   resize_callbacks = evas_object_data_get(evas_object_name_find(evas, "controls"),
                                           "resize_callbacks");

   ecore_list_append(resize_callbacks, cbdata);

   // Resize page

   if ((double) ew - (left_margin + right_margin) / ((double) nw) <
           ((double) eh - (top_margin + bottom_margin) / ((double) nh)))
   {
      scale = (double) (eh - (top_margin + bottom_margin)) / (double) nh;
   }
   else
   {
      scale = (double) (eh - (top_margin + bottom_margin)) / (double) nh;
   }

   doc_scale_set(page, scale);

   doc_page_set(page, 0);
   evas_object_resize(border, nw * scale, nh * scale);

   // Move to left
   evas_object_move(border, ew + left_margin, eh / 2 - nh * scale / 2);

   // Render
   doc_render(page);

   // Save work
   file_data = malloc(sizeof (Document_File_Data));
   file_data->page = page;
   file_data->border = border;
   file_data->tmp_page = NULL;
   file_data->tmp_border = NULL;
   ecore_hash_set(plugin_data->files, filename, file_data);

   return 1;
}

void
show(void **_plugin_data, char *filename, Evas *evas)
{
   Document_Plugin_Data *plugin_data = *_plugin_data;
   Document_File_Data *file_data = NULL;
   Ecore_Hash *hash = NULL;
   Evas_Object *border = NULL;
   Evas_Object *page = NULL;
   Document_Show_Anim_Data *show_anim_data = NULL;

   hash = plugin_data->files;
   file_data = ecore_hash_get(hash, filename);
   border = file_data->border;
   page = file_data->page;
   show_anim_data = malloc(sizeof (Document_Show_Anim_Data));
   show_anim_data->evas = evas;
   show_anim_data->object = border;

   evas_object_geometry_get(border, &(show_anim_data->start_x), NULL, NULL, NULL);
   evas_object_geometry_get(evas_object_name_find(evas, "main_window"),
                            NULL, NULL, &(show_anim_data->ew), NULL);

   setup_toolbar(evas_object_name_find(evas, "controls"));

   evas_object_show(border);
   evas_object_show(page);
   ecore_animator_add(show_anim, show_anim_data);

   evas_object_event_callback_add(evas_object_name_find(evas, "controls"),
                                  EVAS_CALLBACK_MOUSE_WHEEL, mousewheel_cb, NULL);
}

void
mousewheel_cb(void *_data, Evas *evas, Evas_Object *controls, void *event_info)
{
   printf("wheeeeeeeeeel\n");
   Evas_Event_Mouse_Wheel *event = event_info;
   if (event->z > 0)
   {
      page_next_clicked(NULL, controls, NULL, NULL);
   }
   else if (event->z < 0)
   {
      page_prev_clicked(NULL, controls, NULL, NULL);
   }
}

void
page_resize_cb(void *_data, Evas *evas, Evas_Object *controls, void *event_info)
{
   Document_Page_Resize_Cb_Data *data = _data;
   int ew, eh, nw, nh, h_margins, v_margins;
   double scale;
   Evas_Object *page, *border;
   Document_View_Mode *mode;

   page = evas_object_name_find(evas, "page");
   border = evas_object_name_find(evas, "border");
   mode = ecore_evas_data_get(ecore_evas_ecore_evas_get(evas), "viewmode");

   h_margins = data->left_margin + data->right_margin;
   v_margins = data->top_margin + data->bottom_margin;

   evas_object_geometry_get(controls, NULL, NULL, &ew, &eh);
   doc_size_get(page, &nw, &nh);

   if (mode)
   {
      switch (*mode)
      {
      case VIEW_ORIGINAL:
         scale = 1.0;
         break;
      case VIEW_FIT:
         scale = (((double) eh - v_margins) / (double) nh <              \
                 ((double) ew - h_margins) / (double) nw) ?              \
                 (((double) eh - v_margins) / (double) nh) :             \
                 (((double) ew - h_margins) / (double) nw);
         break;
      case VIEW_HFIT:
         scale = ((double) ew - h_margins) / (double) nw;
         break;
      case VIEW_VFIT:
         scale = ((double) eh - v_margins) / (double) nh;
         break;
      case VIEW_CUSTOM:
         scale = *(double *) ecore_evas_data_get(ecore_evas_ecore_evas_get(evas),
                                                 "scale");
         break;
      default: // Just to shut up compiler
         scale = 0.0;
         break;
      }
   }
   else
   {
      printf("Using default view mode\n");
      scale = (((double) eh - v_margins) / (double) nh <              \
              ((double) ew - h_margins) / (double) nw) ?              \
              (((double) eh - v_margins) / (double) nh) :             \
              (((double) ew - h_margins) / (double) nw);
   }

   evas_object_resize(border, nw * scale, nh * scale);
   evas_object_move(border, ew / 2 - nw * scale / 2, eh / 2 - nh * scale / 2);

   doc_scale_set(page, scale);

   printf("Scale: %f, Native w: %d, h: %d\n", scale, nw, nh);

   if (data->render_timer) // We're still resizing, delete timer set on prev resize
      ecore_timer_del(data->render_timer);

   data->render_timer = ecore_timer_add(0.1, page_resize_cb_render_timer,
                                        page);
}

int
page_resize_cb_render_timer(void *data)
{
   doc_render((Evas_Object *) data);
   return 0;
}

void
page_mmove_cb(void *data, Evas *e, Evas_Object *border, void *event_)
{
   Evas_Event_Mouse_Move *event = event_;
   int cx, cy, bx, by;
   cx = cy = bx = by = 0;
   if (event->buttons == 1)
   {
      evas_pointer_canvas_xy_get(e, &cx, &cy);
      evas_object_geometry_get(border, &bx, &by, NULL, NULL);

      evas_object_move(border, cx - (bx - cx), cy - (by - cy)); 
      // TODO: fix flickering!
   }
}

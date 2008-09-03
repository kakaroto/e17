#include "view.h"
#include "defines.h"

#include <Evas.h>
#include <Ecore_Evas.h>
#include <stdlib.h>
#include <stdio.h>

void
zoom_original_clicked_cb(void *data, Evas_Object *obj,
                         const char *emission, const char *src)
{
   Ecore_Evas *ee;
   Document_View_Mode *mode;
   int w, h;

   ee = ecore_evas_ecore_evas_get(evas_object_evas_get(obj));

   mode = ecore_evas_data_get(ee, "viewmode");
   if (!mode)
   {
      mode = malloc(sizeof (Document_View_Mode));
      ecore_evas_data_set(ee, "viewmode", mode);
   }

   *mode = VIEW_ORIGINAL;

   // Resize window to call page_resize_cb
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   ecore_evas_resize(ee, w + 1, h + 1);
   ecore_evas_resize(ee, w, h);
}

void
zoom_fit_clicked_cb(void *data, Evas_Object *obj,
                    const char *emission, const char *src)
{
   Ecore_Evas *ee;
   Document_View_Mode *mode;
   int w, h;

   ee = ecore_evas_ecore_evas_get(evas_object_evas_get(obj));

   mode = ecore_evas_data_get(ee, "viewmode");
   if (!mode)
   {
      mode = malloc(sizeof (Document_View_Mode));
      ecore_evas_data_set(ee, "viewmode", mode);
   }

   *mode = VIEW_FIT;

   // Resize window to call page_resize_cb
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   ecore_evas_resize(ee, w + 1, h + 1);
   ecore_evas_resize(ee, w, h);
}

void
zoom_vfit_clicked_cb(void *data, Evas_Object *obj,
                     const char *emission, const char *src)
{
   Ecore_Evas *ee;
   Document_View_Mode *mode;
   int w, h;

   ee = ecore_evas_ecore_evas_get(evas_object_evas_get(obj));

   mode = ecore_evas_data_get(ee, "viewmode");
   if (!mode)
   {
      mode = malloc(sizeof (Document_View_Mode));
      ecore_evas_data_set(ee, "viewmode", mode);
   }

   *mode = VIEW_VFIT;

   // Resize window to call page_resize_cb
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   ecore_evas_resize(ee, w + 1, h + 1);
   ecore_evas_resize(ee, w, h);
}

void
zoom_hfit_clicked_cb(void *data, Evas_Object *obj,
                     const char *emission, const char *src)
{
   Ecore_Evas *ee;
   Document_View_Mode *mode;
   int w, h;

   ee = ecore_evas_ecore_evas_get(evas_object_evas_get(obj));

   mode = ecore_evas_data_get(ee, "viewmode");
   if (!mode)
   {
      mode = malloc(sizeof (Document_View_Mode));
      ecore_evas_data_set(ee, "viewmode", mode);
   }

   *mode = VIEW_HFIT;

   // Resize window to call page_resize_cb
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   ecore_evas_resize(ee, w + 1, h + 1);
   ecore_evas_resize(ee, w, h);
}

void
zoom_in_clicked_cb(void *data, Evas_Object *obj,
                   const char *emission, const char *src)
{
   Ecore_Evas *ee;
   Document_View_Mode *mode;
   double *scale;
   int w, h;

   ee = ecore_evas_ecore_evas_get(evas_object_evas_get(obj));

   mode = ecore_evas_data_get(ee, "viewmode");
   if (!mode)
   {
      mode = malloc(sizeof (Document_View_Mode));
      ecore_evas_data_set(ee, "viewmode", mode);
   }

   *mode = VIEW_CUSTOM;

   scale = ecore_evas_data_get(ee, "scale");
   if (!scale)
   {
      scale = malloc(sizeof (double));
      ecore_evas_data_set(ee, "scale", scale);
      *scale = doc_scale_get(evas_object_name_find(evas_object_evas_get(obj),
                                                   "page"));
   }

   *scale += 0.05;

   // Resize window to call page_resize_cb
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   ecore_evas_resize(ee, w + 1, h + 1);
   ecore_evas_resize(ee, w, h);
}

void
zoom_out_clicked_cb(void *data, Evas_Object *obj,
                    const char *emission, const char *src)
{
   Ecore_Evas *ee;
   Document_View_Mode *mode;
   double *scale;
   int w, h;

   ee = ecore_evas_ecore_evas_get(evas_object_evas_get(obj));

   mode = ecore_evas_data_get(ee, "viewmode");
   if (!mode)
   {
      mode = malloc(sizeof (Document_View_Mode));
      ecore_evas_data_set(ee, "viewmode", mode);
   }

   *mode = VIEW_CUSTOM;

   scale = ecore_evas_data_get(ee, "scale");
   if (!scale)
   {
      scale = malloc(sizeof (double));
      ecore_evas_data_set(ee, "scale", scale);
      *scale = doc_scale_get(evas_object_name_find(evas_object_evas_get(obj),
                                                   "page"));
   }

   *scale -= 0.05;

   // Resize window to call page_resize_cb
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   ecore_evas_resize(ee, w + 1, h + 1);
   ecore_evas_resize(ee, w, h);
}



#ifndef ESMART_CAIRO_H
#define ESMART_CAIRO_H

#include <Evas.h>
#include <cairo.h>

#ifdef __cplusplus
extern "C" {
#endif

EAPI Evas_Object     *esmart_image_cairo_new              (Evas *evas, Evas_Coord w, Evas_Coord h, Evas_Bool alpha);
EAPI Evas_Object     *esmart_image_cairo_new_from_surface (Evas *evas, cairo_surface_t *cairo_surface);
EAPI cairo_surface_t *esmart_image_cairo_surface_get      (Evas_Object *object);
EAPI Evas_Bool        esmart_image_cairo_surface_set      (Evas_Object *o, cairo_surface_t *cairo_surface);
EAPI void             esmart_image_cairo_fill_auto_set    (Evas_Object *o, Evas_Bool enable);

#ifdef __cplusplus
}
#endif
#endif

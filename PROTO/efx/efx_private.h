#ifndef EFX_PRIVATE_H
#define EFX_PRIVATE_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <math.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include "Efx.h"

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

#define DBG(...)            EINA_LOG_DOM_DBG(_efx_log_dom, __VA_ARGS__)
#define INF(...)            EINA_LOG_DOM_INFO(_efx_log_dom, __VA_ARGS__)
#define WRN(...)            EINA_LOG_DOM_WARN(_efx_log_dom, __VA_ARGS__)
#define ERR(...)            EINA_LOG_DOM_ERR(_efx_log_dom, __VA_ARGS__)
#define CRI(...)            EINA_LOG_DOM_CRIT(_efx_log_dom, __VA_ARGS__)

static const char *efx_speed_str[] =
{
   "LINEAR", "ACCELERATE", "DECELERATE", "SINUSOIDAL"
};

extern int _efx_log_dom;
extern Eina_Hash *_efx_object_manager;

typedef struct EFX
{
   Evas_Object *obj;
   void *spin_data;
   void *rotate_data;
   void *zoom_data;
   double current_rotate;
   double current_zoom;
} EFX;

void _efx_zoom_calc(void *, Evas_Map *map);
void _efx_rotate_calc(void *, Evas_Map *map);
void _efx_spin_calc(void *, Evas_Map *map);


EFX *efx_new(Evas_Object *obj);

#endif

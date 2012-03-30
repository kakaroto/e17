#ifndef EFX_H
#define EFX_H

#include <Evas.h>
#include <Ecore.h>

#ifdef EAPI
# undef EAPI
#endif /* ifdef EAPI */

#ifdef _WIN32
# ifdef EFL_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else /* ifdef DLL_EXPORT */
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else /* ifdef EFL_BUILD */
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_BUILD */
#else /* ifdef _WIN32 */
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else /* if __GNUC__ >= 4 */
#   define EAPI
#  endif /* if __GNUC__ >= 4 */
# else /* ifdef __GNUC__ */
#  define EAPI
# endif /* ifdef __GNUC__ */
#endif /* ! _WIN32 */

typedef void (*Efx_End_Cb)(void *data, double value, Evas_Object *obj);

typedef enum Efx_Effect_Speed
{
   EFX_EFFECT_SPEED_LINEAR = ECORE_POS_MAP_LINEAR,
   EFX_EFFECT_SPEED_ACCELERATE = ECORE_POS_MAP_ACCELERATE,
   EFX_EFFECT_SPEED_DECELERATE = ECORE_POS_MAP_DECELERATE,
   EFX_EFFECT_SPEED_SINUSOIDAL = ECORE_POS_MAP_SINUSOIDAL
} Efx_Effect_Speed;

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

EAPI int efx_init(void);
EAPI void efx_shutdown(void);

EAPI Eina_Bool efx_rotate(Evas_Object *obj, Efx_Effect_Speed speed, double degrees, double total_time, Efx_End_Cb cb, const void *data);
void efx_rotate_reset(Evas_Object *obj);
void efx_rotate_stop(Evas_Object *obj);

EAPI Eina_Bool efx_spin_start(Evas_Object *obj, long dps);
EAPI void efx_spin_reset(Evas_Object *obj);
EAPI void efx_spin_stop(Evas_Object *obj);

Eina_Bool efx_zoom(Evas_Object *obj, Efx_Effect_Speed speed, double starting_zoom, double ending_zoom, Evas_Point *zoom_point, double total_time, Efx_End_Cb cb, const void *data);
void efx_zoom_reset(Evas_Object *obj);
void efx_zoom_stop(Evas_Object *obj);
#ifdef __cplusplus
}
#endif

#endif

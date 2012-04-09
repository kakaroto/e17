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

typedef struct Efx_Map_Data Efx_Map_Data;
/**
 * @typedef Efx_End_Cb
 * This is the callback type used to notify a user about the end of an effect.
 * It is called instantly upon an effect terminating, but only if the effect has run
 * to its full duration. Ending callbacks are NOT called upon stopping/resetting an effect.
 * @param data The data passed when starting the effect
 * @param e The current map data for an object
 * @param obj The object
 */
typedef void (*Efx_End_Cb)(void *data, Efx_Map_Data *e, Evas_Object *obj);

/**
 * @struct Efx_Map_Data
 *
 * This struct is provided to callbacks upon the completion of certain types of effects.
 * It contains information about the current position of an object.
 */
struct Efx_Map_Data
{
   double rotation; /**< The current rotation (in degrees) of the object */
   Evas_Point *rotate_center; /**< The current rotation center for the object */
   double zoom; /**< The current zoom amount of an object */
   Evas_Point *zoom_center; /**< The current zoom center for the object */
};

/**
 * @typedef Efx_Effect_Speed
 *
 * These values are used to set the speed at which an effect will occur.
 * More information can be found by reading about Ecore_Animator objects.
 */
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

/**
 * Initialize the library
 *
 * This function must be called before any other efx functions.
 * @return The number of times the library has been initialized, or 0 on failure
 */
EAPI int efx_init(void);
/**
 * Uninitialize the library
 *
 * This function unregisters the log domain and performs other cleanup routines
 */
EAPI void efx_shutdown(void);

/**
 * Cause one object to mimic the actions of another
 *
 * Using this function, @p follower will copy every effect set on @p obj
 * until @ref efx_unfollow() is called on @p follower. Note that passing
 * a "follower" object as @p obj will cause the object passed as @p follower
 * to be chained to the "owner" object.
 * @param obj The object to follow the actions of
 * @param follower The object to do the following
 * @return EINA_TRUE on success, else EINA_FALSE
 */
EAPI Eina_Bool efx_follow(Evas_Object *obj, Evas_Object *follower);
/**
 * Unchain a following object from its owner
 *
 * This function will cause @p obj to stop following its owner object.
 * It takes effect immediately and cannot fail.
 * @param obj The follower object
 */
EAPI void efx_unfollow(Evas_Object *obj);

/**
 * Rotate an object
 *
 * This function allows rotation of an object around an optional point with
 * specified rotation amount, speed of effect, and duration of effect.
 * @note @p cb will ONLY be called upon successful completion of the effect.
 * @note The actual location of the object will not change; this is a map effect
 * @param obj The object to rotate
 * @param speed The speed to rotate at
 * @param degress The amount to rotate
 * @param center The optional point to rotate around
 * @param total_time The time that the effect should occur over
 * @param cb The optional callback to call when the effect completes
 * @param data Optional data to pass to @p cb
 * @return EINA_TRUE on successful queue of the animation, else EINA_FALSE
 */
EAPI Eina_Bool efx_rotate(Evas_Object *obj, Efx_Effect_Speed speed, double degrees, const Evas_Point *center, double total_time, Efx_End_Cb cb, const void *data);
/**
 * Stop rotation of an object and remove the map
 *
 * Use this function to immediately stop the rotation of an object and restore its original position
 * @param obj An object
 */
EAPI void efx_rotate_reset(Evas_Object *obj);
/**
 * Stop rotation of an object
 *
 * Use this function to immediately stop the rotation of an object without restoring its original position
 * @param obj An object
 */
EAPI void efx_rotate_stop(Evas_Object *obj);

/**
 * Spin an object
 *
 * This function allows rotation of an object around an optional point. The object
 * will rotate at @p dps degrees per second until manually stopped.
 * @note The actual location of the object will not change; this is a map effect
 * @param obj The object to rotate
 * @param dps The degrees per second to rotate
 * @param center The optional point to rotate around
 * @return EINA_TRUE on successful queue of the animation, else EINA_FALSE
 */
EAPI Eina_Bool efx_spin_start(Evas_Object *obj, long dps, const Evas_Point *center);
/**
 * Stop rotation of an object and remove the map
 *
 * Use this function to immediately stop the rotation of an object and restore its original position
 * @param obj An object
 */
EAPI void efx_spin_reset(Evas_Object *obj);
/**
 * Stop rotation of an object
 *
 * Use this function to immediately stop the rotation of an object without restoring its original position
 * @param obj An object
 */
EAPI void efx_spin_stop(Evas_Object *obj);

/**
 * Zoom an object
 *
 * This function allows zooming of an object at an optional point with
 * specified zoom amount, starting amount, speed of effect, and time to complete effect.
 * @note @p cb will ONLY be called upon successful completion of the effect.
 * @note The actual location of the object will not change; this is a map effect
 * @param obj The object to zoom
 * @param speed The speed to zoom at
 * @param starting_zoom The zoom amount to start at, or 0 to use the previously existing zoom amount (defaults to 1.0)
 * @param ending_zoom The zoom amount to end at
 * @param zoom_point The optional point to center the zoom on
 * @param total_time The time that the effect should occur over
 * @param cb The optional callback to call when the effect completes
 * @param data Optional data to pass to @p cb
 * @return EINA_TRUE on successful queue of the animation, else EINA_FALSE
 */
EAPI Eina_Bool efx_zoom(Evas_Object *obj, Efx_Effect_Speed speed, double starting_zoom, double ending_zoom, Evas_Point *zoom_point, double total_time, Efx_End_Cb cb, const void *data);
/**
 * Stop zooming of an object and remove the map
 *
 * Use this function to immediately stop the zoom of an object and restore its original position
 * @param obj An object
 */
EAPI void efx_zoom_reset(Evas_Object *obj);
/**
 * Stop zooming of an object
 *
 * Use this function to immediately stop the zoom of an object without restoring its original position
 * @param obj An object
 */
EAPI void efx_zoom_stop(Evas_Object *obj);

/**
 * Move an object
 *
 * This function animates the movement of an object to a specified point using
 * a designated speed of effect and time to complete the effect.
 * @note @p cb will ONLY be called upon successful completion of the effect.
 * @note The actual location of the object WILL change; this is NOT a map effect
 * @param obj The object to zoom
 * @param speed The speed to move at
 * @param end_point The point to move to
 * @param total_time The time that the effect should occur over
 * @param cb The optional callback to call when the effect completes
 * @param data Optional data to pass to @p cb
 * @return EINA_TRUE on successful queue of the animation, else EINA_FALSE
 */
EAPI Eina_Bool efx_move(Evas_Object *obj, Efx_Effect_Speed speed, Evas_Point *end_point, double total_time, Efx_End_Cb cb, const void *data);

/**
 * Attempt to automatically move+resize an object according to its map
 *
 * This function attempts to move an object to its mapped position and resize it according
 * to its mapped zoom. It will, after calculating, reset the zoom and rotat effects on the object,
 * preserving only its orientation. Ideally, you will not notice any visible change after running this function,
 * but it should be used carefully, as successive effects on a realized object will likely not behave as intended.
 * @param obj The object on which to realize effects
 */
EAPI void efx_realize(Evas_Object *obj);
#ifdef __cplusplus
}
#endif

#endif

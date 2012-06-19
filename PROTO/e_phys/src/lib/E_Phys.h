#ifndef E_PHYS_H
#define E_PHYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Evas.h>
#include <Ecore.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EPHYS_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EVAS_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */


/**
 * Type definition of the point
 */
typedef struct E_Phys_Point E_Phys_Point;

/**
 * The structur for the point
 */
struct E_Phys_Point
{
  float x, y;
};

/**
 * Type definition of the vector
 */
typedef E_Phys_Point E_Phys_Vector;

#include <e_phys_world.h>
#include <e_phys_particle.h>
#include <e_phys_force.h>
#include <e_phys_constraint.h>

#ifdef __cplusplus
}
#endif
#endif

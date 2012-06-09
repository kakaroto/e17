#ifndef  EWEATHER_SMART_H
#define  EWEATHER_SMART_H

#include <Evas.h>
#include <EWeather.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EWEATHER_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EINA_BUILD */
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
#endif

typedef  enum EWeather_Object_Mode EWeather_Object_Mode;

enum EWeather_Object_Mode
{
   EWEATHER_OBJECT_MODE_FULLSCREEN,
   EWEATHER_OBJECT_MODE_EXPOSE
};


EAPI	Evas_Object *	eweather_object_add(Evas *evas);
EAPI	const char *	eweather_object_signal_type_get(EWeather_Type type);
EAPI	EWeather *	eweather_object_eweather_get(Evas_Object *obj);
EAPI	void		eweather_object_mode_set(Evas_Object *obj, EWeather_Object_Mode mode);
EAPI    void            eweather_object_temp_format_set(Evas_Object *obj, EWeather_Temp type, const char *theme);
EAPI    void            eweather_theme_set(Evas_Object *obj, const char *theme);

#endif

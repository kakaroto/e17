// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#ifndef  EWEATHER_SMART_INC
#define  EWEATHER_SMART_INC

#include "EWeather.h"
#include "Evas.h"
#include "Edje.h"
#include <stdio.h>

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

#endif

// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#ifndef  EWEATHER_SMART_INC
#define  EWEATHER_SMART_INC

#include "EWeather.h"
#include "Evas.h"
#include "Edje.h"
#include <stdio.h>

EAPI	Evas_Object *	eweather_object_add(Evas *evas);
EAPI	const char *	eweather_object_signal_type_get(EWeather_Type type);
#endif

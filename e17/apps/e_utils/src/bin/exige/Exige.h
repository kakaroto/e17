#ifndef EXIGE_H
#define EXIGE_H

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Esmart/Esmart_Text_Entry.h>
#include <Edje.h>
#include <Ecore_Config.h>
#include "config.h"

typedef struct _Exige Exige;

struct _Exige {
     Evas *evas;
     Ecore_Evas *ee;
     Evas_Object *gui;
     Evas_Object *txt;
};

#endif

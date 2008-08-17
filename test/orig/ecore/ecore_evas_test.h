#ifndef _ECORE_EVAS_TEST_H
#define _ECORE_EVAS_TEST_H

#include "Ecore.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ecore_Evas.h"

#define IM "data/images/"
#define PT "data/pointers/"
#define FN "data/fonts/"

extern double       start_time;
extern Ecore_Evas  *ee;
extern Evas        *evas;



void calibrate_pos_set(int pos);
int  calibrate_pos_get(void);
void calibrate_finish(void);
void calibrate_start(void);

void bg_resize(double w, double h);
void bg_start(void);
void bg_go(void);
    
int  app_start(int argc, const char **argv);
void app_finish(void);

#endif

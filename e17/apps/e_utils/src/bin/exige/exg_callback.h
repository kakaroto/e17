#ifndef CB_H
#define CB_H

#include "Exige.h"

void key_cb(void *data, Evas *e, Evas_Object *o, void *ei);

void _enter_cb(void *data, const char *str);

void _exg_quit(void *data , Evas_Object *obj, 
	       const char *emission, 
	       const char *src);

void _run_cb(void *data , Evas_Object *obj, 
	     const char *emission, 
	     const char *src);

void  exg_run(Exige *exg, const char *str);

#endif

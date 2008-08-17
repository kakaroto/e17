#ifndef _ETK__TYPES_H___
#define _ETK__TYPES_H___

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#define NEED_newRV_noinc
#include "../ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>

void __etk_perl_init();
void FreeEtkObject(Etk_Object *object);
void FreeObjectCache();
char * getClass(char *type);


void * SvObj(SV *o, char * classname);
SV * newSVObj(void *object, char * classname);
SV * newSVObject(void * object);

SV *			newSVColor(Etk_Color col);
Etk_Color 		SvColor(SV *color);
SV *			newSVGeometry(Etk_Geometry geo);
Etk_Geometry 		SvGeometry(SV * geo);
SV *			newSVGeometryPtr(Etk_Geometry * geo);
Etk_Geometry *		SvGeometryPtr(SV * geo);
SV *			newSVSizePtr(Etk_Size * size);
Etk_Size *		SvSizePtr(SV * size);

Evas_List *		SvEvasList(SV *sv);
SV *			newSVEvasList(Evas_List *list);
SV *			newSVCharEvasList(Evas_List *list);

SV *			newSVEventKeyUp(Etk_Event_Key_Up *ev);
SV *			newSVEventKeyDown(Etk_Event_Key_Down *ev);
SV *			newSVEventMouseWheel(Etk_Event_Mouse_Wheel *ev);
SV *			newSVEventMouseMove(Etk_Event_Mouse_Move *ev);
SV *			newSVEventMouseUp(Etk_Event_Mouse_Up *ev);
SV *			newSVEventMouseDown(Etk_Event_Mouse_Down *ev);
SV *			newSVEventMouseIn(Etk_Event_Mouse_In *ev);
SV *			newSVEventMouseOut(Etk_Event_Mouse_Out *ev);

#endif



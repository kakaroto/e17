#ifndef ELATION_H
#define ELATION_H

#include <Evas.h>
#include <Ecore.h>
#ifndef FB_ONLY
#include <Ecore_X.h>
#else
#include <Ecore_Fb.h>
#endif
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Emotion.h>

#include "config.h"

typedef struct _Elation_Info   Elation_Info;
typedef struct _Elation_Module Elation_Module;

struct _Elation_Info
{
   Evas *evas;
};

struct _Elation_Module
{
   /* methods */
   void (*shutdown) (Elation_Module *em);
   void (*resize) (Elation_Module *em);
   void (*show) (Elation_Module *em);
   void (*hide) (Elation_Module *em);
   void (*focus) (Elation_Module *em);
   void (*unfocus) (Elation_Module *em);
   void (*action) (Elation_Module *em, int action);
   
   void *handle; /* lib handle */
   Elation_Info *info; /* app info */
   Elation_Module *parent; /* parent module */
   Evas_List *children; /* modules loded by this one */
   void *data; /* private modules data */
   
   unsigned char focused : 1;
};

enum {
   ELATION_ACT_NONE,
     ELATION_ACT_NEXT,
     ELATION_ACT_PREV,
     ELATION_ACT_SELECT,
     ELATION_ACT_EXIT,
     ELATION_ACT_UP,
     ELATION_ACT_DOWN,
     ELATION_ACT_LEFT,
     ELATION_ACT_RIGHT,
     ELATION_ACT_MENU,
     ELATION_ACT_INFO,
     ELATION_ACT_INPUT,
     ELATION_ACT_PLAY,
     ELATION_ACT_PAUSE,
     ELATION_ACT_STOP,
     ELATION_ACT_REC,
     ELATION_ACT_SKIP
};

Elation_Module *elation_module_open(Elation_Info *info, Elation_Module *parent, char *name);
void            elation_module_close(Elation_Module *em);

#endif

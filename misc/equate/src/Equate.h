
#define DEBUG 1

#define E(lvl,fmt,args...) \
 do {\
   if(DEBUG>=(lvl)) \
     fprintf(stderr,fmt, args); \
 } while(0)

#include "calc.h"

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <Ewl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>


#include "config.h"

#define BUFLEN 100

/* structures */
typedef struct {
   Ecore_Evas     *ee;
   Evas_Object    *edje;
} Gui;

typedef enum {
   DEFAULT,
   BASIC,
   SCI,
   EDJE
} Mode;

typedef struct {
   Mode            mode;
   char           *theme;
} Conf;

typedef struct {
   Gui             gui;
   Conf            conf;
} Equate;

void            equate_init(Equate * equate);
void            equate_quit();

#include "gui.h"

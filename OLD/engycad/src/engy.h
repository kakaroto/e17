
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Edb.h>
#include <Evas.h>
#include <Imlib2.h>

#include "../config.h"

#include "engytypes.h"
//#include "window.h"
#include "cl.h"
//#include "edjestuff.h"

#include "drawing.h"
#include "shell.h"
#include "misc.h"
#include "trans.h"
#include "graserv.h"
#include "serv.h"
#include "mouse.h"
#include "magnet.h"
#include "globals.h"

#include "layer.h"
#include "line.h"
#include "linestyle.h"
#include "rectangle.h"
#include "point.h"
#include "circle.h"
#include "arc.h"
#include "ellipse.h"
#include "earc.h"
#include "image.h"
#include "text.h"
#include "dim.h"
#include "dimstyle.h"
#include "textstyle.h"
#include "hatchstyle.h"
#include "log.h"
#include "hatch.h"
#include "sel.h"
#include "common.h"
#include "panel.h"
#include "menu.h"
#include "undo.h"

#define TITLE "Engy 0.3.1"

#define _(a) (a)
#define DUP(a) (char*)strdup(a)
#define FREE(a) free(a)
#define IF_FREE(a) if(a) FREE(a);

#define ENGY_ASSERT(a) 

#define _ENGY_ASSERT(a) if(!(a)){fprintf(stderr,\
"Assertion failed in %s, line %d. Program may fall, probably due \
to lack of memory or corrupped resource file. Bug report is welcome.\n"\
,__FILE__,__LINE__); E_DB_FLUSH;}

#define ENGY_ASSERTS(a,b) if(!(a)){fprintf(stderr,"Assertion failed in %s, line %d -- %s\n",__FILE__,__LINE__,(b));E_DB_FLUSH;}

#define LOG_AND_RETURN(a) { if (0 == (a)) return (a);\
	   if (!error_line) error_line = __LINE__; \
	if (!error_file) error_file = __FILE__; return (a);}

#define ERR_NO_ERROR 0
#define ERR_IN_VAL 1
#define ERR_MALLOC 2
#define ERR_MAX_SHELL 3
#define ERR_NO_SHELL 4
#define ERR_NO_SUCH_PATH 5
#define ERR_READ_ONLY 6
#define ERR_CORRUPTED_DATA 7
#define ERR_IO 8
#define ERR_EFL 9

#define ALPHA1 50
#define ALPHA2 120
#define ALPHA3 200
#define ALPHA4 240
#define ALPHA5 255



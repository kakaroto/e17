#include <Esmart/Esmart_Trans.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>

#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

#define DEBUG_LEVEL 5
#define PATH_MAX    255	

#define DEBUG_FUCKED 	1	/* Your Fucked */
#define DEBUG_ERROR	2	/* Recoverable Error */
#define DEBUG_WARN	3	/* Simple Warning */
#define DEBUG_INFO	4	/* Information Only */

typedef struct {
	int	 	sticky;
	int 		lower;
	char		theme[PATH_MAX];
	int		debug;
	char 		*format;
} Config;

typedef struct {
	Config 		conf;
	Evas		*evas;
	Ecore_Evas 	*ee;
	Ecore_Timer	*timer;
	Evas_Object	*edje;
	Evas_Object	*smart;
} Elapse;

/* gui.c */
void elapse_gui_init(Elapse *elapse);
void elapse_cb_window_move(Ecore_Evas *ee);

/* main.c */
void show_help(void);

/* util.c */
void debug(int level, Elapse *elapse, char *message);

/* time.c */
int elapse_time_set(Elapse *elapse);

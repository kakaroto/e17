/** @file enna.h */

#ifndef ENNA_H_
#define ENNA_H_

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Ecore_Txt.h>
#include <Ecore_Str.h>
#include <Ecore_X.h>
#include <Edje.h>
#include <Ecore_Evas.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnmatch.h>
#include <unistd.h>
#include <pthread.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <ctype.h>

#include "enna_inc.h"

//#define ENNA_DEBUG0
#define ENNA_DEBUG 2
//#define ENNA_DEBUG2

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define ARRAY_NB_ELEMENTS(array) (sizeof (array) / sizeof (array[0]))
#define ENNA_FREE(p) { if (p) {free(p); p = NULL;} }
#define ENNA_NEW(s, n) (s *)calloc(n, sizeof(s))
#define ENNA_FREE_LIST(list, free)			\
  do							\
    {							\
       if (list)					\
	 {						\
	    Evas_List *tmp;				\
	    tmp = list;					\
	    list = NULL;				\
	    while (tmp)					\
	      {						\
		 free(tmp->data);			\
		 tmp = evas_list_remove_list(tmp, tmp); \
	      }						\
	 }						\
    }							\
  while (0)

#define ENNA_TIMER_DEL(timer)                           \
    if (timer)                                          \
    {                                                   \
        ecore_timer_del(timer);                         \
        timer = NULL;                                   \
    }                                                   \

#define ENNA_OBJECT_DEL(obj)                            \
    if (obj)                                            \
        evas_object_del(obj);                           \
        

typedef enum
{
    ENNA_MSG_NONE, /* no error messages */
    ENNA_MSG_EVENT, /* notify each incoming event */
    ENNA_MSG_INFO, /* working operations */
    ENNA_MSG_WARNING, /* harmless failures */
    ENNA_MSG_ERROR, /* may result in hazardous behavior */
    ENNA_MSG_CRITICAL, /* prevents lib from working */
} enna_msg_level_t;

/**
 * @struct Enna
 * @brief Main Enna struct, includes all stuct and vars
 */

typedef struct _Enna Enna;

struct _Enna
{
    char *home; /**< Home directory ie $HOME/.enna. */
    Ecore_Evas *ee; /**< Ecore_Evas. */
    Ecore_X_Window ee_winid; /**< Ecore_Evas WindowID */
    Evas *evas; /**< Main enna evas.  */
    Evas_Object *o_background;/**< Background object, it handles key down. */
    Evas_Object *o_edje; /**< Main edje. */
    Evas_Object *o_mainmenu; /**< Top menu. */
    Evas_Object *o_content; /** Edje Object to swallow content */
    enna_msg_level_t lvl; /**< Error message level */
    int use_covers;
    int use_snapshots;
};

extern Enna *enna;

#endif

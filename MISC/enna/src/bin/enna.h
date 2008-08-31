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



#if ENNA_DEBUG == 2
# define dbg(fmt,arg...) fprintf(stderr, "[Enna] [%s/%d] " fmt,__FILE__,__LINE__,##arg)
#elif ENNA_DEBUG == 1
# define dbg(fmt,arg...) fprintf(stderr, "[Enna] " fmt,##arg)
#else
# define dbg(fmt,arg...) do { } while(0)
#endif

#if ENNA_DEBUG == 2
# define dbg_thm(fmt,arg...) fprintf(stderr, "[Enna Edje] [%s/%d] " fmt,__FILE__,__LINE__,##arg)
#elif ENNA_DEBUG == 1
# define dbg_thm(fmt,arg...) fprintf(stderr, "[Enna Edje] " fmt,##arg)
#else
# define dbg_thm(fmt,arg...) do { } while(0)
#endif

#define NBR_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))
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


/**
 * @struct Enna
 * @brief Main Enna struct, includes all stuct and vars
 */

typedef struct _Enna Enna;

struct _Enna
{
   char               *home;        /**< Home directory ie $HOME/.enna. */
   Ecore_Evas         *ee;          /**< Ecore_Evas. */
   Ecore_X_Window      ee_winid;    /**< Ecore_Evas WindowID */
   Evas               *evas;        /**< Main enna evas.  */
   Evas_Object        *o_background;/**< Background object, it handles key down. */
   Evas_Object        *o_edje;      /**< Main edje. */
   Evas_Object        *o_mainmenu;  /**< Top menu. */
   Evas_Object       *o_content;    /** Edje Object to swallow content */

};

extern Enna *enna;

#endif

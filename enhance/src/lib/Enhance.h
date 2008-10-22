/* Enhance
 * Copyright (C) 2006-2008 Hisham Mardam-Bey, Samuel Mendes, Ugo Riboni, Simon Treny
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ENHANCE_H
#define _ENHANCE_H

#include <Etk.h>
#include <EXML.h>

/**
 * @file Enhance.h
 * @brief Contains initialization and GUI loading and manipulation functions
 */

#ifdef __cplusplus
extern "C" {
#endif   

#undef ENHANCE_MEM_DEBUG

/* The following macros are for DEBUGGING ONLY and may break your app! */
#ifdef ENHANCE_MEM_DEBUG

extern long int mem_size;
extern long int mem_total;
extern long int mem_strdup;
extern long int mem_calloc;   
extern Evas_Hash *mem_objects;
extern void *mem_alloc(size_t count, size_t size, char *file, int line);
extern char *strdup2(const char *str, char *file, int line);
   
#undef E_NEW   
#define E_NEW(count, type) \
   mem_alloc(count, sizeof(type), __FILE__, __LINE__);   
   
#undef E_FREE   
#define E_FREE(ptr) \
     if(ptr) \
	 { \
	    long int size = 0; \
	    char *ptrstr; \
	    \
	    ptrstr = calloc(64, sizeof(char)); \
	    snprintf(ptrstr, 64 * sizeof(char), "%p", ptr); \
	    size = (long int)evas_hash_find(mem_objects, ptrstr); \
	    if(size > 0) \
	      { \
		 mem_size -= size; \
		 mem_objects = evas_hash_del(mem_objects, ptrstr, (void*) size); \
		 printf(__FILE__ " %d  : (free) %ld bytes, total = %ld\n", __LINE__, (long int)size, mem_size); \
	      } \
	    free(ptr); \
	    ptr = NULL; \
	 }
   
#undef E_STRDUP   
#define E_STRDUP(str) strdup2(str, __FILE__, __LINE__)   
   
#else

#define E_NEW(count, type) calloc(count, sizeof(type))
#define E_FREE(ptr) if(ptr) { free(ptr); ptr = NULL; }
#define E_STRDUP(str) strdup(str)
   
#endif   

typedef enum Enhance_Signal_Handling
{
   ENHANCE_SIGNAL_CONNECT,         /**< Signals are automatically connected upon file_load */
   ENHANCE_SIGNAL_STORE,           /**< Only a list of signal names + handler names is kept */
   ENHANCE_SIGNAL_NONE,            /**< Signals are completely ignored */
} Enhance_Signal_Handling;

typedef Eina_List* Enhance_Signals_Enumerator;
typedef Eina_List* Enhance_Widgets_Enumerator;

typedef struct _Enhance Enhance;

struct _Enhance
{
   EXML      *xml;            /* xml document */
   Evas_Hash *widgets;        /* all the allocated widgets */
   Evas_Hash *callback_data;  /* void *data variables for callbacks */
   Evas_Hash *signals;        /* signal name + callback name association for each widget */
   Evas_Hash *radio_groups;   /* radio groups used */
   char      *main_window;    /* main window to show */
   Enhance_Signal_Handling   signal_handling; /* how the signals should be handled */
};
   
/* Initialize and shutdown the enhance subsystems */
void        enhance_init();
void        enhance_shutdown();

/* Create / destroy an Enhance object */
Enhance    *enhance_new();
void        enhance_free(Enhance *en);

/* Gets how Enhance will manage the signal handlers it encounters */
Enhance_Signal_Handling  enhance_signal_handling_get(Enhance *en);

/* Sets how Enhance will manage the signal handlers it encounters */
void                     enhance_signal_handling_set(Enhance *en, Enhance_Signal_Handling mode);

/* Loads / parses a Glade XML file showing main_window */
void        enhance_file_load(Enhance *en, const char *main_window, const char *file);

/* Sets / gets a void *data variable of a particular callback */
void        enhance_callback_data_set(Enhance *en, const char *cb_name, void *data);
void       *enhance_callback_data_get(Enhance *en, const char *cb_name);

/* Gets the internal Etk_Widget for a certain part in the GUI */
Etk_Widget *enhance_var_get(Enhance *en, const char *string);

/* Gets the number of signal handlers for a certain part in the GUI */
int                        enhance_signals_count(Enhance *en, const char *widget);

/* Gets the first signal name + handler name for a certain part in the GUI. Returns NULL if there are none. */
Enhance_Signals_Enumerator enhance_signals_first(Enhance *en, const char* widget, char **signal, char **handler);

/* Gets the next signal name + handler name for a certain part in the GUI. Returns NULL if there are no more. */
Enhance_Signals_Enumerator enhance_signals_next(Enhance *en, Enhance_Signals_Enumerator current, char **signal, char **handler);

/* Starts the enumeration of all GUI parts. */
Enhance_Widgets_Enumerator  enhance_widgets_start(Enhance *en);

/* Gets the next GUI part. Return NULL if there are no more GUI parts. */
const char*                 enhance_widgets_next(Enhance *en, Enhance_Widgets_Enumerator* enumeration);

/* Ends an enumeration of all GUI parts. You don't need to call this if widgets_next returns NULL. But calling it anyway is legal. */
void                        enhance_widgets_end(Enhance *en, Enhance_Widgets_Enumerator* enumeration);

#ifdef __cplusplus
}
#endif
#endif         /* _ENHANCE_H */

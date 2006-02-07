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
   
typedef struct _Enhance Enhance;
   
struct _Enhance
{
   EXML      *xml;            /* xml document */
   Evas_Hash *widgets;        /* all the allocated widgets */
   Evas_Hash *callback_data;  /* void *data variables for callbacks */
   Evas_Hash *radio_groups;   /* radio groups used */
   char      *main_window;    /* main window to show */
};   
   
/* Initialize and shutdown the enhance subsystems */
void        enhance_init();
void        enhance_shutdown();

/* Create / destroy an Enhance object */
Enhance    *enhance_new();
void        enhance_free(Enhance *en);
   
/* Loads / parses a Glade XML file showing main_window */
void        enhance_file_load(Enhance *en, char *main_window, char *file);

/* Sets / gets a void *data variable of a particular callback */
void        enhance_callback_data_set(Enhance *en, char *cb_name, void *data);
void       *enhance_callback_data_get(Enhance *en, char *cb_name);

/* Gets the internal Etk_Widget for a certain part in the GUI */
Etk_Widget *enhance_var_get(Enhance *en, char *string);

#ifdef __cplusplus
}
#endif
#endif         /* _ENHANCE_H */

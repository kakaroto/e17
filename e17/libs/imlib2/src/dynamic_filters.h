#ifndef _DYNAMIC_FILTERS_H_
#define _DYNAMIC_FILTERS_H_

#include "script.h"

struct imlib_filter_info
{
   char **filters;
   int num_filters;
};

typedef struct _imlib_external_filter ImlibExternalFilter;
typedef struct _imlib_external_filter *pImlibExternalFilter;
struct _imlib_external_filter
{
   int    num_filters;
   char  *filename;
   void  *handle;
   char **filters;
   void  (*init_filter)( struct imlib_filter_info *info );
   void  (*deinit_filter)();
   void  *(*exec_filter)( char *filter, void *im, pIFunctionParam params );
   pImlibExternalFilter next;
};


void                 __imlib_dynamic_filters_init();
void                 __imlib_dynamic_filters_deinit();
pImlibExternalFilter __imlib_get_dynamic_filter( char *name );
char               **__imlib_ListFilters(int *num_ret);
pImlibExternalFilter __imlib_LoadFilter( char *file );



#endif




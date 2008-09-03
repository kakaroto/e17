#ifndef CONF_H_
#define CONF_H_

#include <Ecore_Data.h>

typedef struct _Args
{
   char *theme_path;
   char *engine;
   Ecore_List *files;
}
Args;

Args *parse_args(int argc, char **argv, Evas_List **startup_errors);

#endif /*CONF_H_*/

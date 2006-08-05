#ifndef _EN_ARGUMENT_H_
#define _EN_ARGUMENT_H_

#include <Evas.h>

/**
 * @defgroup En_Argument En_Argument
 * @{
 */

typedef struct En_Argument En_Argument; 

typedef enum En_Argument_Returns
{
   EN_ARGUMENT_RETURN_OK = 1,                   /* no problems */
   EN_ARGUMENT_RETURN_OK_NONE_PARSED = 2,       /* no problems, no arguments or no valid args */
   EN_ARGUMENT_RETURN_REQUIRED_NOT_FOUND = 3,   /* required arg not found */
   EN_ARGUMENT_RETURN_REQUIRED_VALUE_NOT_FOUND = 4, /* required value for arg no found */
   EN_ARGUMENT_RETURN_MALFORMED = 5             /* malformed argument */  
} En_Argument_Returns;

typedef enum En_Argument_Flags
{
   EN_ARGUMENT_FLAG_REQUIRED = 1 << 0,     /* argument itself is required */
   EN_ARGUMENT_FLAG_OPTIONAL = 1 << 1,     /* argument itself is optional */
   EN_ARGUMENT_FLAG_VALUE_REQUIRED = 1 << 2, /* value of the argument is required */
   EN_ARGUMENT_FLAG_MULTIVALUE = 1 << 3,   /* argument uses multi-valued args */
   EN_ARGUMENT_FLAG_NONE = 1 << 4          /* used when terminating options */
} En_Argument_Flags;

struct En_Argument
{
   char *long_name;           /* long name of argument: --foo */
   char  short_name;          /* short name of argument: -f */
   Evas_List *data;                /* filled with value of argument: -f blah */
   void (*func)(En_Argument *args, int index); /* callback */
   void *func_data;           /* data to the callback */
   En_Argument_Flags flags;  /* flags */
   char *description;         /* description of the argument for help */
};

int en_arguments_parse(En_Argument *args, int argc, char **argv);
void en_argument_help_show(En_Argument *args);  
Evas_List *en_argument_extra_find(const char *key);
int en_argument_is_set(En_Argument *args, const char *long_name, char short_name);

/** @} */

#endif

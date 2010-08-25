#ifndef EXPLICIT_SERVER_H_
# define EXPLICIT_SERVER_H_

#include <Ecore_Con.h>

#include "Explicit_Context.h"

#include "explicit_private.h"

#define EXPLICITD_CACHE ".e/explicitd/explicitd.cache"
#define EXPLICIT_PUBLIC "Pictures/"

typedef struct _Explicit_Reference Explicit_Reference;
typedef struct _Explicit_Server Explicit_Server;
typedef struct _Explicit_Cache Explicit_Cache;
typedef struct _Explicit_File Explicit_File;

struct _Explicit_Server
{
   Eet_File *eet;

   Explicit_Cache *cache;

   Eina_Hash *current;
   Eina_Hash *clients;
};

struct _Explicit_Cache
{
   const char *private_path;
   const char *public_path;

   unsigned long long soft_limit;
   unsigned long long hard_limit;
   unsigned long long file_limit;

   int count;
   int id;

   Eina_List *lru;
   Eina_Hash *done;
   Eina_List *files;
   Eina_List *empty;

   Eina_Bool changed : 1;
};

struct _Explicit_Reference
{
   int offset;
};

struct _Explicit_File
{
   const char *url;
   const char *filename;

   unsigned long long size;

   int id;
   int refcount;

   Eina_Bool empty;
};

void explicit_edd_file_shutdown(void);
void explicit_edd_file_init(void);

#endif

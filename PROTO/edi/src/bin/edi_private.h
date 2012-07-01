#ifndef EDI_PRIVATE_H_
# define EDI_PRIVATE_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <clang-c/Index.h>

extern int _log_domain;

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...)      EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...)      EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...)      EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...)      EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

typedef struct _Edi_File Edi_File;
typedef struct _Edi_Line Edi_Line;

struct _Edi_File
{
   Eina_File *f;
   void *m;

   /* We will probably need a tree of array to handle huge code */
   Eina_List *lines;
   Eina_List *current;

   unsigned int offset;

   /* Clang */
   CXIndex idx;
   CXTranslationUnit tx_unit;
};

struct _Edi_Line
{
   Eina_File_Lines read_only;
};

#endif

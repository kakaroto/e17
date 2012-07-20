#ifndef VIGRID_H_
#define VIGRID_H_

#include <Eina.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_VIGRID_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_VIGRID_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
/**
 * @def EAPI
 * @brief Used to export functions(by changing visibility).
 */
#  define EAPI
# endif
#endif

typedef struct _Vigrid_Massif Vigrid_Massif;
typedef struct _Vigrid_Massif_Snapshot Vigrid_Massif_Snapshot;
typedef struct _Vigrid_Massif_Line Vigrid_Massif_Line;

typedef enum {
  VIGRID_INSTRUCTIONS
} Vigrid_Unit;

typedef enum {
  VIGRID_MASSIF_TOP,
  VIGRID_MASSIF_BELOW,
  VIGRID_MASSIF_DETAIL
} Vigrid_Massif_Type;

struct _Vigrid_Massif
{
   Vigrid_Unit time;
   const char *command;
   const char *description;

   double      threshold;

   Eina_Array  snapshots;
};

struct _Vigrid_Massif_Snapshot
{
   long long int index;
   long long int time;
   long long int mem_heap;
   long long int mem_heap_extra;
   long long int mem_stack;

   Eina_Bool heap_tree;

   Eina_Array callstack;
};

struct _Vigrid_Massif_Line
{
   Vigrid_Massif_Type type;

   long long int mem_usage;
   long long int children;

   union {
      struct {
         int places;
         double threshold;
      } below;
      struct {
         long long int address;

         const char *function;
         const char *file;
         int line;

         Eina_Array children;
      } detail;
   } u;
};


EAPI int vigrid_init(void);
EAPI int vigrid_shutdown(void);

EAPI const Vigrid_Massif *vigrid_massif_alloc(const char *file);
EAPI void vigrid_massif_free(const Vigrid_Massif *massif);

#endif

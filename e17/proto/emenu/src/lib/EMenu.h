#ifndef _EMENU_H
# define _EMENU_H

# ifdef EMENUAPI
#  undef EMENUAPI
# endif

# ifdef WIN32
#  ifdef BUILDING_DLL
#   define EMENUAPI __declspec(dllexport)
#  else
#   define EMENUAPI __declspec(dllimport)
#  endif
# else
#  ifdef GCC_HASCLASSVISIBILITY
#   define EMENUAPI __attribute__ ((visibility("default")))
#  else
#   define EMENUAPI
#  endif
# endif

typedef struct _EMenu_Eap EMenu_Eap;
struct _EMenu_Eap 
{
  char *name; 
};

#define EMENU_EAP_DIR "/.e/e/applications/all"
#define EMENU_FAV_DIR "/.e/e/applications/favorite"

# ifdef __cplusplus
extern "C"
{
# endif
   
   EMENUAPI char *emenu_category_get(const char *category);
   EMENUAPI int   emenu_generate();
   
# ifdef __cplusplus
}
# endif

#endif

#include "E.h"

/* Added by Marty Riedling for HPUX 10 support */
#ifdef HPUX10
#include <dl.h>
#define dlopen(p, f) (shl_load((p), BIND_IMMEDIATE|BIND_VERBOSE|BIND_NOSTART, 0L))
#define dlclose(h)   (shl_unload((shl_t) (h)))
#else
#include <dlfcn.h>
#endif

/* modules.c - generic module loader / unloader */

/* This is the Modules List that we maintain */
typedef struct _ModuleMember
  {

     char               *ModuleName;
     void               *handle;

  }
ModuleMember;

ModuleMember       *ModuleList;

/* This is a few of our return code DEFINES */

#define MODULE_ALREADY_LOADED 1
#define MODULE_LOAD_FAIL      2
#define MODULE_UNLOAD_FAIL    3
#define MODULE_NOT_LOADED     4

int
LoadModule(char *module_name)
{

   void               *handle = NULL;

   /* Well, we don't want to load twice now, do we? */
   if (IsLoadedModule(module_name))
      return (MODULE_ALREADY_LOADED);

   /* We'll attempt to load the module first */
   {

      char                pathoffiletoload[FILEPATH_LEN_MAX];

      Esnprintf(pathoffiletoload, sizeof(pathoffiletoload), "%s/%s",
		FindModulePath(module_name), module_name);
      handle = dlopen(pathoffiletoload, RTLD_LAZY);

      if (!handle)
	 return (MODULE_LOAD_FAIL);
   }

   /* create or append to ModuleList? */
   if (ModuleList)
     {
	ModuleList = Erealloc(ModuleList,
			    ((sizeof(ModuleList) / sizeof(ModuleMember) + 1) *
			     sizeof(ModuleMember)));
     }
   else
     {
	ModuleList = Emalloc(sizeof(ModuleMember));
     }

   /* Now we'll chunk everything useful into the ModuleList */
   ModuleList[(sizeof(ModuleList) / sizeof(ModuleMember) - 1)].ModuleName
      = duplicate(module_name);
   ModuleList[(sizeof(ModuleList) / sizeof(ModuleMember) - 1)].handle =
      handle;

   return (0);

}

int
UnloadModule(char *module_name)
{

   int                 ListLength = 0;
   int                 ModuleID;
   int                 i;

   if (!(ModuleID = IsLoadedModule(module_name)))
      return (MODULE_NOT_LOADED);

   /* fix the ModuleID */
   ModuleID--;

   ListLength = (sizeof(ModuleList) / sizeof(ModuleMember));

   if (dlclose(ModuleList[ModuleID].handle))
      return (MODULE_UNLOAD_FAIL);

   if (ModuleList[ModuleID].ModuleName)
      Efree(ModuleList[ModuleID].ModuleName);

   for (i = ModuleID; i < ListLength; i++)
     {
	ModuleList[i].ModuleName = ModuleList[i + 1].ModuleName;
	ModuleList[i].handle = ModuleList[i + 1].handle;
     }

   ModuleList = Erealloc(ModuleList, ((sizeof(ModuleList) / sizeof(ModuleMember)
				       - 1) * sizeof(ModuleMember)));

   return (0);

}

char               *
ModuleErrorCodeToString(int error_code)
{

   switch (error_code)
     {

     case MODULE_ALREADY_LOADED:
	return ("Module is Already Loaded.\n");
     case MODULE_LOAD_FAIL:
	return ("Module Failed During Load.\n");
     case MODULE_UNLOAD_FAIL:
	return ("Module Unload Failed.\n");
     case MODULE_NOT_LOADED:
	return ("Module is not Loaded.\n");
     default:
	return ("Unknown Module Error.\n");

     }

}

char               *
ModuleListAsString(void)
{

   int                 ListLength = 0;
   int                 i;

   char                returnList[FILEPATH_LEN_MAX];

   ListLength = (sizeof(ModuleList) / sizeof(ModuleMember));

   returnList[0] = 0;

   strcat(returnList, "");
   for (i = 0; i < ListLength; i++)
     {
	strcat(returnList, ModuleList[i].ModuleName);
	strcat(returnList, "\n");

     }

   return (duplicate(returnList));

}

int
IsLoadedModule(char *module_name)
{

   /* unfortunately this must return the module id + 1, since
    * a 0 is the "not loaded" response, and the index can be 0
    * (note to anyone wondering what the hell this was about)
    * --Mandrake
    */

   int                 ListLength = 0;
   int                 i;

   ListLength = (sizeof(ModuleList) / sizeof(ModuleMember));

   for (i = 0; i < ListLength; i++)
     {
	if (!strcmp(ModuleList[i].ModuleName, module_name))
	  {
	     return (i + 1);
	  }
     }

   return (0);

}

char               *
FindModulePath(char *module_name)
{

   /* yeah yeah so it's hard-coded for now.  BITE ME */
   /* *** FIXME *** */
   module_name = module_name;	/* suppress warning until we actually use the module name for something */

   return (ENLIGHTENMENT_ROOT "/modules");

}

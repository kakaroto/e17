#include "Elation.h"

#include <dlfcn.h>

Elation_Module *
elation_module_open(Elation_Info *info, Elation_Module *parent, char *name)
{
   Elation_Module *em;
   void *(*init) (Elation_Module *em);
   void *handle;
   char buf[4096];
   
   snprintf(buf, sizeof(buf), "%s/elation_%s.so", PACKAGE_LIB_DIR"/elation", name);
   handle = dlopen(buf, RTLD_NOW | RTLD_LOCAL);
   if (!handle)
     {
	printf("ERROR loading: %s\n"
	       "ERROR:\n"
	       "%s\n", buf, dlerror());
	return NULL;
     }
   init = dlsym(handle, "init");
   if (!init)
     {
	printf("no init!\n");
	dlclose(handle);
	return NULL;
     }
   em = calloc(1, sizeof(Elation_Module));
   if (!em)
     {
	dlclose(handle);
	return NULL;
     }
   em->handle = handle;
   em->info = info;
   em->parent = parent;
   
   em->data = init(em);
   if (!em->data)
     {
	dlclose(em->handle);
	free(em);
	return NULL;
     }
   return em;
}

void
elation_module_close(Elation_Module *em)
{
   if (em->focused)
     {
	if (em->unfocus) em->unfocus(em);
	if (em->parent)
	  {
	     if (em->parent->focus) em->parent->focus(em->parent);
	  }
     }
   while (em->children)
     elation_module_close((Elation_Module *)em->children->data);
   
   if (em->shutdown) em->shutdown(em);
   dlclose(em->handle);
   
   if (em->parent)
     em->parent->children = evas_list_remove(em->parent->children, em);
   
   free(em);
}


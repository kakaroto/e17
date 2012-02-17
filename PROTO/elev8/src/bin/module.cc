#include <module.h>
#include <elev8_common.h>
#include <dirent.h>

static  Eina_Array *eina_elev8_modules;

void cleanup_modules()
{
   eina_module_list_unload(eina_elev8_modules);
   eina_module_list_free(eina_elev8_modules);
}

static Eina_Bool module_check(Eina_Module *m, void *d)
{
   const char *filename = eina_module_file_get(m);
   if (strcmp((filename + strlen(filename) - 3), ".so"))
     {
        return EINA_FALSE;
     }

   INF("found %s\n", filename);
   return EINA_TRUE;
}


//TODO : Add support for User Home Dir based modules.
void load_modules()
{
   /* load all the modules in the installed path PACKAGE_LIB_DIR */
   INF("DIR To Check = %s\n", PACKAGE_LIB_DIR);

   eina_elev8_modules = eina_module_list_get(NULL, PACKAGE_LIB_DIR,  EINA_FALSE, (Eina_Module_Cb)module_check, NULL);

   INF("Error = %s\n", eina_error_msg_get(eina_error_get()));
   if(!eina_elev8_modules) return ;
   INF("List Count of Modules = %d\n", eina_array_count(eina_elev8_modules));

   eina_module_list_load(eina_elev8_modules);

   INF("Initialized Modules = %d\n", eina_array_count(eina_elev8_modules));
}

void init_modules(v8::Handle<v8::ObjectTemplate> global)
{
   INF("Calling Setup of All Modules\n");
   Eina_Array_Iterator it;
   void *mod;
   unsigned int i = 0;
   INF("List Count of Modules = %d\n", eina_array_count(eina_elev8_modules));
   EINA_ARRAY_ITER_NEXT(eina_elev8_modules, i, mod, it)
     {
        module_info mi;
        Eina_Module *m = (Eina_Module *)mod;
        module_setup setup = (module_setup)eina_module_symbol_get(m,"setup");

        if (!setup)
          {
             eina_module_unload(m);
             WRN("module unloaded\n");
             continue;
          }

        (*setup)(&mi);

        if (mi.init)
          {
             (*(mi.init))(global, NULL);
          }
        else
          {
             eina_module_unload(m);
             WRN("module unloaded\n");
             continue;
          }

        INF("Initialized Module %s\n", mi.name);
     }
}

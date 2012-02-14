#include <module.h>

Eina_Inlist *root=NULL;

void cleanup_modules()
{
   while (root)
     {
        Eina_Inlist *aux = root;
        root = eina_inlist_remove(root, root);
        free(aux);
     }
}

void load_modules(v8::Handle<v8::ObjectTemplate> global)
{
   Eina_Inlist *itr = NULL;
   for (itr = root; itr != NULL; itr = itr->next)
     {
        module_info *ptr = EINA_INLIST_CONTAINER_GET(itr, module_info);

        ptr->init(global, ptr->name);

        printf("Initialized %s\n", ptr->name);

     }
   printf("Exit init modules\n");
}

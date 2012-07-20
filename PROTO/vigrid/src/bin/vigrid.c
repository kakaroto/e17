#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "Vigrid.h"

static void
_vigrid_display_biggest(Eina_Array *stack)
{
   Vigrid_Massif_Line *vl;
   Eina_Array_Iterator iterator;
   unsigned int i;
   long long int max_mem = 0;
   int max_index = -1;

   EINA_ARRAY_ITER_NEXT(stack, i, vl, iterator)
     if (vl->mem_usage > max_mem)
       {
          max_mem = vl->mem_usage;
          max_index = i;
       }

   if (max_index != -1)
     {
        vl = eina_array_data_get(stack, max_index);

        switch (vl->type)
          {
           case VIGRID_MASSIF_TOP:
              _vigrid_display_biggest(&vl->u.detail.children);
              break;
           case VIGRID_MASSIF_BELOW:
              printf("%lli bytes in %i place below threshold (%2.2f)\n",
                     vl->mem_usage, vl->u.below.places, vl->u.below.threshold);
              break;
           case VIGRID_MASSIF_DETAIL:
              printf("'%s' in file '%s' at line %i allocated %lli Bytes.\n",
                     vl->u.detail.function,
                     vl->u.detail.file,
                     vl->u.detail.line,
                     vl->mem_usage);
              if (eina_array_count(&vl->u.detail.children))
                _vigrid_display_biggest(&vl->u.detail.children);
              break;
          }
     }
   else
     {
        printf("No usefull information found in that stack\n");
     }
}

int
main(int argc, char *argv[])
{
   const Vigrid_Massif *ms;

   if (argc != 2) return -1;

   vigrid_init();

   ms = vigrid_massif_alloc(argv[1]);

   if (ms)
     {
        Vigrid_Massif_Snapshot *vs;
        Eina_Array_Iterator iterator;
        unsigned int i = 0;
        long long int max_mem = 0;
        int max_index = -1;

        printf("Finding the biggest allocation chain in the biggest snapshot in %i snapshots for command :\n",
               eina_array_count(&ms->snapshots));
        printf(" '%s'\n", ms->command);

        EINA_ARRAY_ITER_NEXT(&ms->snapshots, i, vs, iterator)
          {
             if (vs->mem_heap + vs->mem_heap_extra > max_mem)
               {
                  max_mem = vs->mem_heap + vs->mem_heap_extra;
                  max_index = i;
               }
          }

        if (max_index != -1)
          {
             vs = eina_array_data_get(&ms->snapshots, max_index);

             printf("Biggest snapshot is number %i with %lli Bytes in Heap and %lli Bytes in stack.\n",
                    max_index,
                    vs->mem_heap + vs->mem_heap_extra,
                    vs->mem_stack);
             _vigrid_display_biggest(&vs->callstack);
          }
        else
          {
             printf("No snapshot, no information !\n");
          }
     }
   else
     {
        fprintf(stderr, "Can't open '%s'\n", argv[1]);
     }

   vigrid_massif_free(ms);

   vigrid_shutdown();
   return 0;
}

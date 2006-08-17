#include "config.h"
#include "global.h"
#include "eaps.h"
#include "order.h"

int
search_list(Ecore_List *list, char *search)
{
   char *tmp;

   if (!search) return 0;
   if (!list) return 0;
   ecore_list_goto_first(list);
   while ((tmp = (char *)ecore_list_next(list)) != NULL)
     {
        if (!strcmp(tmp, search))
           return 1;
     }
   return 0;
}

void
create_order(char *path)
{
   FILE *f;

#ifdef DEBUG
   fprintf(stderr, "Creating Order File %s\n", path);
#endif

   f = fopen(path, "w");
   if (!f)
     {
        fprintf(stderr, "ERROR: Cannot Create Order File %s\n", path);
        exit(-1);
     }
   fclose(f);
}

void
modify_order(char *path, char *entry)
{
   int length, i;
   char *buff, *cat;
   char buffer[PATH_MAX], path2[PATH_MAX], t[PATH_MAX];
   FILE *f;
   Ecore_List *list = NULL;

   if (!ecore_file_exists(path))
     {
        ecore_file_mkpath(path);
        cat = strrchr(path, '/');
        snprintf(t, sizeof(t), "%s", cat);
        if (t[0] == '/')
          {
             for (i = 0; i < strlen(t); i++)
                t[i] = t[i + 1];
          }
        create_dir_eap(path, t);
     }

   snprintf(path2, sizeof(path2), "%s/.order", path);

#ifdef DEBUG
   fprintf(stderr, "Modifying Order File %s\n", path2);
#endif

   entry = ecore_file_get_file(entry);
   list = ecore_list_new();

   /* Stat .order; Create If Not Found */
   if (!ecore_file_exists(path2))
     {
        create_order(path2);
        /* If We Had To Create This Order Then Just Add The Entry */
        if (!ecore_list_append(list, entry))
          {
             fprintf(stderr, "ERROR: Ecore List Append Failed !!\n");
             return;
          }
     }
   else
     {
        /* Open .order File For Parsing */
        f = fopen(path2, "r");
        if (!f)
          {
             fprintf(stderr, "ERROR: Cannot Open Order File %s \n", path2);
             exit(-1);
          }

        /* Read All Entries From Existing Order File, Store In List For Sorting */
        while (fgets(buffer, sizeof(buffer), f) != NULL)
          {
             /* Strip New Line Char */
             if (buffer[(length = strlen(buffer) - 1)] == '\n')
                buffer[length] = '\0';
             if (!search_list(list, strdup(buffer)))
               {
                  if (!ecore_list_append(list, strdup(buffer)))
                    {
                       fprintf(stderr, "ERROR: Ecore List Append Failed !!\n");
                       return;
                    }
               }
          }
        fclose(f);
        buffer[0] = (char)0;

        /* Add This Entry To List Of Existing ? */
        if (!search_list(list, entry))
          {
             if (!ecore_list_append(list, entry))
               {
                  fprintf(stderr, "ERROR: Ecore List Append Failed !!\n");
                  return;
               }
          }
     }

#ifdef DEBUG
   fprintf(stderr, "Rewriting Order %s\n", path2);
#endif

   f = fopen(path2, "w");
   if (!f)
     {
        fprintf(stderr, "ERROR: Cannot Open Order File %s \n", path2);
        if (list)
           ecore_list_destroy(list);
        return;
     }

   ecore_list_goto_first(list);
   while ((buff = ecore_list_next(list)) != NULL)
     {
        snprintf(buffer, sizeof(buffer), "%s\n", buff);
        if (buffer != NULL)
           fwrite(buffer, sizeof(char), strlen(buffer), f);
     }
   fclose(f);

   if (list)
      ecore_list_destroy(list);
}

#include "enlil_private.h"
#include <eina_main.h>

#ifdef HAVE_AZY
#include <Azy.h>
#endif

int ENLIL_LOG_DOMAIN_99;

static int count = 0;

/*
 * Initialise the library.
 */
int
enlil_init(void)
{
   if(count > 0) return ++count;

   eet_init();
   ecore_init();
   ecore_file_init();
   ethumb_client_init();
   eina_threads_init();

   enlil_file_manager_init();
   enlil_thumb_init();
   enlil_trans_init();

#ifdef HAVE_AZY
   azy_init();
#endif

   LOG_DOMAIN = eina_log_domain_register("Enlil", "\033[34;1m");

   return ++count;
}

/*
 * Shutdown the library
 */
int
enlil_shutdown(void)
{
   if(count > 1) return --count;

   eina_log_domain_unregister(LOG_DOMAIN);

   enlil_trans_shutdown();
   enlil_thumb_shutdown();
   enlil_file_manager_shutdown();
   eina_threads_shutdown();
   ethumb_client_shutdown();
   ecore_file_shutdown();
   ecore_shutdown();
   eet_shutdown();

#ifdef HAVE_EABZU
   azy_shutdown();
#endif

   return --count;
}

/*
 * An application can add customs data in the configuration file
 *
 * @param edd The Eet Data Descriptor used to save the data
 * @param key the key use to save the data
 * @param data the data to save
 * @return Returns 1 on success, else 0
 */
int
enlil_eet_app_data_save(Eet_Data_Descriptor *edd,
                        const char          *key,
                        void                *data)
{
   int res;
   Eet_File *f;
   char path[PATH_MAX], buf[PATH_MAX];

   ASSERT_RETURN(edd != NULL);
   ASSERT_RETURN(key != NULL);
   ASSERT_RETURN(data != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB "/" EET_FILE_ROOT_DB, getenv("HOME"));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   snprintf(buf, PATH_MAX, "/app %s", key);
   res = eet_data_write(f, edd, buf, data, 0);

   enlil_file_manager_close(path);

   return res;
}

/*
 * An application can load customs data previously saved in the configuration file
 *
 * @param edd The Eet Data Descriptor used to save the data
 * @param key the key use to save the data
 * @return Returns the data, NULL if an error occurs or if the key is not in the file
 */
void *
enlil_eet_app_data_load(Eet_Data_Descriptor *edd,
                        const char          *key)
{
   void *res;
   Eet_File *f;
   char path[PATH_MAX], buf[PATH_MAX];

   ASSERT_RETURN(edd != NULL);
   ASSERT_RETURN(key != NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB "/" EET_FILE_ROOT_DB, getenv("HOME"));
   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   snprintf(buf, PATH_MAX, "/app %s", key);
   res = eet_data_read(f, edd, buf);

   enlil_file_manager_close(path);

   return res;
}

Eet_Data_Descriptor *
enlil_string_edd_new()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_String);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_String, "string", string, EET_T_STRING);

   return edd;
}

EAPI Eina_List *
eina_list_sorted_diff(Eina_List      *list1,
                      Eina_List      *list2,
                      Eina_List     **elt_list1,
                      Eina_List     **elt_list2,
                      Eina_Compare_Cb func)
{
   Eina_List *elt_common = NULL;
   Eina_List *l1, *l2;
   int cmp;
   void *d1, *d2;

   EINA_SAFETY_ON_NULL_RETURN_VAL(func, NULL);

   if(!elt_list1)
     return NULL;
   *elt_list1 = NULL;

   if(!elt_list2)
     return NULL;
   *elt_list2 = NULL;

   l1 = list1;
   l2 = list2;

   while(l1 && l2)
     {
        d1 = eina_list_data_get(l1);
        d2 = eina_list_data_get(l2);

        cmp = func(d1, d2);
        if(cmp < 0)
          {
             *elt_list1 = eina_list_append(*elt_list1, d1);
             l1 = eina_list_next(l1);
          }
        else if(cmp > 0)
          {
             *elt_list2 = eina_list_append(*elt_list2, d2);
             l2 = eina_list_next(l2);
          }
        else
          {
             elt_common = eina_list_append(elt_common, d1);
             l1 = eina_list_next(l1);
             l2 = eina_list_next(l2);
          }
     }

   while(l1)
     {
        *elt_list1 = eina_list_append(*elt_list1, eina_list_data_get(l1));
        l1 = eina_list_next(l1);
     }

   while(l2)
     {
        *elt_list2 = eina_list_append(*elt_list2, eina_list_data_get(l2));
        l2 = eina_list_next(l2);
     }

   return elt_common;
}

Eina_List *
eina_list_left_sorted_diff(Eina_List      *sorted_list,
                           Eina_List      *list2,
                           Eina_List     **elt_list1,
                           Eina_List     **elt_list2,
                           Eina_Compare_Cb func)
{
   Eina_List *elt_common = NULL;
   Eina_List *l1, *l2;
   Eina_List *list1 = NULL;
   int cmp;
   void *d1, *d2;

   EINA_SAFETY_ON_NULL_RETURN_VAL(func, NULL);

   if(!elt_list1)
     return NULL;
   *elt_list1 = NULL;

   if(!elt_list2)
     return NULL;
   *elt_list2 = NULL;

   l2 = list2;

   list1 = eina_list_clone(sorted_list);

   while(l2)
     {
        d2 = eina_list_data_get(l2);
        l1 = list1;
        while(l1)
          {
             d1 = eina_list_data_get(l1);

             cmp = func(d1, d2);
             if(cmp < 0)
               l1 = eina_list_next(l1);
             else if(cmp > 0)
               {
                  l1 = NULL;
                  break;
               }
             else
               {
                  list1 = eina_list_remove_list(list1, l1);
                  elt_common = eina_list_append(elt_common, d1);
                  l2 = eina_list_next(l2);
                  break;
               }
          }
        if(!l1)
          {
             *elt_list2 = eina_list_append(*elt_list2, d2);
             l2 = eina_list_next(l2);
          }
     }

   l1 = list1;
   while(l1)
     {
        Eina_List *l_save = l1;
        *elt_list1 = eina_list_append(*elt_list1, eina_list_data_get(l1));
        l1 = eina_list_next(l1);
        list1 = eina_list_remove_list(list1, l_save);
     }

   return elt_common;
}

EAPI Eina_List *
eina_list_right_sorted_diff(Eina_List      *list1,
                            Eina_List      *sorted_list,
                            Eina_List     **elt_list1,
                            Eina_List     **elt_list2,
                            Eina_Compare_Cb func)
{
   Eina_List *elt_common = NULL;
   Eina_List *l1, *l2;
   Eina_List *list2 = NULL;
   int cmp;
   void *d1, *d2;

   EINA_SAFETY_ON_NULL_RETURN_VAL(func, NULL);

   if(!elt_list1)
     return NULL;
   *elt_list1 = NULL;

   if(!elt_list2)
     return NULL;
   *elt_list2 = NULL;

   l1 = list1;

   list2 = eina_list_clone(sorted_list);

   while(l1)
     {
        d1 = eina_list_data_get(l1);
        l2 = list2;
        while(l2)
          {
             d2 = eina_list_data_get(l2);

             cmp = func(d1, d2);
             if(cmp < 0)
               {
                  l2 = NULL;
                  break;
               }
             else if(cmp > 0)
               l2 = eina_list_next(l2);
             else
               {
                  list2 = eina_list_remove_list(list2, l2);
                  elt_common = eina_list_append(elt_common, d1);
                  l1 = eina_list_next(l1);
                  break;
               }
          }
        if(!l2)
          {
             *elt_list1 = eina_list_append(*elt_list1, d1);
             l1 = eina_list_next(l1);
          }
     }

   l2 = list2;
   while(l2)
     {
        Eina_List *l_save = l2;
        *elt_list2 = eina_list_append(*elt_list2, eina_list_data_get(l2));
        l2 = eina_list_next(l2);
        list2 = eina_list_remove_list(list2, l_save);
     }

   return elt_common;
}

const char *
album_access_type_to_string(Enlil_Album_Access_Type access_type)
{
   switch(access_type)
     {
      case ENLIL_ALBUM_ACCESS_TYPE_PUBLIC:
        return "public";

      case ENLIL_ALBUM_ACCESS_TYPE_PRIVATE:
        return "private";
     }

   return NULL;
}

Enlil_Album_Access_Type
string_to_album_access_type(const char *access_type)
{
   if(!strcmp(access_type, "public"))
     return ENLIL_ALBUM_ACCESS_TYPE_PUBLIC;
   else
     return ENLIL_ALBUM_ACCESS_TYPE_PRIVATE;
}


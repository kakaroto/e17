#include "main.h"

Eet_Data_Descriptor * _edd_recent_new();
Eet_Data_Descriptor * _edd_new(Eet_Data_Descriptor *edd_recent);



L_Recent_File *recents_file_load()
{
    Eet_Data_Descriptor *edd, *edd_recent;
    Eet_File *f;
    L_Recent_File *data;
    char path[PATH_MAX];

    snprintf(path, PATH_MAX, "%s/.eyelight/", getenv("HOME"));
    if(!ecore_file_exists(path))
        ecore_file_mkpath(path);

    snprintf(path, PATH_MAX, "%s/recents.eet", path);

    f = eet_open(path, EET_FILE_MODE_READ_WRITE);
    if(!f)
        f = eet_open(path, EET_FILE_MODE_WRITE);

    if(!f) return NULL;

    edd_recent = _edd_recent_new();
    edd = _edd_new(edd_recent);

    data = eet_data_read(f, edd, "recents");

    eet_close(f);

    eet_data_descriptor_free(edd_recent);
    eet_data_descriptor_free(edd);

    if(!data)
        data = calloc(1, sizeof(L_Recent_File));

    return data;
}

void recents_file_save(L_Recent_File *l)
{
    Eet_Data_Descriptor *edd, *edd_recent;
    Eet_File *f;
    char path[PATH_MAX];

    snprintf(path, PATH_MAX, "%s/.eyelight/", getenv("HOME"));
    if(!ecore_file_exists(path))
        ecore_file_mkpath(path);

    snprintf(path, PATH_MAX, "%s/recents.eet", path);

    f = eet_open(path, EET_FILE_MODE_READ_WRITE);
    if(!f)
        f = eet_open(path, EET_FILE_MODE_WRITE);

    if(!f) return ;
    edd_recent = _edd_recent_new();
    edd = _edd_new(edd_recent);

    eet_data_write(f, edd, "recents", l, 0);

    eet_close(f);

    eet_data_descriptor_free(edd_recent);
    eet_data_descriptor_free(edd);
}

void recents_file_free(L_Recent_File *l_file)
{
    Recent_File *file;

    EINA_LIST_FREE(l_file->l,file)
    {
        if(file->file)
            eina_stringshare_del(file->file);
        free(file);
    }
    free(l_file);
}

Eet_Data_Descriptor * _edd_recent_new()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Recent_File);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Recent_File, "file", file, EET_T_STRING);

   return edd;
}


Eet_Data_Descriptor * _edd_new(Eet_Data_Descriptor *edd_recent)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   if(!edd_recent) return NULL;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, L_Recent_File);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, L_Recent_File, "l", l, edd_recent);

   return edd;
}


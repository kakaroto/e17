/** @file exalt_sys_conf.c */
#include "exalt_sys_conf.h"
#include "config.h"
#include "libexalt_private.h"


typedef struct Exalt_Eth_Save
{
    int state;
    char* driver;
    Exalt_Configuration *conf;
}Exalt_Eth_Save;

Exalt_Eth_Save* _exalt_eet_eth_load(const char* file, const char* interface);
int _exalt_eet_eth_save(const char* file, Exalt_Eth_Save* s, const char* interface);


Eet_Data_Descriptor * _exalt_eth_save_edd_new();

Exalt_Configuration* _exalt_eet_wireless_conf_load(const char* file, const char* essid);
int _exalt_eet_wireless_conf_save(const char* file,Exalt_Configuration* c);



/**
 * @addgroup Exalt_System_Configuration
 *@{
 */



int exalt_eth_save(const char* file, Exalt_Ethernet* eth)
{
    Exalt_Eth_Save s;

    EXALT_ASSERT_RETURN(eth!=NULL);

    s.state = exalt_eth_up_is(eth);
    s.conf = exalt_eth_configuration_get(eth);
    if(exalt_eth_wireless_is(eth))
        s.driver = (char*)exalt_wireless_wpasupplicant_driver_get(exalt_eth_wireless_get(eth));
    else
        s.driver = "no driver for wired interface";

    return _exalt_eet_eth_save(file, &s, exalt_eth_udi_get(eth));
}

int exalt_eth_state_load(const char* file, const char* udi)
{
    Exalt_Eth_Save *s = _exalt_eet_eth_load(file, udi);
    EXALT_ASSERT_RETURN(s!=NULL);
    int st = s->state;
    EXALT_FREE(s->driver);
    exalt_conf_free(&(s->conf));
    EXALT_FREE(s);
    return st;
}

char* exalt_eth_driver_load(const char* file, const char* udi)
{
    Exalt_Eth_Save *s = _exalt_eet_eth_load(file,  udi);
    EXALT_ASSERT_RETURN(s!=NULL);
    char* driver = s->driver;
    exalt_conf_free(&(s->conf));
    EXALT_FREE(s);
    return driver;
}


Exalt_Configuration* exalt_eth_conf_load(const char* file, const char* udi)
{
    Exalt_Eth_Save *s = _exalt_eet_eth_load(file, udi);
    EXALT_ASSERT_RETURN(s!=NULL);
    Exalt_Configuration *c = s->conf;
    EXALT_FREE(s->driver);
    EXALT_FREE(s);
    return c;
}


int exalt_conf_network_save(const char* file, Exalt_Configuration *c)
{
    Eet_Data_Descriptor *edd_network, *edd;
    int res;
    Eet_File *f;
    char buf[1024];

    edd_network = exalt_conf_network_edd_new();
    edd = exalt_conf_edd_new(edd_network);

    snprintf(buf, 1024, "wireless_network_%s", exalt_conf_network_essid_get(exalt_conf_network_get(c)));

    f = eet_open(file, EET_FILE_MODE_READ_WRITE);
    if(!f)
        f = eet_open(file, EET_FILE_MODE_WRITE);
    res=eet_data_write(f, edd, buf, c, 0);
    EXALT_ASSERT(res!=0);

    eet_close(f);
    eet_data_descriptor_free(edd);
    eet_data_descriptor_free(edd_network);

    return 1;
}

Exalt_Configuration *exalt_conf_network_load(const char *file,const char *network)
{
    Eet_Data_Descriptor *edd, *edd_network;
    char buf[1024];
    Eet_File *f;
    Exalt_Configuration *data;

    snprintf(buf, 1024, "wireless_network_%s", network);

    edd_network = exalt_conf_network_edd_new();
    edd = exalt_conf_edd_new(edd_network);

    f = eet_open(file, EET_FILE_MODE_READ);
    EXALT_ASSERT_RETURN(f!=NULL);

    data = eet_data_read(f, edd, buf);

    eet_close(f);
    eet_data_descriptor_free(edd);
    eet_data_descriptor_free(edd_network);

    return data;
}

Eina_List *exalt_conf_network_list_load(const char* file)
{
    int nb_res,i;
    Eet_File *f;
    char **res;
    int length = strlen("wireless_network_");
    Eina_List *l = NULL;

    f = eet_open(file, EET_FILE_MODE_READ);
    EXALT_ASSERT_RETURN(f!=NULL);

    res = eet_list(f,"wireless_network_*",&nb_res);

    for(i=0;i<nb_res;i++)
    {
        char *s = strdup(res[i]+length);
        if(s) l = eina_list_append(l, s);
    }

    eet_close(f);
    return l;
}

void exalt_conf_network_delete(const char* file, const char *network)
{
    Eet_File *f;
    char buf[1024];

    f = eet_open(file, EET_FILE_MODE_READ_WRITE);
    if(!f)
        f = eet_open(file, EET_FILE_MODE_WRITE);
    EXALT_ASSERT_RETURN_VOID(f!=NULL);

    snprintf(buf, 1024, "wireless_network_%s", network);
    eet_delete(f,buf);

    eet_close(f);
}

/** @} */



/* PRIVATES FUNCTIONS */

/**
 * @brief Load the information about an interface from the configuration file
 * The information are saved in a Exalt_Eth_Save structure
 * @param file the configuration file
 * @param udi the hal udi of the interface
 * @return Returns the information
 */
Exalt_Eth_Save* _exalt_eet_eth_load(const char* file, const char* udi)
{
    Exalt_Eth_Save *data = NULL;
    Eet_Data_Descriptor *edd, *edd_conf, *edd_network;
    Eet_File *f;

    edd_network = exalt_conf_network_edd_new();

    edd_conf = exalt_conf_edd_new(edd_network);
    edd = _exalt_eth_save_edd_new(edd_conf);

    f = eet_open(file, EET_FILE_MODE_READ);
    EXALT_ASSERT_RETURN(f!=NULL);

    data = eet_data_read(f, edd, udi);

    eet_close(f);
    eet_data_descriptor_free(edd);
    eet_data_descriptor_free(edd_network);
    eet_data_descriptor_free(edd_conf);
    return data;
}

/**
 * @brief Save a Exalt_Eth_Save structure in the configuration file
 * @param file the configuration file
 * @param s the Exalt_Eth_Save strucuture
 * @param udi the hal udi of the interface
 * @return Returns 1 if success, else 0
 */
int _exalt_eet_eth_save(const char* file, Exalt_Eth_Save* s, const char* udi)
{
    int res;
    Eet_Data_Descriptor *edd, *edd_conf, *edd_network;
    Eet_File *f;

    edd_network = exalt_conf_network_edd_new();

    edd_conf = exalt_conf_edd_new(edd_network);
    edd = _exalt_eth_save_edd_new(edd_conf);

    f = eet_open(file, EET_FILE_MODE_READ_WRITE);
    if(!f)
        f = eet_open(file, EET_FILE_MODE_WRITE);
    res=eet_data_write(f, edd,udi, s, 0);
    EXALT_ASSERT(res!=0);

    eet_close(f);
    eet_data_descriptor_free(edd);
    eet_data_descriptor_free(edd_network);
    eet_data_descriptor_free(edd_conf);
    return res;
}

/**
 * @brief Create an eet descriptor for the structure Exalt_Eth_Save
 * @return Returns the descriptor
 */
Eet_Data_Descriptor * _exalt_eth_save_edd_new(Eet_Data_Descriptor* edd_conf)
{
    Eet_Data_Descriptor_Class eddc;
    Eet_Data_Descriptor *edd;

    eet_eina_stream_data_descriptor_class_set(&eddc, sizeof (eddc),
					      "Interface", sizeof(Exalt_Eth_Save));
    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Eth_Save, "up", state, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Eth_Save, "driver", driver, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_SUB(edd, Exalt_Eth_Save, "configuration", conf, edd_conf);

    return edd;
}


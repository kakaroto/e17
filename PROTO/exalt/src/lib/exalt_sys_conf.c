/** @file exalt_sys_conf.c */
#include "exalt_sys_conf.h"
#include "config.h"
#include "libexalt_private.h"

typedef struct Exalt_Eth_Save
{
    Exalt_Enum_State state;
    char* driver;
    Exalt_Connection *connection;
}Exalt_Eth_Save;

Exalt_Eth_Save* _exalt_eet_eth_load(const char* file, const char* interface);
int _exalt_eet_eth_save(const char* file, Exalt_Eth_Save* s, const char* interface);


Eet_Data_Descriptor * _exalt_eth_save_edd_new();

Exalt_Connection* _exalt_eet_wireless_conn_load(const char* file, const char* essid);
int _exalt_eet_wireless_conn_save(const char* file,Exalt_Connection* c);



/**
 * @addgroup Exalt_System_Configuration
 *@{
 */



int exalt_eth_save(const char* file, Exalt_Ethernet* eth)
{
    Exalt_Eth_Save s;

    EXALT_ASSERT_RETURN(eth!=NULL);

    s.state = exalt_eth_up_is(eth);
    s.connection = exalt_eth_connection_get(eth);
    if(exalt_eth_wireless_is(eth))
        s.driver = exalt_wireless_wpasupplicant_driver_get(exalt_eth_wireless_get(eth));
    else
        s.driver = "wext";

    return _exalt_eet_eth_save(file, &s, exalt_eth_udi_get(eth));
}

Exalt_Enum_State exalt_eth_state_load(const char* file, const char* udi)
{
    Exalt_Eth_Save *s = _exalt_eet_eth_load(file, udi);
    EXALT_ASSERT_RETURN(s!=NULL);
    Exalt_Enum_State st = s->state;
    EXALT_FREE(s->driver);
    exalt_conn_free(&(s->connection));
    EXALT_FREE(s);
    return st;
}

char* exalt_eth_driver_load(const char* file, const char* udi)
{
    Exalt_Eth_Save *s = _exalt_eet_eth_load(file,  udi);
    EXALT_ASSERT_RETURN(s!=NULL);
    char* driver = s->driver;
    exalt_conn_free(&(s->connection));
    EXALT_FREE(s);
    return driver;
}


Exalt_Connection* exalt_eth_conn_load(const char* file, const char* udi)
{
    Exalt_Eth_Save *s = _exalt_eet_eth_load(file, udi);
    EXALT_ASSERT_RETURN(s!=NULL);
    Exalt_Connection *c = s->connection;
    EXALT_FREE(s->driver);
    EXALT_FREE(s);
    return c;
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
    Eet_Data_Descriptor *edd, *edd_conn, *edd_network, *edd_ie;
    Eet_File *f;

    edd_ie = exalt_wireless_network_ie_edd_new();
    edd_network = exalt_wireless_network_edd_new(edd_ie);

    edd_conn = exalt_conn_edd_new(edd_network);
    edd = _exalt_eth_save_edd_new(edd_conn);

    f = eet_open(file, EET_FILE_MODE_READ);
    EXALT_ASSERT_RETURN(f!=NULL);

    data = eet_data_read(f, edd, udi);

    eet_close(f);
    eet_data_descriptor_free(edd);
    eet_data_descriptor_free(edd_ie);
    eet_data_descriptor_free(edd_network);
    eet_data_descriptor_free(edd_conn);
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
    Eet_Data_Descriptor *edd, *edd_conn, *edd_network, *edd_ie;
    Eet_File *f;

    edd_ie = exalt_wireless_network_ie_edd_new();
    edd_network = exalt_wireless_network_edd_new(edd_ie);

    edd_conn = exalt_conn_edd_new(edd_network);
    edd = _exalt_eth_save_edd_new(edd_conn);

    f = eet_open(file, EET_FILE_MODE_READ_WRITE);
    if(!f)
        f = eet_open(file, EET_FILE_MODE_WRITE);
    res=eet_data_write(f, edd,udi, s, 0);
    EXALT_ASSERT(res!=0);

    eet_close(f);
    eet_data_descriptor_free(edd);
    eet_data_descriptor_free(edd_ie);
    eet_data_descriptor_free(edd_network);
    eet_data_descriptor_free(edd_conn);
    return res;
}

/**
 * @brief Create an eet descriptor for the structure Exalt_Eth_Save
 * @return Returns the descriptor
 */
Eet_Data_Descriptor * _exalt_eth_save_edd_new(Eet_Data_Descriptor* edd_conn)
{
    Eet_Data_Descriptor *edd;

    edd = eet_data_descriptor_new("Interface", sizeof(Exalt_Eth_Save),
            (void*(*)(void*))eina_list_next,
            (void*(*)(void*,void*))eina_list_append,
            (void*(*)(void*))eina_list_data_get,
            (void*(*)(void*))eina_list_free,
            (void(*)(void*,int(*)(void*,const char*,void*,void*),void*))evas_hash_foreach,
            (void*(*)(void*,const char*,void*))evas_hash_add,
            (void(*)(void*))evas_hash_free);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Eth_Save, "up", state, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Eth_Save, "driver", driver, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_SUB(edd, Exalt_Eth_Save, "connection", connection, edd_conn);

    return edd;
}


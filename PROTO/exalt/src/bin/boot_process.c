/*
 * =====================================================================================
 *
 *       Filename:  boot_process.c
 *
 *    Description:  configure the boot process, if the system wait until a iface get a ip address.
 *
 *        Version:  1.0
 *        Created:  01/02/2008 06:30:42 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "boot_process.h"


/*
 * @brief load the list of interfaces from a file
 * @param file the configuration file
 * @return Returns the list of interfaces
 */
Boot_Process_List *waiting_iface_load(const char* file)
{
    Eet_Data_Descriptor*edd;
    Eet_File *f;
    Boot_Process_List *data;

    edd = waiting_iface_edd_new();
    f = eet_open(file, EET_FILE_MODE_READ);

    EXALT_ASSERT_ADV(f!=NULL,
                            data = malloc(sizeof(Boot_Process_List));
                            data->timeout = 30;
                            data->l=NULL,
                    "f!=NULL failed");

    data = eet_data_read(f, edd, "boot process interface list");
    eet_close(f);
    eet_data_descriptor_free(edd);

    EXALT_ASSERT_ADV(data!=NULL,
                            data = malloc(sizeof(Boot_Process_List));
                            data->timeout = 30;
                            data->l=NULL,
                    "data!=NULL failed");

    return data;
}


/*
 * @brief save the list of interfaces
 * @param l the list
 * @param file the configuration file
 */
void waiting_iface_save(const Boot_Process_List* l, const char* file)
{
    Eet_Data_Descriptor* edd;
    Eet_File* f;

    EXALT_ASSERT_RETURN_VOID(l!=NULL);
    EXALT_ASSERT_RETURN_VOID(file!=NULL);

    edd = waiting_iface_edd_new();
    f = eet_open(file, EET_FILE_MODE_READ_WRITE);

    if(!f)
    {
        f = eet_open(file, EET_FILE_MODE_WRITE);
    }
    eet_data_write(f, edd,"boot process interface list", l, 0);

    eet_close(f);
    eet_data_descriptor_free(edd);
}

void waiting_iface_free(Boot_Process_List** l)
{
    EXALT_ASSERT_RETURN_VOID(l!=NULL);
    EXALT_ASSERT_RETURN_VOID((*l)!=NULL);

    //eina_list_free() doesn't free the data, we free them
    {
        Eina_List* levas = (*l)->l;
        while(levas)
        {
            Boot_Process_Elt* data = eina_list_data_get(levas);
            EXALT_FREE(data->interface);
            EXALT_FREE(data);
            levas = eina_list_next(levas);
        }
    }

    eina_list_free((*l)->l);
    EXALT_FREE(*l);
}

/*
 * @brief return true is the interface eth is in the list
 * @param l the list of interface
 * @param eth the interface
 * @return Returns 1 if yes, else 0
 */
int waiting_iface_is(const Boot_Process_List* l,const Exalt_Ethernet* eth)
{
//  int find = 0;
    Eina_List *elt;
    Boot_Process_Elt *data;

    EXALT_ASSERT_RETURN(l!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

/*  by bentejuy

    elt = l->l;
    while(!find && elt)
    {
        data = eina_list_data_get(elt);
        if(data->interface && strcmp(exalt_eth_get_name(eth),data->interface) == 0)
            find = 1;
        else
            elt = eina_list_next(elt);
    }
    return find;
*/
    EXALT_ASSERT_RETURN(l->l!=NULL);

	for(elt = l->l; elt; elt = eina_list_next(elt))
	{
        if(!(data = eina_list_data_get(elt)))
        	continue;

        if(data->interface && !strcmp(exalt_eth_get_name(eth),data->interface))
        	return 1;
	}
	return 0;
}

/*
 * @brief remove a interface in the list
 * @param l the list of interface
 * @param eth the interface
 */
void waiting_iface_done(Boot_Process_List* l,const Exalt_Ethernet* eth)
{
    Eina_List *elt;
    Boot_Process_Elt *data;

    EXALT_ASSERT_RETURN_VOID(l!=NULL);
    EXALT_ASSERT_RETURN_VOID(l->l!=NULL);

    for(elt = l->l; elt; elt = eina_list_next(elt))
    {
        if(!(data = eina_list_data_get(elt)))
            continue;

        if(data->interface && !strcmp(exalt_eth_get_name(eth),data->interface))
        {
            l->l = eina_list_remove(l->l,eina_list_data_get(elt));
            return;
        }
    }
}

/*
 * @brief return 1 if no more interface have to be wait
 * If an interface is in the list but not detect by the computer, the system will not wait this interface
 * @param l the list of interface
 * @return Returns 1 if all interfaces are init, else 0
 */
int waiting_iface_is_done(const Boot_Process_List* l)
{
    Eina_List *elt;
    Boot_Process_Elt *data;
//  int find =  0;

    if(!l)
        return 1;

	// by bentejuy
    EXALT_ASSERT_RETURN(l->l!=NULL);
/*
    elt = l->l;

    while(!find && elt)
    {
        data = eina_list_data_get(elt);
        if(exalt_eth_get_ethernet_byname(data->interface))
            find = 1;
        else
            elt = eina_list_next(elt);
    }

    return !find;
*/
	for(elt = l->l; elt; elt = eina_list_next(elt))
	{
        if(!(data = eina_list_data_get(elt)))
        	continue;

        if(exalt_eth_get_ethernet_byname(data->interface))
        	return 1;
	}
	return 0;
}

/*
 * @brief end waiting interfaces
 * @param data the list of interfaces
 * @return Returns 0
 */
int waiting_iface_stop(void* data)
{
    Boot_Process_List *l = data;

    if( l && waiting_iface_is_done(l))
        printf("All interfaces are configure, continue boot process ...\n");
    else
        printf("Timeout, continue boot process ...\n");

    if(l)
        waiting_iface_free(&l);

    ecore_main_loop_quit();
    return 0;
}


/*
 * @brief add an interface in the configuration file
 * @param interface the interface name
 * @param file the configuration file
 * @return Returns 1 if success
 */
int waiting_iface_add(const char* interface,const char* file)
{
    Exalt_Ethernet* eth;
    Boot_Process_List *l;

    EXALT_ASSERT_RETURN(interface!=NULL);
    EXALT_ASSERT_RETURN(file!=NULL);

    l= waiting_iface_load(file);
    //add the new interface
    Boot_Process_Elt *elt = malloc(sizeof(Boot_Process_Elt));
    EXALT_STRDUP(elt->interface,interface);
    l->l = eina_list_append(l->l, elt);

    //save the new list
    waiting_iface_save(l,file);

    waiting_iface_free(&l);

    //we send a broadcast on dbus
    eth = exalt_eth_get_ethernet_byname(interface);
    if(eth)
        eth_cb(eth,EXALTD_ETH_CB_WAITINGBOOT_CHANGE,exaltd_conn);

    return 1;
}

/*
 * @brief set the timeout
 * @param timeout the timeout value
 * @param file the configuration file
 * @return Returns 1 if success
 */
int waiting_timeout_set(int timeout, const char* file)
{
    Exalt_Ethernet* eth;
    Boot_Process_List *l;

    EXALT_ASSERT_RETURN(file!=NULL);

    l= waiting_iface_load(file);
    l->timeout = timeout;

    //save the new list
    waiting_iface_save(l,file);

    waiting_iface_free(&l);

   /*we can't send a broadcast without specify a interface
     * that's why we create a special ethernet struct without name :)
     */
    eth = exalt_eth_new("No interface is specify with the signal EXALTD_ETH_CB_WAITINGBOOT_CHANGE");
    if(eth)
        eth_cb(eth,EXALTD_ETH_CB_WAITINGBOOT_TIMEOUT_CHANGE,exaltd_conn);

    exalt_eth_free(eth);
    return 1;
}

/*
 * @brief get the timeout
 * @param timeout the timeout value
 * @param file the configuration file
 * @return Returns the timeout if success
 */
int waiting_timeout_get(const char* file)
{
    int timeout;
    Boot_Process_List *l;

    EXALT_ASSERT_RETURN(file!=NULL);

    l = waiting_iface_load(file);
    timeout = l->timeout;

    waiting_iface_free(&l);

    return timeout;
}

/*
 * @brief remove an interface in the configuration file
 * @param interface the interface name
 * @param file the configuration file
 * @return Returns 1 if success
 */
int waiting_iface_remove(const char* interface,const char* file)
{
    Exalt_Ethernet* eth;
    Boot_Process_List *l = waiting_iface_load(file);

    EXALT_ASSERT_RETURN(file!=NULL);
    EXALT_ASSERT_RETURN(interface!=NULL);

    l = waiting_iface_load(file);
    eth = exalt_eth_get_ethernet_byname(interface);

    waiting_iface_done(l,eth);

    //save the new list
    waiting_iface_save(l,file);

    waiting_iface_free(&l);

    //we send a broadcast on dbus
    if(eth)
        eth_cb(eth,EXALTD_ETH_CB_WAITINGBOOT_CHANGE,exaltd_conn);

    return 1;
}

/*
 * @brief test if an interface is in the configuration file
 * @param interface the interface name
 * @param file the configuration file
 * @return Returns 1 if the interface is in, else 0
 */
int waiting_iface_is_inconf(const char* interface,const char* file)
{
    Exalt_Ethernet* eth;
    int is;
    Boot_Process_List *l;

    EXALT_ASSERT_RETURN(file!=NULL);
    EXALT_ASSERT_RETURN(interface!=NULL);

    l = waiting_iface_load(file);
    eth = exalt_eth_get_ethernet_byname(interface);
    is = waiting_iface_is(l,eth);
    waiting_iface_free(&l);

    return is;
}

/*
 * @brief Create the eet descriptor for a list of interfaces (list of char*)
 * @return Return the descriptor
 */
Eet_Data_Descriptor * waiting_iface_edd_new()
{
    Eet_Data_Descriptor *edd_elt, *edd_l;

    edd_elt = eet_data_descriptor_new("elt", sizeof(Boot_Process_Elt),
            (void*(*)(void*))eina_list_next,
            (void*(*)(void*,void*))eina_list_append,
            (void*(*)(void*))eina_list_data_get,
            (void*(*)(void*))eina_list_free,
            (void(*)(void*,int(*)(void*,const char*,void*,void*),void*))evas_hash_foreach,
            (void*(*)(void*,const char*,void*))evas_hash_add,
            (void(*)(void*))evas_hash_free);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_elt, Boot_Process_Elt, "interface", interface, EET_T_STRING);



    edd_l = eet_data_descriptor_new("boot process interface list", sizeof(Boot_Process_List),
            (void*(*)(void*))eina_list_next,
            (void*(*)(void*,void*))eina_list_append,
            (void*(*)(void*))eina_list_data_get,
            (void*(*)(void*))eina_list_free,
            (void(*)(void*,int(*)(void*,const char*,void*,void*),void*))evas_hash_foreach,
            (void*(*)(void*,const char*,void*))evas_hash_add,
            (void(*)(void*))evas_hash_free);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd_l, Boot_Process_List, "timeout (sec)", timeout, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_LIST(edd_l, Boot_Process_List, "interface list", l, edd_elt);

    return edd_l;
}


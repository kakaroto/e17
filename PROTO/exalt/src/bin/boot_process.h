/*
 * =====================================================================================
 *
 *       Filename:  boot_process.h
 *
 *    Description:  configure the boot process, if the system wait until a card get a ip address.
 *
 *        Version:  1.0
 *        Created:  01/02/2008 06:28:04 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  BOOT_PROCESS_INC
#define  BOOT_PROCESS_INC

#include "daemon.h"
#include "Ecore.h"


typedef struct Boot_Process_Elt
{
    char *interface;
} Boot_Process_Elt;

typedef struct Boot_Process_List
{
    int timeout;
    Eina_List *l;
} Boot_Process_List;

Boot_Process_List *waiting_iface_list;
Ecore_Timer* waiting_iface_timer;

void waiting_iface_free(Boot_Process_List** l);
void waiting_iface_save(const Boot_Process_List* l, const char* file);
Boot_Process_List* waiting_iface_load(const char* file);
int waiting_iface_is(const Boot_Process_List* l, const Exalt_Ethernet* eth);
void waiting_iface_done(Boot_Process_List* l, const Exalt_Ethernet* eth);
int waiting_iface_is_done(const Boot_Process_List* l );
int waiting_iface_stop(void* data);

int waiting_iface_add(const char* interface,const char* file);
int waiting_iface_remove(const char* interface,const char* file);
int waiting_iface_is_inconf(const char* interface,const char* file);

int waiting_timeout_set(int timeout, const char* file);
int waiting_timeout_get(const char* file);


Eet_Data_Descriptor * waiting_iface_edd_new();


#endif   /* ----- #ifndef BOOT_PROCESS_INC  ----- */


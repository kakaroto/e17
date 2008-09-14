/** @file libexalt.h */

#ifndef COMMON_H
#define COMMON_H


#define EXALT_FREE(p) do { if(p){ free(p); p=NULL;} }while(0)
#define EXALT_PCLOSE(p) do {if(p){pclose(p); p=NULL;} }while(0)
#define EXALT_DELETE_TIMER(a) do {if(a) {ecore_timer_del(a); a=NULL; } }while(0)
#define EXALT_CLOSE(p) do {if(p>=0){close(p); p=0;} }while(0)
#define EXALT_FCLOSE(p) do {if(p>=0){fclose(p); p=NULL;} }while(0)
#define EXALT_CLEAR_LIST(p) do {if(p){ecore_list_clear(p); p=NULL;} }while(0)
#define EXALT_STRDUP(a,b) do { const char* str99 = b; if(str99){ a=strdup(str99);} else {a=NULL;}}while(0)

#define EXALT_PATH_PROCNET_DEV "/proc/net/dev"
#define EXALT_PATH_ROUTE "/proc/net/route"
#define EXALT_WPA_IFACE_DIR "/var/run/wpa_supplicant"
#define EXALT_TEMP_FILE "/tmp/exalt_temp"



#define EXALT_ASSERT(test) \
    do\
    {\
        if(!(test))\
        {\
            print_error(__FILE__,__func__,__LINE__,"%s failed",#test);\
        }\
    }while(0)



#define EXALT_ASSERT_RETURN(test) \
    do\
    {\
        if(!(test))\
        {\
            print_error(__FILE__,__func__,__LINE__, "%s failed",#test);\
            return 0;\
        }\
    }while(0)

#define EXALT_ASSERT_RETURN_VOID(test) \
    do\
    {\
        if(!(test))\
        {\
            print_error(__FILE__,__func__,__LINE__,"%s failed",#test);\
            return ;\
        }\
    }while(0)



#define EXALT_ASSERT_ADV(test, instr, ...) \
    do \
    { \
        if(!(test))\
        {\
            print_error(__FILE__,__func__, __LINE__,__VA_ARGS__ );\
            instr; \
        }\
    }while(0)




#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/route.h>

#include <linux/types.h>
#include <linux/ethtool.h>

#include <sys/ioctl.h>
#include <linux/sockios.h>

#include <Eet.h>

#include "exalt_ethernet.h"
#include "exalt_wireless_network.h"
#include "exalt_wireless.h"
#include "exalt_dns.h"
#include "exalt_sys_conf.h"
#include "exalt_connection.h"
#include "exalt_vpnc.h"

/**
 * @defgroup Exalt_General
 * @brief Some functions
 * @{
 */

int exalt_init();
int exalt_main();


short exalt_is_address(const char* ip);
short exalt_is_essid(const char* essid);
short exalt_is_admin();
short exalt_is_key(const char* key, Exalt_Enum_Encryption_Mode encryption_mode);

short exalt_wpasupplicant_is_support();
short exalt_dhcp_is_support();

void print_error(const char* file,const char* fct, int line, const char* msg, ...);



#endif

/** @} */

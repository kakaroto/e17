/** @file Exalt.h */

#ifndef EXALT_H
#define EXALT_H

/**
 * @defgroup Exalt_Macros
 * @brief A list of macros used by libexalt, libexalt_dbus and the daemon
 * @ingroup Exalt
 * @{
 */
#define EXALT_FREE(p) do { if(p){ free(p); p=NULL;} }while(0)
#define EXALT_PCLOSE(p) do {if(p){pclose(p); p=NULL;} }while(0)
#define EXALT_DELETE_TIMER(a) do {if(a) {ecore_timer_del(a); a=NULL; } }while(0)
#define EXALT_CLOSE(p) do {if(p>=0){close(p); p=0;} }while(0)
#define EXALT_FCLOSE(p) do {if(p){fclose(p); p=NULL;} }while(0)
#define EXALT_STRDUP(a,b) do { const char* str99 = b; if(str99){ a=strdup(str99);} else {a=NULL;}}while(0)

#define EXALT_PATH_PROCNET_DEV "/proc/net/dev"
#define EXALT_PATH_ROUTE "/proc/net/route"
#define EXALT_WPA_IFACE_DIR "/var/run/wpa_supplicant"
#define EXALT_TEMP_FILE "/tmp/exalt_temp"

#define EXALT_LOG_WARN(...) EINA_LOG_DOM_WARN(EXALT_LOG_DOMAIN, __VA_ARGS__)
#define EXALT_LOG_CRIT(...) EINA_LOG_DOM_CRIT(EXALT_LOG_DOMAIN, __VA_ARGS__)
#define EXALT_LOG_INFO(...) EINA_LOG_DOM_INFO(EXALT_LOG_DOMAIN, __VA_ARGS__)
#define EXALT_LOG_ERR(...) EINA_LOG_DOM_ERR(EXALT_LOG_DOMAIN, __VA_ARGS__)
#define EXALT_LOG_DBG(...) EINA_LOG_DOM_DBG(EXALT_LOG_DOMAIN, __VA_ARGS__)

#define EXALT_ASSERT(test) \
    do\
    {\
        if(!(test))\
        {\
            EXALT_LOG_WARN("%s failed",#test); \
        }\
    }while(0)



#define EXALT_ASSERT_RETURN(test) \
    do\
    {\
        if(!(test))\
        {\
            EXALT_LOG_WARN("%s failed",#test); \
            return 0;\
        }\
    }while(0)

#define EXALT_ASSERT_RETURN_VOID(test) \
    do\
    {\
        if(!(test))\
        {\
            EXALT_LOG_WARN("%s failed",#test); \
            return ;\
        }\
    }while(0)



#define EXALT_ASSERT_ADV(test, instr, ...) \
    do \
    { \
        if(!(test))\
        {\
            EXALT_LOG_WARN(__VA_ARGS__); \
            instr; \
        }\
    }while(0)

#define EXALT_ASSERT_CUSTOM_RET(test, instr) \
    do \
    { \
        if(!(test))\
        {\
            EXALT_LOG_WARN("%s failed",#test); \
            instr; \
        }\
    }while(0)




#define EXALT_STRING_SET(attribut)                      \
    EXALT_STRING_SET1(EXALT_FCT_NAME,attribut)

/// @cond
#define EXALT_STRING_SET1(exalt_file_name,attribut)     \
    EXALT_STRING_SET2(exalt_file_name,attribut)

#define EXALT_STRING_SET2(exalt_file_name,attribut)     \
    void exalt_file_name##_##attribut##_set(            \
            EXALT_STRUCT_TYPE *s,                       \
            const char* attribut)                       \
    {                                                   \
        EXALT_ASSERT_RETURN_VOID(!!s);              \
        EXALT_FREE(s->attribut);                        \
        if(attribut)                              \
            s->attribut = strdup(attribut);             \
        else                                            \
            s->attribut=NULL;                           \
    }
/// @endcond

#define EXALT_SET(attribut,type)                        \
    EXALT_SET1(EXALT_FCT_NAME,attribut,type)

/// @cond
#define EXALT_SET1(exalt_file_name,attribut,type)       \
    EXALT_SET2(exalt_file_name,attribut,type)

#define EXALT_SET2(exalt_file_name,attribut,type)       \
    void exalt_file_name##_##attribut##_set(            \
            EXALT_STRUCT_TYPE *s,                       \
            type attribut)                              \
    {                                                   \
        EXALT_ASSERT_RETURN_VOID(!!s);              \
        s->attribut=attribut;                           \
    }
/// @endcond

#define EXALT_TAB_SET(attribut,type)                    \
    EXALT_TAB_SET1(EXALT_FCT_NAME,attribut,type)

/// @cond
#define EXALT_TAB_SET1(exalt_file_name,attribut,type)   \
    EXALT_TAB_SET2(exalt_file_name,attribut,type)

#define EXALT_TAB_SET2(exalt_file_name,attribut,type)   \
    void exalt_file_name##_##attribut##_set(            \
            EXALT_STRUCT_TYPE *s,                       \
            type attribut,                              \
            int i)                                      \
    {                                                   \
        EXALT_ASSERT_RETURN_VOID(!!s);              \
        s->attribut[i]=attribut;                        \
    }
/// @endcond


#define EXALT_GET(attribut,type)                        \
    EXALT_GET1(EXALT_FCT_NAME,attribut,type)
/// @cond
#define EXALT_GET1(exalt_file_name,attribut,type)       \
    EXALT_GET2(exalt_file_name,attribut,type)

#define EXALT_GET2(exalt_file_name,attribut,type)       \
    type exalt_file_name##_##attribut##_get(            \
            EXALT_STRUCT_TYPE *s)                       \
    {                                                   \
        EXALT_ASSERT_RETURN(!!s);                   \
        return s->attribut;                             \
    }
/// @endcond
#define EXALT_TAB_GET(attribut,type)                    \
    EXALT_TAB_GET1(EXALT_FCT_NAME,attribut,type)
/// @cond
#define EXALT_TAB_GET1(exalt_file_name,attribut,type)   \
    EXALT_TAB_GET2(exalt_file_name,attribut,type)

#define EXALT_TAB_GET2(exalt_file_name,attribut,type)   \
    type exalt_file_name##_##attribut##_get(            \
            EXALT_STRUCT_TYPE *s,                       \
            int i)                                      \
    {                                                   \
        EXALT_ASSERT_RETURN(!!s);                   \
        return s->attribut[i];                          \
    }
/// @endcond

#define EXALT_IS(attribut,type)                         \
    EXALT_IS1(EXALT_FCT_NAME,attribut,type)
/// @cond
#define EXALT_IS1(exalt_file_name,attribut,type)        \
    EXALT_IS2(exalt_file_name,attribut,type)

#define EXALT_IS2(exalt_file_name,attribut,type)        \
    type exalt_file_name##_##attribut##_is(             \
            EXALT_STRUCT_TYPE *s)                       \
    {                                                   \
        EXALT_ASSERT_RETURN(!!s);                   \
        return s->attribut;                             \
    }
/// @endcond
/** @} */

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
#include "exalt_configuration_network.h"
#include "exalt_configuration.h"
#include "exalt_vpnc.h"

/**
 * @defgroup Exalt
 * @brief Exalt is a low level library used by the daemon exalt and some parts are using by applications.<br>
 * This library allows to manages the ethernets and wireless interfaces.
 */


/**
 * @defgroup Exalt_General
 * @brief Main functions and some utils.
 * @ingroup Exalt
 * @{
 */

/**
 * @brief initialise the library
 */
EAPI int exalt_init();
/**
 * @brief Launch the library
 * load cards and watch events
 */
EAPI int exalt_main();

/**
 * @brief Test if a string is a valid address
 * @param address the string
 * return Return 1 if sucess, else 0
 */
EAPI short exalt_address_is(const char* address);
/**
 * @brief Test if a string is a valid essid
 * @param essid the essid
 * @return Return 1 if success, else 0
 */
EAPI short exalt_essid_is(const char* essid);

/**
 * @brief Test if we have the administrator rights
 * @return Return 1 if yes, else 0
 */
EAPI short exalt_admin_is();


//EAPI short exalt_key_is(const char* key, Exalt_Enum_Encryption_Mode encryption_mode);

/**
 * @brief Test if libexalt is built with the support of dhcp
 * @return Return 1 if yes, else 0
 */
EAPI short exalt_dhcp_support_is();

#endif

/** @} */

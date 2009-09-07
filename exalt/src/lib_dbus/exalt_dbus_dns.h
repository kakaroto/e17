/** @file exalt_dbus_dns.h */

#ifndef  EXALT_DBUS_DNS_INC
#define  EXALT_DBUS_DNS_INC

#include "Exalt_DBus.h"

/**
 * @defgroup DNS
 * @brief The purpose of these methods is to manage the list of DNS manually. Generally the DNS are get from a DHCP but in some cases we need to add or delete a DNS manually. These methods return a response which contains an error if the operation failed or success.  Some responses can have extra data as the list of DNS for exalt_dbus_dns_list_get().
 * @ingroup Exalt_DBus
 * @{
 */

EAPI int exalt_dbus_dns_list_get(Exalt_DBus_Conn* conn);
EAPI int exalt_dbus_dns_add(Exalt_DBus_Conn* conn, const char* dns);
EAPI int exalt_dbus_dns_delete(Exalt_DBus_Conn* conn, const char* dns);
EAPI int exalt_dbus_dns_replace(Exalt_DBus_Conn* conn, const char* old_dns, const char* new_dns);

#endif   /* ----- #ifndef EXALT_DBUS_DNS_INC  ----- */

/** @} */

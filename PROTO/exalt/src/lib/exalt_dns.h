/** @file exalt_dns.h */
#ifndef DNS_H
#define DNS_H

#include "libexalt.h"
#include <string.h>

/**
 * @defgroup Exalt_DNS
 * @brief manage the dns list (add, delete, print)
 * @{
 */

#define EXALT_RESOLVCONF_FILE "/etc/resolv.conf"

Ecore_List* exalt_dns_get_list();

int exalt_dns_add(const char* dns);
int exalt_dns_delete(const char* dns);
int exalt_dns_replace(const char* old_dns, const char* new_dns);
void exalt_dns_printf();

/** @} */
#endif


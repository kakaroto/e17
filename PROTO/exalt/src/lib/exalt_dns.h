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

/**
 * @brief get the dns list
 * @return Return the dns list
 */
Ecore_List* exalt_dns_get_list();

/**
 * @brief add a dns
 * @param dns the news dns
 * @return Return 1 if the dns is add, else -0
 */
int exalt_dns_add(const char* dns);
/**
 * @brief delete a dns
 * @param dns the dns
 * @return Return 1 if the dns is delete, else 0
 */
int exalt_dns_delete(const char* dns);
/**
 * @brief replace a dns by a new
 * @param old_dns the old dns
 * @param new_dns the new dns
 * @return Return 1 if the dns is replace, else 0
 */
int exalt_dns_replace(const char* old_dns, const char* new_dns);

/**
 * @brief print the dns list in the standard output
 */
void exalt_dns_printf();

/** @} */
#endif


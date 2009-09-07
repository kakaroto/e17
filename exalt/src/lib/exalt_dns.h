/** @file exalt_dns.h */
#ifndef DNS_H
#define DNS_H

#include "Exalt.h"
#include <string.h>

/**
 * @defgroup Exalt_DNS
 * @brief Manage the dns list (add, delete, load, print). You do not need to init the library (exalt_init()) to use these methods.
 * @ingroup Exalt
 * @{
 */

#define EXALT_RESOLVCONF_FILE "/etc/resolv.conf"

/**
 * @brief Get the dns list
 * @return Returns the dns list, list of char*
 */
EAPI Eina_List* exalt_dns_get_list();

/**
 * @brief Add a dns
 * @param dns the news dns
 * @return Returns 1 if the dns is added, else 0
 */
EAPI int exalt_dns_add(const char* dns);
/**
 * @brief Delete a dns
 * @param dns the dns
 * @return Returns 1 if the dns is deleted, else 0
 */
EAPI int exalt_dns_delete(const char* dns);
/**
 * @brief Replace a dns by a new
 * @param old_dns the old dns
 * @param new_dns the new dns
 * @return Returns 1 if the dns is replaced, else 0
 */
EAPI int exalt_dns_replace(const char* old_dns, const char* new_dns);

/**
 * @brief Print the dns list in the standard output
 */
EAPI void exalt_dns_printf();

/** @} */
#endif


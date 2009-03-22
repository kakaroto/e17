/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_bootprocess.h
 *
 *    Description:  manage the boot process, during the boot process the system can wait a card.
 *
 *        Version:  1.0
 *        Created:  01/04/2008 01:02:05 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_dbus_bootprocess.h */
#ifndef  EXALT_DBUS_BOOTPROCESS_INC
#define  EXALT_DBUS_BOOTPROCESS_INC

#include "libexalt_dbus.h"

/**
 * @defgroup Boot_process
 * @brief When exalt-daemon starts it can wait than an interface is initialise (have an ip address).
 * You can configure a list of interface and a timeout with these functions
 * @{
 */

int exalt_dbus_bootprocess_iface_add(const Exalt_DBus_Conn* conn, const char* interface);
int exalt_dbus_bootprocess_iface_remove(const Exalt_DBus_Conn* conn, const char* interface);
int exalt_dbus_bootprocess_iface_is(const Exalt_DBus_Conn* conn, const char* interface);
int exalt_dbus_bootprocess_timeout_set(const Exalt_DBus_Conn* conn, int timeout);
int exalt_dbus_bootprocess_timeout_get(const Exalt_DBus_Conn* conn);

#endif   /* ----- #ifndef EXALT_DBUS_BOOTPROCESS_INC  ----- */
/** @} */

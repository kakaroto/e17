/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_wireless.h
 *
 *    Description: All functions about a wireless device
 *
 *        Version:  1.0
 *        Created:  08/31/2007 12:34:00 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_dbus_wireless.h */

#ifndef  EXALT_DBUS_WIRELESS_INC
#define  EXALT_DBUS_WIRELESS_INC

#include <libexalt_dbus.h>

/**
 * @defgroup Wireless_interface
 * @brief Functions specific to a wireless interface.
 * @{
 */

int exalt_dbus_wireless_list_get(exalt_dbus_conn* conn);

int exalt_dbus_wireless_essid_get(const exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_wireless_wpasupplicant_driver_get(const exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_wireless_wpasupplicant_driver_set(const exalt_dbus_conn* conn, const char* eth, const char* driver);

int exalt_dbus_wireless_scan_start(const exalt_dbus_conn* conn, const char* eth);
Ecore_List* exalt_dbus_wireless_scan_wait(const exalt_dbus_conn* conn, const char* eth);

#endif   /* ----- #ifndef EXALT_DBUS_WIRELESS_INC  ----- */

/** @} */


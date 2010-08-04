#ifndef  DOC_INC
#define  DOC_INC

/**
 * @mainpage Exalt
 * @author Jonathan Atton
 * @date 2008-2009
 *
 * @section exalt_toc_sec Table of contents
 *
 * <ul>
 *  <li> @ref Exalt
 *  <ul>
 *      <li> @ref Exalt_General
 *      <li> @ref Exalt_Macros
 *      <li> @ref Exalt_Ethernet
 *      <li> @ref Exalt_Wireless
 *      <li> @ref Exalt_Wireless_Network
 *      <li> @ref Exalt_Configuration
 *      <li> @ref Exalt_Configuration_Network
 *      <li> @ref Exalt_DNS
 *      <li> @ref Exalt_Regexp
 *      <li> @ref Exalt_System_Configuration
 *  </ul>
 *  <li> @ref Exalt_DBus
 *  <ul>
 *      <li> @ref General
 *      <li> @ref Defines
 *      <li> @ref Ethernet_Interface
 *      <li> @ref Wireless_Interface
 *      <li> @ref Network
 *      <li> @ref Response
 *      <li> @ref DNS
 *  </ul>
 *  <li> Exalt_DBus examples (source code)
 *  <ul>
 *      <li> @ref tutorial_configuration_get
 *      <Li> @ref tutorial_notifications
 *      <li> @ref tutorial_wireless_scan
 *  </ul>
 * </ul>
 *
 *
 * @section exalt_intro_sec Introduction
 *
 * The Exalt library is a library that implement an API for managing the network interfaces. This package contains 2 libraries and a daemon. The both libraries are :
 * @li Exalt : A low level library for managing the list of interfaces (list the interfaces, set a configuration, get the current configuration, save the configuration ...)
 * @li Exalt_DBus : A high level library that hide the DBus API of the daemon.
 *
 * The daemon provides a DBus API to the clients. The aims of the daemon are :
 * @li Gives an easy way to an application for managing the network interfaces. The application does not need to be run as root as the commands will be execute by the daemon, that's means all users can manage the interfaces.
 * @li Manages the comportment of an interface. If an interface is linked, apply the last known configuration for example.
 * @li informs the clients when an event occurs. For example if an interface is unlinked.
 *
 *
 * Currently Exalt is compatible :
 * @li classic wired ethernet interface
 * @li WEP with an hexadecimal or a ascii key
 * @li WPA1 or WPA2 personnal
 * @li WPA1 or WPA2 enterprise TLS. This configuration has not been tested because I don't have the hardware. If you have some problems, can test it ... Please contact me.
 *
 *
 * A module for Enlightenment DR17 exists but it is not distribute with this package but it is available in the official enlightenment svn : trunk/E-MODULES-EXTRA/exalt-client. The module is called exalt-client and use Exalt_DBus to communicate with the daemon.
 *
 *
 * @section exalt_install Install & configure
 *
 * <b>Install</b>
 *
 * You have to install Exalt before Exalt-client :
 *
 * @code
 * cd exalt
 * ./autogen.sh
 * make
 * as root: make install
 * @endcode
 *
 * @code
 * cd exalt-client
 * ./autogen.sh
 * make
 * as root: make install
 * @endcode
 *
 *
 * If you want the support of wpa_supplicant or dhcp, you need wpa_supplicant and dhclient. During the autogen.sh Exalt try to find these commands in your path. Maybe these commands are only in the path of the user root. You can specify the path with:
 * @code
 * ./autogen.sh --with-wpa_supplicant=/sbin/wpa_supplicant --with-dhcp=/sbin/dhclient
 * @endcode
 *
 *
 *
 * <b>Configure</b>
 *
 * In order to use the daemon you will have to configure the exalt dbus service. The dbus configuration file should be :
 * @code
 * /etc/dbus-1/system.d/exalt.conf
 * @endcode
 *
 * You can use this default configuration or create your own custom file. The file is available here : data/daemon/dbus/exalt.conf
 *
 * @code
 * <!DOCTYPE busconfig PUBLIC
 * "-//freedesktop//DTD D-BUS Bus Configuration 1.0//EN"
 * "http://www.freedesktop.org/standards/dbus/1.0/busconfig.dtd">
 * <busconfig>
 *      <policy user="root">
 *      <allow own="org.e.Exalt"/>
 *                <allow send_destination="org.e.Exalt"/>
 *                <allow send_interface="org.e.Exalt"/>
 *        </policy>
 *        <policy at_console="true">
 *                <allow send_destination="org.e.Exalt"/>
 *                <allow send_interface="org.e.Exalt"/>
 *        </policy>
 *        <policy context="default">
 *                <!-- <deny own="org.e.Exalt"/> -->
 *                <!-- <deny send_destination="org.e.Exalt"/> -->
 *                <!-- <deny send_interface="org.e.Exalt"/> -->
 *                <allow own="org.e.Exalt"/>
 *                <allow send_destination="org.e.Exalt"/>
 *                <allow send_interface="org.e.Exalt"/>
 *        </policy>
 *  </busconfig>
 *  @endcode
 *
 * If the daemon should be automatically starts by a client if it is not running, copy the file org.e.Exalt.service in /usr/share/dbus-1/system-services/. A example of content :
 *
 * @code
 * [D-BUS Service]
 * Name=org.e.Exalt
 * Exec=/usr/local/sbin/exalt-daemon
 * User=root
 * @endcode
 *
 * <b>Gentoo</b>
 *
 * If you want add the daemon in your init, you can use the script in daemon/data/init.d/gentoo:
 * @code
 * cp data/daemon/data/init.d/gentoo /etc/init.d/exaltd
 * rc-update add exaltd default
 * @endcode
 *
 * <b>Others</b>
 *
 * I'm sorry, I don't have a script for others distribution, if you have one you can send me it :)
 *
 * @section exalt_global_archi Architecture
 *
 * @image html "../archi.png" "Architecture of Exalt"
 *
 * @section exalt_contact Contact
 *
 * Jonathan Atton alias Watchwolf <jonathan[dot]atton[at]gmail[dot]com>
 */

/**
 * @page tutorial_configuration_get Get the configuration of an interface
 * @section example_dbus_conf_get How to get the configuration of an interface
 *
 * See the source file : src/examples/dbus_get_configuration.c
 */

/**
 * @page tutorial_notifications  Retrieve the notifications sent by the daemon
 * @section example_dbus_notifications How to retrieve the notifications sent by the daemon
 *
 * See the source file : src/examples/dbus_notifications.c
 */

/**
 * @page tutorial_wireless_scan Perform a wireless scan
 * @section example_dbus_conf_get How to perform a wireless scan on an interface and retrieve the result
 *
 * See the source file : src/examples/dbus_wireless_scan.c
 */

#endif   /* ----- #ifndef DOC_INC  ----- */


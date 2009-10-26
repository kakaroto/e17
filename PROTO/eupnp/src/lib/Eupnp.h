/**
 * @page eupnp_license Copyright Information
 *
 * Eupnp - UPnP library
 *
 * Copyright (C) 2009 Andre Dieb Martins <andre.dieb@gmail.com>
 *
 * This file is part of Eupnp.
 *
 * Eupnp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Eupnp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Eupnp.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author André Dieb Martins
 * @date 2009
 *
 */

#ifndef _EUPNP_H
#define _EUPNP_H

#ifdef EAPI
# undef EAPI
#endif

#ifdef __GNUC__
# if __GNUC__ >= 4
#  define EAPI __attribute__ ((visibility("default")))
# else
#  define EAPI
# endif
#else
# define EAPI
#endif

/**
 * @mainpage
 *
 * @section toc Table Of Contents
 *
 * <ol>
 *
 * <li> @subpage eupnp_about </li>
 *     <ol>
 *         <li> @ref eupnp_subsec_about_whatis </li>
 *         <li> @subpage eupnp_license </li>
 *         <li> @ref eupnp_subsec_features </li>
 *         <li> @ref eupnp_subsec_requirements </li>
 *         <li> @ref eupnp_subsec_todo </li>
 *     </ol>
 *
 * <li> @subpage eupnp_app_writing </li>
 *     <ol>
 *         <li> @ref eupnp_sec_initializing </li>
 *         <li> @ref Eupnp_Event_Bus
 *             <ul>
 *                 <li> @ref eupnp_event_bus_subscribe </li>
 *             </ul>
 *         </li>
 *
 *     </ol>
 *
 * <li> @subpage eupnp_advanced_usage </li>
 *     <ol>
 *         <li> @ref Eupnp_Event_Bus
 *             <ul>
 *                 <li> @ref eupnp_event_bus_unsubscribe </li>
 *                 <li> @ref eupnp_event_bus_publish </li>
 *                 <li> @ref eupnp_event_bus_publish_custom </li>
 *             </ul>
 *         <li> @ref Eupnp_SSDP_Client_Module </li>
 *             <ul>
 *                 <li> @ref eupnp_ssdp_initialize </li>
 *                 <li> @ref eupnp_ssdp_instance </li>
 *                 <li> @ref eupnp_ssdp_events </li>
 *             </ul>
 *     </ol>
 * </ol>
 *
 */

/**
 * @page eupnp_advanced_usage Advanced Usage
 *
 * Though Eupnp provides a high level for building UPnP applications, most of its
 * modules are independent. In this chapter we describe each module and how to
 * use it.
 *
 * @li @ref Eupnp_SSDP_Client_Module
 * @li @ref Eupnp_Event_Bus
 * @li @ref Eupnp_UDP_Transport
 */

/**
 * @page eupnp_about About Eupnp
 *
 * @section eupnp_subsec_about_whatis What is Eupnp?
 *
 * Eupnp (aka libeupnp) is a lightweight event-driven UPnP (Universal Plug n' Play) library written in C. 
 *
 * @section eupnp_subsec_features Features
 *
 * @li SSDP client implementation (Simple Service Discovery Protocol)
 * @li UPnP stack implementation: discovery, description and control of UPnP services and devices
 * @li Toolkit independent. Core interface for integration with any toolkit (Ecore, Glib, Qt) - already integrated with Ecore (aka libeupnp-ecore)
 *
 * @section eupnp_subsec_requirements Requirements
 *
 * @li Eina
 * @li Ecore, Ecore-con (with curl support)
 * @li libxml-2.0
 *
 * @section eupnp_subsec_todo TODO
 *
 * @li Finish GUI examples (IGD client, universal control point, lighting client)
 * @li Implement device icon fetch
 * @li Implement libeupnp-av (UPnP Audio/Video specification)
 * @li Fix building facility to not depend on Elementary, make it optional and disable build of elm-based examples when not present
 * @li Screenshots of examples
 * @li Finish API documentation
 * @li Tutorial
 *
 */

#include "eupnp_core.h"
#include "eupnp_udp_transport.h"
#include "eupnp_http_message.h"
#include "eupnp_utils.h"
#include "eupnp_log.h"
#include "eupnp_event_bus.h"
#include "eupnp_ssdp.h"
#include "eupnp_control_point.h"
#include "eupnp_service_proxy.h"
#include "eupnp_service_info.h"
#include "eupnp_device_info.h"

EAPI int eupnp_init(void);
EAPI int eupnp_shutdown(void);

#endif /* _EUPNP_H */

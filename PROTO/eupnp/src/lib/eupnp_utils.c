/* Eupnp - UPnP library
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
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_private.h"


/*
 * Returns the length of <scheme>://<netloc> from a complete URL.
 */
int
eupnp_utils_url_base_get(const char *url)
{
   if (!url) return 0;
   const char *p;

   /* Base url is <scheme>://<netloc>/ */
   p = strchr(url, ':');

   if (!p) return 0;

   if (*(p + 1) != '/') return 0;
   if (*(p + 2) != '/') return 0;

   p = strchr(p + 3, '/');

   if (!p) return 0;

   return p - url;
}

/*
 * Returns EINA_TRUE if the url is relative, that is, does not contain
 * <scheme>://<netloc>.
 */
Eina_Bool
eupnp_utils_url_is_relative(const char *url)
{
   if (!eupnp_utils_url_base_get(url)) return EINA_TRUE;
   return EINA_FALSE;
}

/*
 * Returns the current system time
 */
double
eupnp_time_get(void) {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return (double)tv.tv_sec + (((double)tv.tv_usec) / 1000000);
}


#define LOOPBACK_IP "127.0.0.1"


/*
 * Get the host IP for the specified interface, or the first up and non-loopback
 * interface if no name is specified.
 *
 * Copyright (C) 2006, 2007, 2008 OpenedHand Ltd.
 *
 * Author: Jorn Baayen <jorn@openedhand.com>
 *
 */
static char *
get_host_ip(const char *name)
{
   struct ifaddrs *ifa_list, *ifa;
   char *ret;

   ret = NULL;

   if (getifaddrs (&ifa_list) != 0)
     {
	fprintf(stderr, "Failed to retrieve list of network interfaces:%s\n",
		strerror (errno));
	return NULL;
   }

   for (ifa = ifa_list; ifa; ifa = ifa->ifa_next)
     {
	char ip[INET6_ADDRSTRLEN];
	const char *p;
	struct sockaddr_in *s4;
	struct sockaddr_in6 *s6;

	if (!ifa->ifa_addr) continue;

	if ((ifa->ifa_flags & IFF_LOOPBACK) || !(ifa->ifa_flags & IFF_UP)) continue;

	/* If a name was specified, check it */
	if (name && strcmp (name, ifa->ifa_name) != 0) continue;

	p = NULL;

	switch (ifa->ifa_addr->sa_family)
	  {
	     case AF_INET:
	       s4 = (struct sockaddr_in *) ifa->ifa_addr;
	       p = inet_ntop(AF_INET, &s4->sin_addr, ip, sizeof (ip));
	       break;
	     case AF_INET6:
	       s6 = (struct sockaddr_in6 *) ifa->ifa_addr;
	       p = inet_ntop(AF_INET6, &s6->sin6_addr, ip, sizeof (ip));
	       break;
	     default:
	       continue; /* Unknown: ignore */
	  }

	if (p)
	  {
	    ret = strdup(p);
	    break;
	  }
     }

   freeifaddrs(ifa_list);

   if (!ret)
     {
	/* Didn't find anything. Let's take the loopback IP. */
	ret = strdup(LOOPBACK_IP);
     }

   return ret;
}

/*
* Get the host IP of the interface used for the default route.  On any error,
* the first up and non-loopback interface is used.
*
* Copyright (C) 2006, 2007, 2008 OpenedHand Ltd.
*
* Author: Jorn Baayen <jorn@openedhand.com>
*
*/
const char *
eupnp_utils_default_host_ip_get(void)
{
   FILE *fp;
   int ret;
   char dev[32];
   unsigned long dest;
   Eina_Bool found = EINA_FALSE;

   fp = fopen ("/proc/net/route", "r");

   /* Skip the header */
   if (fscanf (fp, "%*[^\n]\n") == EOF)
     {
	 fclose (fp);
	 return NULL;
     }

   while ((ret = fscanf(fp,
			"%31s %lx %*x %*X %*d %*d %*d %*x %*d %*d %*d",
			dev, &dest)) != EOF)
     {
	/* If we got a device name and destination, and the destination
	 * is 0, then we have the default route
	 */
	if (ret == 2 && dest == 0)
	  {
	     found = EINA_TRUE;
	      break;
	  }
     }

   fclose (fp);

   return get_host_ip (found ? dev : NULL);
}

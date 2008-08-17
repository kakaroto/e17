/****************************************************************************
 ** enna_lirc.c *************************************************************
 ****************************************************************************
 *
 * enna_lirc  - adds lirc support to ennas
 *
 * Copyright (C) 2007 Philippe Bridant <philippe.bridant@smartjog.com>
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

#include "enna.h"

EAPI int
enna_connect(char *hostname, int port, int block, char *err, int err_len)
{
   int                 sd;
   int                 flags;
   struct sockaddr_in  sockaddr_connect;
   struct hostent     *he;

   he = gethostbyname(hostname);
   sockaddr_connect.sin_port = htons(port);
   sockaddr_connect.sin_family = AF_INET;
   bcopy(he->h_addr, &sockaddr_connect.sin_addr.s_addr,
	 sizeof(sockaddr_connect.sin_addr.s_addr));

   // Preparing socket
   if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
     {
	dbg("Socket creation errror !\n");
	return -1;
     }

   // Connecting to lirc server
   if (connect
       (sd, (struct sockaddr *)&sockaddr_connect, sizeof(sockaddr_connect)) < 0)
     {
	dbg("Socket connection error !\n");
	return -1;
     }

   if (!block)
      return sd;
   // Setting the socket in non blocking mode
   fcntl(sd, F_SETOWN, getpid());
   flags = fcntl(sd, F_GETFL, 0);
   if (flags != -1)
     {
	fcntl(sd, F_SETFL, flags | O_NONBLOCK);
     }
   else
     {
	dbg("Cannot set lirc socket to non block mode\n");
	return sd;
     }
   return sd;
}

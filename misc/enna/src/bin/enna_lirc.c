/*
 * enna_lirc.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_lirc.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_lirc.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>
#include "enna.h"
#include "enna_event.h"
#define DEBOUNCE_TIME 0.3

#ifdef HAVE_LIRC

#include <lirc/lirc_client.h>

static int          _lirc_code_received(void *data,
					Ecore_Fd_Handler * fd_handler);

static int
_lirc_code_received(void *data, Ecore_Fd_Handler * fd_handler)
{
   Enna               *enna = (Enna *) data;
   char               *code, *event;
   int                 ret = -1;
   static double       last_time = 0.0;
   double              time = 0.0;

   while (lirc_nextcode(&code) == 0 && code != NULL)
     {
	while ((ret = lirc_code2char(enna->lirc_cfg, code, &event)) == 0
	       && event != NULL)
	  {
	     time = ecore_time_get();
	     if ((time - last_time) > DEBOUNCE_TIME)
	       {
		  enna_event_lirc_code((void *)enna, event);
		  last_time = time;
	       }
	  }
     }
   return 1;
}

/* LIRC client code */
EAPI void
enna_lirc_init(Enna * enna)
{
   int                 fd;

   if ((fd = lirc_init("enna", 1)) == -1)
     {
	dbg("could not initialize LIRC support\n");
	return;
     }

   if (lirc_readconfig(NULL, &enna->lirc_cfg, NULL) != 0)
     {
	lirc_deinit();
	dbg("could not find Lirc config file\n");
     }

   enna->lirc_fd = fd;
   fcntl(fd, F_SETFL, O_NONBLOCK);
   fcntl(fd, F_SETFD, FD_CLOEXEC);

   ecore_main_fd_handler_add(fd,
			     ECORE_FD_READ,
			     _lirc_code_received, enna, NULL, NULL);
   enna->lirc_timer = NULL;
   enna->lirc_actions = 0;
   enna->event_prec = NULL;
   //enna->lirc_timer = ecore_timer_add(0.01, _check_lirc_data, enna); 

   return;
}

#else

EAPI void
enna_lirc_init(Enna * enna)
{
   dbg("LIRC client is not present! No LIRC support\n");
}

EAPI void
enna_deinit_lirc()
{

}

#endif

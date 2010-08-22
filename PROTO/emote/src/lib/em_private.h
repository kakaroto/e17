#ifndef EMOTE_MAIN_H
#define EMOTE_MAIN_H

#include "config.h"

#include "Emote.h"

#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <alloca.h>
#include <linux/limits.h>
#if HAVE_DLFCN_H
#include <dlfcn.h>
#endif

struct
{
  char libdir[PATH_MAX];
  char datadir[PATH_MAX];
  char protocoldir[PATH_MAX];
} em_paths;

#endif

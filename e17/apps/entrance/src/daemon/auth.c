#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <X11/Xlib.h>
#include <X11/Xauth.h>
#include <X11/extensions/security.h>

#include <Ecore_X.h>
#include <Ecore_Ipc.h>

#include "spawner.h"
#include "md5.h"

#define BUFSIZE 512

char * entranced_cookie_new (void)
{
   int fd;
   int r, i;
   unsigned char buf[BUFSIZE];
   unsigned char digest[MD5_HASHBYTES];
   char *cookie;
   Entranced_MD5_Context *ctx = NULL;

   entranced_md5_init(ctx);

   if ((fd = open("/dev/urandom", O_RDONLY|O_NONBLOCK)) < 0) {
      _DEBUG("Cookie generation failed: could not open /dev/urandom\n");
      exit(-1);
   }

   if((r = read(fd, buf, sizeof(buf))) <= 0) {
      _DEBUG("Cookie geneartion failed: could not read /dev/urandom\n");
      exit(-1);
   }

   entranced_md5_update(ctx, buf, r);
   entranced_md5_final(digest, ctx);

   cookie = calloc(40, 1);
   
   for (i = 0; i < MD5_HASHBYTES; ++i) {
      char tmp[3];
      snprintf(tmp, sizeof(tmp), "%02x", (unsigned int) digest[i]);
      strncat(cookie, tmp, 2);
   }

   return cookie;
}

static int _entranced_auth_entry_add() {
   /* Code */
   return 0;
}

void entranced_auth_display_generate() {
   /* Code */
}

void entranced_auth_user_add() {
   /* Code */
}

static int _entranced_auth_purge() {
   /* Code */
   return 0;
}


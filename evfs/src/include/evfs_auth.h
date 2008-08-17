#ifndef __EVFS_AUTH_H_
#define __EVFS_AUTH_H_

/*This structure needs more development*/
typedef struct evfs_auth_cache
{
   char* plugin;
   char *path;
   char *username;
   char *password;
   int attempts;
} evfs_auth_cache;

#endif

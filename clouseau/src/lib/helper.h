/*

  HELPER.H
  ========
  (c) Paul Griffiths, 1999
  Email: paulgriffiths@cwcom.net

  Interface to socket helper functions. 

  Many of these functions are adapted from, inspired by, or 
  otherwise shamelessly plagiarised from "Unix Network 
  Programming", W Richard Stevens (Prentice Hall).

*/


#ifndef PG_SOCK_HELP
#define PG_SOCK_HELP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>             /*  for ssize_t data type  */

/*  Global constants  */
#define PORT           (8080)
#define MAX_LINE       (1023)
#define LOCALHOST      "127.0.0.1"

enum _client_type
{
   DAEMON,
   APP,  /* Application runs PRELOAD with clouseau */
   GUI   /* The GUI client showing info */
};
typedef enum _client_type client_type;

enum _message_type
{
   ACK,          /* Acknoledge */
   TREE_DATA,    /* Tree daya req/fwd */
};
typedef enum _message_type message_type;

struct _packet
{  /* Packet is BLOB contains: client, type, size of str that follows */
   client_type client;
   message_type type;
   size_t size;
};
typedef struct _packet packet;

/*  Function declarations  */
size_t compose_packet(void **ptr, client_type c, message_type m, void *data, size_t s);
void *get_packet_data(void *ptr);
ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);
#endif  /*  PG_SOCK_HELP  */

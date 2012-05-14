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


#include <unistd.h>             /*  for ssize_t data type  */

/*  Global constants  */
#define ECHO_PORT      (5555)
#define MAX_LINE       (1024)
#define LISTENQ        (1024)   /*  Backlog for listen()   */
#define END_OF_MESSAGE "END_OF_MESSAGE"

/*  Function declarations  */

ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);
#endif  /*  PG_SOCK_HELP  */

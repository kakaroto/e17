#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int
main(int argc, char **argv)
{
   pid_t pid;

   if (argc != 2)
      return 0;

   pid = atoi(argv[1]);
   if (waitpid(pid, NULL, 0) == pid)
      exit(0);
   return -1;
}

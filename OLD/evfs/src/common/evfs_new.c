#include "evfs.h"

evfs_command *
evfs_command_new()
{
   evfs_command *com = NEW(evfs_command);

   return com;
}

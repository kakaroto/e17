#include <evfs.h>

static int evfs_vfolder_init = 0;
static Eet_Data_Descriptor *_evfs_vfolder_edd;

void
evfs_vfolder_initialise()
{
   if (evfs_vfolder_init)
      return;
   evfs_vfolder_init = 1;

}

evfs_vfolder *
evfs_vfolder_new()
{
   return NEW(evfs_vfolder);
}

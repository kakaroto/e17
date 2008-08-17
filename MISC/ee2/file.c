/* EE2 file cruft */

#include "ee2.h"

void
GetFileStats(char *file1)
{
  struct stat st;
  stat(file1, &st);
  EFile.Size = st.st_size;
  EFile.ModTime = st.st_mtime;
}

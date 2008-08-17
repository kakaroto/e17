#include "util.h"

static mode_t       default_mode =
   S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

char *
get_user_home()
{
  static char *home = NULL;

  if (home)
    return home;

  home = getenv("HOME");
  if (!home)
    home = getenv("TMPDIR");
  if (!home)
    home = "/tmp";
  return home;
}

Ecore_Exe *
exec_run_in_dir(char *exec, char *dir)
{
   Ecore_Exe *exe;

   exe = ecore_exe_run(exec, NULL);
   return exe;
}

int
e_file_exists(char *file)
{
   struct stat         st;

   if (stat(file, &st) < 0)
      return 0;

   return 1;
}

int
e_file_mkdir(char *dir)
{
  if (mkdir(dir, default_mode) < 0)
    return 0;

  return 1;
}

int
e_file_cp(char *src, char *dst)
{
  FILE               *f1, *f2;
  char                buf[16384];
  size_t              num;

  f1 = fopen(src, "rb");
  if (!f1)
     return(0);
  f2 = fopen(dst, "wb");
  if (!f2)
    {
       fclose(f1);
       return(0);
    }
  while ((num = fread(buf, 1, 16384, f1)) > 0)
     fwrite(buf, 1, num, f2);
  fclose(f1);
  fclose(f2);

  return 1;
}

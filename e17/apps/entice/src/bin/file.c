#include "entice.h"

time_t
e_file_modified_time(char *file)
{
   struct stat         st;

   if (stat(file, &st) < 0)
      return 0;
   return st.st_mtime;
}

char               *
e_file_home(void)
{
   static char        *home = NULL;

   if (home)
      return home;
   home = getenv("HOME");
   if (!home)
      home = getenv("TMPDIR");
   if (!home)
      home = "/tmp";
   return home;
}

int
e_file_mkdir(char *dir)
{
   if (mkdir
       (dir,
	S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) <
       0)
      return 0;
   return 1;
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
e_file_is_dir(char *file)
{
   struct stat         st;

   if (stat(file, &st) < 0)
      return 0;
   if (S_ISDIR(st.st_mode))
      return 1;
   return 0;
}

char *
e_file_full_name(char *file)
{
   char *buf = 0;
   char *wd = 0;
   
   if (file[0] == '/') return file;
   buf = (char *) malloc(4096);
   wd = getcwd(NULL, 0);
   snprintf(buf, 4096, "%s/%s", wd, file);
   return buf;
}

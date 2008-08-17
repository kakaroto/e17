#include "Eet.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <fnmatch.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int eet_mkdir(char *dir);
void eet_mkdirs(char *s);

void depak_file(Eet_File *ef, char *file);
void depack(char *pak_file);

void list(char *pak_file);

void pak_file(Eet_File *ef, char *file, char **noz, int noz_num);
void pak_dir(Eet_File *ef, char *dir, char **noz, int noz_num);
void pack(char *pak_file, char **files, int count, char **noz, int noz_num);

int
eet_mkdir(char *dir)
{
#ifdef __MINGW32__
   if (mkdir(dir) < 0) return 0;
#else
   mode_t default_mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP
                         | S_IROTH | S_IXOTH;

   if (mkdir(dir, default_mode) < 0) return 0;
#endif

   return 1;
}

void
eet_mkdirs(char *s)
{
   char ss[PATH_MAX];
   int  i, ii;

   i = 0;
   ii = 0;
   while (s[i])
     {
	ss[ii++] = s[i];
	ss[ii] = 0;
	if (s[i] == '/') eet_mkdir(ss);
	i++;
     }
}

void
depak_file(Eet_File *ef, char *file)
{
   void *data;
   int size;
   char *last;

   data = eet_read(ef, file, &size);
   if (data)
     {
	FILE *f;
	char buf[PATH_MAX];
	int len;

	strncpy(buf, file, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = 0;
	if (buf[0] == '/') return;
	if (!strcmp(buf, "..")) return;
	if (!strncmp(buf, "../", 3)) return;
	if (strstr(buf, "/../")) return;
	len = strlen(buf);
	if (len >= 3)
	  {
	     if (!strcmp(&(buf[len - 3]), "/..")) return;
	  }
	last = strrchr(buf, '/');
	if (last)
	  {
	     last[1] = 0;
	     eet_mkdirs(buf);
	  }

	f = fopen(file, "wb");
	if (f)
	  {
	     fwrite(data, 1, size, f);
	     fclose(f);
	     printf("exported: %s\n", file);
	  }
	else
	  printf("error exporting: %s\n", file);
	free(data);
     }
   else
     {
	printf("error reading: %s\n", file);
     }
}

void
depack(char *pak_file)
{
   int i, num;
   char **list;
   Eet_File *ef;

   ef = eet_open(pak_file, EET_FILE_MODE_READ);
   if (!ef)
     {
	printf("cannot open for reading: %s\n", pak_file);
	return;
     }
   list = eet_list(ef, "*", &num);
   if (list)
     {
	for (i = 0; i < num; i++)
	  depak_file(ef, list[i]);
	free(list);
     }
   eet_close(ef);
}

void
list(char *pak_file)
{
   int i, num;
   char **list;
   Eet_File *ef;

   ef = eet_open(pak_file, EET_FILE_MODE_READ);
   if (!ef)
     {
	printf("cannot open for reading: %s\n", pak_file);
	return;
     }
   list = eet_list(ef, "*", &num);
   if (list)
     {
	for (i = 0; i < num; i++)
	  printf("%s\n",list[i]);
	free(list);
     }
   eet_close(ef);
}

void
pak_file(Eet_File *ef, char *file, char **noz, int noz_num)
{
   struct stat st;

   if (stat(file, &st) >= 0)
     {
	void *buf;

	buf = malloc(st.st_size);
	if (buf)
	  {
	     FILE *f;

	     f = fopen(file, "rb");
	     if (f)
	       {
		  int compress = 1;
		  int i;

		  for (i = 0; i < noz_num; i++)
		    {
		       if (!fnmatch(noz[i], file, 0))
			 {
			    compress = 0;
			    break;
			 }
		    }
		  fread(buf, 1, st.st_size, f);
		  if (!eet_write(ef, file, buf, st.st_size, compress))
		    printf("error importing: %s\n", file);
		  else
		    {
		       if (compress)
			 printf("compress: %s\n", file);
		       else
			 printf("imported: %s\n", file);
		    }
		  fclose(f);
	       }
	     free(buf);
	  }
     }
}

void
pak_dir(Eet_File *ef, char *dir, char **noz, int noz_num)
{
   DIR *dirp;
   struct dirent *dp;

   dirp = opendir(dir);
   if (!dirp)
     pak_file(ef, dir, noz, noz_num);
   else
     {
	while ((dp = readdir(dirp)))
	  {
	     char buf[PATH_MAX];

	     if ((!strcmp(".", dp->d_name)) || (!strcmp("..", dp->d_name)))
	       {
	       }
	     else
	       {
		  snprintf(buf, sizeof(buf), "%s/%s", dir, dp->d_name);
		  pak_dir(ef, buf, noz, noz_num);
	       }
	  }
     }
}

void
pack(char *pak_file, char **files, int count, char **noz, int noz_num)
{
   Eet_File *ef;
   int i;

   ef = eet_open(pak_file, EET_FILE_MODE_WRITE);
   if (!ef)
     {
	printf("cannot open for writing: %s\n", pak_file);
	return;
     }
   for (i = 0; i < count; i++) pak_dir(ef, files[i], noz, noz_num);
   printf("done.\n");
   eet_close(ef);
}

void
dumpfunc(void *dumpdata, const char *str)
{
   fprintf(dumpdata, "%s\n", str);
}

void
dump(void *data, int size, const char *file)
{
   FILE *f;
   
   f = fopen(file, "w");
   if (f)
     {
	eet_data_text_dump(data, size, dumpfunc, f);
	fclose(f);
     }
}

void *
undump(const char *file, int *size_ret)
{
   FILE *f;
   int len, sz = 0;
   char *st;           
   void *data = NULL;
   
   f = fopen(file, "r");
   if (f)
     {
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	rewind(f);
	st = malloc(len + 1);
	if (st)
	  {
	     fread(st, len, 1, f);
	     st[len] = 0;
	     data = eet_data_text_undump(st, strlen(st), size_ret);
	     free(st);
	  }
	fclose(f);
     }
   return data;
}

int
main(int argc, char **argv)
{
   if (argc == 3)
     {
	if (!strcmp(argv[1], "-d"))
	  {
	     depack(argv[2]);
	     return 0;
	  }
	else if (!strcmp(argv[1], "-l"))
	  {
	     list(argv[2]);
	     return 0;
	  }
     }
   else if (argc == 3)
     {
	char **noz     = NULL;
	int    noz_num = 0;

	if (!strcmp(argv[1], "-c"))
	  {
	     int i;

	     for (i = 3; i < argc; i++)
	       {
		  if (!strcmp(argv[i], "-nz"))
		    {
		       if (i < (argc - 1))
			 {
			    i++;
			    noz_num++;
			    noz = realloc(noz, noz_num * sizeof(char *));
			    noz[noz_num - 1] = argv[i];
			 }
		    }
		  else
		    break;
	       }
	     pack(argv[2], &(argv[i]), argc - i, noz, noz_num);
	     return 0;
	  }
     }
   else if (argc == 5)
     {
	if (!strcmp(argv[1], "-x"))
	  {
	     return 0;
	  }
	else if (!strcmp(argv[1], "-xz"))
	  {
	     return 0;
	  }
	else if (!strcmp(argv[1], "-i"))
	  {
	     return 0;
	  }
	else if (!strcmp(argv[1], "-iz"))
	  {
	     return 0;
	  }
     }
   printf("usage:\n"
	  "  %s -l  in_file\n"
	  "  %s -d  in_file\n"
	  "  %s -x  in_file key outfile\n"
	  "  %s -i  in_file key outfile\n"
	  "  %s -xz in_file key outfile\n"
	  "  %s -iz in_file key outfile\n"
	  "  %s -c out_file [-nz glob [-nz glob ...]] dir_file1 [dir_file2 ...]\n"
	  "\n"
	  "where:\n"
	  "  -l  in_file     list contents of eet file\n"
	  "  -d  in_file     unpack eet file\n"
	  "  -c  out_file    pack up eet file\n"
	  "  -x  in_file     extract a data encoded key to outfile\n"
	  "  -i  in_file     insert a data encoded key from outfile\n"
	  "  -xz in_file     extract a data encoded key to outfile (compressed)\n"
	  "  -iz in_file     insert a data encoded key from outfile (compressed)\n"
	  "  -nz match       don't compress files matching match glob\n"
	  "\n"
	  "example:\n"
	  "  %s -c out.eet -nz \"*.jpg\" things/\n"
	  "  %s -l out.eet\n"
	  "  %s -d out.eet\n",
	  argv[0], argv[0], argv[0],
	  argv[0], argv[0], argv[0],
	  argv[0], argv[0]);
   return -1;
}

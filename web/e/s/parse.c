#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void parse(char *data, size_t size, char *dir);
void process(int fd, char *dir);

void
parse(char *data, size_t size, char *dir)
{
   int i;
   int in_include = 0, start_include = 0;
   
   for (i = 0; i < (int)size; i++)
     {
	if (!in_include)
	  {
	     if (data[i] != '@')
	       putchar((int)data[i]);
	     else
	       {
		  start_include = i;
		  in_include = 1;
	       }
	  }
	else
	  {
	     if (data[i] == '@')
	       {
		  if (i == start_include + 1)
		    putchar((int)'@');
		  else
		    {
		       char *key;
		       
		       key = (char *)malloc(i - start_include);
		       if (key)
			 {
			    int j;
			    
			    for (j = start_include + 1; j < i; j++)
			      key[j - start_include - 1] = data[j];
			    key[j - start_include - 1] = 0;
			    if (key[0] == ':')
			      {
				 char *val;
				 
				 val = getenv(&(key[1]));
				 if (val);
				 printf("%s", val);
			      }
			    else
			      {
				 int f;
				 char *file;
				 
				 file = malloc(strlen(key) + strlen(dir) + 2);
				 if (file)
				   {
				      strcpy(file, dir);
				      strcat(file, "/");
				      strcat(file, key);
				      f = open(file, O_RDONLY);
				      if (f != -1)
					{
					   process(f, dir);
					   close(f);
					}
				      free(file);
				   }
			      }
			    free(key);
			 }
		    }
		  in_include = 0;
	       }
	  }
     }
}

void
process(int fd, char *dir)
{
   off_t offset;
   size_t size;
   char *data;

   /* figure size */
   offset = lseek(fd, 0, SEEK_END);
   if (offset < 0) return;
   size = (size_t)offset;
   /* go back to the start */
   offset = lseek(fd, 0, SEEK_SET);
   data = malloc(size);
   if (!data) return;
   read(fd, data, size);
   parse(data, size, dir);   
   /* done */
   free(data);
}

int
main (int argc, char **argv)
{
   int f;
   char *dir, *file;
   
   if (argc != 3)
     {
	fprintf(stderr, "usage:\n"
		"\t%s include_dir input_file\n", argv[0]);
	exit(0);
     }
   dir = argv[1];
   file = argv[2];
   f = open(file, O_RDONLY);
   if (f != -1)
     {
	process(f, dir);
	close(f);
     }
   return 0;
}

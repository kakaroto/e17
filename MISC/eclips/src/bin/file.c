#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "eclipse.h"


/**
 * get_shortname_for - chop the absolute path off of a bg filename
 * @filename the file to chop
 * Return the shortened filename, caller should free this
 */
char 
  *get_shortname_for(const char *filename)
{
   
   char *tmp, *result = NULL;
   int length, i;
   
   tmp = strdup(filename);
   length = strlen(filename);
   
   /* start from the back, find the first slash */
   for (i = length; tmp[i] != '/'; i--);
   /* chop the slash */
   i++;
   
   result = strdup(&tmp[i]);
   
   free(tmp);
   return (result);
}

/* inspired by code from entice */
Evas_List 
  *eclips_file_add_dir(Evas_List *filelist,char *file)
{
   
   DIR *d = NULL;
   struct dirent *dent = NULL;
   char buf[PATH_MAX], img[PATH_MAX];
   char *filebuf;
   char *tok;
   
   filebuf = strdup(file);

   if(strstr(filebuf,"*"))
     tok = strtok(filebuf,"*");
   else tok = file;
   
   if ((d = opendir(tok)))
     {	     
	printf("Processing dir\n");
	while ((dent = readdir(d)))
	  {		  
	     if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")
		 || (dent->d_name[0] == '.'))
	       continue;
	     snprintf(buf, PATH_MAX, "%s/%s", tok, dent->d_name);
	     if (!eclips_file_is_dir(buf))
	       {
		  printf("%s\n",buf);
		  filelist = evas_list_append(filelist,(void*)strdup(buf));
	       }
	  }	     
	closedir(d);
     }	
   else 
     {
	snprintf(buf, PATH_MAX, "%s", tok);
	printf("%s (single file)\n",buf);
	filelist = evas_list_append(filelist,(void*)strdup(buf));		
     }   
   return filelist;
}      



int
  eclips_file_is_dir(char *file)
{   
   struct stat st;   
   if (file)
     {	
	if (stat(file, &st) < 0)
	  return (0);
	if (S_ISDIR(st.st_mode))
	  return (1);
     }   
   return (0);
}


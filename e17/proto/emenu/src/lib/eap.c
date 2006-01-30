#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* string functions */
#include <Eet.h>

char *
_get_eap_section(char *eap, char *section) 
{
   int i;
   char *tmp, *value;
   Eet_File *eet;
   
   eet = eet_open(eap, EET_FILE_MODE_READ);
   if (!eet) return NULL;
   
   tmp = eet_read(eet, section, &i);
   eet_close(eet);

   if (!tmp) return NULL;
   
   value = malloc(i + 1);
   memcpy(value, tmp, i);
   value[i] = 0;
   if (!value) return NULL;
   return strdup(value);
}

int
_write_eap_section(char *file, char *section, char *value) 
{
   int ret;
   Eet_File *eet;
   
   eet = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (!eet) return 0;
   
   if (!value) 
     {
	eet_delete(eet, section);
	eet_close(eet);
	return 1;
     }

   if ((!strcmp(section, "app/info/startup_notify")) ||
       (!strcmp(section, "app/info/wait_exit"))) 
     {
	int i;
	i = atoi(value);
	ret = eet_write(eet, section, &i, 1, 0);
     }
   else 
     ret = eet_write(eet, section, value, strlen(value), 0);

   if (ret == 0) 
     printf("Error Writting To Eap\n");
   
   eet_close(eet);
   return ret;
}

#include "util.h"

char  ewl_string_cmp(char *a, char *b, int len)
{
	int i, imax;
	if (!a&&!b)	return 1;
	if (!a||!b) return 0;
	imax = len?len:strlen(a);
	for (i=0; i<imax; i++)
		if (a[i]!=b[i]||((!a[i]&&b[i])||(a[i]&&!b[i])))	return 0;
	return 1;
}

char *ewl_string_dup(char *string)
{
	char *rstr = NULL;
	if (!string)	{
		/* FIXME */
	} else {
		rstr = malloc(strlen(string)+1);
		strncpy(rstr,string, strlen(string)+1);
	}
 	return rstr;
}



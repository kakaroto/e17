
#include <stdio.h>
#include <string.h>


char *
remove_blancks (char *str)
{
  char tmp[strlen (str) + 1];
  int  i, k;
  
  k = 0;
  i = 0;
  while (i < strlen (str))
    {
      if (str[i] == ' ')
	{
	  while (str[i] == ' ') i++;
	  i--;
	  tmp[k] = str[i];
	  k++;
	}
      else
	if (str[i] != '\n')
	  {
	    tmp[k] = str[i];
	    k++;
	  }
      i++;
    }
  if (tmp[k-1] == ' ') k--;
  tmp[k] = '\0';

  if (tmp[0] == ' ')
    return strdup (tmp + 1);
  else
    return strdup (tmp);
}

char *
remove_description (char *str)
{
  char *comment;
  char *p;
  int   l;

  if (!str)
    return NULL;
  
  comment = strstr (str, "/**< ");
  if (!comment)
    return strdup (str);

  l = comment - str - 1;
  p = (char *)malloc (sizeof (char) * (l + 1));
  memcpy (p, str, l);
  p[l] = '\0';
  
  return p;
}

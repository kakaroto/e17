#include "Etox_private.h"
#include "Etox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char *
_etox_loadfile_atword(char *s, int num)
{
  int cnt, i;

  if (!s)
    return NULL;
  cnt = 0;
  i = 0;

  while (s[i]) 
    {
      if ((s[i] != ' ') && (s[i] != '\t')) 
	{
	  if (i == 0) 
	    cnt++; 
	  else if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
	    cnt++;
	  if (cnt == num)
	    return &s[i];
	  i++;
	}
    }
  return NULL;
}

int 
_etox_loadfile_is_whitespace(const char *s)
{
  int i = 0;

  while (s[i]) 
    {
      if ((s[i] != ' ') && (s[i] != '\n') && (s[i] != '\t'))
	return 0;
      i++;
    }
  return 1;
}

char *
_etox_loadfile_get_line(char *s, int size, FILE *f)
{
  /* This function will get a single line from the file */

  char *ret, *ss, inquote;
  int i, j, k;
  static int line_stack_size = 0;
  static char **line_stack = NULL;
  
  s[0] = 0;
  if (line_stack_size > 0) 
    {
      strncpy(s, line_stack[0], size);
      free(line_stack[0]);
      for (i = 0; i < line_stack_size - 1; i++)
	line_stack[i] = line_stack[i + 1];
      line_stack_size--;
      if (line_stack_size > 0) 
	line_stack = realloc(line_stack, line_stack_size * sizeof(char *)); 
      else 
	{
	  free(line_stack);
	  line_stack = NULL;
	}
      return s;
    }
  ret = fgets(s, size, f);
  if (strlen(s) > 0)
    s[strlen(s) - 1] = 0;
  while (_etox_loadfile_is_whitespace(s)) 
    {
      s[0] = 0;
      ret = fgets(s, size, f);
      if (!ret)
	return NULL;
      if (strlen(s) > 0)
	s[strlen(s) - 1] = 0;
    }
  i = 0;
  inquote = 0;
  while (s[i]) 
    {
      if (!inquote) 
       {
	 if (s[i] == '"') 
	   {
	     j = i;
	     while (s[j]) 
	       {
		 s[j] = s[j + 1];
		 j++;
	       }
	     inquote = 1;
	     i--;
	   }
        }
      else 
	{
	  if (s[i] == '"') 
	    {
	      j = i + 1;
	      while (s[j]) 
		{
		  if (s[j] == ';')
		    break;
		  if ((s[j] == '"') && (j == (i + 1)))
		    break;
		  if (!isspace(s[j])) 
		    {
		      j--;
		      break;
		    }
		  j++;
		}
	      k = j - i;
	      j = i;
	      while (s[j]) 
		{
		  s[j] = s[j + k];
		  j++;
		}
	      inquote = 0;
	      i--;
	    }
	}
      i++;
    }
  j = strlen(s);
  if (j > 0) 
    {
      if (strchr(s, ';')) 
	{
	  s[j] = ';';
	  s[j + 1] = 0;
	}
    }
  i = 0;
  ss = s;
  while (s[i]) 
    {
      if (s[i] == ';') 
	{
	  j = (&(s[i]) - ss);
	  if (j > 0) 
	    {
	      line_stack_size++;
	      if (!line_stack)
		line_stack = malloc(line_stack_size * sizeof(char *));
	      else
		line_stack = realloc(line_stack,
				     line_stack_size * sizeof(char *));
	      line_stack[line_stack_size - 1] = malloc(j + 1);
	      strncpy(line_stack[line_stack_size - 1], ss, j);
	      line_stack[line_stack_size - 1][j] = 0;
	      ss = &(s[i + 1]);
	    }
	}
      i++;
    }
  if (line_stack_size > 0) 
    {
      strncpy(s, line_stack[0], size);
      free(line_stack[0]);
      for (i = 0; i < line_stack_size - 1; i++)
	line_stack[i] = line_stack[i + 1];
      line_stack_size--;
      if (line_stack_size > 0) 
	line_stack = realloc(line_stack, line_stack_size * sizeof(char *)); 
      else 
	{
	  free(line_stack);
	  line_stack = NULL;
	}
      return s;
    }
  return ret;
}

int 
_etox_loadfile_is_good(char *path) 
{
  struct stat buf;
  int ret;
  
  ret = stat(path, &buf);
  if (ret < 0)
    return 0;
  if ((buf.st_mode & S_IFLNK) || (buf.st_mode & S_IFREG))
    return 1;
  return 0;
}

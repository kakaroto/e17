#include "Etox_private.h"
#include "Etox.h"

char *
etox_str_remove_beginning_spaces(char *str)
{
  char *p, *q;

  for (p = str; *p == ' '; *p++);

  q = malloc((sizeof(char) * strlen(p)) + 1);
  strncpy(q, p, strlen(p));
  q[strlen(p)] = '\0';

  return q;
}

char *
etox_str_remove_ending_spaces(char *str)
{
  char *p, *q;
  int i;

  p = malloc((sizeof(char) * strlen(str)) + 1);
  strcpy(p, str);

  for (i = strlen(str); i && (!p[i] || (p[i] == ' ')); i--)
    p[i] = '\0';

  q = malloc((sizeof(char) * strlen(p)) + 1);
  strncpy(q, p, strlen(p));
  q[strlen(p)] = '\0';

  free(p);

  return q;
}

char *
etox_str_chop_off_beginning_string(char *str, char *chop)
{
  char *p, *q;

  if (!(p = strstr(str, chop)))
    return str;

  p += strlen(chop);

  q = malloc((sizeof(char) * strlen(p)) + 1);
  strcpy(q, p);

  return q;
}

char *
etox_str_chop_off_beginning_word(char *str)
{
  char *p, *q;
  
  p = str;

  for (p = str; *p != ' '; *p++);

  q = malloc((sizeof(char) * strlen(p)) + 1);
  strncpy(q, p, strlen(p));
  q[strlen(p)] = '\0';

  return q;  
}

char *
etox_str_chop_off_ending_string(char *str, char *chop)
{
  char *p;

  p = malloc((sizeof(char) * (strlen(str) - strlen(chop))) + 1);
  strncpy(p, str, strlen(str) - strlen(chop));
  p[strlen(str) - strlen(chop)] = '\0';
  
  return p;
}

char *
etox_str_chop_off_ending_word(char *str)
{
  char *p, *q, *r;

  p = etox_str_remove_ending_spaces(str);

  for (q = p; strstr(q, " "); *q++);

  r = etox_str_chop_off_ending_string(p, q);
  free(p);

  return r;
}

#include "file.h"

void
md(char *s)
{
  if ((!s) || (!*s))
    return;
  mkdir(s, S_IRWXU);
  return;
}

int
exists(char *s)
{
  struct stat         st;

  if ((!s) || (!*s))
    return (0);
  if (stat(s, &st) < 0)
    return (0);
  return (1);
}

int
isfile(char *s)
{
  struct stat         st;

  if ((!s) || (!*s))
    return (0);
  if (stat(s, &st) < 0)
    return (0);
  if (S_ISREG(st.st_mode))
    return (1);
  return (0);
}

int
isdir(char *s)
{
  struct stat         st;

  if ((!s) || (!*s))
    return (0);
  if (stat(s, &st) < 0)
    return (0);
  if (S_ISDIR(st.st_mode))
    return (1);
  return (0);
}

char              **
ls(char *dir, int *num)
{
  int                 i, dirlen;
  DIR                *dirp;
  char              **names;
  struct dirent      *dp;

  if ((!dir) || (!*dir))
    return (0);
  dirp = opendir(dir);
  if (!dirp)
    {
      *num = 0;
      return (NULL);
    }
  /* count # of entries in dir (worst case) */
  for (dirlen = 0; (dp = readdir(dirp)) != NULL; dirlen++);
  if (!dirlen)
    {
      closedir(dirp);
      *num = dirlen;
      return (NULL);
    }
  /* load up the entries, now that we know how many to make */
  names = (char **)malloc(dirlen * sizeof(char *));

  if (!names)
    return (NULL);

  rewinddir(dirp);
  for (i = 0; i < dirlen;)
    {
      dp = readdir(dirp);
      if (!dp)
	break;
      names[i] = (char *)malloc(strlen(dp->d_name) + 1);
      if (!names)
	return (NULL);
      strcpy(names[i], dp->d_name);
      i++;
    }

  if (i < dirlen)
    dirlen = i;			/* dir got shorter... */
  closedir(dirp);
  *num = dirlen;
  return (names);
}

void
freestrlist(char **l, int num)
{
  if (!l)
    return;
  while (num--)
    if (l[num])
      free(l[num]);
  free(l);
  return;
}

void
rm(char *s)
{
  if ((!s) || (!*s))
    return;
  unlink(s);
  return;
}

void
mv(char *s, char *ss)
{
  if ((!s) || (!ss) || (!*s) || (!*ss))
    return;
  rename(s, ss);
  return;
}

void
cp(char *s, char *ss)
{
  int                 i;
  FILE               *f, *ff;
  unsigned char       buf[1];

  if ((!s) || (!ss) || (!*s) || (!*ss))
    return;
  if (!exists(s))
    return;
  i = filesize(s);
  f = fopen(s, "r");
  if (!f)
    return;
  ff = fopen(ss, "w");
  if (!ff)
    {
      fclose(f);
      return;
    }
  while (fread(buf, 1, 1, f))
    fwrite(buf, 1, 1, ff);
  fclose(f);
  fclose(ff);
  return;
}

time_t
moddate(char *s)
{
  struct stat         st;

  if ((!s) || (!*s))
    return (0);
  if (!stat(s, &st) < 0)
    return (0);
  if (st.st_mtime > st.st_ctime)
    {
      return (st.st_mtime);
    }
  else
    return (st.st_ctime);
  return (0);
}

int
filesize(char *s)
{
  struct stat         st;

  if ((!s) || (!*s))
    return (0);
  if (stat(s, &st) < 0)
    return (0);
  return ((int)st.st_size);
}

void
cd(char *s)
{
  if ((!s) || (!*s))
    return;
  chdir(s);
  return;
}

char               *
cwd(void)
{
  return (duplicate(getcwd(NULL, -1)));
}

int
permissions(char *s)
{
  struct stat         st;

  if ((!s) || (!*s))
    return (0);
  if (!stat(s, &st) < 0)
    return (0);
  return (st.st_mode);
}

int
owner(char *s)
{
  struct stat         st;

  if ((!s) || (!*s))
    return (0);
  if (!stat(s, &st) < 0)
    return (0);
  return (st.st_uid);
}

int
group(char *s)
{
  struct stat         st;

  if ((!s) || (!*s))
    return (0);
  if (!stat(s, &st) < 0)
    return (0);
  return (st.st_gid);
}

char               *
username(int uid)
{
  char               *s;
  struct passwd      *pwd;

  pwd = getpwuid(uid);
  if (pwd)
    {
      s = duplicate(pwd->pw_name);
/*      free(pwd); */
      return (s);
    }
  return (duplicate("unknown"));
}

char               *
homedir(int uid)
{
  char               *s;
  struct passwd      *pwd;

  pwd = getpwuid(uid);
  if (pwd)
    {
      s = duplicate(pwd->pw_dir);
/*      free(pwd); */
      return (s);
    }
  return (duplicate("/tmp"));
}

char               *
usershell(int uid)
{
  char               *s;
  struct passwd      *pwd;

  pwd = getpwuid(uid);
  if (pwd)
    {
      s = duplicate(pwd->pw_shell);
/*      free(pwd); */
      return (s);
    }
  return (duplicate("/bin/sh"));
}

char               *
duplicate(char *s)
{
  char               *ss;
  int                 sz;

  if (!s)
    return (NULL);
  sz = strlen(s);
  ss = malloc(sz + 1);
  strncpy(ss, s, sz + 1);
  return (ss);
}

char               *
atword(char *s, int num)
{
  int                 cnt, i;

  if (!s)
    return (NULL);
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
	    return (&s[i]);
	}
      i++;
    }
  return (NULL);
}

char               *
atchar(char *s, char c)
{
  int                 i;

  if (!s)
    return (NULL);
  i = 0;
  while (s[i] != 0)
    {
      if (s[i] == c)
	return (&s[i]);
      i++;
    }
  return (NULL);
}

void
word(char *s, int num, char *wd)
{
  int                 cnt, i;
  char               *start, *finish, *ss, *w;

  if (!s)
    return;
  if (!wd)
    return;
  if (num <= 0)
    {
      *wd = 0;
      return;
    }
  cnt = 0;
  i = 0;
  start = NULL;
  finish = NULL;
  ss = NULL;
  w = wd;

  while (s[i])
    {
      if ((cnt == num) && ((s[i] == ' ') || (s[i] == '\t')))
	{
	  finish = &s[i];
	  break;
	}
      if ((s[i] != ' ') && (s[i] != '\t'))
	{
	  if (i == 0)
	    {
	      cnt++;
	      if (cnt == num)
		start = &s[i];
	    }
	  else if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
	    {
	      cnt++;
	      if (cnt == num)
		start = &s[i];
	    }
	}
      i++;
    }
  if (cnt == num)
    {
      if ((start) && (finish))
	{
	  for (ss = start; ss < finish; ss++)
	    *wd++ = *ss;
	}
      else if (start)
	{
	  for (ss = start; *ss != 0; ss++)
	    *wd++ = *ss;
	}
      *wd = 0;
    }
  return;
}

int
canread(char *s)
{
  if ((!s) || (!*s))
    return (0);
  return (1 + access(s, R_OK));
}

int
canwrite(char *s)
{
  if ((!s) || (!*s))
    return (0);
  return (1 + access(s, W_OK));
}

int
canexec(char *s)
{
  if ((!s) || (!*s))
    return (0);
  return (1 + access(s, X_OK));
}

char               *
fileof(char *s)
{
  char                ss[1024];
  int                 i, p1, p2;

  i = 0;
  p1 = -1;
  p2 = -1;
  for (i = strlen(s) - 1; i >= 0; i--)
    {
      if ((s[i] == '.') && (p2 < 0) && (p1 < 0))
	p2 = i;
      if ((s[i] == '/') && (p1 < 0))
	p1 = i;
    }
  if (p2 < 0)
    p2 = strlen(s);
  if (p1 < 0)
    p1 = 0;
  for (i = 0; i < (p2 - p1 - 1); i++)
    ss[i] = s[p1 + 1 + i];
  ss[i] = 0;
  return (duplicate(ss));
}

char               *
fullfileof(char *s)
{
  char                ss[1024];
  int                 i, p1, p2;

  i = 0;
  p1 = -1;
  for (i = strlen(s) - 1; i >= 0; i--)
    {
      if ((s[i] == '/') && (p1 < 0))
	p1 = i;
    }
  p2 = strlen(s);
  for (i = 0; i < (p2 - p1 - 1); i++)
    ss[i] = s[p1 + 1 + i];
  ss[i] = 0;
  return (duplicate(ss));
}

char               *
pathtoexec(char *file)
{
  char               *p, *cp, *ep;
  char               *s;
  int                 len, exelen;

  if (file[0] == '/')
    {
      if (canexec(file))
	return (duplicate(file));
    }
  p = getenv("PATH");
  if (!p)
    return (duplicate(file));
  if (!file)
    return (NULL);
  cp = p;
  exelen = strlen(file);
  while ((ep = strchr(cp, ':')))
    {
      len = ep - cp;
      s = malloc(len + 1);
      if (s)
	{
	  strncpy(s, cp, len);
	  s[len] = 0;
	  s = realloc(s, len + 2 + exelen);
	  strcat(s, "/");
	  strcat(s, file);
	  if (canexec(s))
	    return (s);
	  free(s);
	}
      cp = ep + 1;
    }
  len = strlen(cp);
  s = malloc(len + 1);
  if (s)
    {
      strncpy(s, cp, len);
      s[len] = 0;
      s = realloc(s, len + 2 + exelen);
      strcat(s, "/");
      strcat(s, file);
      if (canexec(s))
	return (s);
      free(s);
    }
  return (NULL);
}

char               *
pathtofile(char *file)
{
  char               *p, *cp, *ep;
  char               *s;
  int                 len, exelen;

  if (file[0] == '/')
    {
      if (exists(file))
	return (duplicate(file));
    }
  p = getenv("PATH");
  if (!p)
    return (duplicate(file));
  if (!file)
    return (NULL);
  cp = p;
  exelen = strlen(file);
  while ((ep = strchr(cp, ':')))
    {
      len = ep - cp;
      s = malloc(len + 1);
      if (s)
	{
	  strncpy(s, cp, len);
	  s[len] = 0;
	  s = realloc(s, len + 2 + exelen);
	  strcat(s, "/");
	  strcat(s, file);
	  if (exists(s))
	    return (s);
	  free(s);
	}
      cp = ep + 1;
    }
  len = strlen(cp);
  s = malloc(len + 1);
  if (s)
    {
      strncpy(s, cp, len);
      s[len] = 0;
      s = realloc(s, len + 2 + exelen);
      strcat(s, "/");
      strcat(s, file);
      if (exists(s))
	return (s);
      free(s);
    }
  return (NULL);
}

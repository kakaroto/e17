#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define LINEBUF_SIZE 1024
int esd_no_spawn=1; /* If we can't find even the system config file,
		       things are screwed up - don't try to make things
		       worse. */
int esd_spawn_wait_ms=100; /* Time to wait trying to connect to an
			      autospawned ESD, in milliseconds. */
char esd_spawn_options[LINEBUF_SIZE] = "-terminate -nobeeps -as 2";

static int read_esd_config = 0;

static void esd_config_read_file(FILE *fh);

void
esd_config_read(void)
{
  FILE *fh;
  char *fn, *tmpenv;

  if(read_esd_config) return;

  fn = malloc(sizeof(SYSCONFDIR) + sizeof("/esd.conf"));
  strcpy(fn, SYSCONFDIR "/esd.conf");
  fh = fopen(fn, "r");
  if(fh)
    {
      esd_config_read_file(fh);
      fclose(fh);
    }
  free(fn);

  tmpenv = getenv("HOME");
  if(tmpenv) {
    fn = malloc(strlen(tmpenv) + sizeof("/.esd.conf"));
    sprintf(fn, "%s/.esd.conf", tmpenv);

    fh = fopen(fn, "r");

    if(fh)
      {
	esd_config_read_file(fh);
	fclose(fh);
      }

    free(fn);
  }

  tmpenv=getenv("ESD_NO_SPAWN");
  if(tmpenv)
    esd_no_spawn=1;

  tmpenv = getenv("ESD_SPAWN_OPTIONS");
  if(tmpenv && strlen(tmpenv) < (sizeof(esd_spawn_options) - 1))
    strcpy(esd_spawn_options, tmpenv);

  read_esd_config = 1;
}

static void
esd_config_read_file(FILE *fh)
{
  char aline[LINEBUF_SIZE];
  char *key, *value, *start;
  int i;

  while(fgets(aline, sizeof(aline), fh))
    {
      /* first, chomp & chug */
      for(start = aline; *start && isspace(*start); start++) /**/;
      if(*start && start != aline) memmove(aline, start, strlen(start) + 1);

      i = strlen(aline) - 1;
      while(i >= 0 && isspace(aline[i])) aline[i--] = '\0';

      switch(aline[0])
	{
	case '#': /* it's a comment, skip it */
	  continue;
	case '[': /* It's a section delimiter to placate gnome_config,
		     skip it */
	  continue;
	case '\0': /* Junk line, skip it */
	  continue;
	default:
	  break;
	}

      key = strtok(aline, "=");
      if(!key) continue;
      value = strtok(NULL, "=");
      if(!value) value = "";

      if(!strcasecmp(key, "auto_spawn"))
	{
	  if(!strcasecmp(value, "true")
	     || !strcasecmp(value, "yes")
	     || !strcasecmp(value, "1"))
	    esd_no_spawn=0;
	  else if(!strcasecmp(value, "false")
		  || !strcasecmp(value, "no")
		  || !strcasecmp(value, "0"))
	    esd_no_spawn=1;
	  else
	    fprintf(stderr, "Invalid value %s for option %s\n", value, key);
	}
      else if(!strcasecmp(key, "spawn_options"))
	{
	  strcpy(esd_spawn_options, value);
	}
      else if(!strcasecmp(key, "spawn_wait_ms"))
	{
	  char *endptr;
	  long val = strtol(value, &endptr, 0);
	  if(value != '\0' && *endptr == '\0')
	    esd_spawn_wait_ms = (int) val;
	  else
	    fprintf(stderr, "Invalid value %s for option %s\n", value, key);
	}
      else
	fprintf(stderr, "Unknown option %s.\n", key);
    } /* while(fgets(...)) */
}

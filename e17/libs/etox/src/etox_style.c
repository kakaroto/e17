#include "Etox_private.h"
#include "Etox.h"

Etox_Style *style_list;
int         num_styles=0;

static int     fspath_num = 0;
static char  **fspath = NULL;

void 
etox_style_add_path(char *path) 
{
  fspath_num++;
  if (!fspath)
    fspath = malloc(sizeof(char *));
  else
    fspath = realloc(fspath, (fspath_num * sizeof(char *)));
  fspath[fspath_num - 1] = strdup(path);
}

void 
etox_style_del_path(char *path) 
{
  int i, j;
  
  for (i = 0; i < fspath_num; i++)
    {
      if (!strcmp(path, fspath[i]))
	{
	  fspath_num--;
	  for (j = i; j < fspath_num; j++)
	    fspath[j] = fspath[j + 1];
	  if (fspath_num > 0)
	    fspath = realloc(fspath, fspath_num * sizeof(char *));
	  else
	    {
	      free(fspath);
	      fspath = NULL;
	    }
	}
    }
}

char **
etox_style_get_paths(int *number_return) 
{
  *number_return = fspath_num;
  return fspath;
}


Etox_Style 
etox_style_new(char *path) 
{
  Etox_Style style;
  FILE *font_file;
  char s[4096];
  int i1, i2, i3, fields;
  char s2[4096];
  char nbuf[4096];
  
  if(!path)
    return NULL;
  
  style = malloc(sizeof(struct _Etox_Style));
  style->in_use = 0;
  
  style->bits = NULL;
  style->name = malloc((strlen(path) * sizeof(char)) + 1);
  strcpy(style->name,path);
  style->num_bits = 0;
  
  /* Look for the style file */
  
  if (!_etox_loadfile_is_good(path))
    {
      int ok = 0;
      int fspath_iter = 0;

      if (!strstr(path,".style") )
	{
	  strncpy(nbuf, path, 4000);
	  strcat(nbuf, ".style");
	  
	  ok = _etox_loadfile_is_good(nbuf);
	}
      while (!ok) 
	{
	  if (fspath_iter >= fspath_num)
	    return NULL;
	  strncpy(nbuf, fspath[fspath_iter], 4000);
	  if (!(nbuf[strlen(nbuf)-1] == '/'))
	    strcat(nbuf,"/");
	  strncat(nbuf, path, (4000 - strlen(nbuf)));
	  ok = _etox_loadfile_is_good(nbuf);
	  if (!ok)
	    if (!strstr(path,".style") )
	      {
		strcat(nbuf, ".style");
	      
		ok = _etox_loadfile_is_good(nbuf);
	      }
	  fspath_iter++;
	}
    }
  else
    strncpy(nbuf, path, 4000);

  font_file = fopen(nbuf,"r");
  while(_etox_loadfile_get_line(s,4096,font_file)) 
    {
      i1=i2=i3=0;
      memset(s2,0,4096);
      fields = sscanf(s,"%4000[^=]= %i %i %i",s2,&i1,&i2,&i3);
      if (fields < 3) 
	{
	  fclose(font_file);
	  return style;
	}
      style->num_bits++;
      if(style->bits)
	{
	  style->bits = realloc(style->bits,(style->num_bits * 
					     sizeof(struct _Etox_Style_Bit) + 
					     1));
	} 
      else 
	{
	  style->bits = malloc(style->num_bits * 
			       sizeof(struct _Etox_Style_Bit) + 1);
	}
      if(!strcmp(s2,"sh"))
	style->bits[style->num_bits - 1].type = ETOX_STYLE_TYPE_SHADOW;
      if(!strcmp(s2,"fg"))
	style->bits[style->num_bits - 1].type = ETOX_STYLE_TYPE_FOREGROUND;
      if(!strcmp(s2,"ol"))
	style->bits[style->num_bits - 1].type = ETOX_STYLE_TYPE_OUTLINE;
      style->bits[style->num_bits - 1].x = i1;
      style->bits[style->num_bits - 1].y = i2;
      style->bits[style->num_bits - 1].alpha = i3;
    }
  fclose(font_file);
  
  (style->in_use)++;
  return style;
}

void 
etox_style_free(Etox_Style style) 
{
  if (!style)
    return;

  (style->in_use)--;

  if (style->in_use <=0) 
    {
      if(style->name)
	free(style->name);
      if(style->bits)
	free(style->bits);
      free(style);
    }
}

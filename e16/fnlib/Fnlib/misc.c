#include "Fnlib.h"
#include "file.h"

#ifdef __EMX__
extern char *__XOS2RedirRoot(const char *);
#endif

FnlibChar          *_fnlib_get_char(FnlibData * fd, FnlibFont * fn, char orientation,
				    int size, int index);
FnlibChar          *_fnlib_get_char_mes(FnlibData * fd, FnlibFont * fn, char orientation,
					int size, int index);
void                _fnlib_read_cfg(FnlibData * fd, char *file);

FnlibData          *
Fnlib_init(ImlibData * id)
{
  FnlibData          *fd;
  char                s[2048], *home;

  if (!id)
    return NULL;
  fd = malloc(sizeof(FnlibData));
  if (!fd)
    return NULL;
  fd->id = id;
  fd->dirs = NULL;
  fd->num_dirs = 0;
  fd->num_fonts = 0;
  fd->font = NULL;
#ifndef __EMX__
  _fnlib_read_cfg(fd, SYSTEM_FNRC);
#else
  _fnlib_read_cfg(fd, __XOS2RedirRoot(SYSTEM_FNRC));
#endif
  home = homedir(getuid());
  if (home)
    {
      sprintf(s, "%s/.fnrc", home);
/*      snprintf(s, 2048, "%s/.fnrc", home); */
      free(home);
      _fnlib_read_cfg(fd, s);
    }
  return fd;
}

void
Fnlib_add_dir(FnlibData * fd, char *dir)
{
  fd->num_dirs++;
  fd->dirs = realloc(fd->dirs, fd->num_dirs * sizeof(char *));

  fd->dirs[fd->num_dirs - 1] = strdup(dir);
}

void
Fnlib_del_dir(FnlibData * fd, char *dir)
{
  int                 i, j;

  j = -1;
  for (i = 0; i < fd->num_dirs; i++)
    {
      if (!strcmp(dir, fd->dirs[i]))
	j = i;
    }
  if (j < 0)
    return;
  free(fd->dirs[j]);
  fd->num_dirs--;
  for (i = j; i < fd->num_dirs; i++)
    fd->dirs[i] = fd->dirs[i + 1];
  fd->dirs = realloc(fd->dirs, fd->num_dirs * sizeof(char *));
}

char              **
Fnlib_list_dirs(FnlibData * fd, int *count)
{
  char              **list;
  int                 i;

  *count = fd->num_dirs;
  if (!fd->num_dirs)
    return NULL;
  list = malloc(fd->num_dirs * sizeof(char *));

  for (i = 0; i < fd->num_dirs; i++)
    list[i] = strdup(fd->dirs[i]);
  return list;
}

FnlibFont          *
Fnlib_load_font(FnlibData * fd, char *name)
{
  FnlibFont          *fn;
  char               *fdir, **dir, s[2048];
  int                 i, j, num, found;
  FILE               *f;
  
  fdir = NULL;
  for (i = 0; i < fd->num_fonts; i++)
    {
      if (!strcmp(fd->font[i]->name, name))
	{
	  fd->font[i]->refs++;
	  return fd->font[i];
	}
    }
  found = 0;
  for (i = 0; (i < fd->num_dirs) && (!found); i++)
    {
      dir = ls(fd->dirs[i], &num);
      for (j = 0; (j < num) && (!found); j++)
	{
	  if (!strcmp(name, dir[j]))
	    {
	      sprintf(s, "%s/%s/fontinfo", fd->dirs[i], dir[j]);
/*                snprintf(s, 2048, "%s/%s/fontinfo", fd->dirs[i], dir[j]); */
	      if (isfile(s))
		{
		  found = 1;
		  sprintf(s, "%s/%s", fd->dirs[i], dir[j]);
/*                     snprintf(s, 2048, "%s/%s", fd->dirs[i], dir[j]); */
		  fdir = strdup(s);
		}
	    }
	}
      freestrlist(dir, num);
    }
  if (!found)
    return NULL;
/*   snprintf(s, 2048, "%s/fontinfo", fdir); */
  sprintf(s, "%s/fontinfo", fdir);
#ifndef __EMX__
  f = fopen(s, "r");
#else
  f = fopen(s, "rt");
#endif
  if (!f)
    {
      free(fdir);
      return NULL;
    }
  fn = malloc(sizeof(FnlibFont));
  if (!fn)
    {
      fclose(f);
      free(fdir);
      return NULL;
    }
  fn->refs = 1;
  fn->name = strdup(name);
  fn->dir = fdir;
  fn->num = 0;
  fn->fonts = NULL;
  {
    char                ss[2048];
    int                 a, b, c, d;

    while (fgets(s, 2048, f))
      {
	word(s, 1, ss);
	if (!strcmp("size", ss))
	  {
	    fn->num++;
	    fn->fonts = realloc(fn->fonts, fn->num * sizeof(FnlibFontSize));
	    sscanf(s, "%*s %i %i", &a, &b);
	    fn->fonts[fn->num - 1].size = a;
	    fn->fonts[fn->num - 1].orientation = b;
	    fn->fonts[fn->num - 1].default_char = 32;
	    fn->fonts[fn->num - 1].num_chars = 0;
	    fn->fonts[fn->num - 1].chars = NULL;
	    fn->fonts[fn->num - 1].current.size = 0;
	  }
	else if (!strcmp("default", ss))
	  {
	    sscanf(s, "%*s %i", &a);
	    fn->fonts[fn->num - 1].default_char = a;
	  }
	else if (!strcmp("max", ss))
	  {
	    sscanf(s, "%*s %i", &a);
	    fn->fonts[fn->num - 1].num_chars = a;
	    fn->fonts[fn->num - 1].chars = malloc((a + 1) * sizeof(FnlibChar));
	    for (b = 0; b < a; b++)
	      {
		fn->fonts[fn->num - 1].chars[b].file = NULL;
		fn->fonts[fn->num - 1].chars[b].im = NULL;
		fn->fonts[fn->num - 1].chars[b].x = 0;
		fn->fonts[fn->num - 1].chars[b].y = 0;
		fn->fonts[fn->num - 1].chars[b].width = 0;
		fn->fonts[fn->num - 1].chars[b].current.x = 0;
		fn->fonts[fn->num - 1].chars[b].current.y = 0;
		fn->fonts[fn->num - 1].chars[b].current.width = 0;
		fn->fonts[fn->num - 1].chars[b].current.rendered_width = 0;
		fn->fonts[fn->num - 1].chars[b].current.rendered_height = 0;
		fn->fonts[fn->num - 1].chars[b].current.pmap = 0;
		fn->fonts[fn->num - 1].chars[b].current.mask = 0;
	      }
	  }
	else
	  {
	    word(s, 1, ss);
	    a = atoi(ss);
	    word(s, 3, ss);
	    b = atoi(ss);
	    word(s, 4, ss);
	    c = atoi(ss);
	    word(s, 5, ss);
	    d = atoi(ss);
	    word(s, 2, ss);
	    strcpy(s, fn->dir);
	    strcat(s, "/");
	    strcat(s, ss);
	    fn->fonts[fn->num - 1].chars[a].file = strdup(s);
	    fn->fonts[fn->num - 1].chars[a].x = b;
	    fn->fonts[fn->num - 1].chars[a].y = c;
	    fn->fonts[fn->num - 1].chars[a].width = d;
	  }
      }
  }
  fclose(f);
  fd->num_fonts++;
  fd->font = realloc(fd->font, fd->num_fonts * (sizeof(FnlibFont *)));
  fd->font[fd->num_fonts - 1] = fn;
  return fn;
}

void
Fnlib_free_font(FnlibData * fd, FnlibFont * fn)
{
  int                 i, j;

  if (!fn)
    return;
  fn->refs--;
  for (i = 0; i < fd->num_fonts; i++)
    {
      if (fd->font[i] == fn)
	break;
    }
  if (i < fd->num_fonts)
    {
      for (j = i; j < fd->num_fonts - 1; j++)
	fd->font[j] = fd->font[j + 1];
    }
  if (fn->refs > 0)
    return;
  fd->num_fonts--;
  fd->font = realloc(fd->font, fd->num_fonts * (sizeof(FnlibFont *)));
  free(fn->name);
  free(fn->dir);
  for (i = 0; i < fn->num; i++)
    {
      for (j = 0; j < fn->fonts[i].num_chars; j++)
	{
	  free(fn->fonts[i].chars[j].file);
	  if (fn->fonts[i].chars[j].current.pmap)
	    Imlib_free_pixmap(fd->id, fn->fonts[i].chars[j].current.pmap);
	  if (fn->fonts[i].chars[j].im)
	    Imlib_destroy_image(fd->id, fn->fonts[i].chars[j].im);
	}
    }
  free(fn->fonts);
  free(fn);
  fn = NULL;
}

void
Fnlib_draw(FnlibData * fd, FnlibFont * fn, Drawable win, Pixmap mask,
	   int x, int y, int width, int height, int xin, int yin,
	   int size, FnlibStyle * style, unsigned char *text)
{
  char                mode, orientation;
  int                 justification, spacing;
  int                 cx, cy;
  int                 char_num, index, wcount, lastw, wordw, linew, len,
                      linec, i, prevspace;
  int                 srcx, srcy, srcwidth, srcheight, destx, desty;
  unsigned char      *s;
  FnlibChar          *fchar;
  GC                  gc, gc_pmap, gc_mask;
  XGCValues           gcv;
  Display            *disp;
  Pixmap              tmp_pmap, tmp_mask;

  if ((!win) && (!mask))
    return;
  wordw = 0;
  mode = style->mode;
  orientation = style->orientation;
  justification = style->justification;
  spacing = style->spacing;
  disp = fd->id->x.disp;
  tmp_pmap = XCreatePixmap(disp, win, width, height, fd->id->x.depth);
  tmp_mask = XCreatePixmap(disp, win, width, height, 1);
  gcv.graphics_exposures = False;
  gc = XCreateGC(disp, win, GCGraphicsExposures, &gcv);
  gc_pmap = XCreateGC(disp, tmp_pmap, GCGraphicsExposures, &gcv);
  gc_mask = XCreateGC(disp, tmp_mask, GCGraphicsExposures, &gcv);
  XSetForeground(disp, gc_mask, 0);
  XFillRectangle(disp, tmp_mask, gc_mask, 0, 0, width, height);
  XSetForeground(disp, gc_mask, 1);
  len = strlen((char *)text);
  s = malloc(len + 1);
  switch (orientation)
    {
    case FONT_TO_RIGHT:
      cx = xin;
      cy = yin;
      char_num = 0;
      while ((char_num < len) && (cy < height))
	{
	  linew = 0;
	  linec = 0;
	  wcount = 0;
	  lastw = 0;
	  switch (mode)
	    {
	    case MODE_VERBATIM:
	      while ((text[char_num]) && (text[char_num] != '\n'))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      s[linec++] = text[char_num++];
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_CHAR:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= width))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= width)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_WORD:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= width))
		{
		  index = (int)text[char_num];
		  /* if "b " */
		  if (((char_num > 0) && (isspace(index)) && (!isspace(text[char_num - 1])))
		      || (char_num == (len - 1)))
		    {
		      wcount++;
		      wordw = linew;
		    }
		  /* if " b" */
		  if ((char_num > 0) && (!isspace(index)) && (isspace(text[char_num - 1])))
		    lastw = linec;
		  if (char_num == (len - 1))
		    {
		      lastw = linec + 1;
		    }
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= width)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      if ((text[char_num] == '\n') || (isspace(text[char_num])))
		{
		  wcount++;
		  wordw = linew;
		  lastw = linec;
		}
	      if (wcount > 0)
		{
		  char_num -= linec - lastw;
		  linec = lastw;
		  linew = wordw;
		}
	      s[linec] = 0;
	      break;
	    }
	  prevspace = 0;
	  cx += (((width - linew) * justification) >> 10) - (((width - linew) * spacing * justification) >> 20);
	  for (i = 0; i < linec; i++)
	    {
	      index = (int)s[i];
	      fchar = _fnlib_get_char(fd, fn, orientation, size, index);
	      if (fchar)
		{
		  if (fchar->current.pmap)
		    {
		      srcx = 0;
		      srcy = 0;
		      srcwidth = fchar->current.rendered_width;
		      srcheight = fchar->current.rendered_height;
		      destx = cx + fchar->current.x;
		      desty = cy + fchar->current.y;
		      if (destx < 0)
			{
			  srcx -= destx;
			  srcwidth += destx;
			  destx = 0;
			}
		      if (desty < 0)
			{
			  srcy -= desty;
			  srcheight += desty;
			  desty = 0;
			}
		      if (!((srcwidth <= 0) || (srcheight <= 0)))
			{
			  if (destx + srcwidth > width)
			    srcwidth = width - destx;
			  if (desty + srcheight > height)
			    srcheight = height - desty;
			  if (!((srcwidth <= 0) || (srcheight <= 0)))
			    {
			      XCopyArea(disp, fchar->current.pmap, tmp_pmap, gc_pmap,
			       srcx, srcy, srcwidth, srcheight, destx, desty);
			      if (fchar->current.mask)
				XCopyArea(disp, fchar->current.mask, tmp_mask, gc_mask,
				srcx, srcy, srcwidth, srcheight, destx, desty);
			      else
				XFillRectangle(disp, tmp_mask, gc_mask, destx, desty,
					       srcwidth, srcheight);
			    }
			}
		    }
		  cx += fchar->current.width;
		  cx += ((((width - linew) * spacing * (i + 1)) >> 10) / linec) - prevspace;
		  prevspace = (((width - linew) * spacing * (i + 1)) >> 10) / linec;
		}
	    }
	  if ((text[char_num] == ' ') || (text[char_num] == '\n'))
	    char_num++;
	  cx = xin;
	  cy += size;
	}
      break;
    case FONT_TO_DOWN:
      cx = width;
      cy = yin;
      char_num = 0;
      while ((char_num < len) && (cx >= 0))
	{
	  linew = 0;
	  linec = 0;
	  wcount = 0;
	  lastw = 0;
	  switch (mode)
	    {
	    case MODE_VERBATIM:
	      while ((text[char_num]) && (text[char_num] != '\n'))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      s[linec++] = text[char_num++];
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_CHAR:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= height))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= height)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_WORD:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= height))
		{
		  index = (int)text[char_num];
		  /* if "b " */
		  if (((char_num > 0) && (isspace(index)) && (!isspace(text[char_num - 1])))
		      || (char_num == (len - 1)))
		    {
		      wcount++;
		      wordw = linew;
		    }
		  /* if " b" */
		  if ((char_num > 0) && (!isspace(index)) && (isspace(text[char_num - 1])))
		    lastw = linec;
		  if (char_num == (len - 1))
		    {
		      lastw = linec + 1;
		    }
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= height)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      if ((text[char_num] == '\n') || (isspace(text[char_num])))
		{
		  wcount++;
		  wordw = linew;
		  lastw = linec;
		}
	      if (wcount > 0)
		{
		  char_num -= linec - lastw;
		  linec = lastw;
		  linew = wordw;
		}
	      break;
	    }
	  prevspace = 0;
	  cy += (((height - linew) * justification) >> 10) - (((height - linew) * spacing * justification) >> 20);
	  for (i = 0; i < linec; i++)
	    {
	      index = (int)s[i];
	      fchar = _fnlib_get_char(fd, fn, orientation, size, index);
	      if (fchar)
		{
		  if (fchar->current.pmap)
		    {
		      srcx = 0;
		      srcy = 0;
		      srcwidth = fchar->current.rendered_width;
		      srcheight = fchar->current.rendered_height;
		      destx = cx + fchar->current.x;
		      desty = cy + fchar->current.y;
		      if (destx < 0)
			{
			  srcx -= destx;
			  srcwidth += destx;
			  destx = 0;
			}
		      if (desty < 0)
			{
			  srcy -= desty;
			  srcheight += desty;
			  desty = 0;
			}
		      if (!((srcwidth <= 0) || (srcheight <= 0)))
			{
			  if (destx + srcwidth > width)
			    srcwidth = destx - width;
			  if (desty + srcheight > height)
			    srcheight = desty - height;
			  if (!((srcwidth <= 0) || (srcheight <= 0)))
			    {
			      XCopyArea(disp, fchar->current.pmap, tmp_pmap, gc_pmap,
			       srcx, srcy, srcwidth, srcheight, destx, desty);
			      if (fchar->current.mask)
				XCopyArea(disp, fchar->current.mask, tmp_mask, gc_mask,
				srcx, srcy, srcwidth, srcheight, destx, desty);
			      else
				XFillRectangle(disp, tmp_mask, gc_mask, destx, desty,
					       srcwidth, srcheight);
			    }
			}
		    }
		  cy += fchar->current.width;
		  cy += ((((height - linew) * spacing * (i + 1)) >> 10) / linec) - prevspace;
		  prevspace = (((height - linew) * spacing * (i + 1)) >> 10) / linec;
		}
	    }
	  if ((text[char_num] == ' ') || (text[char_num] == '\n'))
	    char_num++;
	  cy = yin;
	  cx -= size;
	}
      break;
    case FONT_TO_UP:
      cx = xin;
      cy = yin;
      char_num = 0;
      while ((char_num < len) && (cx < width))
	{
	  linew = 0;
	  linec = 0;
	  wcount = 0;
	  lastw = 0;
	  switch (mode)
	    {
	    case MODE_VERBATIM:
	      while ((text[char_num]) && (text[char_num] != '\n'))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      s[linec++] = text[char_num++];
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_CHAR:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= height))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= height)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_WORD:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= height))
		{
		  index = (int)text[char_num];
		  /* if "b " */
		  if (((char_num > 0) && (isspace(index)) && (!isspace(text[char_num - 1])))
		      || (char_num == (len - 1)))
		    {
		      wcount++;
		      wordw = linew;
		    }
		  /* if " b" */
		  if ((char_num > 0) && (!isspace(index)) && (isspace(text[char_num - 1])))
		    lastw = linec;
		  if (char_num == (len - 1))
		    {
		      lastw = linec + 1;
		    }
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= height)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      if ((text[char_num] == '\n') || (isspace(text[char_num])))
		{
		  wcount++;
		  wordw = linew;
		  lastw = linec;
		}
	      if (wcount > 0)
		{
		  char_num -= linec - lastw;
		  linec = lastw;
		  linew = wordw;
		}
	      break;
	    }
	  prevspace = 0;
	  cy += height - ((((height - linew) * justification) >> 10) - (((height - linew) * spacing * justification) >> 20));
	  for (i = 0; i < linec; i++)
	    {
	      index = (int)s[i];
	      fchar = _fnlib_get_char(fd, fn, orientation, size, index);
	      if (fchar)
		{
		  if (fchar->current.pmap)
		    {
		      srcx = 0;
		      srcy = 0;
		      srcwidth = fchar->current.rendered_width;
		      srcheight = fchar->current.rendered_height;
		      destx = cx + fchar->current.x;
		      desty = cy + fchar->current.y - fchar->current.rendered_height;
		      if (destx < 0)
			{
			  srcx -= destx;
			  srcwidth += destx;
			  destx = 0;
			}
		      if (desty < 0)
			{
			  srcy -= desty;
			  srcheight += desty;
			  desty = 0;
			}
		      if (!((srcwidth <= 0) || (srcheight <= 0)))
			{
			  if (destx + srcwidth > width)
			    srcwidth = width - destx;
			  if (desty + srcheight > height)
			    srcheight = height - desty;
			  if (!((srcwidth <= 0) || (srcheight <= 0)))
			    {
			      XCopyArea(disp, fchar->current.pmap, tmp_pmap, gc_pmap,
			       srcx, srcy, srcwidth, srcheight, destx, desty);
			      if (fchar->current.mask)
				XCopyArea(disp, fchar->current.mask, tmp_mask, gc_mask,
				srcx, srcy, srcwidth, srcheight, destx, desty);
			      else
				XFillRectangle(disp, tmp_mask, gc_mask, destx, desty,
					       srcwidth, srcheight);
			    }
			}
		    }
		  cy -= fchar->current.width;
		  cy -= ((((height - linew) * spacing * (i + 1)) >> 10) / linec) - prevspace;
		  prevspace = (((height - linew) * spacing * (i + 1)) >> 10) / linec;
		}
	    }
	  if ((text[char_num] == ' ') || (text[char_num] == '\n'))
	    char_num++;
	  cy = yin;
	  cx += size;
	}
      break;
    case FONT_TO_LEFT:
      cx = xin;
      cy = height;
      char_num = 0;
      while ((char_num < len) && (cy >= 0))
	{
	  linew = 0;
	  linec = 0;
	  wcount = 0;
	  lastw = 0;
	  switch (mode)
	    {
	    case MODE_VERBATIM:
	      while ((text[char_num]) && (text[char_num] != '\n'))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      s[linec++] = text[char_num++];
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_CHAR:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= width))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= width)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_WORD:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= width))
		{
		  index = (int)text[char_num];
		  /* if "b " */
		  if (((char_num > 0) && (isspace(index)) && (!isspace(text[char_num - 1])))
		      || (char_num == (len - 1)))
		    {
		      wcount++;
		      wordw = linew;
		    }
		  /* if " b" */
		  if ((char_num > 0) && (!isspace(index)) && (isspace(text[char_num - 1])))
		    lastw = linec;
		  if (char_num == (len - 1))
		    {
		      lastw = linec + 1;
		    }
		  fchar = _fnlib_get_char(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= width)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      if ((text[char_num] == '\n') || (isspace(text[char_num])))
		{
		  wcount++;
		  wordw = linew;
		  lastw = linec;
		}
	      if (wcount > 0)
		{
		  char_num -= linec - lastw;
		  linec = lastw;
		  linew = wordw;
		}
	      break;
	    }
	  prevspace = 0;
	  cx += width - ((((width - linew) * justification) >> 10) - (((width - linew) * spacing * justification) >> 20));
	  for (i = 0; i < linec; i++)
	    {
	      index = (int)s[i];
	      fchar = _fnlib_get_char(fd, fn, orientation, size, index);
	      if (fchar)
		{
		  if (fchar->current.pmap)
		    {
		      srcx = 0;
		      srcy = 0;
		      srcwidth = fchar->current.rendered_width;
		      srcheight = fchar->current.rendered_height;
		      destx = cx + fchar->current.x - fchar->current.rendered_width;
		      desty = cy + fchar->current.y;
		      if (destx < 0)
			{
			  srcx -= destx;
			  srcwidth += destx;
			  destx = 0;
			}
		      if (desty < 0)
			{
			  srcy -= desty;
			  srcheight += desty;
			  desty = 0;
			}
		      if (!((srcwidth <= 0) || (srcheight <= 0)))
			{
			  if (destx + srcwidth > width)
			    srcwidth = width - destx;
			  if (desty + srcheight > height)
			    srcheight = height - desty;
			  if (!((srcwidth <= 0) || (srcheight <= 0)))
			    {
			      XCopyArea(disp, fchar->current.pmap, tmp_pmap, gc_pmap,
			       srcx, srcy, srcwidth, srcheight, destx, desty);
			      if (fchar->current.mask)
				XCopyArea(disp, fchar->current.mask, tmp_mask, gc_mask,
				srcx, srcy, srcwidth, srcheight, destx, desty);
			      else
				XFillRectangle(disp, tmp_mask, gc_mask, destx, desty,
					       srcwidth, srcheight);
			    }
			}
		    }
		  cx -= fchar->current.width;
		  cx -= ((((width - linew) * spacing * (i + 1)) >> 10) / linec) - prevspace;
		  prevspace = (((width - linew) * spacing * (i + 1)) >> 10) / linec;
		}
	    }
	  if ((text[char_num] == ' ') || (text[char_num] == '\n'))
	    char_num++;
	  cx = xin;
	  cy -= size;
	}
      break;
    default:
      break;
    }
  gcv.clip_mask = tmp_mask;
  gcv.clip_x_origin = x;
  gcv.clip_y_origin = y;
  XChangeGC(disp, gc, GCClipMask | GCClipXOrigin |
	    GCClipYOrigin, &gcv);
  if (win)
    XCopyArea(disp, tmp_pmap, win, gc, 0, 0, width, height, x, y);
  XSetFunction(disp, gc_mask, GXor);
  if (mask)
    XCopyArea(disp, tmp_pmap, mask, gc_mask, 0, 0, width, height, x, y);
  XFreeGC(disp, gc);
  XFreeGC(disp, gc_pmap);
  XFreeGC(disp, gc_mask);
  XFreePixmap(disp, tmp_pmap);
  XFreePixmap(disp, tmp_mask);
  free(s);
}

int
Fnlib_measure(FnlibData * fd, FnlibFont * fn,
	      int x, int y, int width, int height, int xin, int yin,
	      int size, FnlibStyle * style, unsigned char *text,
	      int px, int py, int *ret_x, int *ret_y,
	      int *ret_width, int *ret_height, int *ret_char_x,
	      int *ret_char_y, int *ret_char_width, int *ret_char_height)
{
  char                mode, orientation;
  int                 justification, spacing;
  int                 cx, cy;
  int                 char_num, index, wcount, lastw, wordw, linew, len,
                      linec, i, prevspace;
  int                 srcx, srcy, srcwidth, srcheight, destx, desty;
  unsigned char      *s;
  FnlibChar          *fchar;

  wordw = 0;
  mode = style->mode;
  orientation = style->orientation;
  justification = style->justification;
  spacing = style->spacing;
  len = strlen((char *)text);
  s = malloc(len + 1);
  *ret_x = x + width;
  *ret_y = y + height;
  *ret_width = 0;
  *ret_height = 0;
  switch (orientation)
    {
    case FONT_TO_RIGHT:
      cx = xin;
      cy = yin;
      char_num = 0;
      while ((char_num < len) && (cy < height))
	{
	  linew = 0;
	  linec = 0;
	  wcount = 0;
	  lastw = 0;
	  switch (mode)
	    {
	    case MODE_VERBATIM:
	      while ((text[char_num]) && (text[char_num] != '\n'))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      s[linec++] = text[char_num++];
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_CHAR:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= width))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= width)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_WORD:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= width))
		{
		  index = (int)text[char_num];
		  /* if "b " */
		  if (((char_num > 0) && (isspace(index)) && (!isspace(text[char_num - 1])))
		      || (char_num == (len - 1)))
		    {
		      wcount++;
		      wordw = linew;
		    }
		  /* if " b" */
		  if ((char_num > 0) && (!isspace(index)) && (isspace(text[char_num - 1])))
		    lastw = linec;
		  if (char_num == (len - 1))
		    {
		      lastw = linec + 1;
		    }
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= width)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      if ((text[char_num] == '\n') || (isspace(text[char_num])))
		{
		  wcount++;
		  wordw = linew;
		  lastw = linec;
		}
	      if (wcount > 0)
		{
		  char_num -= linec - lastw;
		  linec = lastw;
		  linew = wordw;
		}
	      s[linec] = 0;
	      break;
	    }
	  prevspace = 0;
	  cx += (((width - linew) * justification) >> 10) - (((width - linew) * spacing * justification) >> 20);
	  for (i = 0; i < linec; i++)
	    {
	      index = (int)s[i];
	      fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
	      if (fchar)
		{
		  if (fchar->current.pmap)
		    {
		      srcx = 0;
		      srcy = 0;
		      srcwidth = fchar->current.rendered_width;
		      srcheight = fchar->current.rendered_height;
		      destx = cx + fchar->current.x;
		      desty = cy + fchar->current.y;
		      if (destx < *ret_x)
			*ret_x = destx;
		      if (desty < *ret_y)
			*ret_y = desty;
		      if (((destx + srcwidth) - *ret_x) > *ret_width)
			*ret_width = (destx + srcwidth) - *ret_x;
		      if (((desty + srcheight) - *ret_y) > *ret_height)
			*ret_height = (desty + srcheight) - *ret_y;
		    }
		  cx += fchar->current.width;
		  cx += ((((width - linew) * spacing * (i + 1)) >> 10) / linec) - prevspace;
		  prevspace = (((width - linew) * spacing * (i + 1)) >> 10) / linec;
		}
	    }
	  if ((text[char_num] == ' ') || (text[char_num] == '\n'))
	    char_num++;
	  cx = xin;
	  cy += size;
	}
      break;
    case FONT_TO_DOWN:
      cx = width;
      cy = yin;
      char_num = 0;
      while ((char_num < len) && (cx >= 0))
	{
	  linew = 0;
	  linec = 0;
	  wcount = 0;
	  lastw = 0;
	  switch (mode)
	    {
	    case MODE_VERBATIM:
	      while ((text[char_num]) && (text[char_num] != '\n'))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      s[linec++] = text[char_num++];
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_CHAR:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= height))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= height)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_WORD:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= height))
		{
		  index = (int)text[char_num];
		  /* if "b " */
		  if (((char_num > 0) && (isspace(index)) && (!isspace(text[char_num - 1])))
		      || (char_num == (len - 1)))
		    {
		      wcount++;
		      wordw = linew;
		    }
		  /* if " b" */
		  if ((char_num > 0) && (!isspace(index)) && (isspace(text[char_num - 1])))
		    lastw = linec;
		  if (char_num == (len - 1))
		    {
		      lastw = linec + 1;
		    }
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= height)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      if ((text[char_num] == '\n') || (isspace(text[char_num])))
		{
		  wcount++;
		  wordw = linew;
		  lastw = linec;
		}
	      if (wcount > 0)
		{
		  char_num -= linec - lastw;
		  linec = lastw;
		  linew = wordw;
		}
	      break;
	    }
	  prevspace = 0;
	  cy += (((height - linew) * justification) >> 10) - (((height - linew) * spacing * justification) >> 20);
	  for (i = 0; i < linec; i++)
	    {
	      index = (int)s[i];
	      fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
	      if (fchar)
		{
		  if (fchar->current.pmap)
		    {
		      srcx = 0;
		      srcy = 0;
		      srcwidth = fchar->current.rendered_width;
		      srcheight = fchar->current.rendered_height;
		      destx = cx + fchar->current.x;
		      desty = cy + fchar->current.y;
		      if (destx < *ret_x)
			*ret_x = destx;
		      if (desty < *ret_y)
			*ret_y = desty;
		      if (((destx + srcwidth) - *ret_x) > *ret_width)
			*ret_width = (destx + srcwidth) - *ret_x;
		      if (((desty + srcheight) - *ret_y) > *ret_height)
			*ret_height = (desty + srcheight) - *ret_y;
		    }
		  cy += fchar->current.width;
		  cy += ((((height - linew) * spacing * (i + 1)) >> 10) / linec) - prevspace;
		  prevspace = (((height - linew) * spacing * (i + 1)) >> 10) / linec;
		}
	    }
	  if ((text[char_num] == ' ') || (text[char_num] == '\n'))
	    char_num++;
	  cy = yin;
	  cx -= size;
	}
      break;
    case FONT_TO_UP:
      cx = xin;
      cy = yin;
      char_num = 0;
      while ((char_num < len) && (cx < width))
	{
	  linew = 0;
	  linec = 0;
	  wcount = 0;
	  lastw = 0;
	  switch (mode)
	    {
	    case MODE_VERBATIM:
	      while ((text[char_num]) && (text[char_num] != '\n'))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      s[linec++] = text[char_num++];
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_CHAR:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= height))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= height)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_WORD:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= height))
		{
		  index = (int)text[char_num];
		  /* if "b " */
		  if (((char_num > 0) && (isspace(index)) && (!isspace(text[char_num - 1])))
		      || (char_num == (len - 1)))
		    {
		      wcount++;
		      wordw = linew;
		    }
		  /* if " b" */
		  if ((char_num > 0) && (!isspace(index)) && (isspace(text[char_num - 1])))
		    lastw = linec;
		  if (char_num == (len - 1))
		    {
		      lastw = linec + 1;
		    }
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= height)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      if ((text[char_num] == '\n') || (isspace(text[char_num])))
		{
		  wcount++;
		  wordw = linew;
		  lastw = linec;
		}
	      if (wcount > 0)
		{
		  char_num -= linec - lastw;
		  linec = lastw;
		  linew = wordw;
		}
	      break;
	    }
	  prevspace = 0;
	  cy += height - ((((height - linew) * justification) >> 10) - (((height - linew) * spacing * justification) >> 20));
	  for (i = 0; i < linec; i++)
	    {
	      index = (int)s[i];
	      fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
	      if (fchar)
		{
		  if (fchar->current.pmap)
		    {
		      srcx = 0;
		      srcy = 0;
		      srcwidth = fchar->current.rendered_width;
		      srcheight = fchar->current.rendered_height;
		      destx = cx + fchar->current.x;
		      desty = cy + fchar->current.y - fchar->current.rendered_height;
		      if (destx < *ret_x)
			*ret_x = destx;
		      if (desty < *ret_y)
			*ret_y = desty;
		      if (((destx + srcwidth) - *ret_x) > *ret_width)
			*ret_width = (destx + srcwidth) - *ret_x;
		      if (((desty + srcheight) - *ret_y) > *ret_height)
			*ret_height = (desty + srcheight) - *ret_y;
		    }
		  cy -= fchar->current.width;
		  cy -= ((((height - linew) * spacing * (i + 1)) >> 10) / linec) - prevspace;
		  prevspace = (((height - linew) * spacing * (i + 1)) >> 10) / linec;
		}
	    }
	  if ((text[char_num] == ' ') || (text[char_num] == '\n'))
	    char_num++;
	  cy = yin;
	  cx += size;
	}
      break;
    case FONT_TO_LEFT:
      cx = xin;
      cy = height;
      char_num = 0;
      while ((char_num < len) && (cy >= 0))
	{
	  linew = 0;
	  linec = 0;
	  wcount = 0;
	  lastw = 0;
	  switch (mode)
	    {
	    case MODE_VERBATIM:
	      while ((text[char_num]) && (text[char_num] != '\n'))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      s[linec++] = text[char_num++];
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_CHAR:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= width))
		{
		  index = (int)text[char_num];
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= width)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      break;
	    case MODE_WRAP_WORD:
	      while ((text[char_num]) && (text[char_num] != '\n') && (linew <= width))
		{
		  index = (int)text[char_num];
		  /* if "b " */
		  if (((char_num > 0) && (isspace(index)) && (!isspace(text[char_num - 1])))
		      || (char_num == (len - 1)))
		    {
		      wcount++;
		      wordw = linew;
		    }
		  /* if " b" */
		  if ((char_num > 0) && (!isspace(index)) && (isspace(text[char_num - 1])))
		    lastw = linec;
		  if (char_num == (len - 1))
		    {
		      lastw = linec + 1;
		    }
		  fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
		  if (fchar)
		    {
		      linew += fchar->current.width;
		      if (linew <= width)
			s[linec++] = text[char_num++];
		      else
			{
			  linew -= fchar->current.width;
			  break;
			}
		    }
		  else
		    char_num++;
		}
	      if ((text[char_num] == '\n') || (isspace(text[char_num])))
		{
		  wcount++;
		  wordw = linew;
		  lastw = linec;
		}
	      if (wcount > 0)
		{
		  char_num -= linec - lastw;
		  linec = lastw;
		  linew = wordw;
		}
	      break;
	    }
	  prevspace = 0;
	  cx += width - ((((width - linew) * justification) >> 10) - (((width - linew) * spacing * justification) >> 20));
	  for (i = 0; i < linec; i++)
	    {
	      index = (int)s[i];
	      fchar = _fnlib_get_char_mes(fd, fn, orientation, size, index);
	      if (fchar)
		{
		  if (fchar->current.pmap)
		    {
		      srcx = 0;
		      srcy = 0;
		      srcwidth = fchar->current.rendered_width;
		      srcheight = fchar->current.rendered_height;
		      destx = cx + fchar->current.x - fchar->current.rendered_width;
		      desty = cy + fchar->current.y;
		      if (destx < *ret_x)
			*ret_x = destx;
		      if (desty < *ret_y)
			*ret_y = desty;
		      if (((destx + srcwidth) - *ret_x) > *ret_width)
			*ret_width = (destx + srcwidth) - *ret_x;
		      if (((desty + srcheight) - *ret_y) > *ret_height)
			*ret_height = (desty + srcheight) - *ret_y;
		    }
		  cx -= fchar->current.width;
		  cx -= ((((width - linew) * spacing * (i + 1)) >> 10) / linec) - prevspace;
		  prevspace = (((width - linew) * spacing * (i + 1)) >> 10) / linec;
		}
	    }
	  if ((text[char_num] == ' ') || (text[char_num] == '\n'))
	    char_num++;
	  cx = xin;
	  cy -= size;
	}
      break;
    default:
      break;
    }
  free(s);
  return 1;
}

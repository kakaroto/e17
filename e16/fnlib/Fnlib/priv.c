#include "Fnlib.h"
#include "file.h"

#ifdef __EMX__
extern char *__XOS2RedirRoot(const char *);
#endif

void
_fnlib_read_cfg(FnlibData * fd, char *file)
{
  FILE               *f;
  char                s[2048], ss[2048];

#ifndef __EMX__
  f = fopen(file, "r");
#else
  f = fopen(file, "rt");
#endif
  if (!f)
    return;
  while (fgets(s, 2048, f))
    {
      sscanf(s, "%s", ss);
      if (s[0] != '#')
	{
	  if (!strcmp("FontDir", ss))
	    {
	      sscanf(s, "%*s %s", ss);
#ifdef __EMX__
	      if (ss[0] == '/' && isdir(__XOS2RedirRoot(ss)))
		Fnlib_add_dir(fd, __XOS2RedirRoot(ss));
	      else
#endif
	      if (isdir(ss))
		Fnlib_add_dir(fd, ss);
	    }
	}
    }
  fclose(f);
}

FnlibChar          *
_fnlib_get_char(FnlibData * fd, FnlibFont * fn, char orientation,
		int size, int index)
{
  int                 i, closest, dist, diff, newload;
  FnlibFontSize      *fsize;
  FnlibChar          *fchar;

  closest = -1;
  dist = 0x7fffffff;
  for (i = 0; i < fn->num; i++)
    {
      if (fn->fonts[i].orientation == orientation)
	{
	  diff = fn->fonts[i].size - size;
	  if ((diff < dist) && (diff >= 0))
	    {
	      closest = i;
	      dist = diff;
	    }
	}
    }
  if (closest < 0)
    {
      dist = 0x7fffffff;
      for (i = 0; i < fn->num; i++)
	{
	  if (fn->fonts[i].orientation == orientation)
	    {
	      diff = fn->fonts[i].size - size;
	      if (diff < 0)
		diff = -diff;
	      if (diff < dist)
		{
		  closest = i;
		  dist = diff;
		}
	    }
	}
    }
  if (closest < 0)
    {
      dist = 0x7fffffff;
      for (i = 0; i < fn->num; i++)
	{
	  if (fn->fonts[i].orientation == FONT_TO_RIGHT)
	    {
	      diff = fn->fonts[i].size - size;
	      if ((diff < dist) && (diff >= 0))
		{
		  closest = i;
		  dist = diff;
		}
	    }
	}
      if (closest < 0)
	{
	  dist = 0x7fffffff;
	  for (i = 0; i < fn->num; i++)
	    {
	      if (fn->fonts[i].orientation != orientation)
		{
		  diff = fn->fonts[i].size - size;
		  if (diff < 0)
		    diff = -diff;
		  if (diff < dist)
		    {
		      closest = i;
		      dist = diff;
		    }
		}
	    }
	}
      if (closest < 0)
	return NULL;
      fn->num++;
      fn->fonts = realloc(fn->fonts, sizeof(FnlibFontSize) * fn->num);
      fn->fonts[fn->num - 1].size = fn->fonts[closest].size;
      fn->fonts[fn->num - 1].num_chars = fn->fonts[closest].num_chars;
      fn->fonts[fn->num - 1].orientation = orientation;
      fn->fonts[fn->num - 1].default_char = fn->fonts[closest].default_char;
      fn->fonts[fn->num - 1].current.size = 0;
      fn->fonts[fn->num - 1].chars = malloc(fn->fonts[fn->num - 1].num_chars * sizeof(FnlibChar));
      for (i = 0; i < fn->fonts[fn->num - 1].num_chars; i++)
	{
	  fn->fonts[fn->num - 1].chars[i].file = fn->fonts[closest].chars[i].file;
	  if (fn->fonts[fn->num - 1].chars[i].file)
	    {
	      if (!fn->fonts[closest].chars[i].im)
		fn->fonts[closest].chars[i].im =
		  Imlib_load_image(fd->id, fn->fonts[closest].chars[i].file);
	      if (fn->fonts[closest].chars[i].im)
		{
		  fn->fonts[fn->num - 1].chars[i].im =
		    Imlib_clone_image(fd->id, fn->fonts[closest].chars[i].im);
		  switch (orientation)
		    {
		    case FONT_TO_DOWN:
		      Imlib_rotate_image(fd->id, fn->fonts[fn->num - 1].chars[i].im, 1);
		      Imlib_flip_image_horizontal(fd->id, fn->fonts[fn->num - 1].chars[i].im);
		      break;
		    case FONT_TO_UP:
		      Imlib_rotate_image(fd->id, fn->fonts[fn->num - 1].chars[i].im, 1);
		      Imlib_flip_image_vertical(fd->id, fn->fonts[fn->num - 1].chars[i].im);
		      break;
		    case FONT_TO_LEFT:
		      Imlib_flip_image_horizontal(fd->id, fn->fonts[fn->num - 1].chars[i].im);
		      Imlib_flip_image_vertical(fd->id, fn->fonts[fn->num - 1].chars[i].im);
		      break;
		    }
		  fn->fonts[fn->num - 1].chars[i].x =
		    fn->fonts[closest].chars[i].x;
		  fn->fonts[fn->num - 1].chars[i].y =
		    fn->fonts[closest].chars[i].y;
		  fn->fonts[fn->num - 1].chars[i].width =
		    fn->fonts[closest].chars[i].width;
		  fn->fonts[fn->num - 1].chars[i].current.x = 0;
		  fn->fonts[fn->num - 1].chars[i].current.y = 0;
		  fn->fonts[fn->num - 1].chars[i].current.width = 0;
		  fn->fonts[fn->num - 1].chars[i].current.rendered_width = 0;
		  fn->fonts[fn->num - 1].chars[i].current.rendered_height = 0;
		  fn->fonts[fn->num - 1].chars[i].current.pmap = 0;
		  fn->fonts[fn->num - 1].chars[i].current.mask = 0;
		  fn->fonts[fn->num - 1].chars[i].current.size = 0;
		}
	    }
	}
      closest = fn->num - 1;
    }
  if (closest < 0)
    return NULL;
  fsize = &(fn->fonts[closest]);
  if ((index >= fsize->num_chars) || (index < 0))
    fchar = &(fsize->chars[fsize->default_char]);
  else
    fchar = &(fsize->chars[index]);
  if (!fchar->file)
    return NULL;
  newload = 0;
  if (!fchar->im)
    {
      fchar->im = Imlib_load_image(fd->id, fchar->file);
      newload = 1;
    }
  if (!fchar->im)
    return NULL;
  if ((fchar->current.size != size) || (newload) || (!fchar->current.pmap))
    {
      fchar->current.size = size;
      if (fchar->current.pmap)
	Imlib_free_pixmap(fd->id, fchar->current.pmap);
      fchar->current.x = (fchar->x * size) / fsize->size;
      fchar->current.y = (fchar->y * size) / fsize->size;
      fchar->current.width = (fchar->width * size) / fsize->size;
      fchar->current.rendered_width = (fchar->im->rgb_width * size) / fsize->size;
      fchar->current.rendered_height = (fchar->im->rgb_height * size) / fsize->size;
      Imlib_render(fd->id, fchar->im, fchar->current.rendered_width,
		   fchar->current.rendered_height);
      fchar->current.pmap = Imlib_move_image(fd->id, fchar->im);
      fchar->current.mask = Imlib_move_mask(fd->id, fchar->im);
    }
  return fchar;
}

FnlibChar          *
_fnlib_get_char_mes(FnlibData * fd, FnlibFont * fn, char orientation,
		int size, int index)
{
  int                 i, closest, dist, diff, newload;
  FnlibFontSize      *fsize;
  FnlibChar          *fchar;

  closest = -1;
  dist = 0x7fffffff;
  for (i = 0; i < fn->num; i++)
    {
      if (fn->fonts[i].orientation == orientation)
	{
	  diff = fn->fonts[i].size - size;
	  if ((diff < dist) && (diff >= 0))
	    {
	      closest = i;
	      dist = diff;
	    }
	}
    }
  if (closest < 0)
    {
      dist = 0x7fffffff;
      for (i = 0; i < fn->num; i++)
	{
	  if (fn->fonts[i].orientation == orientation)
	    {
	      diff = fn->fonts[i].size - size;
	      if (diff < 0)
		diff = -diff;
	      if (diff < dist)
		{
		  closest = i;
		  dist = diff;
		}
	    }
	}
    }
  if (closest < 0)
    {
      dist = 0x7fffffff;
      for (i = 0; i < fn->num; i++)
	{
	  if (fn->fonts[i].orientation == FONT_TO_RIGHT)
	    {
	      diff = fn->fonts[i].size - size;
	      if ((diff < dist) && (diff >= 0))
		{
		  closest = i;
		  dist = diff;
		}
	    }
	}
      if (closest < 0)
	{
	  dist = 0x7fffffff;
	  for (i = 0; i < fn->num; i++)
	    {
	      if (fn->fonts[i].orientation == orientation)
		{
		  diff = fn->fonts[i].size - size;
		  if (diff < 0)
		    diff = -diff;
		  if (diff < dist)
		    {
		      closest = i;
		      dist = diff;
		    }
		}
	    }
	}
      if (closest < 0)
	return NULL;
      fn->num++;
      fn->fonts = realloc(fn->fonts, sizeof(FnlibFontSize) * fn->num);
      fn->fonts[fn->num - 1].size = fn->fonts[closest].size;
      fn->fonts[fn->num - 1].num_chars = fn->fonts[closest].num_chars;
      fn->fonts[fn->num - 1].orientation = orientation;
      fn->fonts[fn->num - 1].default_char = fn->fonts[closest].default_char;
      fn->fonts[fn->num - 1].current.size = 0;
      fn->fonts[fn->num - 1].chars = malloc(fn->fonts[fn->num - 1].num_chars * sizeof(FnlibChar));
      for (i = 0; i < fn->fonts[fn->num - 1].num_chars; i++)
	{
	  fn->fonts[fn->num - 1].chars[i].file = fn->fonts[closest].chars[i].file;
	  if (fn->fonts[fn->num - 1].chars[i].file)
	    {
	      fn->fonts[fn->num - 1].chars[i].x =
		fn->fonts[closest].chars[i].x;
	      fn->fonts[fn->num - 1].chars[i].y =
		fn->fonts[closest].chars[i].y;
	      fn->fonts[fn->num - 1].chars[i].width =
		fn->fonts[closest].chars[i].width;
	      fn->fonts[fn->num - 1].chars[i].current.x = 0;
	      fn->fonts[fn->num - 1].chars[i].current.y = 0;
	      fn->fonts[fn->num - 1].chars[i].current.width = 0;
	      fn->fonts[fn->num - 1].chars[i].current.rendered_width = 0;
	      fn->fonts[fn->num - 1].chars[i].current.rendered_height = 0;
	      fn->fonts[fn->num - 1].chars[i].current.pmap = 0;
	      fn->fonts[fn->num - 1].chars[i].current.mask = 0;
	      fn->fonts[fn->num - 1].chars[i].current.size = 0;
	    }
	}
      closest = fn->num - 1;
    }
  if (closest < 0)
    return NULL;
  fsize = &(fn->fonts[closest]);
  if ((index >= fsize->num_chars) || (index < 0))
    fchar = &(fsize->chars[fsize->default_char]);
  else
    fchar = &(fsize->chars[index]);
  if (!fchar->file)
    return NULL;
  newload = 0;
  if (!fchar->im)
    {
      newload = 1;
    }
  if (!fchar->im)
    return NULL;
  if ((fchar->current.size != size) || (newload) || (!fchar->current.pmap))
    {
      fchar->current.size = size;
      fchar->current.x = (fchar->x * size) / fsize->size;
      fchar->current.y = (fchar->y * size) / fsize->size;
      fchar->current.width = (fchar->width * size) / fsize->size;
    }
  return fchar;
}

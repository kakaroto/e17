#include "E-Mountbox.h"

static void
error_exit(void)
{
  Esync();
  Epplet_cleanup();
  exit(1);
}

static void
CallbackHelp(void *data)
{
  Epplet_show_about("E-Mountbox");
  return;
  data = NULL;
}


static void
CallbackConfigure(void *data)
{
  Epplet_dialog_ok("You are using a development version of this epplet.\n"
		   "This button doesn't do anything yet --\n"
		   "We apologize for the inconvenience :)");
  return;
  data = NULL;
}


static void
CallbackAnimate(void *data)
{
  static double k = 0.0;
  static double step = M_PI / 10.0;

  int i, j, linear, linear_w;
  double ratio;

  if (k < M_PI)
    {
      if (anim_mount)
	ratio = ((cos(k) + 1.0)/ 4.0) * 1.3;
      else
	ratio = ((cos(k + M_PI) + 1.0)/ 4.0) * 1.3;

      for (i=0; i<32; i++)
	{
	  for (j=0; j<44; j++)
	    {
	      if (!IsTransparent(current_tile->image, j, i))
		{
		  linear = 3*(i * 44 + j);
		  linear_w = (i*44*3*num_tiles) + (current_tile_index*44*3) + 3*j;
		    window_buf->im->rgb_data[linear] =
		    widescreen_buf->im->rgb_data[linear_w] = 
		      ratio * (widescreen_canvas_buf->im->rgb_data[linear_w])
		      + (1.0-ratio) * (current_tile->image->rgb_data[linear]);
		    window_buf->im->rgb_data[linear+1] =
		    widescreen_buf->im->rgb_data[linear_w+1] = 
		      ratio * (widescreen_canvas_buf->im->rgb_data[linear_w+1])
		      + (1.0-ratio) * (current_tile->image->rgb_data[linear+1]);
		    window_buf->im->rgb_data[linear+2] =
		    widescreen_buf->im->rgb_data[linear_w+2] = 
		      ratio * (widescreen_canvas_buf->im->rgb_data[linear_w+2])
		      + (1.0-ratio) * (current_tile->image->rgb_data[linear+2]);
		}
	    } 
	}
      Epplet_paste_buf(window_buf, Epplet_get_drawingarea_window(action_area), -2, -2);
      k += step;
      Epplet_timer(CallbackAnimate, NULL, 0.05, "Anim");
    }
  else
    k = 0.0;

  return;
  data = NULL;
}


void
UpdateView(int dir, int fast)
{
  int i,j;
  double start_t, delta_t, wait;
  double step = M_PI / 44;

  if (dir == 0)
    {
      for (i=0; i<32; i++)
	{
	  memcpy(window_buf->im->rgb_data + i * 44 * 3,
		 widescreen_buf->im->rgb_data + (i*44*3*num_tiles) + (current_tile_index*44*3),
		 44*3*sizeof(unsigned char));
	}
      Epplet_paste_buf(window_buf, Epplet_get_drawingarea_window(action_area), -2, -2);
    }
  else
    {
      for (j=0; j<=44; j++)
	{
	  if (!fast)
	    {
	      start_t = Epplet_get_time();
	      wait = fabs(cos(j*step)) / 100.0;
	      while ((delta_t = Epplet_get_time() - start_t) < wait) ;
	    }
	  for (i=0; i<32; i++)
	    {
	      memcpy(window_buf->im->rgb_data + i * 44 * 3,
		     widescreen_buf->im->rgb_data + (i*44*3*num_tiles) + (current_tile_index*44*3) + (dir)*j*3,
		     44*3*sizeof(unsigned char));
	    }
	  Epplet_paste_buf(window_buf, Epplet_get_drawingarea_window(action_area), -2, -2);
	}
    }
}


int
IsTransparent(ImlibImage *im, int x, int y)
{
  int        linear;
  ImlibColor ic;

  if (!im || x < 0 || y < 0 || x >= im->rgb_width || y >= im->rgb_height)
    return 0;

  Imlib_get_image_shape(id, im, &ic);
  if ((ic.r == -1) && (ic.g == -1) && (ic.b == -1))
    return 0;

  linear = 3*(y * im->rgb_width + x);

  if ((im->rgb_data[linear] == ic.r)
      && (im->rgb_data[linear+1] == ic.g)
      && (im->rgb_data[linear+2] == ic.b))
    return 1;

  return 0;
}


/* mount handling */
void        
SetupMounts(void)
{

  /* first, parse /etc/fstab to see what user-mountable mountpoints we have */
  if (!(ParseFstab()))
    {
      /* Couldn't read /etc/fstab */
      Epplet_dialog_ok("Could not read mountpoint information.");
      error_exit();
    }
  
  /* do we have user-mountable fs's at all? */
  if (num_tiles == 0)
    {
      Epplet_dialog_ok("Could not find any usable mountpoints.");
      error_exit();
    }

  /* now, check if these are actually mounted already */

  /* check for /proc/mounts */
  if (!(ParseProcMounts()))
    /* no? ok, check for /etc/mtab */
    if (!(ParseEtcMtab()))
      /* damnit, look if files exist in the mountpoints. */
      VisitMountPoints();

  /* Man, this code has far too many comments :) */
}


void            
AddMountPoint(char *device, char *path)
{
  Tile *newtile = NULL;
  char *tmp_dev = NULL;
  char *tmp_path = NULL;
  int   i;

  if (!tiles)
    {
      tiles = (Tile*)malloc(sizeof(Tile));
      if (tiles)
	{
	  memset(tiles, 0, sizeof(Tile));
	  num_tiles = 1;
	  current_tile = tiles;
	}
    }
  else
    {
      newtile = (Tile*)malloc(sizeof(Tile));
      if (newtile)
	{
	  memset(newtile, 0, sizeof(Tile));
	  newtile->next = tiles;
	  tiles->prev = newtile;
	  current_tile = tiles = newtile;
	  num_tiles++;
	}
    }
  current_tile_index = 0;

  if (current_tile)
    {
      current_tile->mountpoint = (MountPoint*)malloc(sizeof(MountPoint));
      if (current_tile->mountpoint)
	{
	  memset(current_tile->mountpoint, 0, sizeof(MountPoint));
	  if (device)
	    current_tile->mountpoint->device = strdup(device);
	  if (path)
	    current_tile->mountpoint->path = strdup(path);
	}
      if (device && path)
	{
	  tmp_path = strdup(path);
	  tmp_dev = strdup(device);
	  for (i=0; i<(int)strlen(tmp_path); i++)
	    tmp_path[i] = (char)(tolower(tmp_path[i]));
	  for (i=0; i<(int)strlen(tmp_dev); i++)
	    tmp_dev[i] = (char)(tolower(tmp_dev[i]));
	  if (tmp_path && tmp_dev)
	    {
	      if (strstr(tmp_dev, "fd"))
		{
		  current_tile->mountpoint->type = TYPE_FD;
		}
	      else if (strstr(tmp_dev, "cd") || strstr(tmp_path, "cd"))
		{
		  current_tile->mountpoint->type = TYPE_CD;
		}	      
	      else if (strstr(tmp_path, "zip"))
		{
		  current_tile->mountpoint->type = TYPE_ZIP;
		}
	      else
		{
		  current_tile->mountpoint->type = TYPE_HD;
		}
	      free(tmp_path);
	      free(tmp_dev);
	    }
	} 
    }
}


void
FreeImages(void)
{
  int i;

  for (i=0; i<MAXTYPE; i++)
    {
      Imlib_destroy_image(id, images[i]);
      images[i] = NULL;
    }
}


void        
FreeMounts(void)
{
  Tile *current, *tmp;

  current = tiles;
  while (current)
    {
      if (current->mountpoint)
	{
	  if (current->mountpoint->device)
	    free(current->mountpoint->device);
	  if (current->mountpoint->path)
	    free(current->mountpoint->path);
	  free(current->mountpoint);
	}
      /* images need _not_ be freed here */
      tmp = current;
      current = current->next;
      free(tmp);
    }
}


void        
Mount(MountPoint * mp)
{
  char s[1024];

  if (mp)
    {
      if (mp->mounted)
	return;
      if (mp->path)
	{
	  Esnprintf(s, sizeof(s), "%s %s", MOUNT_CMD, mp->path);
	  if (!Epplet_run_command(s))
	    {
	      mp->mounted = 1;
	      anim_mount = 1;
	      Epplet_timer(CallbackAnimate, NULL, 0, "Anim");
	    }
	  else
	    {
	      s[0] = 0;
	      Esnprintf(s, sizeof(s), "Could not mount %s.", mp->path);
	      Epplet_dialog_ok(s);
	    }
	}
    }
}


void        
Umount(MountPoint * mp)
{
  char s[1024];

  s[0] = 0;

  if (mp)
    {
      if (!(mp->mounted))
	return;
      if (mp->path)
	{
	  Esnprintf(s, sizeof(s), "%s %s", UMOUNT_CMD, mp->path);
	  if (!Epplet_run_command(s))
	    {
	      mp->mounted = 0;
	      anim_mount = 0;
	      Epplet_timer(CallbackAnimate, NULL, 0, "Anim");
	    }
	  else
	    {
	      s[0] = 0;
	      Esnprintf(s, sizeof(s), "Could not unmount %s.", mp->path);
	      Epplet_dialog_ok(s);
	    }
	}
    }
}


int        
ParseFstab(void)
{
  FILE *f;
  char  s[1024];
  char *token = NULL;
  char *info[4];
  int   i;

  if ((f = fopen(FSTAB, "r")) == NULL)
    return 0;
  *s = 0;
  for (; fgets(s, sizeof(s), f);)
    {
      /* skip comments and blank lines */
      if (!(*s) || (*s == '\n') || (*s == '#'))
        {
          continue;
        }

      /* parse out tokens we need */
      i = 0;
      token = strtok(s, " \t");
      if (token)
	{
	  info[i++] = strdup(token);
	}
      while((token = strtok(NULL, " \t")))
	{
	  info[i++] = strdup(token);
	}

     /* see if device is user-mountable */
      if (strstr(info[3], "user"))
	  {
	    AddMountPoint(info[0], info[1]);
	  }

      for (i=0; i<5; i++)
	if (info[i])
	  free(info[i]);
    }

  fclose(f);
  return 1;
}


int        
ParseProcMounts(void)
{
  FILE *f;
  char  s[1024];
  char *token = NULL, *device = NULL, *path = NULL;
  Tile *tile;

  if ((f = fopen(PROCMOUNTS, "r")) == NULL)
    return 0;
  *s = 0;
  for (; fgets(s, sizeof(s), f);)
    {
      /* skip comments and blank lines (shouldn't be there, actually ...)*/
      if (!(*s) || (*s == '\n') || (*s == '#'))
        {
          continue;
        }

      /* parse out tokens we need */
      token = strtok(s, " \t");
      if (token)
	{
	  device = strdup(token);
	}
      token = NULL;
      token = strtok(NULL, " \t");
      if (token)
	{
	  path = strdup(token);
	}

      /* set that device mounted in our list ... */
      tile = tiles;
      while (tile)
	{
	  if (tile->mountpoint)
	    {
	      if (!strcmp(tile->mountpoint->path, path))
		{
		  tile->mountpoint->mounted = 1;
		}
	    }
	  tile = tile->next;
	}
      
      if (device)
	free(device);
      if (path)
	free(path);
    }

  fclose(f);
  return 1;
}


int        
ParseEtcMtab(void)
{
  FILE *f;
  char  s[1024];
  char *token = NULL, *device = NULL, *path = NULL;
  Tile *tile;

  if ((f = fopen(ETCMTAB, "r")) == NULL)
    return 0;
  *s = 0;
  for (; fgets(s, sizeof(s), f);)
    {
      /* skip comments and blank lines (shouldn't be there, actually ...)*/
      if (!(*s) || (*s == '\n') || (*s == '#'))
        {
          continue;
        }

      /* parse out tokens we need */
      token = strtok(s, " \t");
      if (token)
	{
	  device = strdup(token);
	}
      token = NULL;
      token = strtok(NULL, " \t");
      if (token)
	{
	  path = strdup(token);
	}

      /* set that device mounted in our list ... */
      tile = tiles;
      while (tile)
	{
	  if (tile->mountpoint)
	    {
	      if (!strcmp(tile->mountpoint->path, path))
		{
		  tile->mountpoint->mounted = 1;
		}
	    }
	  tile = tile->next;
	}
      
      if (device)
	free(device);
      if (path)
	free(path);
    }

  fclose(f);
  return 1;
}


void
VisitMountPoints(void)
{
  DIR            *dir;
  int             num_entries;
  struct dirent  *dp;
  Tile           *tile = NULL;
  
  tile = tiles;
  while (tile)
    {
      if (tile->mountpoint)
	{
	  if (tile->mountpoint->path)
	    {
	      dir = NULL;
	      dir = opendir(tile->mountpoint->path);
	      if (dir)
		{
		  num_entries = 0;
		  for (num_entries = 0; (dp = readdir(dir)) != NULL; num_entries++);
		  if (num_entries > 2)
		    {
		      tile->mountpoint->mounted = 1;
		    }
		}
	    }
	}
      tile = tile->next;
    }
}


MountPoint *
FindMountPointByClick(int x, int y)
{
  if (!IsTransparent(current_tile->image, x, y))
    return current_tile->mountpoint;

  return NULL;
}


static void     
CallbackExit(void * data)
{
  data = NULL;
  FreeMounts();
  FreeImages();
  Esync();
  Epplet_cleanup();
  exit(0);
}


static void
CallbackButtonUp (void *data, Window win, int x, int y, int b)
{
  MountPoint *mountpoint = NULL;
  char        s[1024];

  if (win == Epplet_get_drawingarea_window(action_area))
    {
      mountpoint = FindMountPointByClick(x,y);
      if (mountpoint)
	{
	  if (b == 1)
	    {
	      if (mountpoint->mounted)
		Umount(mountpoint);
	      else
		Mount(mountpoint);
	    }
	  else
	    {
	      Epplet_gadget popup = Epplet_create_popup();

	      s[0] = 0;
	      Esnprintf(s, sizeof(s), "%s at %s.", mountpoint->device, mountpoint->path);
	      Epplet_add_popup_entry(popup, s, NULL, NULL, NULL);
	      Epplet_pop_popup(popup, 0);
	    }
	}
    }
  return;
  data = NULL;
}


static void
CallbackSlideLeft(void *data)
{
  if (current_tile->prev)
    {
      UpdateView(-1, 0);
      current_tile = current_tile->prev;
      current_tile_index--;
    }
  else
    {
      while (current_tile->next)
	{
	  UpdateView(+1, 1);
	  current_tile = current_tile->next;
	  current_tile_index++;
	}
    }
  return;
  data = NULL;
}


static void
CallbackSlideRight(void *data)
{
  if (current_tile->next)
    {
      UpdateView(1, 0);
      current_tile = current_tile->next;
      current_tile_index++;
    }
  else
    {
      while (current_tile->prev)
	{
	  UpdateView(-1, 1);
	  current_tile = current_tile->prev;
	  current_tile_index--;
	}
    }
  return;
  data = NULL;
}


static void     
CallbackExpose(void *data, Window win, int x, int y, int w, int h)
{
  UpdateView(0, 0);
  return;
  data = NULL;
  win = x = y = w = h = 0;
}

static void
in_cb(void *data, Window w) {

  Epplet_gadget_show(button_help);
  Epplet_gadget_show(button_config);
  Epplet_gadget_show(button_close);
  return;
  data = NULL;
  w = (Window) 0;
}

static void
out_cb(void *data, Window w) {

  Epplet_gadget_hide(button_help);
  Epplet_gadget_hide(button_config);
  Epplet_gadget_hide(button_close);
  return;
  data = NULL;
  w = (Window) 0;
}


void
SetupDefaults(void)
{
  int i;

  for (i=0; i<(int)(sizeof(defaults)/sizeof(ConfigItem)); i++)
    {
      if (!Epplet_query_config(defaults[i].key))
	Epplet_add_config(defaults[i].key, defaults[i].value);
    }
}


void
SetupGraphx(void)
{
  int         i, j, k, check, linear, linear_w;
  ImlibImage *tmp = NULL;
  ImlibBorder border;
  Tile       *tile;

  id = Epplet_get_imlib_data();

  /* load all images, scaled appropriately */
  memset(images, 0, MAXTYPE * sizeof(ImlibImage*));
  tmp = Imlib_load_image(id, Epplet_query_config("CD_IMAGE"));  
  if (tmp)
    {
      images[TYPE_CD] = Imlib_clone_scaled_image(id, tmp, 44, 32);
      Imlib_destroy_image(id, tmp);
      tmp = NULL;
    }
  tmp = Imlib_load_image(id, Epplet_query_config("HD_IMAGE"));  
  if (tmp)
    {
      images[TYPE_HD] = Imlib_clone_scaled_image(id, tmp, 44, 32);
      Imlib_destroy_image(id, tmp);
      tmp = NULL;
    }
  tmp = Imlib_load_image(id, Epplet_query_config("FD_IMAGE"));  
  if (tmp)
    {
      images[TYPE_FD] = Imlib_clone_scaled_image(id, tmp, 44, 32);
      Imlib_destroy_image(id, tmp);
      tmp = NULL;
    }
  tmp = Imlib_load_image(id, Epplet_query_config("ZIP_IMAGE"));  
  if (tmp)
    {
      images[TYPE_ZIP] = Imlib_clone_scaled_image(id, tmp, 44, 32);
      Imlib_destroy_image(id, tmp);
      tmp = NULL;
    }
  tmp = Imlib_load_image(id, Epplet_query_config("BG_IMAGE"));  
  if (tmp)
    {
      sscanf(Epplet_query_config("BG_BORDER"), "%i %i %i %i",
	     &(border.left), &(border.right), &(border.top), &(border.bottom));    
      Imlib_set_image_border(id, tmp, &border);
      images[TYPE_BG] = Imlib_clone_scaled_image(id, tmp, 44 * num_tiles, 32);
      Imlib_destroy_image(id, tmp);
      tmp = NULL;
    }

  /* see if we got all of them */
  for (check=1, i=0; i<MAXTYPE; i++)
    check = (check && images[i]);
  if (!check)
    {
      Epplet_dialog_ok("Could not load all images.");
      /* should I abort here ? .... hmmm dunno yet */
    }

  /* set tile images according to mountpoint types */
  tile = tiles;
  while (tile)
    {
      switch (tile->mountpoint->type)
	{
	case TYPE_CD:
	  tile->image = images[TYPE_CD];
	  break;
	case TYPE_FD:
	  tile->image = images[TYPE_FD];
	  break;
	case TYPE_ZIP:
	  tile->image = images[TYPE_ZIP];
	  break;
	default:
	  tile->image = images[TYPE_HD];
	  break;
	}
      tile = tile->next;
    }

  /* setup widescreen according to current mounts */
  window_buf = Epplet_make_rgb_buf(44, 32);  
  widescreen_buf = Epplet_make_rgb_buf((44 * num_tiles), 32);  
  widescreen_canvas_buf = Epplet_make_rgb_buf((44 * num_tiles), 32);  

  memcpy(widescreen_buf->im->rgb_data, images[TYPE_BG]->rgb_data,
	 sizeof(unsigned char) * 44 * 3 * num_tiles * 32); 
  memcpy(widescreen_canvas_buf->im->rgb_data, images[TYPE_BG]->rgb_data,
	 sizeof(unsigned char) * 44 * 3 * num_tiles * 32); 

  tile = tiles;
  for (k=0; k<num_tiles; k++, tile = tile->next)
    {
      for (i=0; i<32; i++)
	{
	  for (j=0; j<44; j++)
	    {
	      if (!IsTransparent(tile->image, j, i))
		{
		  linear = 3*(i * 44 + j);
		  linear_w = (i*44*3*num_tiles) + (k*44*3) + 3*j;
		  if (tile->mountpoint->mounted)
		    {
		      widescreen_buf->im->rgb_data[linear_w] = tile->image->rgb_data[linear];
		      widescreen_buf->im->rgb_data[linear_w+1] = tile->image->rgb_data[linear+1];
		      widescreen_buf->im->rgb_data[linear_w+2] = tile->image->rgb_data[linear+2];
		    }
		  else
		    {
		      widescreen_buf->im->rgb_data[linear_w] =
			0.65 * widescreen_buf->im->rgb_data[linear_w] + 0.35 * tile->image->rgb_data[linear];
		      widescreen_buf->im->rgb_data[linear_w+1] =
			0.65 * widescreen_buf->im->rgb_data[linear_w+1] + 0.35 * tile->image->rgb_data[linear+1];
		      widescreen_buf->im->rgb_data[linear_w+2] =
			0.65 * widescreen_buf->im->rgb_data[linear_w+2] + 0.35 * tile->image->rgb_data[linear+2];
		    }
		} 
	    }
	}      
    }


  Epplet_gadget_show((button_left = Epplet_create_button(NULL, NULL, 
							 2, 34, 0, 0, "ARROW_LEFT", 0, NULL, 
							 CallbackSlideLeft, NULL)));
  Epplet_gadget_show((button_right = Epplet_create_button(NULL, NULL, 
							  33, 34, 0, 0, "ARROW_RIGHT", 0, NULL, 
							  CallbackSlideRight, NULL)));
  Epplet_gadget_show((action_area = Epplet_create_drawingarea(2, 2, 44, 32)));
  button_help = Epplet_create_button(NULL, NULL, 3, 3, 0, 0, "HELP", 0, NULL, CallbackHelp, NULL);
  button_close = Epplet_create_button(NULL, NULL, 33, 3, 0, 0, "CLOSE", 0, NULL, CallbackExit, NULL);
  button_config = Epplet_create_button(NULL, NULL, 18, 3, 0, 0, "CONFIGURE", 0, NULL, CallbackConfigure, NULL);

  Epplet_register_focus_in_handler(in_cb, NULL);
  Epplet_register_focus_out_handler(out_cb, NULL);
  Epplet_register_expose_handler(CallbackExpose, NULL);
  Epplet_register_button_release_handler(CallbackButtonUp, NULL);

  /* Setup the current view */
  Epplet_show();
  UpdateView(0, 0);
}


int
main(int argc, char** argv)
 {
   Epplet_Init("E-Mountbox", "0.1", "Enlightenment Mount Epplet",
	       3, 3, argc, argv, 0);
   Epplet_load_config();

   SetupDefaults();
   SetupMounts();
   SetupGraphx();

   Epplet_Loop();
   error_exit();
}

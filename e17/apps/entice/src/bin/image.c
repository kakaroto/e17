#include "entice.h"

void image_create_list(int argc, char **argv)
{
  int i;

  for (i = 1; i < argc; i++)
    {
      Image *im;

      if (argv[i][0] == '/')
	{
	  /* CS */
	  /* printf("%s\n",argv[i]); */

	  im = e_image_new(argv[i]);
	}
      else
	{
	  char buf[4096];
	  char wd[4096];

	  getcwd(wd, sizeof(wd));
	  sprintf(buf, "%s/%s", wd, argv[i]);

	  /* CS */
	  /* printf("%s\n",buf); */

	  im = e_image_new(buf);
	}
      images = evas_list_append(images, im);
    }
  current_image = images;
}

void image_create_list_dir(char *dir)
{
  DIR *d;
  struct dirent *dent;
  Image *im;
  Evas_List l;

  d=opendir(dir);

  while( ( dent=readdir(d) )!=NULL )
  // while( readdir_r(d,dent,&dent) )
    {
      /* skip these */
      if( !strcmp(dent->d_name,".") || !strcmp(dent->d_name,"..")
	  || dent->d_name[0]=='.' )
	continue;

      /* CS */
      /* printf("%s\n",dent->d_name); */

      im = e_image_new(dent->d_name);
      images = evas_list_append(images, im);

      /* CS */
      /* printf("%p\n",images); */
    }
  closedir(d);

  current_image=images;
  /* CS */
  /* printf("%p\n",current_image); */

  /* CS */
  /*
  for(l=images ; l ; l=l->next)
    {
      im=(Image*)l->data;
      printf("%s\n",im->file);
      printf("%p\n",l);
      printf("%p\n\n",im);
    }
  */
}

void image_create_thumbnails(void)
{
  Evas_List l;
  int i;
	
  i = 1;
  for (l = images; l; l = l->next, i++)
    {
      Image *im;
	     
      im = l->data;

      /* CS */
      /* printf("%s\n",im->file); */

      im->o_thumb = evas_add_image_from_file(evas, IM"thumb.png");
      evas_callback_add(evas, im->o_thumb, CALLBACK_MOUSE_DOWN, e_list_click, l);
      evas_callback_add(evas, im->o_thumb, CALLBACK_MOUSE_IN, e_list_item_in, l);
      evas_callback_add(evas, im->o_thumb, CALLBACK_MOUSE_OUT, e_list_item_out, l);
      im->subst = 1;
      evas_set_image_border(evas, im->o_thumb, 4, 4, 4, 4);
      evas_move(evas, im->o_thumb, 2, 2 + ((48 + 2) * (i - 1)));
      evas_resize(evas, im->o_thumb, 48, 48);
      evas_set_image_fill(evas, im->o_thumb, 0, 0, 48, 48);
      evas_set_layer(evas, im->o_thumb, 210);
      evas_show(evas, im->o_thumb);
    }
}

void image_destroy_list(void)
{
  Evas_List l;
  Image *im;

  for (l = images; l; l = l->next)
    {
      im=l->data;

      evas_callback_del(evas,im->o_thumb,CALLBACK_MOUSE_DOWN );
      evas_callback_del(evas,im->o_thumb,CALLBACK_MOUSE_IN   );
      evas_callback_del(evas,im->o_thumb,CALLBACK_MOUSE_OUT  );

      evas_del_object(evas,im->o_thumb);

      e_image_free(im);
    }

  images=evas_list_free(images);
}

Image *e_image_new(char *file)
{
  Image *im;
   
  im = malloc(sizeof(Image));
  im->file = strdup(file);
  im->generator = 0;
  im->thumb = NULL;
  im->o_thumb = NULL;
  im->subst = 0;
  return im;
}

void e_image_free(Image *im)
{
  if (im->file) free(im->file);
  if (im->thumb) free(im->thumb);
  free(im);
}

void e_display_current_image(void)
{
  scroll_x = 0;
  scroll_y = 0;
  scroll_sx = 0;
  scroll_sy = 0;
  if (o_mini_image)
    {
      evas_del_object(evas, o_mini_image);
      o_mini_image = NULL;
    }
  if (current_image)
    {
      char title[4096];
	
      if (o_image)
	{
	  evas_del_object(evas, o_image);
	  o_image = NULL;
	}
      o_image = evas_add_image_from_file(evas, 
					 ((Image *)(current_image->data))->file);
      evas_callback_add(evas, o_image, CALLBACK_MOUSE_DOWN, next_image, NULL);
      evas_callback_add(evas, o_image, CALLBACK_MOUSE_UP, next_image_up, NULL);
      evas_callback_add(evas, o_image, CALLBACK_MOUSE_MOVE, next_image_move, NULL);
      evas_show(evas, o_image);
      if (evas_get_image_load_error(evas, o_image) != IMLIB_LOAD_ERROR_NONE)
	{
	  sprintf(txt_info[0], "Error LoadingFile: %s", ((Image *)(current_image->data))->file);
	  sprintf(txt_info[1], "");
	  sprintf(title, "Entice (Error Loading): %s",
		  ((Image *)(current_image->data))->file);
	  ecore_window_set_title(main_win, title);
	  evas_del_object(evas, o_image);
	  o_image = NULL;
	}
      else
	{
	  int w, h;
	     
	  evas_get_image_size(evas, o_image, &w, &h);
	  sprintf(txt_info[0], "File: %s", ((Image *)(current_image->data))->file);
	  sprintf(txt_info[1], "Size: %ix%i", w, h);
	  e_fade_info_in(0, NULL);
	     
	  sprintf(title, "Entice: %s",
		  ((Image *)(current_image->data))->file);
	  ecore_window_set_title(main_win, title);	     
	}
    }
  else
    {
      ecore_window_set_title(main_win, "Entice (No Image)");
      evas_del_object(evas, o_image);
      o_image = NULL;	
    }
  if ((o_image) && (current_image))
    {
      o_mini_image = evas_add_image_from_file(evas,
					      ((Image *)(current_image->data))->file);
    }
  e_handle_resize();
  e_fix_icons();
  e_scroll_list(0, NULL);
  e_fade_scroller_in(0, (void *)1);
}

void next_image(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  down_x = _x;
  down_y = _y;
  down_sx = scroll_x;
  down_sy = scroll_y;
  e_fade_scroller_in(0, NULL);
}

void next_image_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (((_x - down_x) * (_x - down_x)) +
      ((_y - down_y) * (_y - down_y)) > 9)
    {
      scroll_x = scroll_sx;
      scroll_y = scroll_sy;
      e_fade_scroller_out(0, NULL);
      return;
    }
  if ((_o == o_showpanel) && (panel_active)) return;
  if (!current_image)
    current_image = images;
  else
    {
      if ((_b == 1) && (current_image->next))
	current_image = current_image->next;
      else if (_b == 3)
	current_image = current_image->prev;
    }
  e_display_current_image();
}

void next_image_move(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (_b != 0)
    {
      scroll_x = _x - down_x + down_sx;
      scroll_y = _y - down_y + down_sy;
	
      e_handle_resize();
    }
}

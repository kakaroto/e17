#include "entice.h"

void
image_add_from_dnd(char *item)
{
   DIR                *d;
   struct dirent      *dent;
   Image              *im;
   char                buf[4096];

   if (e_file_is_dir(item))
     {
	d = opendir(item);
	while ((dent = readdir(d)) != NULL)
	  {
	     if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")
		 || dent->d_name[0] == '.')
		continue;

	     sprintf(buf, "%s/%s", item, dent->d_name);
	     if (e_file_is_dir(buf))
		image_add_from_dnd(buf);
	     else
	       {
		  im = e_image_new(buf);
		  im->subst = 1;
		  images =
		     evas_list_prepend_relative(images, im,
						current_image->data);
		  current_image = current_image->prev;
	       }
	  }
	closedir(d);
     }
   else
     {
	im = e_image_new(item);
	im->subst = 1;
	images = evas_list_prepend_relative(images, im, current_image->data);
	current_image = current_image->prev;
     }
   need_thumbs = 1;
   e_display_current_image();
   return;
}

void
image_create_list(int argc, char **argv)
{
   int                 i;

   for (i = 1; i < argc; i++)
     {
	Image              *im;

	if (argv[i][0] == '/')
	  {
	     /* CS */
	     /* printf("%s\n",argv[i]); */

	     im = e_image_new(argv[i]);
	  }
	else
	  {
	     char                buf[4096];
	     char                wd[4096];

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

void
image_create_list_dir(char *dir)
{
   DIR                *d;
   struct dirent      *dent;
   Image              *im;

   d = opendir(dir);

   while ((dent = readdir(d)) != NULL)
      // while( readdir_r(d,dent,&dent) )
     {
	/* skip these */
	if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")
	    || dent->d_name[0] == '.')
	   continue;

	/* CS */
	/* printf("%s\n",dent->d_name); */

	im = e_image_new(dent->d_name);
	images = evas_list_append(images, im);

	/* CS */
	/* printf("%p\n",images); */
     }
   closedir(d);

   current_image = images;
   /* CS */
   /* printf("%p\n",current_image); */

   /* CS */
   /*
    * for(l=images ; l ; l=l->next)
    * {
    * im=(Image*)l->data;
    * printf("%s\n",im->file);
    * printf("%p\n",l);
    * printf("%p\n\n",im);
    * }
    */
}

void
image_create_thumbnails(void)
{
   Evas_List          *l;
   int                 i;

   i = 1;
   for (l = images; l; l = l->next, i++)
     {
	Image              *im;

	im = l->data;

	/* CS */
	/* printf("%s\n",im->file); */

	im->o_thumb = evas_object_image_add(evas);
	evas_object_image_file_set(im->o_thumb, IM "thumb.png", NULL);
	evas_object_event_callback_add(im->o_thumb, EVAS_CALLBACK_MOUSE_MOVE,
				       e_list_item_drag, l);
	evas_object_event_callback_add(im->o_thumb, EVAS_CALLBACK_MOUSE_DOWN,
				       e_list_item_click, l);
	evas_object_event_callback_add(im->o_thumb, EVAS_CALLBACK_MOUSE_UP,
				       e_list_item_select, l);
	evas_object_event_callback_add(im->o_thumb, EVAS_CALLBACK_MOUSE_IN,
				       e_list_item_in, l);
	evas_object_event_callback_add(im->o_thumb, EVAS_CALLBACK_MOUSE_OUT,
				       e_list_item_out, l);
	im->subst = 1;
	evas_object_image_border_set(im->o_thumb, 4, 4, 4, 4);
	evas_object_move(im->o_thumb, 2, 2 + ((48 + 2) * (i - 1)));
	evas_object_resize(im->o_thumb, 48, 48);
	evas_object_image_fill_set(im->o_thumb, 0, 0, 48, 48);
	evas_object_layer_set(im->o_thumb, 210);
	evas_object_show(im->o_thumb);
     }
}

void
image_destroy_list(void)
{
   Evas_List          *l;
   Image              *im;

   for (l = images; l; l = l->next)
     {
	im = l->data;

	evas_object_event_callback_del(im->o_thumb, EVAS_CALLBACK_MOUSE_DOWN,
				       e_list_item_click);
	evas_object_event_callback_del(im->o_thumb, EVAS_CALLBACK_MOUSE_IN,
				       e_list_item_in);
	evas_object_event_callback_del(im->o_thumb, EVAS_CALLBACK_MOUSE_OUT,
				       e_list_item_out);

	evas_object_del(im->o_thumb);

	e_image_free(im);
     }

   images = evas_list_free(images);
}

Image              *
e_image_new(char *file)
{
   Image              *im;

   im = malloc(sizeof(Image));
   im->file = strdup(file);
   im->generator = 0;
   im->thumb = NULL;
   im->o_thumb = NULL;
   im->subst = 0;
   return im;
}

void
e_image_free(Image * im)
{
   if (im->file)
      free(im->file);
   if (im->thumb)
      free(im->thumb);
   free(im);
}

void
image_delete(Image * im)
{
   if (im)
     {
	if (im->o_thumb)
	   evas_object_del(im->o_thumb);

	images = evas_list_remove(images, (void *)im);
	e_image_free(im);
     }
}

static void
e_flip_object(Evas_Object *obj, int direction)
{
   int w;
   int h;
   DATA32 *image_data;
   Imlib_Image image;

   if (!obj)
       return;

   /* Get image data from Evas */
   evas_object_image_size_get(obj, &w, &h);
   image_data = evas_object_image_data_get(obj, 0);
   if (!image_data)
     {
         evas_object_image_data_set(obj, image_data);
         return;
     }

   /* Set up imlib image */
   image = imlib_create_image_using_copied_data(w, h, image_data);
   evas_object_image_data_set(obj, image_data);
   imlib_context_set_image(image);

   /* Flip image */
   if (direction==1)
         imlib_image_flip_horizontal();
   else
         imlib_image_flip_vertical();

   /* Get image data from Imlib */
   image_data = imlib_image_get_data_for_reading_only();

   /* Set Evas Image Data */
   evas_object_image_size_set(obj, w, h);
   evas_object_image_data_copy_set(obj, image_data);

   /* Free Imlib image */
   imlib_image_put_back_data(image_data);
   imlib_free_image();
}

static void
e_flip_current_image(int direction)
{
   Image *im;

   if (!current_image || !current_image->data)
       return;

   im = (Image *) (current_image->data);

   /* Flip image */
   e_flip_object(o_image, direction);
   e_flip_object(o_mini_image, direction);
   e_flip_object(im->o_thumb, direction);

   /* Update Display */
   e_turntable_reset();
   e_handle_resize();
   e_fix_icons();
   e_scroll_list(0, NULL);
   e_fade_scroller_in(0, (void *)1);

}

void
e_flip_h_current_image(void)
{
	e_flip_current_image(1);
}

void
e_flip_v_current_image(void)
{
	e_flip_current_image(2);
}

static void
e_rotate_object(Evas_Object *obj, int rotation)
{
   int w;
   int h;
   DATA32 *image_data;
   Imlib_Image image;

   if (!obj)
       return;

   /* Get image data from Evas */
   evas_object_image_size_get(obj, &w, &h);
   image_data = evas_object_image_data_get(obj, 0);
   if (!image_data)
     {
         evas_object_image_data_set(obj, image_data);
         return;
     }

   /* Set up imlib image */
   image = imlib_create_image_using_copied_data(w, h, image_data);
   evas_object_image_data_set(obj, image_data);
   imlib_context_set_image(image);

   /* Rotate image */
   imlib_image_orientate(rotation);
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   /* Get image data from Imblib */
   image_data = imlib_image_get_data_for_reading_only();

   /* Set Evas Image Data */
   evas_object_image_size_set(obj, w, h);
   evas_object_image_data_copy_set(obj, image_data);

   /* Free Imlib image */
   imlib_image_put_back_data(image_data);
   imlib_free_image();
}

static void
e_rotate_current_image(int rotation)
{
   Image *im;

   if (!current_image || !current_image->data)
       return;

   im = (Image *) (current_image->data);

   /* Rotate image */
   e_rotate_object(o_image, rotation);
   e_rotate_object(o_mini_image, rotation);
   e_rotate_object(im->o_thumb, rotation);

   /* Update Display */
   e_turntable_reset();
   e_handle_resize();
   e_fix_icons();
   e_scroll_list(0, NULL);
   e_fade_scroller_in(0, (void *)1);

}

void
e_rotate_r_current_image(void)
{
	e_rotate_current_image(1);
}

void
e_rotate_l_current_image(void)
{
	e_rotate_current_image(3);
}

#define TURNTABLE_COUNT 20
Imlib_Image turntable_image[TURNTABLE_COUNT];
int turntable_image_no = -1;

void
e_turntable_object_init(Evas_Object *obj)
{
   int i;
   int w;
   int h;
   double angle;
   DATA32 *image_data;
   Imlib_Image image;

   if (!obj || turntable_image_no >= 0)
       return;

   	/* Get image data from Evas */
   evas_object_image_size_get(obj, &w, &h);
   image_data = evas_object_image_data_get(obj, 0);
   if (!image_data)
     {
         evas_object_image_data_set(obj, image_data);
         return;
     }

   /* Set up imlib image */
   image = imlib_create_image_using_copied_data(w, h, image_data);
   evas_object_image_data_set(obj, image_data);

   angle = 0;
   for(i = 0 ; i < TURNTABLE_COUNT ; i ++) {
	angle = i * 360 * acos(0) / 90 / TURNTABLE_COUNT;
   	imlib_context_set_image(image);
   	turntable_image[i] = imlib_create_rotated_image(angle);
   	imlib_context_set_image(turntable_image[i]);
   }
	
  /* Free Imlib image */
  imlib_context_set_image(image);
  imlib_free_image();

  turntable_image_no = 0;
}

static void
e_turntable_reset()
{
   int i;

   if (turntable_image_no < 0)
       return;

   turntable_image_no = -1;

   for(i = 0 ; i < TURNTABLE_COUNT ; i ++) {
   	imlib_context_set_image(turntable_image[i]);
	imlib_free_image();
   }
}

static void
e_turntable_object_next(Evas_Object *obj, int rotation)
{
   int w;
   int h;
   DATA32 *image_data;

   if (!obj)
       return;

   e_turntable_object_init(obj);

   if (rotation == 1) {
      if(++turntable_image_no >= TURNTABLE_COUNT) {
	   turntable_image_no = 0;
      }
   }
   else {
      if(--turntable_image_no < 0) {
	   turntable_image_no = TURNTABLE_COUNT - 1;
      }
   }

   /* Get image data from Imblib */
   imlib_context_set_image(turntable_image[turntable_image_no]);
   image_data = imlib_image_get_data_for_reading_only();
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   /* Set Evas Image Data */
   evas_object_image_size_set(obj, w, h);
   evas_object_image_data_set(obj, image_data);

  imlib_image_put_back_data(image_data);

}

static void
e_turntable_object(int rotation, Evas_Object *obj)
{
   if (turntable_image_no < 0)
	 return;

   e_turntable_object_next(obj, rotation);
   ecore_add_event_timer("e_turntable_object()", 
		   (double)60/(double)45/(double)TURNTABLE_COUNT, 
		   e_turntable_object, rotation, o_image);

   /* Update Display */
   e_handle_resize();
}

static void
e_turntable_current_image(int rotation)
{
   if (!current_image || !current_image->data)
       return;

   e_turntable_object_init(o_image);
   e_turntable_object(rotation, o_image);

}

void
e_turntable_r_current_image(void)
{
	e_turntable_current_image(1);
}

void
e_turntable_l_current_image(void)
{
	e_turntable_current_image(2);
}

void
e_delete_current_image(void)
{
   Evas_List          *l = NULL;

   Image              *im;

   if (!current_image || !current_image->data)
	return;

   im = (Image *) (current_image->data);

   if (im->file)
      unlink(im->file);
   if (im->thumb)
      unlink(im->thumb);
   if (current_image->next)
      l = current_image->next;
   else if (current_image->prev)
      l = current_image->prev;
   else
      l = NULL;

   if (im->o_thumb)
      evas_object_del(im->o_thumb);
   e_image_free((Image *) current_image->data);
   images = evas_list_remove(images, current_image->data);

   current_image = l;

   e_display_current_image();
}

void
e_save_current_image(void)
{
   int w;
   int h;
   Image *im;
   DATA32 *image_data;
   Imlib_Image image;
   int alpha_team; /* Speed Racer! */
   const char *format;

   if (!current_image || !current_image->data || !o_image)
       return;

   im = (Image *) (current_image->data);

   /* Get image data from Evas */
   evas_object_image_size_get(o_image, &w, &h);
   image_data = evas_object_image_data_get(o_image, 0);
   if (!image_data)
     {
         evas_object_image_data_set(o_image, image_data);
         return;
     }

   /* Set up imlib image */
   image = imlib_create_image_using_copied_data(w, h, image_data);
   evas_object_image_data_set(o_image, image_data);
   imlib_context_set_image(image);

   alpha_team = evas_object_image_alpha_get(o_image);
   format = strrchr(im->file, '.') + 1;

   /* Save Image */
   imlib_image_set_format(format);
   imlib_image_set_has_alpha(alpha_team);  /* Go Speed, Go */
   imlib_save_image(im->file);


   /* Free Imlib image */
   imlib_free_image();
}
   

void
e_display_current_image(void)
{
   scroll_x = 0;
   scroll_y = 0;
   scroll_sx = 0;
   scroll_sy = 0;

   e_turntable_reset();
   if (o_mini_image)
     {
	evas_object_del(o_mini_image);
	o_mini_image = NULL;
     }
   if (current_image)
     {
	char                title[4096];

	if (o_image)
	  {
	     evas_object_del(o_image);
	     o_image = NULL;
	  }
	o_image = evas_object_image_add(evas);
	evas_object_image_file_set(o_image,
				   ((Image *) (current_image->data))->file,
				   NULL);
	evas_object_event_callback_add(o_image, EVAS_CALLBACK_MOUSE_DOWN,
				       next_image, NULL);
	evas_object_event_callback_add(o_image, EVAS_CALLBACK_MOUSE_UP,
				       next_image_up, NULL);
	evas_object_event_callback_add(o_image, EVAS_CALLBACK_MOUSE_MOVE,
				       next_image_move, NULL);
	evas_object_show(o_image);
	if (evas_object_image_load_error_get(o_image) != EVAS_LOAD_ERROR_NONE)
	  {
	     sprintf(txt_info[0], "Error LoadingFile: %s",
		     ((Image *) (current_image->data))->file);
	     *txt_info[1] = '\0';
	     sprintf(title, "Entice (Error Loading): %s",
		     ((Image *) (current_image->data))->file);
	     ecore_window_set_title(main_win, title);
	     evas_object_del(o_image);
	     o_image = NULL;
	  }
	else
	  {
	     int                 w, h;

	     evas_object_image_size_get(o_image, &w, &h);
	     sprintf(txt_info[0], "File: %s",
		     ((Image *) (current_image->data))->file);
	     sprintf(txt_info[1], "Size: %ix%i", w, h);
	     e_fade_info_in(0, NULL);

	     sprintf(title, "Entice: %s",
		     ((Image *) (current_image->data))->file);
	     ecore_window_set_title(main_win, title);
	  }
     }
   else
     {
	ecore_window_set_title(main_win, "Entice (No Image)");
          {
	     evas_object_del(o_image);
	     o_image = NULL;
	  }
     }
   if ((o_image) && (current_image))
     {
	o_mini_image = evas_object_image_add(evas);
	evas_object_image_smooth_scale_set(o_mini_image, 0);
	evas_object_image_file_set(o_mini_image,
				   ((Image *) (current_image->data))->file,
				   NULL);
     }
   e_handle_resize();
   e_fix_icons();
   e_scroll_list(0, NULL);
   e_fade_scroller_in(0, (void *)1);
}

void
next_image(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;

   ev = event_info;

   down_x = ev->output.x;
   down_y = ev->output.y;
   down_sx = scroll_x;
   down_sy = scroll_y;
   e_fade_scroller_in(0, NULL);
}

void
next_image_up(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;

   ev = event_info;
   if (((ev->output.x - down_x) * (ev->output.x - down_x)) +
       ((ev->output.y - down_y) * (ev->output.y - down_y)) > 9)
     {
	scroll_x = scroll_sx;
	scroll_y = scroll_sy;
	e_fade_scroller_out(0, NULL);
	return;
     }
   if ((obj == o_showpanel) && (panel_active))
      return;
   if (!current_image)
      current_image = images;
   else
     {
	if ((ev->button == 1) && (current_image->next))
	   current_image = current_image->next;
	else if (ev->button == 3)
	   current_image = current_image->prev;
     }
   e_display_current_image();
}

void
next_image_move(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;

   ev = event_info;
   if (ev->buttons != 0)
     {
	scroll_x = ev->cur.output.x - down_x + down_sx;
	scroll_y = ev->cur.output.y - down_y + down_sy;

	e_handle_resize();
     }
}

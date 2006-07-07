#include "Photo.h"

static char *_display_init(void);
static void  _display_shutdown(void);

/*
 * Public functions
 */

int photo_picture_init(void)
{
   E_Container *cont;

   cont = e_container_current_get(e_manager_current_get());
   photo->e_evas = cont->bg_evas;

   if (!photo_picture_local_init())
     return 0;
   //if (!photo_picture_net_init())
   // return 0;

   if (!_display_init())
     return 0;

   return 1;
}

void photo_picture_shutdown(void)
{
   photo_picture_setbg_purge(1);

   _display_shutdown();

   photo_picture_local_shutdown();
   //photo_picture_net_shutdown();
}


int photo_picture_load(Picture *pic, Evas *evas)
{
   Evas_Object *im = NULL;

   if (pic->picture)
     return 1;

   im = photo_picture_object_get(pic, evas);
   if (!im)
     return 0;
   
   pic->picture = im;

   return 1;
}

void photo_picture_unload(Picture *pic)
{
   if (!pic->picture)
     return;

   evas_object_del(pic->picture);
   pic->picture = NULL;
}


int photo_picture_free(Picture *p, int force, int force_now)
{
   if (p->pi)
     {
        if (!force) return 0;
        if (!force_now)
          {
             p->delete = 1;
             return 0;
          }
     }

   if (p->path) free(p->path);
   if (p->thumb_path) free(p->thumb_path);
   if (p->picture) evas_object_del(p->picture);

   if (p->infos.name) free(p->infos.name);
   if (p->infos.author) free(p->infos.author);
   if (p->infos.where_from) free(p->infos.where_from);
   if (p->infos.date) free(p->infos.date);
   if (p->infos.comments) free(p->infos.comments);

   photo_picture_histo_picture_del(p);

   free(p);
   return 1;
}

Evas_Object *photo_picture_object_get(Picture *pic, Evas *evas)
{
   Evas_Object *im = NULL;
   Eet_File *ef;
   Evas_Coord sw, sh;

   if (!pic) return NULL;

   if (pic->thumb_path)
     {
        /* load picture thumb */
        ef = eet_open(pic->thumb_path, EET_FILE_MODE_READ);
        if (!ef)
          {
             eet_close(ef);
             return 0;
          }

        im = e_icon_add(evas);
        e_icon_file_key_set(im, pic->thumb_path, "/thumbnail/data");

        e_icon_size_get(im, &sw, &sh);
        evas_object_resize(im, sw, sh);
        e_icon_fill_inside_set(im, 1);

        /* debug : modify e_icon */
        /*
        im = e_icon_add(evas);
        e_icon_file_key_set(im, pic->thumb_path, "/thumbnail/data");
        e_icon_size_get(im, &sw, &sh);
        evas_object_resize(im, pic->original_w, pic->original_h);
        e_icon_fill_inside_set(im, 1);
        */

        /* debug : try without e_icon */
        /*
        im = evas_object_image_add(evas);
        evas_object_image_file_set(im, pic->thumb_path, "/thumbnail/data");
	evas_object_image_fill_set(im, 0, 0, pic->original_w, pic->original_h);
        evas_object_resize(im, pic->original_w, pic->original_h);
        evas_object_move(im, 0, 0);
        */

        eet_close(ef);
     }
   else
     {
        /* load picture */
        im = e_icon_add(evas);
        e_icon_file_set(im, pic->path);

        e_icon_size_get(im, &sw, &sh);
        evas_object_resize(im, sw, sh);
        e_icon_fill_inside_set(im, 1);
     }

   return im;
}

char *photo_picture_name_get(char *url)
{
   char buf[4096];
   char *name, *ext;
   int name_l;

   name = strrchr(url, '/');
   if (name)
     name++;
   else
     name = url;

   ext = strrchr(name, '.');
   if (ext)
     name_l = strlen(name) - strlen(ext);
   else
     name_l = strlen(name);
   
   strncpy(buf, name, name_l);
   name[name_l] = '\0';

   return strdup(name);
}

char *photo_picture_infos_get(Picture *p)
{
   char buf[4096];
   char buf_ext[4096];

   if (ecore_file_exists(p->path))
     {
        time_t date;
        char *date_ascii;

        date = ecore_file_mod_time(p->path);
        date_ascii = (char *)ctime(&date);
        date_ascii[strlen(date_ascii) - 1] = ' ';
        snprintf(buf_ext, sizeof(buf_ext),
                 "<underline=on underline_color=#000>Date :</> %s<br>"
                 "<underline=on underline_color=#000>Size :</> %.2fMo",
                 date_ascii, (float)ecore_file_size(p->path) / 1000000.0);
     }
   else
     {
        snprintf(buf_ext, sizeof(buf_ext), "This file does not exist anymore on the disk !");
     }

   snprintf(buf, sizeof(buf),
            "<underline=on underline_color=#000>Picture path :</> %s<br>"
	    "<br>"
            "%s",
            p->path,
	    buf_ext);

   return strdup(buf);
}

void photo_picture_setbg_add(char *name)
{
   char buf[4096];
   char *home;

   home = e_user_homedir_get();
   snprintf(buf, sizeof(buf), "%s/.e/e/backgrounds/%s.edj", home, name);
   free(home);

   photo->setbg_topurge = evas_list_append(photo->setbg_topurge, strdup(buf));
}

void photo_picture_setbg_purge(int shutdown)
{
   char *file;
   int n = 0;

   while( (file = evas_list_nth(photo->setbg_topurge, n)) )
     {
        n++;
        DMAIN(("set_bg check remove %s", file));
        if (!e_config->desktop_default_background ||
            strcmp(e_config->desktop_default_background, file))
          {
             DMAIN(("set_bg remove %s !", file));
             if (ecore_file_exists(file))
               ecore_file_unlink(file);
             photo->setbg_topurge = evas_list_remove(photo->setbg_topurge, file);
             free(file);
             n--;
          }
        else
          {
             if (shutdown)
               free(file);
          }
     }

   if (shutdown)
     evas_list_free(photo->setbg_topurge);

   return;
}

/*
 * Private functions
 *
 */

static char *
_display_init(void)
{
   char *display = NULL;
   char *tmp;

   tmp = getenv("DISPLAY");
   if (tmp)
     display = strdup(tmp);

   /* make sure the display var is of the form name:0.0 or :0.0 */
   if (display)
     {
        char *p;
        char buf[1024];

        p = strrchr(display, ':');
        if (!p)
          {
             snprintf(buf, sizeof(buf), "%s:0.0", display);
             free(display);
             display = strdup(buf);
          }
        else
          {
             p = strrchr(p, '.');
             if (!p)
               {
                  snprintf(buf, sizeof(buf), "%s.0", display);
                  free(display);
                  display = strdup(buf);
               }
          }
     }
   else
     display = strdup(":0.0");

   /* init e Lib */
   if (display)
     e_lib_init(display);
   else
     {
        display = strdup(":0.0");
        e_lib_init(display);
     }

   photo->display = display;

   return display;
}

static void
_display_shutdown(void)
{
   if (!photo->display)
     return;

   E_FREE(photo->display);
   e_lib_shutdown();
}

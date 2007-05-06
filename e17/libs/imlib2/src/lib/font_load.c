#include "config.h"
#include "common.h"
#include "colormod.h"
#include "image.h"
#include "blend.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "font.h"
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include "file.h"
#include "updates.h"
#include "rgbadraw.h"
#include "rotate.h"

extern FT_Library   ft_lib;

static int          font_cache_usage = 0;
static int          font_cache = 0;
static char       **fpath = NULL;
static int          fpath_num = 0;
static Imlib_Object_List *fonts = NULL;

static int          font_modify_cache_cb(Imlib_Hash * hash, const char *key,
                                         void *data, void *fdata);
static int          font_flush_free_glyph_cb(Imlib_Hash * hash, const char *key,
                                             void *data, void *fdata);

/* FIXME now! listdir() from evas_object_text.c */

/* separate fontname and size, find font file 
*/
static char*
imlib_font_find_file(const char *fontname, int *out_size)
{
   int                 j, size;
   char               *name = NULL, *file = NULL, *tmp = NULL;

   /* split font name (in format name/size) */
   for (j = strlen(fontname) - 1; (j >= 0) && (fontname[j] != '/'); j--);
   /* no "/" in font after the first char */
   if (j <= 0)
      return NULL;
   /* get size */
   size = atoi(&(fontname[j + 1]));
   /* split name in front off */
   name = malloc((j + 1) * sizeof(char));
   memcpy(name, fontname, j);
   name[j] = 0;
   /* find file if it exists */
   tmp = malloc(strlen(name) + 4 + 1);
   if (!tmp)
     {
        free(name);
        return NULL;
     }
   sprintf(tmp, "%s.ttf", name);
   if (__imlib_FileIsFile(tmp))
      file = strdup(tmp);
   else
     {
        sprintf(tmp, "%s.TTF", name);
        if (__imlib_FileIsFile(tmp))
           file = strdup(tmp);
        else
          {
             sprintf(tmp, "%s", name);
             if (__imlib_FileIsFile(tmp))
                file = strdup(tmp);
          }
     }
   free(tmp);
   if (!file)
     {
        for (j = 0; (j < fpath_num) && (!file); j++)
          {
             tmp = malloc(strlen(fpath[j]) + 1 + strlen(name) + 4 + 1);
             if (!tmp)
               {
                  free(name);
                  return NULL;
               }
             else
               {
                  sprintf(tmp, "%s/%s.ttf", fpath[j], name);
                  if (__imlib_FileIsFile(tmp))
                     file = strdup(tmp);
                  else
                    {
                       sprintf(tmp, "%s/%s.TTF", fpath[j], name);
                       if (__imlib_FileIsFile(tmp))
                          file = strdup(tmp);
                       else
                         {
                            sprintf(tmp, "%s/%s", fpath[j], name);
                            if (__imlib_FileIsFile(tmp))
                               file = strdup(tmp);
                         }
                    }
               }
             free(tmp);
          }
     }
   free(name); *out_size = size;
   return file;
}

/* find font file, start imlib_font_load() */
ImlibFont          *
imlib_font_load_joined(const char *fontname)
{
   char       *file;
   ImlibFont  *fn;
   int size;

   file = imlib_font_find_file(fontname, &size);

   /* didnt find a file? abort */
   if (!file)
      return NULL;
   fn = imlib_font_load(file, size);
   free(file);
   return fn;
}

static ImlibFont *
imlib_font_create_font_struct(const char *name, int size)
{
   int                 error;
   ImlibFont          *fn;
   char               *file;

   imlib_font_init();

   fn = malloc(sizeof(ImlibFont));
   file = (char *)name;

   error = FT_New_Face(ft_lib, file, 0, &(fn->ft.face));
   if (error)
     {
        free(fn);
        return NULL;
     }
   error = FT_Set_Char_Size(fn->ft.face, 0, (size * 64), 96, 96);
   if (error)
      error = FT_Set_Pixel_Sizes(fn->ft.face, 0, size);
   if (error)
     {
        int                 i;
        int                 chosen_size = 0;
        int                 chosen_width = 0;

        for (i = 0; i < fn->ft.face->num_fixed_sizes; i++)
          {
             int                 s;
             int                 d, cd;

             s = fn->ft.face->available_sizes[i].height;
             cd = chosen_size - size;
             if (cd < 0)
                cd = -cd;
             d = s - size;
             if (d < 0)
                d = -d;
             if (d < cd)
               {
                  chosen_width = fn->ft.face->available_sizes[i].width;
                  chosen_size = s;
               }
             if (d == 0)
                break;
          }
        error = FT_Set_Pixel_Sizes(fn->ft.face, chosen_width, chosen_size);
        if (error)
          {
             /* couldn't choose the size anyway... what now? */
          }
     }

   error = FT_Select_Charmap(fn->ft.face, ft_encoding_unicode);
   if (error)
     {
     }

   fn->file = strdup(file);
   fn->name = strdup(file);
   fn->size = size;

   fn->glyphs = NULL;
   fn->next_in_set = NULL;
   fn->usage = 0;

   fn->references = 1;

   return fn;
}

ImlibFont          *
imlib_font_load(const char *name, int size)
{
   ImlibFont          *fn;

   fn = imlib_font_find(name, size);
   if (fn)
      return fn;

   if((fn = imlib_font_create_font_struct(name, size)) != NULL)
     fonts = imlib_object_list_prepend(fonts, fn);

   return fn;
}

#define MAX_FONTNAMES 31

static char *skip_white(char *s)
{
    while(isspace(*s)) ++s;
    return s;
}

/* Skip duplicates */
static void collect_fontnames(const char *fon_s, char *names[])
{
    char *s, *comma, *buf, *last, *name;
    int i, j;

    buf = strdup(fon_s);
    i = 0;
    s = skip_white(buf);
    while(*s)
   {
	names[i] = NULL;
    if((comma = strchr(s, ',')))
      *comma = 0;
    last = s + strlen(s) - 1;
	while(last > s && isspace(*last)) --last; *++last = 0;
	j = 0;
	while((name = names[j]) != NULL)
  {
    if(strcmp(name, s) == 0)
      break;
    ++j;
  }
    if(name == NULL)
  {
    names[i] = strdup(s);
	if(++i > MAX_FONTNAMES)
	  break;
  }
    if(comma == NULL)
      break;
    s = skip_white(comma + 1);
   }
    names[i] = NULL; free(buf);
}

/* font_name contains a comma separated list of "name/size" elements.
 * sets must be disjoint: if two sets begin with the same font,
 * the existing set's chain would be destroyed.
*/
ImlibFont *
imlib_font_load_fontset(const char *font_name)
{
    ImlibFont *head, *tail, *fn;
    char *name, *file;
    int i, size;
    char *names[MAX_FONTNAMES + 1];

    collect_fontnames(font_name, names);

	if(names[0] == NULL)
	  return NULL;
    head = tail = NULL; i = 0;

    while((name = names[i]))
   {
    if((file = imlib_font_find_file(name, &size)) != NULL)
  {
    if((fn = imlib_font_create_font_struct(file, size)) != NULL)
 {
    if(tail)
      tail->next_in_set = fn;
    else
      head = fn;
    tail = fn;
 }
    free(file);
  }
    free(name); ++i;
   }
    return head;
}

ImlibFont *
imlib_font_find_face_in_fontset(ImlibFont *fn, ImlibFont *fn_list,
    unsigned long uni_id, int encoding_id,
    int force_missing_glyph, FT_UInt *out_glyph_id,
    int *out_missing)
{
    FT_Face face;
    FT_UInt glyph_id;
    ImlibFont *first_invalid;

    if(!fn)
      fn = fn_list;
    if(out_missing)
      *out_missing = 0;
    if(fn->ft.face == NULL)
   {
    FT_New_Face(ft_lib, fn->file, 0, &face);
    fn->ft.face = face;
   }
    first_invalid = NULL;
repeat_upto_invalid:
    while(fn)
   {
    if(fn == first_invalid)
  {
    first_invalid = NULL;
    break;
  }
    face = fn->ft.face;

    if(face->charmap != NULL
    || FT_Select_Charmap(face, encoding_id) == 0
      )
  {
    FT_Set_Char_Size(face, 0, fn->size * 64, 96, 96);

    if(force_missing_glyph)
      glyph_id = 0;
    else
      glyph_id = FT_Get_Char_Index(face, uni_id);

    if(glyph_id || force_missing_glyph)
 {
    *out_glyph_id = glyph_id;
    return fn;
 }
    if(first_invalid == NULL)
      first_invalid = fn;
  }
    fn = fn->next_in_set;
   }/* while(fn) */
    if(first_invalid)
   {
    fn = fn_list;
    goto repeat_upto_invalid;
   }
    if(out_missing)
      *out_missing = 1;
    return
      imlib_font_find_face_in_fontset(NULL, fn_list, 0, encoding_id, 1,
        out_glyph_id, NULL);
}

void
imlib_font_free_fontset(ImlibFont *fn_list)
{
   ImlibFont *fn;

   while((fn = fn_list))
  {
   fn_list = fn_list->next_in_set;
/*   imlib_font_free(fn); */
   imlib_hash_free(fn->glyphs);
  
   if (fn->file)
      free(fn->file);
   if (fn->name)
      free(fn->name);
   FT_Done_Face(fn->ft.face);
   free(fn);
  }
}

void
imlib_font_free(ImlibFont * fn)
{
   fn->references--;
   if (fn->references == 0)
     {
        imlib_font_modify_cache_by(fn, 1);
        imlib_font_flush();
     }
}

static int
font_modify_cache_cb(Imlib_Hash * hash, const char *key, void *data,
                     void *fdata)
{
   int                *dir;
   Imlib_Font_Glyph   *fg;

   fg = data;
   dir = fdata;
   font_cache_usage += (*dir) * ((fg->glyph_out->bitmap.width * fg->glyph_out->bitmap.rows) + sizeof(Imlib_Font_Glyph) + sizeof(Imlib_Object_List) + 400);      /* fudge values */
   return 1;
   hash = 0;
   key = 0;
}

void
imlib_font_modify_cache_by(ImlibFont * fn, int dir)
{
   int                 sz_name = 0, sz_file = 0, sz_hash = 0;

   if (fn->name)
      sz_name = strlen(fn->name);
   if (fn->file)
      sz_file = strlen(fn->file);
   if (fn->glyphs)
      sz_hash = sizeof(Imlib_Hash);
   imlib_hash_foreach(fn->glyphs, font_modify_cache_cb, &dir);
   font_cache_usage += dir * (sizeof(ImlibFont) + sz_name + sz_file + sz_hash + sizeof(FT_FaceRec) + 16384);    /* fudge values */
}

int
imlib_font_cache_get(void)
{
   return font_cache;
}

void
imlib_font_cache_set(int size)
{
   font_cache = size;
   imlib_font_flush();
}

void
imlib_font_flush(void)
{
   if (font_cache_usage < font_cache)
      return;
   while (font_cache_usage > font_cache)
      imlib_font_flush_last();
}

static int
font_flush_free_glyph_cb(Imlib_Hash * hash, const char *key, void *data,
                         void *fdata)
{
   Imlib_Font_Glyph   *fg;

   fg = data;
   FT_Done_Glyph(fg->glyph);
   free(fg);
   return 1;
   hash = 0;
   key = 0;
   fdata = 0;
}

void
imlib_font_flush_last(void)
{
   Imlib_Object_List  *l;
   ImlibFont          *fn = NULL;

   for (l = fonts; l; l = l->next)
     {
        ImlibFont          *fn_tmp;

        fn_tmp = (ImlibFont *) l;
        if (fn_tmp->references == 0)
           fn = fn_tmp;
     }
   if (!fn)
      return;

   fonts = imlib_object_list_remove(fonts, fn);
   imlib_font_modify_cache_by(fn, -1);

   imlib_hash_foreach(fn->glyphs, font_flush_free_glyph_cb, NULL);
   imlib_hash_free(fn->glyphs);

   if (fn->file)
      free(fn->file);
   if (fn->name)
      free(fn->name);
   FT_Done_Face(fn->ft.face);
   free(fn);
}

ImlibFont          *
imlib_font_find(const char *name, int size)
{
   Imlib_Object_List  *l;

   for (l = fonts; l; l = l->next)
     {
        ImlibFont          *fn;

        fn = (ImlibFont *) l;
        if ((fn->size == size) && (!strcmp(name, fn->name)))
          {
             if (fn->references == 0)
                imlib_font_modify_cache_by(fn, -1);
             fn->references++;
             fonts = imlib_object_list_remove(fonts, fn);
             fonts = imlib_object_list_prepend(fonts, fn);
             return fn;
          }
     }
   return NULL;
}

/* font pathes */
void
imlib_font_add_font_path(const char *path)
{
   fpath_num++;
   if (!fpath)
      fpath = malloc(sizeof(char *));
   else
      fpath = realloc(fpath, (fpath_num * sizeof(char *)));
   fpath[fpath_num - 1] = strdup(path);
}

void
imlib_font_del_font_path(const char *path)
{
   int                 i, j;

   for (i = 0; i < fpath_num; i++)
     {
        if (!strcmp(path, fpath[i]))
          {
             if (fpath[i])
                free(fpath[i]);
             fpath_num--;
             for (j = i; j < fpath_num; j++)
                fpath[j] = fpath[j + 1];
             if (fpath_num > 0)
                fpath = realloc(fpath, fpath_num * sizeof(char *));
             else
               {
                  free(fpath);
                  fpath = NULL;
               }
          }
     }
}

int
imlib_font_path_exists(const char *path)
{
   int                 i;

   for (i = 0; i < fpath_num; i++)
     {
        if (!strcmp(path, fpath[i]))
           return 1;
     }
   return 0;
}

char              **
imlib_font_list_font_path(int *num_ret)
{
   *num_ret = fpath_num;
   return fpath;
}

/* fonts list */
char              **
imlib_font_list_fonts(int *num_ret)
{
   int                 i, j, d, l = 0;
   char              **list = NULL, **dir, *path;
   FT_Error            error;
   char               *p;

   imlib_font_init();

   for (i = 0; i < fpath_num; i++)
     {
        dir = __imlib_FileDir(fpath[i], &d);
        if (dir)
          {
             for (j = 0; j < d; j++)
               {
                  path = malloc(strlen(fpath[i]) + strlen(dir[j]) + 2);
                  sprintf(path, "%s/%s", fpath[i], dir[j]);
                  /* trim .ttf if it is there */
                  if ((p = strrchr(dir[j], '.')))
                     *p = '\0';
                  if (!__imlib_ItemInList(list, l, dir[j]))
                    {
                       if (__imlib_FileIsFile(path))
                         {
                            FT_Face             f;

                            error = FT_New_Face(ft_lib, path, 0, &f);
                            if (!error)
                              {
                                 FT_Done_Face(f);
                                 l++;
                                 if (list)
                                    list = realloc(list, sizeof(char *) * l);
                                 else
                                    list = malloc(sizeof(char *));
                                 list[l - 1] = strdup(dir[j]);
                              }
                            free(dir[j]);
                         }
                    }
                  free(path);
               }
             free(dir);
          }
     }
   *num_ret = l;
   return list;
}

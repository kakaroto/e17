typedef struct _Evoak_Font_Dir   Evoak_Font_Dir;
typedef struct _Evoak_Font       Evoak_Font;
typedef struct _Evoak_Font_Alias Evoak_Font_Alias;

struct _Evoak_Font_Dir
{
   Evas_Hash          *lookup;
   Evas_List          *fonts;
   Evas_List          *aliases;
   unsigned long long  dir_mod_time;
   unsigned long long  fonts_dir_mod_time;
   unsigned long long  fonts_alias_mod_time;
};

struct _Evoak_Font
{
   char     type;
   struct {
      char *prop[14];
   } x;
   struct {
      char *name;
   } simple;
   char    *path;
};

struct _Evoak_Font_Alias
{
   char      *alias;
   Evoak_Font *fn;
};

/* font dir cache */
static Evas_Hash *_evoak_font_dirs = NULL;

static char               *_evoak_file_path_join (char *p1, char *p2);
static Evas_List          *_evoak_file_path_list (char *path, char *match, int m
atch_case);
static unsigned long long  _evoak_file_modified_time(char *file);

static char *
_evoak_file_path_join(char *p1, char *p2)
{
   char *s;
   int l1, l2;

   l1 = strlen(p1);
   l2 = strlen(p2);
   s = malloc(l1 + 1 + l2 + 1);
   if (!s) return NULL;
   strcpy(s, p1);
   s[l1] = '/';
   strcpy(&(s[l1 + 1]), p2);
   return s;
}

static Evas_List *
_evoak_file_path_list(char *path, char *match, int match_case)
{
   Evas_List *files = NULL;
   DIR *dir;

   dir = opendir(path);
   if (!dir) return NULL;
     {
        struct dirent      *dp;
        int flags;

        flags = FNM_PATHNAME;
#ifdef FNM_CASEFOLD
        if (!match_case) flags |= FNM_CASEFOLD;
#else
# warning "Your libc does not provide case-insensitive matching!"
#endif
        while ((dp = readdir(dir)))
          {
             if ((!strcmp(dp->d_name, ".")) || (!strcmp(dp->d_name, "..")))
               continue;
             if (match)
               {
                  if (fnmatch(match, dp->d_name, flags) == 0)
                    files = evas_list_append(files, strdup(dp->d_name));
               }
             else files = evas_list_append(files, strdup(dp->d_name));
          }
        closedir(dir);
     }
   return files;
}

static unsigned long long
_evoak_file_modified_time(char *file)
{
   struct stat st;

   if (stat(file, &st) < 0) return 0;
   if (st.st_ctime > st.st_mtime) return (unsigned long long)st.st_ctime;
   else return (unsigned long long)st.st_mtime;
   return 0;
}

/* private methods for font dir cache */
static char           *_evoak_font_cache_find(char *dir, char *font);
static Evoak_Font_Dir *_evoak_font_cache_dir_update(char *dir, Evoak_Font_Dir *f
d);
static Evoak_Font     *_evoak_font_cache_font_find_x(Evoak_Font_Dir *fd, char *f
ont);
static Evoak_Font     *_evoak_font_cache_font_find_file(Evoak_Font_Dir *fd, char
 *font);
static Evoak_Font     *_evoak_font_cache_font_find_alias(Evoak_Font_Dir *fd, cha
r *font);
static Evoak_Font     *_evoak_font_cache_font_find(Evoak_Font_Dir *fd, char *fon
t);
static Evoak_Font_Dir *_evoak_font_cache_dir_add(char *dir);
static void            _evoak_font_cache_dir_del(char *dir, Evoak_Font_Dir *fd);
static int             _evoak_font_string_parse(char *buffer, char dest[14][256]
);

static Evoak_Bool _evoak_font_cache_dir_free_cb(Evas_Hash *hash, const char *key
, void *data, void *fdata);
static Evoak_Bool
_evoak_font_cache_dir_free_cb(Evas_Hash *hash, const char *key, void *data, void
 *fdata)
{
   _evoak_font_cache_dir_del((char *) key, data);
   return 1;
}

/* FIXME: this needs to be accessed on evoak_shutdown */
static void
_evoak_font_dir_cache_free(void)
{
   if (!_evoak_font_dirs) return;
   evas_hash_foreach(_evoak_font_dirs, _evoak_font_cache_dir_free_cb, NULL);
   evas_hash_free(_evoak_font_dirs);
   _evoak_font_dirs = NULL;
}

static char *
_evoak_font_cache_find(char *dir, char *font)
{
   Evoak_Font_Dir *fd;

   fd = evas_hash_find(_evoak_font_dirs, dir);
   fd = _evoak_font_cache_dir_update(dir, fd);
   if (fd)
     {
        Evoak_Font *fn;

        fn = _evoak_font_cache_font_find(fd, font);
        if (fn) return fn->path;
     }
   return NULL;
}

static Evoak_Font_Dir *
_evoak_font_cache_dir_update(char *dir, Evoak_Font_Dir *fd)
{
   unsigned long long mt;
   char *tmp;

   if (fd)
     {
        mt = _evoak_file_modified_time(dir);
        if (mt != fd->dir_mod_time)
          {
             _evoak_font_cache_dir_del(dir, fd);
             _evoak_font_dirs = evas_hash_del(_evoak_font_dirs, dir, fd);
          }
        else
          {
             tmp = _evoak_file_path_join(dir, "fonts.dir");
             if (tmp)
               {
                  mt = _evoak_file_modified_time(tmp);
                  free(tmp);
                  if (mt != fd->fonts_dir_mod_time)
                    {
                       _evoak_font_cache_dir_del(dir, fd);
                       _evoak_font_dirs = evas_hash_del(_evoak_font_dirs, dir, f
d);
                    }
                  else
                    {
                       tmp = _evoak_file_path_join(dir, "fonts.alias");
                       if (tmp)
                         {
                            mt = _evoak_file_modified_time(tmp);
                            free(tmp);
                         }
                       if (mt != fd->fonts_alias_mod_time)
                         {
                            _evoak_font_cache_dir_del(dir, fd);
                            _evoak_font_dirs = evas_hash_del(_evoak_font_dirs, d
ir, fd);
                         }
                       else
                         return fd;
                    }
               }
          }
     }
   return _evoak_font_cache_dir_add(dir);
}

static Evoak_Font *
_evoak_font_cache_font_find_x(Evoak_Font_Dir *fd, char *font)
{
   Evas_List *l;
   char font_prop[14][256];
   int num;

   num = _evoak_font_string_parse(font, font_prop);
   if (num != 14) return NULL;
   for (l = fd->fonts; l; l = l->next)
     {
        Evoak_Font *fn;

        fn = l->data;
        if (fn->type == 1)
          {
             int i;
             int match = 0;

             for (i = 0; i < 14; i++)
               {
                  if ((font_prop[i][0] == '*') && (font_prop[i][1] == 0))
                    match++;
                  else
                    {
                       if (!strcasecmp(font_prop[i], fn->x.prop[i])) match++;
                       else break;
                    }
               }
             if (match == 14) return fn;
          }
     }
   return NULL;
}

static Evoak_Font *
_evoak_font_cache_font_find_file(Evoak_Font_Dir *fd, char *font)
{
   Evas_List *l;

   for (l = fd->fonts; l; l = l->next)
     {
        Evoak_Font *fn;

        fn = l->data;
        if (fn->type == 0)
          {
             if (!strcasecmp(font, fn->simple.name)) return fn;
          }
     }
   return NULL;
}

static Evoak_Font *
_evoak_font_cache_font_find_alias(Evoak_Font_Dir *fd, char *font)
{
   Evas_List *l;

   for (l = fd->aliases; l; l = l->next)
     {
        Evoak_Font_Alias *fa;

        fa = l->data;
        if (!strcasecmp(fa->alias, font)) return fa->fn;
     }
   return NULL;
}

static Evoak_Font *
_evoak_font_cache_font_find(Evoak_Font_Dir *fd, char *font)
{
   Evoak_Font *fn;

   fn = evas_hash_find(fd->lookup, font);
   if (fn) return fn;
   fn = _evoak_font_cache_font_find_alias(fd, font);
   if (!fn) fn = _evoak_font_cache_font_find_x(fd, font);
   if (!fn) fn = _evoak_font_cache_font_find_file(fd, font);
   if (!fn) return NULL;
   fd->lookup = evas_hash_add(fd->lookup, font, fn);
   return fn;
}

static Evoak_Font_Dir *
_evoak_font_cache_dir_add(char *dir)
{
   Evoak_Font_Dir *fd;
   char *tmp;
   Evas_List *fdir;

   fd = calloc(1, sizeof(Evoak_Font_Dir));
   if (!fd) return NULL;
   _evoak_font_dirs = evas_hash_add(_evoak_font_dirs, dir, fd);

   /* READ fonts.alias, fonts.dir and directory listing */

   /* fonts.dir */
   tmp = _evoak_file_path_join(dir, "fonts.dir");
   if (tmp)
     {
        FILE *f;

        f = fopen(tmp, "r");
        if (f)
          {
             int num;
             char fname[4096], fdef[4096];

             if (fscanf(f, "%i\n", &num) != 1) goto cant_read;
             /* read font lines */
             while (fscanf(f, "%4090s %[^\n]\n", fname, fdef) == 2)
               {
                  char font_prop[14][256];
                  int i;

                  /* skip comments */
                  if ((fdef[0] == '!') || (fdef[0] == '#')) continue;
                  /* parse font def */
                  num = _evoak_font_string_parse((char *)fdef, font_prop);
                  if (num == 14)
                    {
                       Evoak_Font *fn;

                       fn = calloc(1, sizeof(Evoak_Font));
                       if (fn)
                         {
                            fn->type = 1;
                            for (i = 0; i < 14; i++)
                              {
                                 fn->x.prop[i] = strdup(font_prop[i]);
                                 /* FIXME: what if strdup fails! */
                              }
                            fn->path = _evoak_file_path_join(dir, fname);
                            /* FIXME; what is evas_file_path_join fails! */
                            fd->fonts = evas_list_append(fd->fonts, fn);
                         }
                    }
               }
             cant_read: ;
             fclose(f);
          }
        free(tmp);
     }

                       fn = calloc(1, sizeof(Evoak_Font));
                       if (fn)
                         {
                            fn->type = 1;
                            for (i = 0; i < 14; i++)
                              {
                                 fn->x.prop[i] = strdup(font_prop[i]);
                                 /* FIXME: what if strdup fails! */
                              }
                            fn->path = _evoak_file_path_join(dir, fname);
                            /* FIXME; what is evas_file_path_join fails! */
                            fd->fonts = evas_list_append(fd->fonts, fn);
                         }
                    }
               }
             cant_read: ;
             fclose(f);
          }
        free(tmp);
     }

   /* directoy listing */
   fdir = _evoak_file_path_list(dir, "*.ttf", 0);
   while (fdir)
     {
        tmp = _evoak_file_path_join(dir, fdir->data);
        if (tmp)
          {
             Evoak_Font *fn;

             fn = calloc(1, sizeof(Evoak_Font));
             if (fn)
               {
                  fn->type = 0;
                  fn->simple.name = strdup(fdir->data);
                  if (fn->simple.name)
                    {
                       char *p;

                       p = strrchr(fn->simple.name, '.');
                       if (p) *p = 0;
                    }
                  fn->path = _evoak_file_path_join(dir, fdir->data);
                  fd->fonts = evas_list_append(fd->fonts, fn);
               }
             free(tmp);
          }
        free(fdir->data);
        fdir = evas_list_remove(fdir, fdir->data);
     }

   /* fonts.alias */
   tmp = _evoak_file_path_join(dir, "fonts.alias");
      if (tmp)
     {
        FILE *f;

        f = fopen(tmp, "r");
        if (f)
          {
             char fname[4096], fdef[4096];

             /* read font alias lines */
             while (fscanf(f, "%4090s %[^\n]\n", fname, fdef) == 2)
               {
                  Evoak_Font_Alias *fa;

                  /* skip comments */
                  if ((fdef[0] == '!') || (fdef[0] == '#')) continue;
                  fa = calloc(1, sizeof(Evoak_Font_Alias));
                  if (fa)
                    {
                       fa->alias = strdup(fname);
                       fa->fn = _evoak_font_cache_font_find_x(fd, fdef);
                       if ((!fa->alias) || (!fa->fn))
                         {
                            if (fa->alias) free(fa->alias);
                            free(fa);
                         }
                       else
                         fd->aliases = evas_list_append(fd->aliases, fa);
                    }
               }
             fclose(f);
          }
        free(tmp);
     }

   fd->dir_mod_time = _evoak_file_modified_time(dir);
   tmp = _evoak_file_path_join(dir, "fonts.dir");
   if (tmp)
     {
        fd->fonts_dir_mod_time = _evoak_file_modified_time(tmp);
        free(tmp);
     }
   tmp = _evoak_file_path_join(dir, "fonts.alias");
   if (tmp)
     {
        fd->fonts_alias_mod_time = _evoak_file_modified_time(tmp);
        free(tmp);
     }

   return fd;
}

static void
_evoak_font_cache_dir_del(char *dir, Evoak_Font_Dir *fd)
{
   if (fd->lookup) evas_hash_free(fd->lookup);
   while (fd->fonts)
     {
        Evoak_Font *fn;
        int i;

        fn = fd->fonts->data;
        fd->fonts = evas_list_remove(fd->fonts, fn);
        for (i = 0; i < 14; i++)
          {
             if (fn->x.prop[i]) free(fn->x.prop[i]);
          }
        if (fn->simple.name) free(fn->simple.name);
        if (fn->path) free(fn->path);
        free(fn);
     }
   while (fd->aliases)
     {
        Evoak_Font_Alias *fa;

        fa = fd->aliases->data;
        fd->aliases = evas_list_remove(fd->aliases, fa);
        if (fa->alias) free(fa->alias);
        free(fa);
     }
   free(fd);
}

static int
_evoak_font_string_parse(char *buffer, char dest[14][256])
{
   char *p;
   int n, m, i;

   n = 0;
   m = 0;
   p = buffer;
   if (p[0] != '-') return 0;
   i = 1;
   while (p[i])
     {
        dest[n][m] = p[i];
        if ((p[i] == '-') || (m == 256))
          {
             dest[n][m] = 0;
             n++;
             m = -1;
          }
        i++;
        m++;
        if (n == 14) return n;
     }
   dest[n][m] = 0;
   n++;
   return n;
}




static void *
_evoak_object_text_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Text *m;

   m = calloc(1, sizeof(Evoak_Meta_Text));
   if (!m) return NULL;
   m->text = strdup("");
   m->name = strdup("");
   return m;
}

static void
_evoak_object_text_meta_free(void *meta)
{
   Evoak_Meta_Text *m;

   m = meta;
   if (m->name) free(m->name);
   if (m->source) free(m->source);
   if (m->ffile) free(m->ffile);
   if (m->text) free(m->text);
   if (m->font) evas_imaging_font_free(m->font);
   free(m);
}

/*
 * Create a new text object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new text object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_text_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_TEXT_MAGIC;
   o->type = "text";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_TEXT;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
        free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   o->meta = _evoak_object_text_meta_new(o);
   o->meta_free = _evoak_object_text_meta_free;
   return o;
}

void
evoak_object_text_font_source_set(Evoak_Object *o, const char *source)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Text_Source_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if ((m->source) && (source) && (!strcmp(source, m->source))) return;
        if ((!source) && (!m->source)) return;
        if (m->source)
          {
             free(m->source);
             m->source = NULL;
          }
        if (source) m->source = strdup(source);
     }
   p.source = (char *)source;
   d = _evoak_proto[EVOAK_PR_OBJECT_TEXT_SOURCE_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_TEXT_SOURCE_SET, o->id, 0, d,
s);
        free(d);
    }
}

const char *
evoak_object_text_font_source_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        return m->source;
     }
   return NULL;
}

void
evoak_object_text_font_set(Evoak_Object *o, const char *font, Evoak_Font_Size si
ze)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Text_Font_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return;
   if (o->delete_me) return;
   if (size < 0) size = 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if ((m->name) && (font) && (!strcmp(m->name, font)))
          {
             if (m->size == size) return;
          }
        if ((!font) && (!m->name)) return;
        if (m->font)
          {
             evas_imaging_font_free(m->font);
             m->font = NULL;
          }
        if (font != m->name)
          {
             if (m->name)
               {
                  free(m->name);
                  m->name = NULL;
               }
             if (font) m->name = strdup(font);
          }
        if (m->ffile)
          {
             free(m->ffile);
             m->ffile = NULL;
          }
        m->size = size;
        if (m->name)
          {
             if (m->source)
               {
                  m->font = evas_imaging_font_load(m->source, m->name, m->size);
               }
             if (!m->font)
               {
                  if (m->name[0] == '/') /* FIXME: portable */
                    m->font = evas_imaging_font_load(m->name, NULL, m->size);
                  else
                    {
                       Evas_List *l;

                       for (l = o->evoak->font_path; l; l = l->next)
                         {
                            char *f_file;

                            f_file = _evoak_font_cache_find(l->data, m->name);
                            if (f_file)
                              {
                                 m->font = evas_imaging_font_load(f_file, NULL,
m->size);
                                 m->ffile = strdup(f_file);
                                 if (m->font) break;
                              }
                         }
                    }
               }
          }
        if (m->font)
          {
             if (m->text)
               {
                  int w, h;

                  evas_imaging_font_string_size_query(m->font, m->text, &w, &h);
                  o->w = w;
                  o->h = h;
               }
             else
               {
                  int asc, desc;

                  asc = evas_imaging_font_max_ascent_get(m->font);
                  desc = evas_imaging_font_max_descent_get(m->font);
                  o->w = 0;
                  o->h = asc + desc;
               }
          }
        else
          {
             o->w = 0;
             o->h = 0;
          }
        p.size = size;
        if (m->ffile) p.font = m->ffile;
        else if (font) p.font = (char *)font;
        else p.font = "";
        d = _evoak_proto[EVOAK_PR_OBJECT_TEXT_FONT_SET].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_TEXT_FONT_SET, o->id, 0,
d, s);
             free(d);
          }
     }
}

void
evoak_object_text_font_get(Evoak_Object *o, const char **font, Evoak_Font_Size *
size)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if (font) *font = m->name;
        if (size) *size = m->size;
     }
}

void
evoak_object_text_text_set(Evoak_Object *o, const char *text)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Text_Text_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if (text == m->text) return;
        if ((text) && (m->text) && (!strcmp(m->text, text))) return;
        if ((!text) && (!m->text)) return;
        if (m->text)
          {
             free(m->text);
             m->text = NULL;
          }
        if (text) m->text = strdup(text);
        if (m->font)
          {
             if (m->text)
               {
                  int w, h;

                  evas_imaging_font_string_size_query(m->font, m->text, &w, &h);
                  o->w = w;
                  o->h = h;
               }
             else
               {
                  int asc, desc;

                  asc = evas_imaging_font_max_ascent_get(m->font);
                  desc = evas_imaging_font_max_descent_get(m->font);
                  o->w = 0;
                  o->h = asc + desc;
               }
          }
        else
          {
             o->w = 0;
             o->h = 0;
          }
        if (text) p.text = (char *)text;
        else p.text = "";
        d = _evoak_proto[EVOAK_PR_OBJECT_TEXT_TEXT_SET].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_TEXT_TEXT_SET, o->id, 0,
d, s);
             free(d);
          }
     }
}

const char *
evoak_object_text_text_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        return m->text;
     }
   return NULL;
}

Evoak_Coord
evoak_object_text_ascent_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if (m->font) return evas_imaging_font_ascent_get(m->font);
     }
   return 0;
}

Evoak_Coord
evoak_object_text_descent_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if (m->font) return evas_imaging_font_descent_get(m->font);
     }
   return 0;
}

Evoak_Coord
evoak_object_text_max_ascent_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if (m->font) return evas_imaging_font_max_ascent_get(m->font);
     }
   return 0;
}

Evoak_Coord
evoak_object_text_max_descent_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if (m->font) return evas_imaging_font_max_descent_get(m->font);
     }
   return 0;
}

Evoak_Coord
evoak_object_text_horiz_advance_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if ((m->font) && (m->text))
          {
             int x, y;

             evas_imaging_font_string_advance_get(m->font, m->text, &x, &y);
             return x;
          }
     }
   return 0;
}

Evoak_Coord
evoak_object_text_vert_advance_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if ((m->font) && (m->text))
          {
             int x, y;

             evas_imaging_font_string_advance_get(m->font, m->text, &x, &y);
             return y;
          }
     }
   return 0;
}

Evoak_Coord
evoak_object_text_inset_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if ((m->font) && (m->text))
          return evas_imaging_font_string_inset_get(m->font, m->text);
     }
   return 0;
}

int
evoak_object_text_char_pos_get(Evoak_Object *o, int pos, Evoak_Coord *cx, Evoak_
Coord *cy, Evoak_Coord *cw, Evoak_Coord *ch)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if ((m->font) && (m->text))
          {
             int v;
             int inset, max_ascent;
             Evoak_Coord ccx, ccy, ccw, cch;

             inset = evas_imaging_font_string_inset_get(m->font, m->text);
             max_ascent = evas_imaging_font_max_ascent_get(m->font);
             v = evas_imaging_font_string_char_coords_get(m->font, m->text, pos,
 &ccx, &ccy, &ccw, &cch);
             ccx -= inset;
             ccy += max_ascent;
             if (ccx < 0)
               {
                  ccw += ccx;
                  ccx = 0;
               }
             if (ccx + ccw > o->w) ccw = o->w - ccx;
             if (ccw < 0) ccw = 0;
             if (ccy < 0)
               {
                  cch += ccy;
                  ccy = 0;
               }
             if (ccy + cch > o->h) cch = o->h - ccy;
             if (cch < 0) cch = 0;
             if (cx) *cx = ccx;
             if (cy) *cy = ccy;
             if (cw) *cw = ccw;
             if (ch) *ch = cch;
             return v;
          }
     }
   return 0;
}

int
evoak_object_text_char_coords_get(Evoak_Object *o, Evoak_Coord x, Evoak_Coord y,
 Evoak_Coord *cx, Evoak_Coord *cy, Evoak_Coord *cw, Evoak_Coord *ch)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return -1;
   if (o->magic2 != EVOAK_OBJECT_TEXT_MAGIC) return -1;
   if (o->delete_me) return -1;
     {
        Evoak_Meta_Text *m;

        m = o->meta;
        if ((m->font) && (m->text))
          {
             int v;
             int inset, max_ascent;
             Evoak_Coord ccx, ccy, ccw, cch;

             inset = evas_imaging_font_string_inset_get(m->font, m->text);
             max_ascent = evas_imaging_font_max_ascent_get(m->font);
             v = evas_imaging_font_string_char_at_coords_get(m->font, m->text, x
 + inset, y - max_ascent, &ccx, &ccy, &ccw, &cch);
             ccx -= inset;
             ccy += max_ascent;
             if (ccx < 0)
               {
                  ccw += ccx;
                  ccx = 0;
               }
             if (ccx + ccw > o->w) ccw = o->w - ccx;
             if (ccw < 0) ccw = 0;
             if (ccy < 0)
               {
                  cch += ccy;
                  ccy = 0;
               }
             if (ccy + cch > o->h) cch = o->h - ccy;
             if (cch < 0) cch = 0;
             if (cx) *cx = ccx;
             if (cy) *cy = ccy;
             if (cw) *cw = ccw;
             if (ch) *ch = cch;
             return v;
          }
     }
   return -1;
}

void
evoak_font_path_clear(Evoak *ev)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   while (ev->font_path)
     {
        free(ev->font_path->data);
        ev->font_path = evas_list_remove(ev->font_path, ev->font_path->data);
     }
}

void
evoak_font_path_append(Evoak *ev, const char *path)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   ev->font_path = evas_list_append(ev->font_path, strdup(path));
}

void
evoak_font_path_prepend(Evoak *ev, const char *path)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   ev->font_path = evas_list_prepend(ev->font_path, strdup(path));
}

const Evoak_List *
evoak_font_path_list(Evoak *ev)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   return (Evoak_List *)ev->font_path;
}

int
evoak_string_char_next_get(const char *str, int pos, int *decoded)
{
   return evas_string_char_next_get(str, pos, decoded);
}

int
evoak_string_char_prev_get(const char *str, int pos, int *decoded)
{
   return evas_string_char_prev_get(str, pos, decoded);
}


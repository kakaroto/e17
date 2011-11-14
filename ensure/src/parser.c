#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <Eina.h>
#include <Elementary.h>

#include "config.h"
#include "ensure.h"
#include "enobj.h"
#include "enasn.h"
#include "parser.h"

static char         *parse_string(char **p, bool shared);
static int           parser_read_fd(struct ensure *, int fd);

static struct enobj *parse_object(struct ensure *ensure, char *line, struct enobj *eno);
static void          parse_line(struct ensure *ensure, char *line);

static int           parse_objid(struct enobj *eno, const char *prefix, char **linep);
static int           parse_name(struct enobj *eno, const char *prefix, char **linep);
static int           parse_parent(struct enobj *eno, const char *prefix, char **linep);
static int           parse_geo(struct enobj *eno, const char *prefix, char **linep);
static int           parse_color(struct enobj *eno, const char *prefix, char **linep);
static int           parse_image(struct enobj *eno, const char *prefix, char **linep);
static int           parse_image_err(struct enobj *eno, const char *prefix, char **linep);
static int           parse_text(struct enobj *eno, const char *prefix, char **linep);
static int           parse_font(struct enobj *eno, const char *prefix, char **linep);
static int           parse_source(struct enobj *eno, const char *prefix, char **linep);
static int           parse_edje(struct enobj *eno, const char *prefix, char **linep);
static int           parse_edje_err(struct enobj *eno, const char *prefix, char **linep);

static const struct parser
{
   const char *prefix;
   int         (*parser)(struct enobj *eno, const char *prefix, char **linep);
} parsers[] =
{
   {
      "Object:", parse_objid
   },
   {
      "Parent:", parse_parent
   },
   {
      "Clip:", parse_parent
   },
   {
      "Name:", parse_name
   },
   {
      "Geometry:", parse_geo
   },
   {
      "Color", parse_color
   },
   {
      "Image:", parse_image
   },
   {
      "ImageErr:", parse_image_err
   },
   {
      "Text:", parse_text
   },
   {
      "Font:", parse_font
   },
   {
      "FontSource:", parse_source
   },
   {
      "Edje:", parse_edje
   },
   {
      "EdjeErr:", parse_edje_err
   },
};

#define N_PARSERS ((int)(sizeof(parsers) / sizeof(parsers[0])))

/*
 * Child data received: Hopefully an object
 */
Eina_Bool
child_data(void *data, Ecore_Fd_Handler *hdlr)
{
   int fd;
   struct ensure *ensure = data;

   assert(hdlr);

   assert(ensure->magic == (int)ENSURE_MAGIC);

   fd = ecore_main_fd_handler_fd_get(hdlr);
   assert(fd >= 0);

   parser_read_fd(ensure, fd);

   return 1;
}

int
parser_readfile(struct ensure *ensure, const char *file)
{
   int fd;

   fd = open(file, O_RDONLY);
   if (fd < 0)
     return -1;

   while (parser_read_fd(ensure, fd))
     ;
   close(fd);
   return 0;
}

static int
parser_read_fd(struct ensure *ensure, int fd)
{
   static char buf[BUFSIZ];
   static int buffered;
   char *p, *start;
   int len;

   len = read(fd, buf + buffered, sizeof(buf) - 1 - buffered);
   if (len == 0)
     {
        /* FIXME: child exit */
        printf("No data ready??\n");
        return 0;
     }
   if (len < 0)
     {
        perror("Error reading from pipe");
        return 0;
     }
   buf[buffered + len] = 0;

   for (p = buf, start = buf; *p; p++)
     {
        if (*p != '\n')
          continue;

        *p = 0;
        parse_line(ensure, start);
        start = p + 1;
     }

   if (*start)
     {
        /* Need to buffer some */
        memmove(buf, start, p - start);
        buffered = p - start;
     }

   return 1;
}

static void
parse_line(struct ensure *ensure, char *line)
{
   static struct enwin *enwin = NULL;
   struct enobj *obj;

   assert(ensure->magic == (int)ENSURE_MAGIC);

   if (strncmp(line, "Object", 6) == 0)
     {
        obj = parse_object(ensure, line, NULL);
        obj->enwin = enwin;
     }
   else if (strncmp(line, "Ensure done", 11) == 0)
     {
        printf("Got to the end...\n");
        enasn_check(ensure);
        enobj_prepare(ensure);
        view_dirty_set(ensure);
     }
   else if (strncmp(line, "E: ", 3) == 0)
     {
        printf("Got window\n");
        enwin = calloc(1, sizeof(struct enwin));
        line += 3;
        enwin->id = strtoll(line, &line, 0);
        enwin->name = parse_string(&line, true);
        enwin->w = strtol(line, &line, 0);
        enwin->h = strtol(line, &line, 0);
        ensure->cur->windows = eina_list_append(ensure->cur->windows, enwin);
     }
   else
     {
        printf("Line was nothing...'%s'\n", line);
     }
}

static struct enobj *
parse_object(struct ensure *ensure, char *line, struct enobj *eno)
{
   char *p;
   int i;

   if (!eno)
     {
        eno = calloc(1, sizeof(struct enobj));
        eno->ensure = ensure;

        assert(eno);
     }

   p = line;
   while (*p)
     {
        for (i = 0; i < N_PARSERS; i++)
          {
             if (strncmp(parsers[i].prefix, p, strlen(parsers[i].prefix)))
               continue;

             p += strlen(parsers[i].prefix);
             while (isspace(*p))
               p++;

             parsers[i].parser(eno, parsers[i].prefix, &p);
             line = p;
             break;
          }
        if (i == N_PARSERS)
          {
             printf("Don't handle %10.10s\n", p);
             p++;
          }
        while (*p && isspace(*p))
          p++;
     }

   assert(eno->id);
   enobj_add(ensure, eno);

   return eno;
}

static int
parse_objid(struct enobj *eno, const char *prefix, char **linep)
{
   char *p, *start;
   assert(eno);
   assert(prefix);
   assert(linep);
   assert(eno->id == 0);

   eno->id = strtoll(*linep, &p, 0);
   p++;
   if (*p == '\'')
     {
        p++;
        start = p;
        while (*p != '\'')
          p++;
        eno->type = eina_stringshare_add_length(start, p - start);
        p++;
     }

   /* FIXME: Check it parsed okay: Need safe strtol */

   *linep = p;
   return 0;
}

static int
parse_name(struct enobj *eno, const char *prefix __UNUSED__, char **linep)
{
   char *p, *start;

   p = *linep;

   while (*p != '\'')
     p++;
   p++;
   start = p;
   while (*p != '\'')
     p++;
   eno->name = strndup(start, p - start);
   p++;

   *linep = p;

   return 0;
}

static int
parse_parent(struct enobj *eno, const char *prefix, char **linep)
{
   uintptr_t id;
   char *p;
   assert(eno);
   assert(prefix);
   assert(linep);

   /* FIXME: safe strtol */
   id = strtoll(*linep, &p, 0);

   if (strncmp(prefix, "Parent", 4) == 0)
     {
        eno->parent = id;
     }
   else
     {
        eno->clip = id;
     }
   *linep = p;
   return 0;
}

static int
parse_geo(struct enobj *eno, const char *prefix, char **linep)
{
   int len, ct;
   assert(eno);
   assert(prefix);
   assert(linep);

   ct = sscanf(*linep, "%d%d(%dx%d)%n", &eno->x, &eno->y, &eno->w, &eno->h, &len);
   if (ct != 4)
     {
        printf("Failed to parse geo '%.30s'\n", *linep);
        return -1;
     }
   *linep += len;
   return 0;
}

static int
parse_color(struct enobj *eno, const char *prefix, char **linep)
{
   int len, ct;
   assert(eno);
   assert(prefix);
   assert(linep);

   ct = sscanf(*linep, "(%hhd,%hhd,%hhd,%hhd)%n", &eno->r, &eno->g, &eno->b, &eno->a, &len);

   *linep += len;

   return 0;
}

static int
parse_image(struct enobj *eno, const char *prefix __UNUSED__, char **linep)
{
   char *p;
   eno->data.image.file = parse_string(linep, true);
   p = *linep;
   while (isspace(*p))
     p++;
   if (*p == '\'')
     eno->data.image.key = parse_string(linep, true);

   return 0;
}

static int
parse_image_err(struct enobj *eno, const char *prefix __UNUSED__, char **linep)
{
   eno->data.image.err = parse_string(linep, true);
   return 0;
}

static int
parse_text(struct enobj *eno, const char *prefix __UNUSED__, char **linep)
{
   char *p, *start;

   p = *linep;
   while (*p != '[')
     p++;
   p++;
   p++;
   start = p;
   while (*p != ']')
     p++;
   eno->data.text.text = eina_stringshare_add_length(start, p - start);
   p++;
   p++;

   *linep = p;
   return 0;
}

static int
parse_font(struct enobj *eno, const char *prefix __UNUSED__, char **linep)
{
   eno->data.text.font = parse_string(linep, true);
   eno->data.text.size = strtol(*linep, linep, 10);
   return 0;
}

static int
parse_source(struct enobj *eno, const char *prefix __UNUSED__, char **linep)
{
   eno->data.text.source = parse_string(linep, true);
   return 0;
}

static int
parse_edje(struct enobj *eno, const char *prefix __UNUSED__, char **linep)
{
   char *p;
   eno->data.edje.file = parse_string(linep, true);
   p = *linep;
   while (isspace(*p))
     p++;
   eno->data.edje.group = parse_string(linep, true);

   return 0;
}

static int
parse_edje_err(struct enobj *eno, const char *prefix __UNUSED__, char **linep)
{
   eno->data.edje.err = parse_string(linep, true);
   return 0;
}

static char *
parse_string(char **linep, bool shared)
{
   char *p, *start;

   p = *linep;
   while (*p != '\'')
     p++;
   p++;
   start = p;
   while (*p != '\'')
     p++;
   *linep = p + 1;

   if (shared)
     return (char *)eina_stringshare_add_length(start, p - start);
   else
     return strndup(start, p - start);
}

int
parser_save_window(struct enwin *w, FILE *outfile)
{
   fprintf(outfile, "E: %p '%s' %d %d\n", (void *)w->id, w->name ? : "", w->w, w->h);
   return 0;
}

int
parser_save_object(struct enobj *o, FILE *outfile)
{
   fprintf(outfile, "Object: %p '%s' ", (void *)o->id, o->type);
   if (o->name)
     fprintf(outfile, "Name: '%s' ", o->name);
   if (o->parent)
     fprintf(outfile, "Parent: %p ", (void *)o->parent);
   fprintf(outfile, "Geometry: %+d%+d(%dx%d) ", o->x, o->y, o->w, o->h);
   if (o->clip)
     fprintf(outfile, "Clip: %p ", (void *)o->clip);

   fprintf(outfile, "Color (%d,%d,%d,%d) ", o->r, o->g, o->b, o->a);

   /* Type specific things here */
   if (strcmp(o->type, "image") == 0)
     {
        if (o->data.image.key)
          fprintf(outfile, "Image: '%s' '%s' ", o->data.image.file, o->data.image.key);
        else
          fprintf(outfile, "Image: '%s' ", o->data.image.file);
        if (o->data.image.err)
          fprintf(outfile, "ImageErr: '%s' ", o->data.image.err);
     }
   else if (strcmp(o->type, "text") == 0)
     {
        fprintf(outfile, "Text: [[%s]] ", o->data.text.text);
        fprintf(outfile, "Font: '%s' %d ", o->data.text.font, o->data.text.size);
        if (o->data.text.source)
          fprintf(outfile, "FontSource: '%s' %d ", o->data.text.source, o->data.text.size);
     }
   else if (strcmp(o->type, "rectangle") == 0)
     {
     }
   else if (strcmp(o->type, "edje") == 0)
     {
        fprintf(outfile, "Edje: '%s' '%s' ", o->data.edje.file, o->data.edje.group);
        if (o->data.edje.err)
          fprintf(outfile, "EdjeErr: '%s' ", o->data.edje.err);
     }

   fprintf(outfile, "\n");

   return 0;
}


#include <Eina.h>

#include "Vigrid.h"

#define EXPECTED_STR(Line, Str)                         \
  if ((Line->length < strlen(Str) + 1) ||               \
      strncmp(Str, Line->line.start, strlen(Str)))      \
      goto on_error;

#define PUSH_STRINGSHARE(Line, Space)           \
  eina_stringshare_add_length(Line->line.start + Space, Line->length - Space);

#define VIGRID_NUMBER_SEARCH "0123456789"

static long long int
_vigrid_int_get(Eina_File_Lines *line, unsigned long long *offset)
{
   long long int n = 0;
   const char *s;

   for (s = line->line.start + *offset; s < line->line.end; s++)
     {
        const char *convert = VIGRID_NUMBER_SEARCH;
        const char *r;

        r = strchr(convert, *s);
        if (!r)
          {
             *offset = s - line->line.start;
             return n;
          }
        n = n * 10 + (r - convert);
     }

   *offset = line->length;
   return n;
}

static double
_vigrid_double_get(Eina_File_Lines *line, unsigned long long *offset)
{
   long long int over = 0;
   long long int below = 0;
   int count = 0;
   double r;

   over = _vigrid_int_get(line, offset);
   if (*offset < line->length && line->line.start[*offset] == '.')
     {
        unsigned long long saved = *offset;
        
        below = _vigrid_int_get(line, offset);
        count = *offset - saved;
     }

   r = over;
   if (count) r += below / (10 * count);

   return r;
}

static long long int
_vigrid_hex_get(Eina_File_Lines *line, unsigned long long *offset)
{
   long long int n = 0;
   const char *s;

   for (s = line->line.start + *offset; s < line->line.end; s++)
     {
        const char *convert = "0123456789ABCDEF";
        const char *r;

        r = strchr(convert, *s);
        if (!r)
          {
             *offset = s - line->line.start;
             return n;
          }
        n = (n << 4) + (r - convert);;
     }

   *offset = line->length;
   return n;
}

static Eina_Bool
_vigrid_int_search(Eina_File_Lines *line, unsigned long long *offset)
{
   while (strchr(VIGRID_NUMBER_SEARCH, line->line.start[*offset]) == NULL &&
          *offset < line->length)
     (*offset)++;

   if (*offset == line->length) return EINA_FALSE;
   return EINA_TRUE;
}

static Eina_Bool
_vigrid_massif_parse_top(Eina_File_Lines *line, Vigrid_Massif_Line *l, Vigrid_Massif_Snapshot *vs)
{
   unsigned long long offset = 1;
   /* look like 'n16: 1686831 (heap allocation functions) malloc/new/new[], --alloc-fns, etc.' */
   /* extract:    ^      ^                                                                     */

   eina_array_push(&vs->callstack, l);

   l->type = VIGRID_MASSIF_TOP;
   l->children = _vigrid_int_get(line, &offset);
   if (offset == line->length ||
       line->line.start[offset] != ':')
     return EINA_FALSE;

   if (!_vigrid_int_search(line, &offset))
     return EINA_FALSE;

   l->mem_usage = _vigrid_int_get(line, &offset);
   if (offset == line->length)
     return EINA_FALSE;

   eina_array_step_set(&l->u.detail.children, sizeof (Eina_Array), 4);
   return EINA_TRUE;
}

static Eina_Bool
_vigrid_massif_parse_children(Eina_File_Lines *line, Vigrid_Massif_Line *l, Vigrid_Massif_Line *parent,
                              unsigned long long start)
{
   unsigned long long offset = start + 1;
   /* choose between :
    * - 'n0: 407937 in 404 places, all below massif's threshold (01.00%)'
    * ex: ^    ^        ^                                           ^
    * - 'n1: 21840 0x804FDE1: elm_main (main.c:308)'
    * ex: ^    ^        ^        ^         ^    ^
    * - 'n1: 65536 0x62143B7: ???'
    * ex: ^    ^        ^
    * - 'n2: 33890 0x4ADAE19: ??? (in /usr/lib/i386-linux-gnu/libfreetype.so.6.8.0)'
    * ex: ^    ^        ^      ^             ^
    */

   eina_array_push(&parent->u.detail.children, l);

   l->children = _vigrid_int_get(line, &offset);
   if (offset == line->length ||
       line->line.start[offset] != ':')
     return EINA_FALSE;

   if (!_vigrid_int_search(line, &offset))
     return EINA_FALSE;

   l->mem_usage = _vigrid_int_get(line, &offset);
   if (offset == line->length) return EINA_FALSE;

   if (!strncmp(" 0x", line->line.start + offset, 3))
     {
        const char *r;

        offset += 3;
        l->type = VIGRID_MASSIF_DETAIL;
        eina_array_step_set(&l->u.detail.children, sizeof (Eina_Array), 4);

        /* Lookup for address */
        l->u.detail.address = _vigrid_hex_get(line, &offset);
        if (offset + 2 >= line->length) return EINA_FALSE;
        offset += 2;

        /* Special case when no symbol information */
        if (offset + 3 == line->length &&
            !strncmp("???", line->line.start + offset, 3))
          {
             l->u.detail.function = eina_stringshare_add("???");
             l->u.detail.file = eina_stringshare_add("???");
             l->u.detail.line = -1;
             goto end;
          }

        /* Lookup for function name */
        r = memchr(line->line.start + offset, ' ', line->length - offset);
        if (!r) return EINA_FALSE;
        l->u.detail.function = eina_stringshare_add_length(line->line.start + offset,
                                                           r - (line->line.start + offset));
        offset += r - (line->line.start + offset);
        if (offset + 2 >= line->length) return EINA_FALSE;
        offset += 2;

        /* Special case for library */
        if (offset + 3 < line->length &&
            !strncmp("in ", line->line.start + offset, 3))
          {
             offset += 3;

             l->u.detail.file = eina_stringshare_add_length(line->line.start + offset,
                                                            line->length - offset - 1);
             l->u.detail.line = -1;
             goto end;
          }

        /* Lookup for file name */
        r = memchr(line->line.start + offset, ':', line->length - offset);
        if (!r) return EINA_FALSE;
        l->u.detail.file = eina_stringshare_add_length(line->line.start + offset,
                                                       r - (line->line.start + offset));
        offset += r - (line->line.start + offset);
        if (offset + 1 >= line->length) return EINA_FALSE;
        offset += 1;

        /* Lookup for line name */
        l->u.detail.line = _vigrid_int_get(line, &offset);
        if (offset == line->length) return EINA_FALSE;
     }
   else if (!strncmp(" in", line->line.start + offset, 3))
     {
        offset += 3;
        l->type = VIGRID_MASSIF_BELOW;

        /* Lookup for places */
        if (!_vigrid_int_search(line, &offset)) return EINA_FALSE;
        l->u.below.places = _vigrid_int_get(line, &offset);
        if (offset == line->length) return EINA_FALSE;

        /* Lookup for the threshold */
        if (!_vigrid_int_search(line, &offset)) return EINA_FALSE;
        l->u.below.threshold = _vigrid_double_get(line, &offset);
        if (offset == line->length) return EINA_FALSE;
     }
   else return EINA_FALSE;

 end:
   return EINA_TRUE;
}

static void
_vigrid_massif_line_free(Vigrid_Massif_Line *l)
{
   if (l->type == VIGRID_MASSIF_DETAIL)
     {
        Vigrid_Massif_Line *child;

        eina_stringshare_del(l->u.detail.function);
        eina_stringshare_del(l->u.detail.file);

        while ((child = eina_array_pop(&l->u.detail.children)) != NULL)
          _vigrid_massif_line_free(child);
        eina_array_flush(&l->u.detail.children);
     }

   free(l);
} 

EAPI void
vigrid_massif_free(const Vigrid_Massif *massif)
{
   Vigrid_Massif_Snapshot *vs;
   Vigrid_Massif_Line *vl;
   Vigrid_Massif *ms = (Vigrid_Massif*) massif;

   if (!massif) return ;

   while ((vs = eina_array_pop(&ms->snapshots)) != NULL)
     {
        while ((vl = eina_array_pop(&vs->callstack)) != NULL)
          _vigrid_massif_line_free(vl);
        eina_array_flush(&vs->callstack);
        free(vs);
     }

   eina_stringshare_del(ms->command);
   eina_stringshare_del(ms->description);
   eina_array_flush(&ms->snapshots);
   free(ms);
}

EAPI const Vigrid_Massif *
vigrid_massif_alloc(const char *file)
{
   Eina_File_Lines *line;
   Vigrid_Massif *ms = NULL;
   Vigrid_Massif_Snapshot *vs;
   Eina_Iterator *it = NULL;
   Eina_File *f;
   Eina_Array vl;
   Eina_Bool time_unit = EINA_FALSE;

   eina_array_step_set(&vl, sizeof (Eina_Array), 4);

   f = eina_file_open(file, EINA_FALSE);
   if (!f) return NULL;

   ms = calloc(1, sizeof (Vigrid_Massif));
   if (!ms) goto on_error;

   eina_array_step_set(&ms->snapshots, sizeof (Eina_Array), 4);

   it = eina_file_map_lines(f);
   EINA_ITERATOR_FOREACH(it, line)
     {
        if (line->length == 0) continue;

        switch (line->line.start[0])
          {
           case '#':
              /* Basically comment so don't care */
              break;
           case 'd':
              EXPECTED_STR(line, "desc: ");

              ms->description = PUSH_STRINGSHARE(line, 6);
              break;
           case 'c':
              EXPECTED_STR(line, "cmd: ");

              ms->command = PUSH_STRINGSHARE(line, 5);
              break;
           case 's':
             {
#define VIGRID_SNAPSHOT_S "snapshot="
                unsigned long long offset = strlen(VIGRID_SNAPSHOT_S);
                long long int n;

                EXPECTED_STR(line, VIGRID_SNAPSHOT_S);

                n = _vigrid_int_get(line, &offset);
                if (offset != line->length) goto on_error;

                vs = calloc(1, sizeof (Vigrid_Massif_Snapshot));
                if (!vs) goto on_error;

                vs->index = n;
                eina_array_step_set(&vs->callstack, sizeof (Eina_Array), 16);
                eina_array_clean(&vs->callstack);

                eina_array_push(&ms->snapshots, vs);
                break;
             }
           case 't':
              if (!time_unit)
                {
                   EXPECTED_STR(line, "time_unit: ");

                   switch (line->line.start[11])
                     {
                      case 'i': ms->time = VIGRID_INSTRUCTIONS; break;
                      default:
                         fprintf(stderr, "Unknown time unit '%c'\n", line->line.start[12]);
                         break;
                     }
                   ms->description = PUSH_STRINGSHARE(line, 11);
                   time_unit = EINA_TRUE;
                }
              else
                {
#define VIGRID_TIME_S "time="
                   unsigned long long offset = strlen(VIGRID_TIME_S);
                   long long int t;

                   if (!vs) goto on_error;
                   EXPECTED_STR(line, VIGRID_TIME_S);

                   t = _vigrid_int_get(line, &offset);
                   if (offset != line->length) goto on_error;

                   vs->time = t;
                }
              break;
           case 'm':
             {
                long long int m;
                unsigned long long length = 0;
                unsigned long long offset;

                if (!vs) goto on_error;

#define LINE_MATCH(Line, Str, Out)                                      \
                if (Line->length > strlen(Str) && !strncmp(Str, Line->line.start, strlen(Str))) \
                  Out = strlen(Str);

                LINE_MATCH(line, "mem_heap_B=", length)
                else LINE_MATCH(line, "mem_heap_extra_B=", length)
                else LINE_MATCH(line, "mem_stacks_B=", length);

#undef LINE_MATCH

                if (length == 0) goto on_error;

                offset = length;
                m = _vigrid_int_get(line, &offset);
                if (offset != line->length) goto on_error;

                switch (length)
                  {
                     /* "mem_heap_B=" */
                   case 11: vs->mem_heap = m; break;
                      /* "mem_heap_extra_B=" */
                   case 17: vs->mem_heap_extra = m; break;
                      /* "mem_stacks_B=" */
                   case 13: vs->mem_stack = m; break;
                  }

                break;
             }
           case 'h':
             {
                if (!vs) goto on_error;
                EXPECTED_STR(line, "heap_tree=");

#define LINE_MATCH(Line, Str)                                           \
                (Line->length == strlen(Str) && !strncmp(Str, Line->line.start, strlen(Str))) \

                if (LINE_MATCH(line, "heap_tree=detailed"))
                  vs->heap_tree = EINA_TRUE;
                else if (LINE_MATCH(line, "heap_tree=empty"))
                  vs->heap_tree = EINA_FALSE;

#undef LINE_MATCH
                break;
             }
           case 'n':
           case ' ':
             {
                Vigrid_Massif_Line *l;
                const char *s;
                long long int i = 0;
                Eina_Bool r = EINA_FALSE;

                if (!vs) goto on_error;

                for (s = line->line.start; s < line->line.end && *s == ' '; ++s)
                  i++;

                if (*s != 'n') goto on_error;

                while (eina_array_count(&vl) > i)
                  eina_array_pop(&vl);

                l = calloc(1, sizeof (Vigrid_Massif_Line));
                if (!l) goto on_error;

                eina_array_push(&vl, l);

                if (i == 0) r = _vigrid_massif_parse_top(line, l, vs);
                else r = _vigrid_massif_parse_children(line, l,
                                                       eina_array_data_get(&vl, eina_array_count(&vl) - 2),
                                                       i);
                if (!r) goto on_error;
                if (l->type == VIGRID_MASSIF_BELOW)
                  {
                     if (ms->threshold == 0)
                       {
                          ms->threshold = l->u.below.threshold;
                       }
                     else
                       {
                          if (ms->threshold != l->u.below.threshold)
                            goto on_error;
                       }
                  }

                break;
             }
          }
     }
   eina_iterator_free(it);
   eina_file_close(f);

   eina_array_flush(&vl);

   return ms;

 on_error:
   if (ms) vigrid_massif_free(ms);

   if (it) eina_iterator_free(it);
   eina_file_close(f);

   eina_array_flush(&vl);

   return NULL;
}

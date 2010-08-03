/*
 * JS Pages Template fast C parser.
 * ported from the lua version written by ViCToRZ
 *
 * embed javascript in any kind of document
 *
 * opening tags are:
 *  `<?js' or `<%' for javascript code
 *  `<?js=' or `<%=' for expression (printed code)
 *
 * closing tags are:
 *  `?>' or `%>' for both code and expression
 *
 * template exemple:
 * ===
 * <title><?js= var_title ?></title>
 * <% if (body) { %>
 *   <body>head, arm, eyes, ...</body>
 * <% } %>
 * ===
 *
 * javascript code exemple:
 * ===
 *  elx.load('jsptp');
 *  // open the template as a script (template can be put in cache)
 *  template = jsptp.parse_file('tmpl/index.tmpl');
 *  // set variables
 *  var_title = "my_title";
 *  body = true;
 *  // exec the template
 *  template.exec();
 * ===
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "Elixir.h"

#define BUFFER_SIZE     4096
#define ERR_SIZE	64

enum elixir_template_state {
  JSPT_TEXT,
  JSPT_CODE,
  JSPT_EXP,
};

enum elixir_template_next_state {
  JSPT_NONE,
  JSPT_BC1,
  JSPT_BC2,
  JSPT_BC3,
  JSPT_BC4,
  JSPT_EC1,
  JSPT_EC2,
};

typedef struct elixir_template_parser_s elixir_template_parser_t;
struct elixir_template_parser_s
{
  const unsigned char   *in;

  const char            *write_func;

  int                    state;
  int                    next_state;

  int                    inp;
  int                    ins;

  char                  *out;
  int                    outs;
  int                    lines;

  unsigned char          tmp[6];
  int                    tmps;

  char                   err_msg[ERR_SIZE];
};


static int
end_text(elixir_template_parser_t *p, int new_state)
{
   int size;

   p->out[p->outs++] = '\'';
   p->out[p->outs++] = ')';
   p->out[p->outs++] = ';';
   while (p->lines > 0) {
      p->out[p->outs++] = '\n';
      p->lines--;
   }
   p->out[p->outs] = 0;

   size = p->outs;

   p->outs = 0;
   p->state = new_state;
   p->next_state = JSPT_NONE;
   p->tmps = 0;

   return size;
}

static int
end_code(elixir_template_parser_t *p)
{
   int size;

   if (p->state == JSPT_EXP) {
      p->out[p->outs++] = ')';
      p->out[p->outs++] = ';';
   }
   p->out[p->outs] = 0;

   size = p->outs;

   p->state = JSPT_TEXT;
   p->next_state = JSPT_NONE;
   p->tmps = 0;
   p->outs = 0;

   return size;
}

static int
end_buffer(elixir_template_parser_t *p)
{
   int size;

   p->out[p->outs] = 0;

   size = p->outs;

   p->tmps = 0;
   p->outs = 0;

   return size;
}

static void
outc(elixir_template_parser_t *p, unsigned char c)
{
   int i;

   if (p->next_state != JSPT_NONE) {
      for (i = 0; i < p->tmps; i++)
        p->out[p->outs++] = p->tmp[i];
      p->tmps = 0;
      p->next_state = JSPT_NONE;
   }
   if (p->state == JSPT_TEXT) {
      if (c == '\n') {
         p->lines++;
         p->out[p->outs++] = '\\';
         p->out[p->outs++] = 'n';
      } else {
         if (c == '\\' || c == '\'')
           p->out[p->outs++] = '\\';
         p->out[p->outs++] = c;
      }
   } else
     p->out[p->outs++] = c;
}

static int
_elixir_template_parse(elixir_template_parser_t *p, char *buffer, int len)
{
   unsigned char c;

   p->out = buffer;

   if (p->state == JSPT_TEXT) {
      p->outs = sprintf(p->out, "%s('", p->write_func);
   }

   if (p->state == JSPT_EXP) {
      p->outs = sprintf(p->out, "%s(", p->write_func);
   }

   while (p->ins > 0) {
      if (p->inp == p->ins) {
         if (p->state != JSPT_TEXT) {
            sprintf(p->err_msg, "Ending '?>' missing\n");
            return -1;
         }
         return end_text(p, JSPT_TEXT);
      }

      c = p->in[p->inp];
      p->inp++;

      switch (c) {
       case '\r':
          continue;

       case '<':
          if (p->state == JSPT_TEXT) {
             p->next_state = JSPT_BC1;
             p->tmp[p->tmps++] = '<';
          } else
            outc(p, c);
          break;

       case '%':
       case '?':
          if (p->next_state == JSPT_BC1) {
             p->next_state = c == '%' ? JSPT_BC4 : JSPT_BC2;
             p->tmp[p->tmps++] = c;
          } else if (p->next_state == JSPT_EC1) {
             p->next_state = JSPT_EC2;
             p->tmp[p->tmps++] = c;
          } else
            outc(p, c);
          break;

       case 'j':
          if (p->next_state == JSPT_BC2) {
             p->next_state = JSPT_BC3;
             p->tmp[p->tmps++] = 'j';
          } else
            outc(p, c);
          break;

       case 's':
          if (p->next_state == JSPT_BC3) {
             p->next_state = JSPT_BC4;
             p->tmp[p->tmps++] = 's';
          } else
            outc(p, c);
          break;

       case '=':
          if (p->next_state == JSPT_BC4) {
             return end_text(p, JSPT_EXP);
          }
          outc(p, c);
          break;

       case '>':
          if (p->next_state == JSPT_EC2)
            return end_code(p);
          outc(p, c);
          break;

       case '\n':
       case '\t':
       case ' ':
          if (p->next_state == JSPT_BC4) {
             int size = end_text(p, JSPT_CODE);
             p->tmp[p->tmps++] = c;
             return size;
          }
          outc(p, c);
          if (p->state == JSPT_CODE || p->state == JSPT_EXP)
            p->next_state = JSPT_EC1;
          break;

       default:
          outc(p, c);
          break;
      }

      if (p->outs >= len - 1)
        return end_buffer(p);
   }
   return 0;
}

static elixir_template_parser_t *
_elixir_template_parser_init(const char *outfunc, const unsigned char *in, unsigned int length)
{
   elixir_template_parser_t     *p;

   p = malloc(sizeof (elixir_template_parser_t));
   if (!p)
     return NULL;

   p->write_func = outfunc;
   p->err_msg[0] = 0;
   p->state = JSPT_TEXT;
   p->next_state = JSPT_NONE;
   p->tmps = 0;
   p->ins = length;
   p->in = in;
   p->outs = 0;
   p->lines = 0;

   return p;
}

static void
_elixir_template_parser_destroy(elixir_template_parser_t *p)
{
   free(p);
}

char *
elixir_template(const unsigned char *file_data, unsigned int file_length, unsigned int *template_length)
{
   elixir_template_parser_t     *p;
   char                         *buffer = NULL;
   unsigned int                  used = 0;
   unsigned int                  size = 0;
   int                           len;

   p = _elixir_template_parser_init("elx.print", file_data, file_length);
   if (!p)
     return NULL;

   do
     {
	if (used > size - BUFFER_SIZE / 2)
	  {
	     char *tmp;
	     size += BUFFER_SIZE;
	     tmp = realloc(buffer, size);
	     if (!tmp) goto exit_clean;
	     buffer = tmp;
	  }

	len = _elixir_template_parse(p, buffer + used, size - used);
	used += len;
     }
   while (len > 0);

   if (len < 0) goto exit_clean;

   _elixir_template_parser_destroy(p);
   *template_length = used;

   return buffer;
exit_clean:
   _elixir_template_parser_destroy(p);
   free(buffer);
   *template_length = 0;
   return NULL;
}


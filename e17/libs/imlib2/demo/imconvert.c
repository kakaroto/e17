/* Convert images between formats, using Imlib2's API. Smart enough to know
 * about edb files; defaults to jpg's.
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <Imlib2.h>

int
main(int argc,
     char **argv)
{
  char *dot, *colon, *n, *oldn;
  Imlib_Image im;
  int usage;

  usage = 0;
  /* I'm just plain being lazy here.. get our basename. */
  for (oldn = n = argv[0]; n; oldn = n) n = strchr(++oldn, '/');
  if (argc < 3 || !strcmp(argv[1], "-h")) usage = 1;
  else im = imlib_load_image(argv[1]);
  if (!usage && im)
    {
      /* we only care what format the export format is. */
      imlib_context_set_image(im);
      /* hopefully the last one will be the one we want.. */
      dot = strrchr(argv[2], '.');
      /* if there's a format, snarf it and set the format. */
      if (dot && *(dot + 1))
        {
          colon = strrchr(++dot, ':');
          /* if a db file with a key, export it to a db. */
          if (colon && *(colon + 1))
            {
              *colon = 0;
              /* beats having to look for strcasecmp() */
              if (!strncmp(dot, "db", 2) || !strncmp(dot, "dB", 2) ||
                  !strncmp(dot, "DB", 2) || !strncmp(dot, "Db", 2))
                {
                   imlib_image_set_format("db");
                }
              *colon = ':';
            }
          else
            {
              char *p, *q;

              /* max length of 8 for format name. seems reasonable. */
              q = p = malloc(8);
              memset(p, 0, 8);
              strncpy(p, dot, (strlen(dot) < 9) ? strlen(dot) : 8);
              /* Imlib2 only recognizes lowercase formats. convert it. */
              for (q[8] = 0; *q; q++) *q = tolower(*q);
              imlib_image_set_format(p);
              free(p);
            }
          dot--;
        }
      else imlib_image_set_format("jpg");
      imlib_save_image(argv[2]);
    }
  else
    {
      /* usage stuff. */
      fprintf(stderr, "%s: Convert images between formats (part of the " \
                      "Imlib2 package)\n\n" \
                      "Usage: %s [ -h | <image1> <image2[.fmt]> ]\n" \
                      "  <fmt> defaults to jpg if not provided; images in " \
                      "edb files are supported via\n" \
                      "        the file.db:/key/name convention.\n" \
                      "  -h shows this help.\n\n", argv[0], argv[0]);
      exit(-1);
    }
  return 0;
}

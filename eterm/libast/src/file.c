/*
 * Copyright (C) 1997-2002, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

static const char cvs_ident[] = "$Id$";

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libast_internal.h>

int
libast_temp_file(char *template, size_t len)
{
  char buff[256];
  int fd;

  if (getenv("TMPDIR")) {
    snprintf(buff, sizeof(buff), "%s/%sXXXXXX", getenv("TMPDIR"), template);
  } else if (getenv("TMP")) {
    snprintf(buff, sizeof(buff), "%s/%sXXXXXX", getenv("TMP"), template);
  } else {
    snprintf(buff, sizeof(buff), "/tmp/%sXXXXXX", template);
  }
  fd = mkstemp(buff);
  if ((fd < 0) || fchmod(fd, (S_IRUSR | S_IWUSR))) {
    return (-1);
  }

  if (len) {
    strncpy(template, buff, len);
    template[len - 1] = 0;
  }
  return (fd);
}

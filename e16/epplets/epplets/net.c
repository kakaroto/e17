/*
 * Copyright (C) 1999, Michael Jennings
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

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#ifdef __sun__
# include <unistd.h>
# include <kstat.h>
# include <sys/sysinfo.h>
#endif
#include "epplet.h"
#include "net.h"

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif

char **
net_get_devices(void) {

#ifdef linux
  return ((char **) NULL);
#elif defined(__sun__)
  return ((char **) NULL);
#else
  return ((char **) NULL);
#endif

}

unsigned char
net_get_bytes_inout(const char *device, double *in_bytes, double *out_bytes) {

#ifdef __sun__
  kstat_ctl_t *kc;
  kstat_t *ksp;
  kstat_named_t kned[100];
#elif defined(linux)
  unsigned char match = 0;
  static FILE *fp;
  char buff[256], *colon = NULL, dev[64], in_str[64], out_str[64];
#endif

  if (device == NULL) {
    return (EFAULT);
  }
#ifdef __sun__
  kc = kstat_open();
  if (kc == NULL) {
    return (EACCES);
  }
  ksp = kstat_lookup(kc, 0, -1, device);
  if (ksp == NULL) {
    return (ENODEV);
  }
  kstat_read(kc, ksp, &kned);
  if (in_bytes != NULL) {
    *in_bytes = (double) kned[0].value.ul;
  }
  if (out_bytes != NULL) {
    *out_bytes = (double) kned[2].value.ul;
  }
  kstat_close(kc);
  return 0;
#elif defined(linux)
  fp = fopen("/proc/net/dev", "r");
  if (fp == NULL) {
    return (ENOENT);
  }
  fgets(buff, sizeof(buff), fp);
  fgets(buff, sizeof(buff), fp);

  for (; fgets(buff, sizeof(buff), fp); ) {
    colon = strchr(buff, ':');
    if (colon) {
      *colon = ' ';
    }
    sscanf(buff, "%s %s %*s %*s %*s %*s %*s %*s %*s %s", dev, in_str, out_str);
    if (!strcmp(dev, device)) {
      match = 1;
      if (in_bytes != NULL) {
        *in_bytes = atof(in_str);
      }
      if (out_bytes != NULL) {
        *out_bytes = atof(out_str);
      }
      break;
    }
  }
  fclose(fp);
  return ((match) ? (0) : (ENODEV));
#else
  /* Unsupported platform. */
  if (in_bytes != NULL) {
    *in_bytes = -1.0;
  }
  if (out_bytes != NULL) {
    *out_bytes = -1.0;
  }
  return (EPERM);
#endif
}

const char *
net_strerror(unsigned char code) {

  switch (code) {
    case 0:       return ("Success (no error)"); break;
    case EFAULT:  return ("Invalid pointer passed"); break;
    case EACCES:  return ("Access to kernel stats denied"); break;
    case ENODEV:  return ("No such device"); break;
    case ENOENT:  return ("Read of stats file failed"); break;
    case EPERM:   return ("Unsupported platform"); break;
    default:      return ("Unknown error"); break;
  }
}

double
net_get_bytes_in(const char *device) {

  double in_bytes = -1.0;

  net_get_bytes_inout(device, &in_bytes, (double *) NULL);
  return (in_bytes);
}

double
net_get_bytes_out(const char *device) {

  double out_bytes = -1.0;

  net_get_bytes_inout(device, (double *) NULL, &out_bytes);
  return (out_bytes);
}

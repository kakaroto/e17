/*
 * Copyright (C) 1999-2000, Michael Jennings
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

#ifndef EPPLET_NET_H
#define EPPLET_NET_H

#include <errno.h>

#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

/* Returns an array containing the names of all valid network devices on the system */
extern char       **net_get_devices(unsigned long *count);

/* Stores the number of bytes received and transmitted by device into
   the supplied pointers.  Returns 0 on success, a valid errno value on
   failure.  You can pass NULL for either pointer to not get that value.
   Passing NULL for both pointers will verify that the device exists. */
extern unsigned char net_get_bytes_inout(const char *device, double *in_bytes,
					 double *out_bytes);

/* Just retrieves the number of bytes received by the device. */
extern double       net_get_bytes_in(const char *device);

/* Just retrieves the number of bytes transmitted by the device. */
extern double       net_get_bytes_out(const char *device);

/* Returns a string that describes the error encountered. */
extern const char  *net_strerror(unsigned char err_code);

/* Checks to see if the device actually exists. */
#define net_check_device(dev)   (net_get_bytes_inout(device, (double *) NULL, (double *) NULL));

#endif /* EPPLET_NET_H */

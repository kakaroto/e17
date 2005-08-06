/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifndef efsd_io_h
#define efsd_io_h

#include <efsd.h>

#if HAVE_ECORE
int      efsd_io_write_command(Ecore_Ipc_Server* server, EfsdCommand *ecom);
#else
int      efsd_io_write_command(int sockfd, EfsdCommand *ecom);
#endif

int      efsd_io_read_command(int sockfd, EfsdCommand *ecom);

int      efsd_io_write_event(int sockfd, EfsdEvent *ee);
int      efsd_io_read_event(int sockfd, EfsdEvent *ee);

int      efsd_io_write_option(int sockfd, EfsdOption *eo);
int      efsd_io_read_option(int sockfd, EfsdOption *eo);

#endif

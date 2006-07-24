/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
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
#ifndef _FILE_H_
#define _FILE_H_

#include <time.h>

void                Etmp(char *s);

void                E_md(const char *s);
void                E_mv(const char *s, const char *ss);
void                E_rm(const char *s);
char              **E_ls(const char *dir, int *num);

int                 exists(const char *s);
int                 isdir(const char *s);
int                 isfile(const char *s);
int                 canread(const char *s);
int                 canwrite(const char *s);
int                 canexec(const char *s);
time_t              moddate(const char *s);
int                 fileinode(const char *s);
int                 filedev_map(int dev);
int                 filedev(const char *s);

int                 isabspath(const char *s);
const char         *FileExtension(const char *file);
char               *fileof(const char *s);
char               *fullfileof(const char *s);
char               *pathtoexec(const char *file);
char               *pathtofile(const char *file);

#endif /* _FILE_H_ */
